import inspect

class EventHandler(object):
    """
    Handle events.
    """

    def __init__(self, arg):
        super(EventHandler, self).__init__()

        self.debug_mode = arg.debugv
        self.buffer_mode = False

        self.event_buffer = {
            "master": {
                "global_info": [],
                "global_error": []
            }
        }

    def _add_new(self, cookie):
        """
        Setup the output for a new client
        """

        # give it a copy of everything so far
        self.event_buffer[cookie.decode()] = self.event_buffer["master"]

    def _clear_buffer(self, cookie):
        """
        Clear the event buffer, without clearing all the non standard dicts
        """

        values = []

        for v in self.event_buffer[cookie]:
            values.append(v)

        del self.event_buffer[cookie]

        self.event_buffer[cookie] = {
            "global_info": [],
            "global_error": []
        }

        for value in values:
            if (value != "global_info") or (value != "global_error"):
                self.event_buffer[cookie][value] = []

    def _create_beacon_holder(self, beacon):
        """
        Create the dict to hold output from the beacon
        """

        for cookie in self.event_buffer:
            self.event_buffer[cookie][beacon] = []

    def _add_new_event(self, event, location):
        """
        Add a new event to all buffers
        """

        for cookie in self.event_buffer:
            for storage in self.event_buffer[cookie]:
                if storage == location:
                    self.event_buffer[cookie][storage].append(event)

    def debug_log(self, text):
        """
        Log a debug message
        """

        if self.debug_mode:
            print("[DEBUG]", text)

        return

    def global_info(self, text):
        """
        Infomation not specific to a session
        """

        if self.buffer_mode is False:
            print(f"\033[1;34m[i]\033[0m {text}", end='\n')
        else:
            self._add_new_event(text, 'global_info')

        return

    def global_error(self, text):
        """
        Error not specific to a session
        """

        if self.buffer_mode is False:
            print(f"\033[1;31m[!]\033[0m {text}", end='\n')
        else:
            self._add_new_event(text, 'global_error')

        return

    def beacon_info(self, beacon, text):
        """
        Store infomation specific to a beacon
        """

        if self.buffer_mode is False:
            print(text)
        else:
            self._add_new_event(text, beacon)