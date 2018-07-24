#!/usr/local/bin/python3
def convertToBase64(i):
    if i < 26:
        return chr(ord('A') + i)
    elif i < 52:
        return chr(ord('a') + i - 26)
    elif i < 62:
        return chr(ord('0') + i - 62)
    elif i == 62:
        return '+'
    elif i == 63:
        return '\\'
    else:
        return '='


if __name__ == "__main__":
    with open("SPEC.md", "w") as fp:
        print('# base64 conversion table',file=fp)
        print('|10進数|2進数|base64文字|', file=fp)
        print('|:-|:'+'-'*6 + ':|-|', file=fp)
        for i in range(2**6):
            print('|{:<2}|0b{:<6}|{}|'.format(
                i, format(i, '06b'), convertToBase64(i)), file=fp)
