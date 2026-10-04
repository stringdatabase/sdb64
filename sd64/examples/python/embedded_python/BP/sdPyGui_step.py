# rem all imports happen within the _setup.py script

_gui_closed = False
_close_event_sent = False
values = {} 

def gui_step(max_milliseconds=5):
    # note we must declare as global otherwise our sd functions will not find in global dict 
    global _gui_closed, _close_event_sent, values

    if _gui_closed:
        return 0

    if not window.TKroot.winfo_exists():
        _gui_closed = True
        return 0

    event, values = window.read(timeout=max_milliseconds)
    if event == sg.WIN_CLOSED:
       window.close()
       _gui_closed = True
       _close_event_sent = True
       sd.post_event("window", "closed")
    elif event == '-OK-':
       # payload = json.dumps(values)
        payload = "check values dictionary"
        sd.post_event(event,payload)
    elif event == '__TIMEOUT__':   
       sd.post_event(event, event) 
    return 0