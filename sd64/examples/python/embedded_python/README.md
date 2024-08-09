Steps to build SD with embedded python using the SDEXT function.

If necessary edit:

py_Makefile line 35 for correct location of python header files.
35 PYHDRS   := /usr/include/python3.12

sdext_py.c line 59 for correct location of python header files.
59 #include <python3.12/Python.h>  
      
sdguitest.py line 3 for proper location of FreeSimpleGUI.py

From the top level directory containing the SD install (~/sdb64)
  execute the py_debian-installsd.sh script.  eg)  
  
~/sdb64$   sd64/examples/python/embedded_python/py_debian-installsd.sh

Note: py_debian-installsd.sh modifies gpl.src with the following line.

# make sure we compile sdext_py.c
echo "sdext_py" >> gpl.src

If you need to rerun either install script, you must delete the line containing
 "sdext_py" from gpl.src.