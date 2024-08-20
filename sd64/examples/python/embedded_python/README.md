**********************
Embedding Python in SD
**********************

Steps to build SD with embedded python using the SDEXT function.

If necessary edit:

  py_Makefile line for correct location of python header files.

    PYHDRS   := /usr/include/python3.12

  sdext_py.c line for correct location of python header files.

    #include <python3.12/Python.h>  
      
From the top level directory containing the SD install (~/sdb64)

  execute the py_debian-installsd.sh script.  eg)  
  
~/sdb64$   sd64/examples/python/embedded_python/py_debian-installsd.sh

Note: py_debian-installsd.sh modifies gpl.src with the following line.

echo "sdext_py" >> gpl.src

If you need to rerun either install script, you must delete the line containing
 "sdext_py" from gpl.src.

The following subroutine programs are copied to GPL.BP and provide the BASIC interface to the python interpreter.

     PY_INITIALIZE  calls api function Py_Initialize() - start the python interpreter
     PY_RUNSTRING   calls api function PyRun_String() - run a python script from string
     PY_RUNFILE     calls api function PyRun_File() - run a python script from file
     PY_GETATTR     calls api function PyMapping_GetItemString() - access the value of a python object.
      remember strings are UTF-8 in python and must be converted to bytes (decode in encode out)
      good reference here: https://nedbatchelder.com/text/unipain.html 
 

There are two sample programs copied to BP, PY_TEST and PY_GUI_TEST

PY_TEST executes a simple python script:

    FMRK   = chr(254)
    VMRKM  = chr(253)
    SVMRK  = chr(252)
    result = ""
    countdown = {"3" : "Three", "2" : "Two", "1" : "One", "0" : "Ignition!"}
    for count in countdown:
      result += count + " " + countdown[count] + FMRK 
    result = bytes(result, 'latin')

Running PY_TEST should result in the following out:

    :RUN BP PY_TEST
    embedded python test, hit <enter> to start?
    ==================================================
    attempt init
    status = 0
    ==================================================
    script text:
    FMRK   = chr(254)
    VMRKM  = chr(253)
    SVMRK  = chr(252)
    result = ""
    countdown = {"3" : "Three", "2" : "Two", "1" : "One", "0" : "Ignition!"}
    for count in countdown:
      result += count + " " + countdown[count] + FMRK 
    result = bytes(result, 'latin')
    ==================================================
    attempt to run script:
    status = 0
    ==================================================
    attempt to access python object: result
    status = 0
    result:
    3 Three�2 Two�1 One�0 Ignition!�
    3 Three
    2 Two
    1 One
    0 Ignition!

    ==================================================
    attempt to access python object: countdown
    status = 0
    countdown:
    {'3': 'Three', '2': 'Two', '1': 'One', '0': 'Ignition!'}
    ==================================================
    attempt to access nonexistent python object: dummy
    status = 12007
    dummy:
    dummy not found
    ==================================================
    complete

PY_GUI_TEST

Program creates a simple gui dialog via a script file executed from with in SD.
User populates fields, hits ok, program prints entered data back out.

Uses FreeSimpleGUI which must be installed prior to running the program.
https://github.com/spyoungtech/FreeSimpleGUI

Script file can be found in example/python/embedded_python/sdguitest.py. 

edit PY_GUI_TEST line for correct location of python script file.

    script_file = "/home/xyz/python_stuff/sdguitest.py"

This program cannot be run from root unless you have played with your systems default settings.
Either copy the program to your BP directory and compile, or compile and catalog global, running from
your user account.

    
