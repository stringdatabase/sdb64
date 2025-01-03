**********************
Embedding Python in SD
**********************

PY_GUI_TEST

Program creates a simple gui dialog via a script file executed from with in SD.
User populates fields, hits ok, program prints entered data back out.

Uses FreeSimpleGUI which must be installed prior to running the program.
https://github.com/spyoungtech/FreeSimpleGUI

Script file can be found in example/python/embedded_python/BP/sdguitest.py. 

edit PY_GUI_TEST line for correct location of python script file.

    script_file = "/home/xyz/python_stuff/sdguitest.py"

This program cannot be run from root unless you have played with your systems default settings.
Either copy the program to your BP directory and compile, or compile and catalog global, running from
your user account.

    
