__description__ = "Go back to the initial shell"
__author__ = "@HashtagMarkus"
__type__ = "beacon"


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    shad0w.current_beacon = None
