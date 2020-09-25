import base64
import sqlite3
import hashlib
import random
import datetime

class Authentication(object):

    def __init__(self):
        super(Authentication, self).__init__()

        self.cookies = {}

        self.database = "/root/shad0w/db/shad0w_teamsrv.sqlite"

        self.conn = sqlite3.connect(self.database, check_same_thread=False)

    def _get_rand(self, len=5):
        """
        Get random numbers
        """

        nums = []

        for _ in range(0, len):
            nums.append(str(random.randint(0, 9)))

        return ''.join(nums)

    def _gen_cookie(self, user, password):
        """
        Generate and store the auth cookie to use.
        If one already exists use that.
        """

        # init the cookie
        jcook = None

        # check if there alreadys is a cookie for that user.
        try:
            jcook = self.cookies[user]
        except:
            pass

        # make the cookie... yeh probly a better way to do this
        if jcook is None:
            japnd = password + self._get_rand() + user + self._get_rand() + str(datetime.datetime.now())
            joint = base64.b64encode(japnd.encode())
            jhash = hashlib.sha256(joint).digest()
            jcook = base64.b64encode(jhash)

            # add cookie to list
            self.cookies[user] = jcook

        return jcook

    def login(self, username, password):
        """
        Handle the login for the team server.
        """

        # get the values from the database
        cursor = self.conn.execute("SELECT * FROM Users;")
        # _, user, pwhash = cursor.fetchall()

        for row in cursor.fetchall():
            _, user, pwhash = row

            # calculate the sha256 of the password
            password_hash = hashlib.sha256(password.encode()).hexdigest()

            # check if the hashes and usernames match
            if (user == username) and (pwhash == password_hash):
                auth_cookie = self._gen_cookie(username, password)
                return True, auth_cookie

        # auth failed
        return False, None

    def validate_cookie(self, cookie):
        """
        Check that a cookie is valid.
        """

        # iter though the cookies
        for user in self.cookies:
            # check if they match
            if cookie == self.cookies[user].decode():
                # return the user and that its valid
                return user, True

        return None, False