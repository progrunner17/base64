#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <strings.h>

#define MODE_DECODE 0
#define MODE_ENCODE 1

FILE *fp_in = NULL;
FILE *fp_out = NULL;
int num = 0;
int mode = MODE_ENCODE;
int parse_args(int argc, char *const argv[]);
void encode(void);
void decode(void);

int main(int argc, char *const argv[])
{
    if (parse_args(argc, argv) < 0)
    {
        perror("parse_args");
        exit(EXIT_FAILURE);
    }

    switch (mode)
    {
    case MODE_ENCODE:
        encode();
        break;
    case MODE_DECODE:
        decode();
        break;
    }

    fclose(fp_in);
    fclose(fp_out);
    return 0;
}

int parse_args(int argc, char *const argv[])
{
    struct option longopts[] = {
        {"help", no_argument, NULL, 'h'},
        {"decode", no_argument, NULL, 'D'},
        {"break", required_argument, NULL, 'b'},
        {"input", optional_argument, NULL, 'i'},
        {"output", optional_argument, NULL, 'o'},
        {0, 0, 0, 0},
    };
    int opt;
    int longindex;

    char const *in_filename = NULL;
    char const *out_filename = NULL;

    char const *help_str =
        "Usage:  base64 [-hvD] [-b num] [-i in_file] [-o out_file]\n"
        "-h, --help     display this message\n"
        "-D, --decode   decodes input (To be supported)\n"
        "-b, --break    break encoded string into num character lines (To be supported)\n"
        "-i, --input    input file (default: \" - \" for stdin)\n"
        "-o, --output   output file (default: \" - \" for stdout)\n";

    while ((opt = getopt_long(argc, argv, "hDb:i::o::", longopts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'h':
            fprintf(stderr, "%s", help_str);
            exit(EXIT_SUCCESS);
            break;
        case 'D':
            mode = MODE_DECODE;
            break;
        case 'b':
            fprintf(stderr, "this option is not suppoted\n");
            fprintf(stderr, "%s", help_str);
            exit(EXIT_SUCCESS);
            break;
        case 'i':
            if (strlen(optarg) > 0)
                in_filename = optarg;

            break;
        case 'o':
            if (strlen(optarg) > 0)
                out_filename = optarg;

            break;
        default:
            fprintf(stderr, "%s", help_str);
            return -1;
        }
    }

    if (in_filename == NULL)
        fp_in = stdin;
    else if (mode == MODE_DECODE)
        fp_in = fopen(in_filename, "r");
    else if (mode == MODE_ENCODE)
        fp_in = fopen(in_filename, "rb");

    if (fp_out == NULL)
        fp_out = stdout;
    else if (mode == MODE_DECODE)
        fp_out = fopen(out_filename, "wb");
    else if (mode == MODE_ENCODE)
        fp_out = fopen(out_filename, "w");

    return 0;
}

void encode()
{
    int n = 0;
    int count = 0;
    int c = 0;
    unsigned int buf;
    int i = 0;
    while ((n = fread(&buf, 1, 3, fp_in)) > 0)
    {
        buf = ((buf & 0xAAAAAAAA) >> 1) | ((buf & 0x55555555) << 1);
        buf = ((buf & 0xCCCCCCCC) >> 2) | ((buf & 0x33333333) << 2);
        buf = ((buf & 0xF0F0F0F0) >> 4) | ((buf & 0x0F0F0F0F) << 4);
        buf &= 0xFFFFFF >> ((3 - n) * 8);

        for (i = 0; i < 4; i++)
        {
            c = 0x3F & buf;
            c = ((c & 9) << 2) | ((c & 36) >> 2) | (c & 18);
            c = ((c & 7) << 3) | ((c & 56) >> 3);
            if (i > n)
                c = '=';
            else if (c < 26)
                c = 'A' + c;
            else if (c < 52)
                c = 'a' + c - 26;
            else if (c < 62)
                c = '0' + c - 52;
            else if (c == 62)
                c = '+';
            else if (c == 63)
                c = '\\';
            fputc(c, fp_out);
            buf >>= 6;
        }
    }
    fputc('\n', fp_out);
}

void decode()
{
    int n, c, pad = 0;
    char buf[4];
    uint64_t data;
    while ((n = fread(buf, 1, 4, fp_in)))
    {
        data = 0;
        if (n < 4)
            return;
        for (int i = 0; i < 4; i++)
        {

            pad = 0;
            if ('A' <= buf[i] && buf[i] <= 'Z')
                c = buf[i] - 'A';
            else if ('a' <= buf[i] && buf[i] <= 'z')
                c = buf[i] - 'a' + 26;
            else if ('0' <= buf[i] && buf[i] <= '9')
                c = buf[i] - '0' + 52;
            else if (buf[i] == '+')
                c = 62;
            else if (buf[i] == '\\')
                c = 63;
            else if (buf[i] == '=')
            {
                c = 0;
                pad++;
            }
            c = ((c & 9) << 2) | ((c & 36) >> 2) | (c & 18);
            c = ((c & 7) << 3) | ((c & 56) >> 3);
            data |= c << (i * 6);
        }

        data = ((data & 0xAAAAAAAA) >> 1) | ((data & 0x55555555) << 1);
        data = ((data & 0xCCCCCCCC) >> 2) | ((data & 0x33333333) << 2);
        data = ((data & 0xF0F0F0F0) >> 4) | ((data & 0x0F0F0F0F) << 4);
        if (pad == 0)
            fwrite(&data, 3, 1, fp_out);
        else
        {
            fwrite(&data, 4 - pad, 1, fp_out);
        }
    }
}