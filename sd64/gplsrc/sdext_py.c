/* SDEXT_PY.C
 * python integration for SD via SDEXT (op_sdext.c) BASIC function
 * Copyright (c)2024 The SD Developers, All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
 *
 * to do- add STATUS() = 0 successful call, or  STATUS() = 1 unsuccessful call
 * 
 * START-HISTORY:
 * 11 Aug 2024 mab add PyErr_Print() to file and string script execution failure
 * 15 Jul 2024 MAB add SDME_PY.C 
 * 19 Jul 2024 mab remove Py_DECREF(pdict) at finalize, not necessary (at least samples I have seen dont do it)
 *   Also note, original idea was initialize - run script - finalize for each use (call from BASIC prog).
 *   This does not always appear to work, sometimes end up with double free or corruption (out) error on second call to 
 *   Py_Run_File.  Searching python forums seems to suggest only initialize and finalize once per SD session.
 *   Code already checks for python already initiailize, but how to handle Finalize?
 *   Could add test at SD close if Py_IsInitialized() then Py_FinalizeEx() ?? TBD
 * START-DESCRIPTION:
 *
 *
 * END-DESCRIPTION
 *
 * defined in keys.h
 * embedded python key codes
 * SD_PyInit        2000   initialize the python interpreter   
 * SD_PyFinal       2001  * Finalize the python interpreter   
 * SD_PyRunStr      2010  * Take the string in qmBasic variable VAL and run in python interpreter   
 * SD_PyRunFile     2011  * Take the file and path defined in qmBasic variable VAL and run in python interpreter   
 * SD_PyGetAtt      2100  * Return the (string) value of python attribute defined in qmBasic variable VAL   
 *
 * Embedded Python Error codes   *
 * defined in err.h
 * SD_PyEr_NotInit    -2001    * interperter not initiialized 
 * SD_PyEr_Dict       -2002    * PyDict_New() failed 
 * SD_PyEr_Builtin    -2003    * failed to set __builtins__ link to the built-in scope 
 * SD_PyEr_Excpt      -2004    * exception on PyRun_String 
 * SD_PyEr_FinalEr    -2005    *  error reported by GPL.BP Program PY_FINALIZE 
 * SD_PyEr_NOF        -2006    * could not open script file 
 

 * START-CODE
 */


#include "sd.h"
#include <linux/limits.h>
#include <libgen.h>            /* needed for basename function */ 
#include <python3.12/Python.h>        


#include "keys.h"


/* python objects for embedded python, these need to hang around between calls! */
PyObject *pval, *psval, *pbval, *prun;
PyObject *pdict; 


