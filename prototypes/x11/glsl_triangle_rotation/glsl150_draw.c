#include <stdio.h>
#include <math.h>

#include "gldrawcontext.h"

void glsl150_resize( unsigned int width, unsigned int height, GLDrawContext *pCtx)
{
	printf("Resize event\n");
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
}

void glsl150_redraw( Display *dpy, Window w, GLDrawContext *pCtx )
{

	const GLfloat scale = 0.40;
	//printf("Redraw event\n");

	glUseProgram(pCtx->programId);
#if 0
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//視点の設定 (0.0, 0.0, 1.0)の場所
	//           (0.0, 0.0, 0.0)の方向を向いて,
	//           (0.0, 1.0, 0.0)が上方向
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//回転を加える.
	glRotatef(pCtx->rotate,0.0,0.0,-1.0);
	//縮小する.
	glScalef(scale,scale,scale);

	// 三角形を書く
	glBegin(GL_TRIANGLES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(cos(M_PI/2), 0.9*sin(M_PI/2), 0.0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(cos(M_PI/2+2.0*M_PI/3.0), sin(M_PI/2+2.0*M_PI/3.0), 0.0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(cos(M_PI/2+2.0*M_PI*2.0/3.0), sin(M_PI/2+2.0*M_PI*2.0/3.0), 0.0);
	glEnd();
#endif
	
	glFlush();

	glUseProgram(0);

	glXSwapBuffers( dpy, w );

}

