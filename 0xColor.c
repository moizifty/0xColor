#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void Usage(void)
{
    printf("0xColor [-option][argument]\n"
           "        -h        print this message\n"
           "        -o        true/false (set override redirect)\n"
           "The below options only work if -o option is set to true\n"
           "But only for tiling window managers, for floating window\n"
           "managers the options below should world regardless of -o\n"
           "being set to true (the positioning flags might not work\n"
           "depending on the window manager\n\n"
           "        -g        string (standard X application geometry string\n"
           "                  run 'man XParseGeometry' to get format of string)\n"
           "        -geometry\n\n"
           "Note: the width and height values passed in the above flags\n"
           "are the width and height of each individual color block\n\n"
           "The following flags control window position, if -g option\n"
           "is present, these flags should appear after\n\n"
           "        -yt       sets windows vertical position to top\n"
           "        -ym       sets windows vertical position to middle\n"
           "        -yb       sets windows vertical position to bottom\n"
           "        -xl       sets windows horizontal position to left\n"
           "        -xm       sets windows horizontal position to middle\n"
           "        -xr       sets windows horizontal position to right\n");
}

static void Init(void);
static int ReadStdin(void);
static void GenerateColors(char **cols, int num, XColor *out);

static XColor *xcolors;
static char **colorsRead;
static int numLinesRead;
static int numValidColors;

static Display *disp;
static Window root;
static int scrNum;
static unsigned long rootWidth;
static unsigned long rootHeight;

int main(int argc, char **argv)
{
    Window win;
    unsigned int width = 100;
    unsigned int height = 100;
    int x = 0;
    int y = 0;
    Bool enableOverrideRedir = False;
    argv++;

    Init();
 
    while(*argv != NULL)
    {
        if(!strcmp(*argv, "-o"))
            enableOverrideRedir = !strcmp(*++argv , "true") ? True : False;
        else if(!strcmp(*argv, "-g") || !strcmp(*argv, "-geometry"))
            XParseGeometry(*++argv, &x, &y, &width, &height);
        else if(!strcmp(*argv, "-yt")) // options should come over after -g as geometry is calculated
            y = 0;
        else if(!strcmp(*argv, "-ym"))
            y = (rootHeight / 2) - (height / 2);
        else if(!strcmp(*argv, "-yb"))
            y = rootHeight - (width / 2);
        else if(!strcmp(*argv, "-xl")) // options should come over after -g as geometry is calculated
            x = 0;
        else if(!strcmp(*argv, "-xm"))
            x = (rootWidth / 2) - ( (width * numValidColors) / 2);
        else if(!strcmp(*argv, "-xr"))
            x = rootWidth - (width * numValidColors);
        else if(!strcmp(*argv, "-h"))
            Usage();
        else
        {
            fprintf(stderr, "option not valid: '%s'\n", *argv);
            Usage();
        }
        argv++;
    }

    XSetWindowAttributes wa = 
    {
        .override_redirect = enableOverrideRedir,
        .event_mask = ExposureMask | StructureNotifyMask | ButtonPressMask,
    };

    win = XCreateWindow(disp, root, x, y, width * numValidColors, height, 4,
                        CopyFromParent, CopyFromParent, 
                        CopyFromParent, CWEventMask | CWOverrideRedirect,
                        &wa);
    XMapWindow(disp, win); 

    XGCValues val = 
    {
        .background = WhitePixel(disp, scrNum),
        .foreground = xcolors[0].pixel,
    };

    GC gc = XCreateGC(disp, win, GCBackground | GCForeground, &val);

    //with odd amount of colors entered, the width of the window doesn't split up evenly
    //this variable stores the num pixels which were missed when splitting
    int leftOverPixels = 0;
    XEvent e;
    while(True)
    {
        XNextEvent(disp, &e);
        switch(e.type)
        {
            case Expose:
            {
                if(e.xexpose.count != 0)
                    break;
                int x = 0;
                for(XColor *pC = xcolors; pC < xcolors + numValidColors; pC++, x+=width, leftOverPixels--)
                {
                    if(leftOverPixels < 0) 
                        leftOverPixels = 0;
                    XSetForeground(disp, gc, pC->pixel);
                    XFillRectangle(disp, win, gc, x, 0, width + ((leftOverPixels) ? 1 : 0), height);
                    x += ((leftOverPixels) ? 1 : 0);
                }
                break;
            }
            case ConfigureNotify:
            {
                width = e.xconfigure.width / numValidColors;
                leftOverPixels = e.xconfigure.width % numValidColors;
                height = e.xconfigure.height;
                break;
            }
        }
    }
    free(colorsRead);
    free(xcolors);
    XCloseDisplay(disp);
    return 0;
}

