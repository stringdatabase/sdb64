# rem all imports happen within the _setup.py script

_gui_closed = False
_close_event_sent = False

def gui_step(max_milliseconds=5):
    global _gui_closed, _close_event_sent

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
    elif event == 'Ok':
        payload = json.dumps(values)
        sd.post_event(event,payload)
    elif event == '__TIMEOUT__':   
       sd.post_event(event, event) 
    return 0