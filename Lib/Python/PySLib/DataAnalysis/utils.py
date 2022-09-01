import sys
write = sys.stdout.write

import shutil
import os


def printThere(x, y, text, doFlush=False):
     write("\x1b7\x1b[%d;%df%s\x1b8" % (x, y, text))

     if doFlush: sys.stdout.flush()

def clearTerminal():
    os.system('clear')

def getTerminalSize():
    return shutil.get_terminal_size()

ascii_gs = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`\'. "

def asciiGrayScale(normVal):
    len_ags = len(ascii_gs)
    i = int(normVal*(len_ags-1))

    if i < 0: return "__UNDERFLOW__"
    elif i > len_ags-1: return "__OVERFLOW__"

    return str(ascii_gs[i])

def isIterable(p_object):
    try:
        iter(p_object)
    except TypeError:
        return False
    return True

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKGREEN = '\033[92m'
WARNING = '\033[93m'
FAIL = '\033[91m'
ENDC = '\033[0m'
BOLD = '\033[1m'
UNDERLINE = '\033[4m'

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

    def okfail(string, outcome):
        if outcome is True:
            return bcolors.FAIL + bcolors.BOLD + string + bcolors.ENDC

        return bcolors.OKGREEN + bcolors.BOLD + string + bcolors.ENDC

    def yellow(string):
        return bcolors.WARNING + string + bcolors.ENDC

    def blue(string):
        return bcolors.OKBLUE + string + bcolors.ENDC

    def Bblue(string):
        return bcolors.BOLD + bcolors.blue(string)

def okfail(string, outcome):
    if outcome is True:
        return FAIL + BOLD + string + ENDC

    return OKGREEN + BOLD + string + ENDC

def yellow(string):
    return WARNING + string + ENDC

def blue(string):
    return OKBLUE + string + ENDC

def Bblue(string):
    return BOLD + blue(string)

def colorFail():
    write(FAIL)

def colorEndc():
    write(ENDC)

def colorBold():
    write(BOLD)
def round_sd(x, n=1):
    ''' Arrendondar ate certo numero de digitos significativos e retornar float. '''

    return float(('{:.'+str(n)+'g}').format(x))

def fillSpaces(val: int) -> str:
    return "{:>2}".format(val)

def fmt(x: float, n=2) -> str:
    return fmtFunc(n)(x)

#	Example: "{:10.4f}"
#	The empty string before the colon means "take the next provided
# argument to format()" – in this case the x as the only argument.
#	The 10.4f part after the colon is the format specification.
#	The f denotes fixed-point notation.
#	The 10 is the total width of the field being printed, lefted-padded by spaces.
#	The 4 is the number of digits after the decimal point.

def fmtExp(x):
    return "{:+11.3e}".format(x)

def fmtFunc(n):
    return ('{:.'+str(n)+'f}').format

if __name__ == '__main__':
    pass
