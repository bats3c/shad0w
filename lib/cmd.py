from prompt_toolkit.styles import Style

from pygments.lexer import Lexer, RegexLexer, do_insertions, bygroups, \
    include, default, this, using, words
from pygments.token import Punctuation, \
    Text, Comment, Operator, Keyword, Name, String, Number, Generic

class Shad0wLexer(RegexLexer):
    name = 'shad0w'
    aliases = ['shad0w']

    commands = ("help execute cat mkdir \
                 upload cd elevate beacons exit \
                 ls rm pwd download whoami mimikatz \
                 rubeus seatbelt sharpwmi sharpchrome \
                 sharpdpapi sharpup lockless safetykatz \
                 sharpdump sharpsocks sharphound").split()

    lex_style = Style.from_dict({
    'pygments.keyword':   '#FFFFFF',
    })

    tokens = {
        'root': [
            include('basic'),
        ],
        'basic': [
            (r'\b({})(\s*)\b'.format('|'.join(commands)), bygroups(Keyword, Text)),
            (r'-\S*', Operator),
            (r'(["\'])(?:(?=(\\?))\2.)*?\1', String.Single),
        ],
    }
