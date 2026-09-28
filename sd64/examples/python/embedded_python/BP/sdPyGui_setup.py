# sdPyGuiTest.py
import sys
# sys.path.append('/home/xyz/python_stuff')   <=== directory that contains FreeSimpleGUI.py. Required for Ubuntu / Debian
#                                             On Debian, Python environment is externally managed
#                                             FreeSimpleGUI must be either locally installed and add path or use a virtual environment.
#                                            (Create an isolated environment using python3 -m venv .venv, to activate it, and run pip safely inside it).
import FreeSimpleGUI as sg
import sd


SDME_FM  = chr(254)
SDME_VM  = chr(253)
SDME_SVM = chr(252)
#

layout = [ 
            [sg.Text('User Name', size=(12)), sg.Input(key='-UNAME-', size=(25))],
            [sg.Text('DOB', size=(12)), sg.Input(key='-DOB-', size=(25))],
            [sg.Text('Account', size=(12)), sg.Input(key='-ACCOUNT-', size=(25))],
            [sg.Button('Ok')]
            ]

window = sg.Window('Simple Inputs', layout, element_justification='r',finalize=True)


window.TKroot.update_idletasks()
window.TKroot.update()