void Init(void)
{
    if(!(numLinesRead = ReadStdin()))
    {
        fprintf(stderr, "No colors were read\n");
        exit(EXIT_FAILURE);
    }

    if( (disp = XOpenDisplay(NULL)) == NULL)
    {
        fprintf(stderr, "Could not connect to X server with Display: %s\n", XDisplayName(NULL));
        exit(EXIT_FAILURE);
    }
    scrNum = DefaultScreen(disp);
    root = RootWindow(disp, scrNum);
    rootWidth = DisplayWidth(disp, scrNum);
    rootHeight = DisplayHeight(disp, scrNum);

    if( (xcolors = malloc(sizeof(XColor) * 100)) == NULL)
    {
        fprintf(stderr, "Could not allocate memory for XColor array\n");
        exit(EXIT_FAILURE);
    }
    GenerateColors(colorsRead, numLinesRead,xcolors);
    printf("Colors given: %d <> Colors valid: %d\n", numLinesRead, numValidColors);
    if(numValidColors == 0)
    {
        fprintf(stderr, "No valid colors\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Reads from standard input and stores the data in colors
 * returns the number of items read
 */
int ReadStdin(void)
{
    int size = 0;
    int num = 0;
    char buf[BUFSIZ + 1] = {0};
    char *q;
    for(num = 0; fgets(buf, 256, stdin); num++) 
    {
        if( (colorsRead = realloc(colorsRead, (size += BUFSIZ))) == NULL)
        {
            fprintf(stderr, "Could not reallocate memory for colors\n");
            exit(EXIT_FAILURE);
        }
        //remove the new line from the end of the color (it cant be parsed)
        if( (q = strchr(buf, '\n')))
            *q = '\0';
        if( (*(colorsRead + num) = strdup(buf)) == NULL)
        {
            fprintf(stderr, "Could not allocate memory for color string\n");
            exit(EXIT_FAILURE);
        }
        if(!strcmp(*(colorsRead + num), "help"))
        {
            Usage();
            exit(EXIT_FAILURE);
        }
    }
    return num;
}

void GenerateColors(char **cols, int num, XColor *outCols)
{
    for(; num > 0; cols++, outCols++, num--)
    {
        if(!XParseColor(disp, DefaultColormap(disp, scrNum), *cols, outCols))
        {
            Bool found = False;
            char *subcol = *cols;
            while( (subcol = strchr(++subcol, '#')) != NULL)
            {
                char c[] = {subcol[0], subcol[1], subcol[2], subcol[3], subcol[4], subcol[5], subcol[6]};
                if(!XParseColor(disp, DefaultColormap(disp, scrNum), c, outCols))
                    fprintf(stderr, "Could not parse color: %s\n", *cols);
                else
                {
                    printf("%s\n", c);
                    numValidColors += 1;
                    found = True;
                }
            }
            if(!found)
                --outCols;
        }
        else 
        {
            printf("%s\n", *cols); 
            numValidColors += 1;
        }
        if(!XAllocColor(disp, DefaultColormap(disp, scrNum), outCols))
            fprintf(stderr, "Could not allocate memory for color: %s\n", *cols);
    }
}