void sdext_py(int key, char* Arg){

  char *pyResult; 
  FILE *pyfd;         /* file descriptor for python script file */
  char *pyscriptname; /* script name from pyfd */
  char pyFilePath[PATH_MAX+1] init("");   /* max length a file path can be, defined in linux/limits.h */
  
  int myResult;
  char shutdown[] = "shutdown";
  char nullresult[] = "";
  
  process.status = 0;   /* setup status() value */
  myResult = 0;         /* init result response */
  /*Evaluate KEY */
  
  switch (key) {

    case SD_PyInit: /* Initialize python */
      if (!Py_IsInitialized()) {  /* only initialize if not already so */
        Py_Initialize();          /* There is no return value; it is a fatal error if the initialization fails. */
       /* "dictionaries that serve as namespaces for running code are generally required 
        to have a __builtins__ link to the built-in scope searched last for name lookups"
        from Programming Python 4th edition, Basic Embedding Techniques,
        Running Strings in Dictionaries       */
        if (Py_IsInitialized()) {     /* did initialization succeed? */
          pdict = PyDict_New(); /* return a new empty dictionary, or NULL on failure, New reference. */
          if (pdict == NULL){
            myResult = SD_PyEr_Dict;  /* could not create the dict object??*/
          }else{
            myResult =  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
            if (myResult < 0){
              myResult =  SD_PyEr_Builtin;  /* failed to set __builtins__ link to the built-in scope */
            }
          }
        }else{
          myResult =  SD_PyEr_NotInit; /* initialization failed */
        }
      
      } 
      process.status = myResult;
      InitDescr(e_stack, INTEGER);
      (e_stack++)->data.value = (int32_t)myResult;
      break;

    case SD_PyFinal: /* Finalize the python interpreter   */
                  
      if (Py_IsInitialized()) {  /* only finalize if previously initialized */
        myResult = Py_FinalizeEx();
      } else {
        myResult = 0; 
      }
      
      if (strcmp(Arg, shutdown) != 0){    /* test for shutdown, nothing to return */
        process.status = myResult;
        InitDescr(e_stack, INTEGER);
        (e_stack++)->data.value = (int32_t)myResult;
      }
      break;

    case SD_IsPyInit: /* Is python interpreter initialized?   */
                  
      myResult = Py_IsInitialized();

      InitDescr(e_stack, INTEGER);
      (e_stack++)->data.value = (int32_t)myResult;
      break;



    case SD_PyRunStr:   /* Take the string in Arg passed from op_sdme_ext (from SDME.EXT Arg value) and run in python interpreter  */
                         
      if (Py_IsInitialized()) {
        prun = PyRun_String(Arg, Py_file_input, pdict, pdict);    /* run statements */
        if (prun == NULL){
        /* exception */
          myResult = SD_PyEr_Excpt;
          PyErr_Print();
        } else {
          Py_DECREF(prun);
        }
      }else{
        myResult = SD_PyEr_NotInit;  
      }  
      
      process.status = myResult;
      InitDescr(e_stack, INTEGER);
      (e_stack++)->data.value = (int32_t)myResult;
      break; 

    case SD_PyRunFile:   /* Take the string in Arg passed from op_sdme_ext (from SDME.EXT Arg value) and treat as a file to run in python interpreter   */
                            /* must move to pyFilePath[PATH_MAX]   */
	    snprintf(pyFilePath, PATH_MAX,"%s",Arg);

      if (Py_IsInitialized()) {
        pyfd = fopen(pyFilePath, "r");
        if (pyfd == NULL) {
          myResult = SD_PyEr_NOF;  /* failed to open file, could look at global variable errno ?*/
        }else{
          pyscriptname = basename(pyFilePath);  /* rem The basename() function may modify the string pointed to by path */
          prun = PyRun_File(pyfd,pyscriptname,Py_file_input, pdict, pdict);    /* run statements */
          if (prun == NULL){
          /* exception */
            myResult = SD_PyEr_Excpt;
            PyErr_Print();
          } else {
             Py_DECREF(prun);
          }
          fclose(pyfd);
        }  
      }else{
        myResult = SD_PyEr_NotInit;  
      }  
      process.status = myResult;
      InitDescr(e_stack, INTEGER);
      (e_stack++)->data.value = (int32_t)myResult;
      break;      

    case SD_PyGetAtt:   /* Take the string in Arg passed from op_sdme_ext (from SDME.EXT Arg value) and treat as the python variable to return   */

         /* PyMapping_GetItemString - Fetches (indexes) a dictionary value by key
           "Besides allowing you to partition code string namespaces independent of any Python
            module files on the underlying system, this scheme provides a natural communication
            mechanism. Values that are stored in the new dictionary before code is run serve as
            inputs, and names assigned by the embedded code can later be fetched out of the dictionary
            to serve as code outputs." from Programming Python 4th edition, Basic Embedding Techniques,
             Running Strings in Dictionaries       */
       
      /* access the python dictionary entry for the object we are after */
      
     if (!Py_IsInitialized()) {
      /* we dont have a running python interpreter */
        process.status = SD_PyEr_NotInit;
        k_put_c_string(nullresult, e_stack);   /* sets descr as type string and place the python value into it */
        e_stack++;
        break;
      }
      pval =  PyMapping_GetItemString(pdict, Arg);             /* get my result string */
      if (pval == NULL) {
        PyErr_Clear();  /* we most likely had a key error, once set Py_Run will not work, clear it*/  
      /* fetch of dictionary value failed for some reason */
        process.status = SD_PyEr_Key;
        k_put_c_string(nullresult, e_stack);   /* sets descr as type string and place the python value into it */
        e_stack++;
        break;
      } 
      
      /* fetch of dictionary value succeeded, so what do we have here? */
      if (PyBytes_Check(pval)) {
        /* bytes object hopefully we encoded using latin, otherwise we will have issues with @FM @VM @SVM */
        pyResult = PyBytes_AsString(pval);   /* get the c string */
        
      }else{
        /* not a byte object */
        /* for now all non byte objects are converted to string then encoded as Latin, more work to be done here  */
        psval =  PyObject_Str(pval);              /* convert result object to string representation */
        if (psval == NULL) {
                                                  /* error converting python object to string */
          process.status = SD_PyEr_ObToStr;
          k_put_c_string(nullresult, e_stack);    /* sets descr as type string and place the python value into it */
          Py_DECREF(pval);
          e_stack++;
          break;      
        }
        /* encode our string object to bytes */
        pbval =  PyUnicode_AsLatin1String(psval); /* encode to bytes using Latin1 */
        if (pbval == NULL) {
                                                 /* error encoding unicode python string to to Latin */
          myResult = SD_PyErr_UniToStr;
          process.status = SD_PyErr_UniToStr;
          k_put_c_string(nullresult, e_stack);   /* sets descr as type string and place the python value into it */
          Py_DECREF(pval);
          Py_DECREF(psval);
          e_stack++;
          break;
        }
 
        pyResult = PyBytes_AsString(pbval);        /* finally get the c string */
        Py_DECREF(psval);
        Py_DECREF(pbval);
      }
      k_put_c_string(pyResult, e_stack);   /* sets descr as type string and place the python value into it */
      Py_DECREF(pval);
      e_stack++;
      break;  

  }

  return;
}
