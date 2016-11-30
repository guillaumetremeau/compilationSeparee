#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
 * strucure cle (nom), valeur (note)
 */

int ERROR = 0;

#define ERROR_OK          0
#define ERROR_LIST_ALLOC  1
#define ERROR_FILE        1

typedef struct cell_s {
    char key[30];
    int  value;
    struct cell_s * nk, *nv;
} cell_t;

typedef struct list_s {
    cell_t * key;
    cell_t * value;
} list_t;

typedef struct gdata_s {
    Window         root;       
    Window         win;           
    Display       *dpy;    
    int            ecran;        
    GC             gcontext;         
    XGCValues      gcv;              
    Visual        *visual;
    Colormap       colormap;
    Font           font;
} gdata_t;

#define HISTOSIZE 21

/* #define DEBUG */

#ifdef DEBUG
    #define LOG(A) printf A
#else
    #define LOG(A) 
#endif

typedef int histogram_t[HISTOSIZE];

void initList(list_t * plist) {
    plist->key = plist->value = 0;
}

int insert(list_t * plist, char * nom, int note) {
    int ret = 0;
    cell_t * prec = 0, *cour =0;
    cell_t * cell = (cell_t * )malloc(sizeof(cell_t));

    if (cell) {
        strcpy(cell->key, nom);
        cell->value = note;
        cell->nk = cell->nv = 0;
        LOG(("** INSERT %s %d\n", cell->key, cell->value));

        prec = 0;
        cour = plist->key;
        while (cour &&  strcmp(cell->key, cour->key)>0) {
            prec = cour;
            cour = cour->nk;
        }
        if (prec) {
            cell->nk = prec->nk; 
            prec->nk = cell;
        } else {
            cell->nk = plist->key;
            plist->key = cell;
        }
        prec = 0;
        cour = plist->value;
        while (cour &&  cell->value < cour->value) {
            prec = cour;
            cour = cour->nv;
        }
        if (prec) {
            cell->nv = prec->nv; 
            prec->nv = cell;
        } else {
            cell->nv = plist->value;
            plist->value = cell;
        }
        ret = 1;
    } else ERROR = ERROR_LIST_ALLOC;
    
    return ret;
}

void displayByKey(list_t list) {
    cell_t * cour = list.key;
    
    printf("by key\n");
    while (cour) {
        printf("%s %d\n", cour->key, cour->value);
        cour = cour->nk;
    }
}

void displayByValue(list_t list) {
    cell_t * cour = list.value;
    
    printf("by value\n");
    while (cour) {
        printf("%s %d\n", cour->key, cour->value);
       cour = cour->nv;
    }
}

void freeList(list_t *plist) {
    plist-> key = plist-> value = NULL;
}

void computeHisto(histogram_t h, list_t l) {
    int i = 0;
    int j = 0;
    cell_t * cour = l.key;

    for (i=0; i < HISTOSIZE ; ++i)
        h[i] = 0.0;

    while (cour) {
        ++h[cour->value];
        cour = cour->nk;
        ++j;
    }
} 

void displayHisto(histogram_t h) {
    int i = 0;
    
    for (i=0; i < HISTOSIZE ; ++i)
        printf("[%2d] : %3d\n", i, h[i] );
}

int maxHisto(histogram_t h) {
    int i, max = h[0];
    
    for(i=1; i< HISTOSIZE; ++i)
        if (h[i]>max) max=h[i];
    
    return max;
}

float meanHisto(histogram_t h) {
    float som =0.;
    int   tot = 0, i;
    
    for (i=0; i< HISTOSIZE; ++i) {
        som  = h[i] * i;
        tot += h[i];
    }
    LOG(("MEAN %f / %d\n", som, tot));
    
    return som/(float)tot;
}

int countHisto(histogram_t h) {
    int   tot = 0, i;
    for (i=0; i< HISTOSIZE; ++i) {
        tot += h[i];
    }
    LOG(("TOT %d\n", tot));
  
    return tot;
}

