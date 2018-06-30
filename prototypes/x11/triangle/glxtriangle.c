

#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

static void redraw( Display *dpy, Window w )
{
	printf("Redraw event\n");

	glClear( GL_COLOR_BUFFER_BIT );
#if 0
	glColor3f( 1.0, 1.0, 0.0 );
	glRectf( -0.8, -0.8, 0.8, 0.8 );
#else
	glBegin(GL_TRIANGLES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex2d(0.9*cos(M_PI/2), 0.9*sin(M_PI/2));
	glColor3d(0.0, 1.0, 0.0);
	glVertex2d(0.9*cos(M_PI/2+2.0*M_PI/3.0), 0.9*sin(M_PI/2+2.0*M_PI/3.0));
	glColor3d(0.0, 0.0, 1.0);
	glVertex2d(0.9*cos(M_PI/2+2.0*M_PI*2.0/3.0), 0.9*sin(M_PI/2+2.0*M_PI*2.0/3.0));
	glEnd();
#endif
	glFlush();
	glXSwapBuffers( dpy, w );

}

static void resize( unsigned int width, unsigned int height )
{
	printf("Resize event\n");
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//gluPerspective(60.0, (double)width / (double)height, 1.0, 100.0);
	glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
}

static Window make_rgb_db_window( Display *dpy,
				  unsigned int width, unsigned int height )
{
	int attrib[] = { GLX_RGBA,
					 GLX_RED_SIZE, 1,
					 GLX_GREEN_SIZE, 1,
					 GLX_BLUE_SIZE, 1,
					 GLX_DOUBLEBUFFER,
					 None };
	int scrnum;
	XSetWindowAttributes attr;
	unsigned long mask;
	Window root;
	Window win;
	GLXContext ctx;
	XVisualInfo *visinfo;

	scrnum = DefaultScreen( dpy );
	root = RootWindow( dpy, scrnum );

	visinfo = glXChooseVisual( dpy, scrnum, attrib );
	if (!visinfo) {
		printf("Error: couldn't get an RGB, Double-buffered visual\n");
		exit(1);
	}

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateWindow( dpy, root, 0, 0, width, height,
						 0, visinfo->depth, InputOutput,
						 visinfo->visual, mask, &attr );

	ctx = glXCreateContext( dpy, visinfo, NULL, True );
	if (!ctx) {
		printf("Error: glXCreateContext failed\n");
		exit(1);
	}

	glXMakeCurrent( dpy, win, ctx );

	return win;
}

static void event_loop( Display *dpy )
{
	XEvent event;

	for(;;) {
		XNextEvent( dpy, &event );

		switch (event.type) {
		case Expose:
			redraw( dpy, event.xany.window );
			break;
		case ConfigureNotify:
			resize( event.xconfigure.width, event.xconfigure.height );
			break;
		}
	}
}

int main( int argc, char *argv[] )
{
	Display *dpy;
	Window win;

	dpy = XOpenDisplay(NULL);

	win = make_rgb_db_window( dpy, 300, 300 );

	{
		const char *glVendor = (const char *) glGetString(GL_VENDOR);
		const char *glRenderer = (const char *) glGetString(GL_RENDERER);
		const char *glVersion = (const char *) glGetString(GL_VERSION);
		const char *glShaderVersion = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
		printf("OpenGL vender: %s\n", glVendor);
		printf("OpenGL render: %s\n", glRenderer);
		printf("OpenGL version: %s\n", glVersion);
		printf("GLSL version: %s\n", glShaderVersion);
	}

	glClearColor( 0.5, 0.5, 0.5, 1.0 );

	XMapWindow( dpy, win );

	event_loop( dpy );

	return 0;
}
