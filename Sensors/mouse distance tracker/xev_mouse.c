//gcc -Wall xev.c -lX11
// TODO
// snap back (wrappointer)
// display cumulative

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define XDIST 300
#define YDIST 300

Display *dpy;
int screen;
XIC xic = NULL;

int main (int argc, char **argv) {
    char *displayname = NULL;
    int i;
    XSizeHints hints;
    Window w;
    XSetWindowAttributes attr;
    unsigned long mask = 0L;
    XIM xim;
    XIMStyles *xim_styles;
    XIMStyle xim_style = 0;
    char *imvalret;
	long long int m_x = 0, m_y = 0;
	long long int last_m_x = 0, last_m_y = 0;


    w = 0;
    dpy = XOpenDisplay (displayname); // open display
    if (!dpy) {
      printf (">> unable to open display '%s'\n", XDisplayName (displayname));
      exit (1);
    }

    xim = XOpenIM (dpy, NULL, NULL, NULL);
    if (xim == NULL) {
        printf (">> XOpenIM failed\n");
    }

    if (xim) {
        imvalret = XGetIMValues (xim, XNQueryInputStyle, &xim_styles, NULL);
        if (imvalret != NULL || xim_styles == NULL) {
            printf(">> input method doesn't support any styles\n");
        }

        if (xim_styles) {
            xim_style = 0;
            for (i = 0;  i < xim_styles->count_styles;  i++) {
                if (xim_styles->supported_styles[i] ==
                    (XIMPreeditNothing | XIMStatusNothing)) {
                    xim_style = xim_styles->supported_styles[i];
                    break;
                }
            }

            if (xim_style == 0) {
                printf (">> input method doesn't support the style we support\n");
            }
            XFree (xim_styles);
        }
    }

    screen = DefaultScreen (dpy);

    // select for all events
	attr.event_mask = ButtonPressMask | PointerMotionMask;

	hints.width = hints.min_width = XDIST * 2;
	hints.height = hints.min_height = YDIST * 2;
	hints.flags = PMinSize;
	hints.x = hints.y = 30;
	attr.background_pixel = WhitePixel(dpy,screen);;
	mask |= (CWBackPixel | CWBorderPixel | CWEventMask);

	w = XCreateWindow (dpy, RootWindow (dpy, screen), hints.x, hints.y,
					hints.width, hints.height, 0, 0,
					InputOutput, (Visual *)CopyFromParent,
					mask, &attr);

	XSetStandardProperties (dpy, w, "MouseTrack", NULL, (Pixmap) 0, argv, argc, &hints);
	XMapWindow (dpy, w);

	if (xim && xim_style) {
		xic = XCreateIC (xim,
						XNInputStyle, xim_style,
						XNClientWindow, w,
						XNFocusWindow, w,
						NULL);
		if (xic == NULL) {
			printf ("XCreateIC failed\n");
		}
	}
	XChangePointerControl(dpy, 1, 1, 0, 1, 0);
	//if ( ! XGrabPointer(dpy, w, 0, ButtonPressMask | PointerMotionMask, GrabModeSync,GrabModeAsync, w, None, CurrentTime) ) {
	//	printf(">> Couldn't grab pointer");
	//}
    while(1) {
		XEvent event;
		XNextEvent (dpy, &event);

		if (event.type == MotionNotify) {
			XMotionEvent *e = (XMotionEvent *) &event;
			if( e->x != XDIST || e->y != YDIST ) {
				m_x += e->x - XDIST;
				m_y += e->y - YDIST;
				printf ("Moved : time %lu, (%d,%d), cumulative:(%lld,%lld), root:(%d,%d)\n", e->time, e->x, e->y, m_x, m_y, e->x_root, e->y_root);
				XWarpPointer(dpy, None, w, 0, 0, 0, 0, XDIST, YDIST);
			}
		} else if (event.type == ButtonPress)
			m_x = m_y = 0;
	}

    XCloseDisplay (dpy);
    return 0;
}