void displayGraphicalHisto(gdata_t g, histogram_t h) {                          
    char          chaine[255];
    int maxx = 600;
    int maxy = 400;
    int i;   
    int j = maxHisto(h);

    XClearWindow(g.dpy, g.win);

    for(i=0; i<HISTOSIZE; ++i) {
        XDrawLine(g.dpy,g.win,g.gcontext,(int)(maxx/22.0*(i+1)), maxy-(int)(h[i]/(float)j*300.) ,(int)(maxx/22.0*(i+1)),maxy);
        sprintf(chaine, "%d", i);
        XDrawString(g.dpy, g.win, g.gcontext, (int)(maxx/22.0*(i+1)-4), 420, chaine, strlen(chaine));
        if (h[i]>0) {
            sprintf(chaine, "%d", h[i]);
            XDrawString(g.dpy, g.win, g.gcontext, (int)(maxx/22.0*(i+1)-4), 75, chaine, strlen(chaine));
        }
    }
    strcpy(chaine, "Occurences");
    XDrawString(g.dpy, g.win, g.gcontext, maxx-100, 50, chaine, strlen(chaine));
    sprintf(chaine, "Notes [%d]", countHisto(h));
    XDrawString(g.dpy, g.win, g.gcontext, 10, 445, chaine, strlen(chaine));
    sprintf(chaine, "Moyenne : %f", meanHisto(h));
    XDrawString(g.dpy, g.win, g.gcontext, 10, 470, chaine, strlen(chaine));
}

void displayGraph(histogram_t h) {
    gdata_t g;
    int wpx, bpx;
    int sortie = 0;
    XEvent ev;
    XGCValues      gcv;      
    char ** list;
    int count;         
    unsigned long  gcmask;

    g.dpy      = XOpenDisplay(0); 
    g.ecran    = DefaultScreen(g.dpy);
    g.root     = DefaultRootWindow(g.dpy);

    bpx      = BlackPixel(g.dpy,g.ecran);
    wpx      = WhitePixel(g.dpy,g.ecran);

    g.visual   = DefaultVisual(g.dpy, g.ecran);
    g.win = XCreateSimpleWindow(g.dpy,g.root,30,40,600,500,1,wpx,bpx);
    XStoreName(g.dpy,g.win,"Repartition des notes");

    XSelectInput(g.dpy,g.win,ButtonPressMask |
        ButtonReleaseMask |
        Button1MotionMask |
        KeyPressMask |
        ExposureMask );

    gcv.foreground = wpx;
    gcv.background = bpx;
    gcv.line_width = 2;
    gcv.function   = GXcopy;
    gcmask         = GCForeground | GCBackground | GCLineWidth | GCFunction;
    g.gcontext       = XCreateGC(g.dpy,g.win,gcmask,&gcv);

    list = XListFonts(g.dpy, "*-14-*", 10, &count); 
    g.font = XLoadFont(g.dpy, list[0] );  
    XSetFont(g.dpy,g.gcontext,g.font); 

    XMapWindow(g.dpy,g.win);           

    while(!sortie) {                                             
        XNextEvent(g.dpy,&ev);
        switch(ev.type) {
            case Expose   : displayGraphicalHisto(g, h);
                break;
            case KeyPress : sortie =1;
                break;
        }
    }

    XUnloadFont(g.dpy,g.font); 
    XFreeFontNames(list);
    /*XFreeColormap(dpy, colormap);*/
    XDestroyWindow(g.dpy , g.win );
    XCloseDisplay(g.dpy);
}


void displayText(histogram_t h) {
    displayHisto(h);
}

int main(int argc, char ** argv) {
    list_t list;
    histogram_t h;
    int text = 1;
    char * file = NULL;

    if (argc<2) {
        printf("[HELP] %s file mode\n", argv[0]);
        printf("       where file is a text file containing the data to display\n");
        printf("       where mode belongs to text/graph\n\n");
    } else {
        /* first parameter is file */
        if (strcmp(argv[1], "text")  && strcmp(argv[1], "graph") ) {
            file = argv[1];
            if (argc>=3)
            text = strcmp(argv[2], "graph");  
        } else {
            /* first parameter is text/graph */
            text = strcmp(argv[1], "graph");
            if (argc>=3)
                file=argv[2];
        }

        if (file) {
            fprintf(stderr, "Reading external file not implemented\n\n");
            ERROR = ERROR_FILE;
        } else {

            printf("DEMO MODE -- dummy data");
      
            insert(&list, "un", 20);
            insert(&list, "deux", 10);
            insert(&list, "trois", 20);
            insert(&list, "quatre", 15);
            insert(&list, "cinq", 15);
            insert(&list, "six", 15);
            insert(&list, "sept", 0);
            insert(&list, "huit", 14);
            insert(&list, "neuf", 11);
            insert(&list, "dix", 7);
            displayByKey(list);
            displayByValue(list);
            computeHisto(h,list);

            if (text) {
                displayText(h);
            } else {
                displayGraph(h);
            }

            freeList(&list);
        }
    }

    return ERROR;
}   