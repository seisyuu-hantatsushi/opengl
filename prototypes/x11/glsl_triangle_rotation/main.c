

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

#include <errno.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include <math.h>

#include <getopt.h>

typedef struct GLDrawContext {
	uint32_t glslversion;
	GLfloat rotate;
	GLuint programId;
} GLDrawContext;

static const struct option long_options[] = {
	{"help",    no_argument,       NULL, 'h'},
	{"version", required_argument, NULL, 'v'},
	{NULL,      0,                 NULL,  0 }
};


static void showHelp(){
	fprintf(stderr, "-h,--help: this help.\n");
	fprintf(stderr, "-v,--version: specified glsl version {120|150}\n");
	return;
}

#define HELP_MODE 1
static int parseOption(int argc, char **argv, GLDrawContext *pCtx){
	int ret = 0;
	int c,opt_index;
	for(;;){
		c = getopt_long(argc, argv, "v:h", long_options, &opt_index);
		if(c == -1){
			// normal exit
			break;
		}
		else if(c == 'v'){
			char *pEnd = NULL;
			pCtx->glslversion = strtol(optarg, &pEnd, 10);
			if(optarg[0] != '\0' && *pEnd == '\0'){
				printf("glsl version: %u\n", pCtx->glslversion);
			}
			else {
				fprintf(stderr, "version option is invalid\n");
				ret = HELP_MODE;
			}
		}
	}
	return ret;
}


static void readGLError(const char *message){
	GLenum error = glGetError();
	if(error != GL_NO_ERROR){
		const GLubyte *errorStr = gluErrorString(error);
		fprintf(stderr, "%s %d:%s\n", message, error, errorStr);
	}
	return;
}

static const char *readShaderCode(const char *pFile){
	int ret;
	char *pShaderCode = NULL;
	int fd = -1;
	FILE *fp = NULL;
	struct stat stbuf;
	size_t readsize;

	fd = open(pFile, O_RDONLY);
	if(fd == -1){
		int lastError = errno;
		fprintf(stderr, "failed to open file. %s. <%s>\n", pFile, strerror(lastError));
		goto error_exit;
	}

	fp = fdopen(fd, "r");
	if(fp == NULL){
		int lastError = errno;
		fprintf(stderr, "failed to open file. %s. <%s>\n", pFile, strerror(lastError));
		goto error_exit;
	}

	ret = fstat(fd, &stbuf);
	if(ret == -1){
		int lastError = errno;
		fprintf(stderr, "failed to get stat %s. <%s>\n", pFile, strerror(lastError));
		goto error_exit;
	}

	pShaderCode = malloc(stbuf.st_size+1);
	if(pShaderCode == NULL){
		int lastError = errno;
		fprintf(stderr, "out of memory %s. <%s>\n", pFile, strerror(lastError));
		goto error_exit;
	}

	readsize = fread(pShaderCode, sizeof(char), stbuf.st_size, fp);
	if(readsize < stbuf.st_size){
		int lastError = errno;
		fprintf(stderr, "readsize %zd\n", readsize);
		fprintf(stderr, "short of code %s. <%s>\n", pFile, strerror(lastError));
	}

 error_exit:

	if(fp != NULL){
		fclose(fp);
	}
	else {
		if(fd != -1){
			close(fd);
		}
	}
	return pShaderCode;
}

static int32_t creareProgramShader(const char *pVertexShaderCode,
								   const char *pFragmentShaderCode,
								   GLuint *pProgramId){
	int ret = 0;
	GLint compiled;
	GLsizei size,len;

	GLuint vertextShader=0;
	GLuint fragmentShader=0;

	*pProgramId = 0;

	if(pVertexShaderCode != NULL){
		GLint length = strlen(pVertexShaderCode);
		const GLchar **ppCode = &pVertexShaderCode;
		const GLint  *pLengths = &length;

		vertextShader = glCreateShader(GL_VERTEX_SHADER);
		if(vertextShader == 0){
			readGLError("failed to create vertex shader");
			ret = -1;
			goto error_exit;
		}
		glShaderSource(vertextShader, 1, ppCode, pLengths);
		glCompileShader(vertextShader);
		glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			fprintf(stderr, "failed to compile vertex shader.\n");
			glGetProgramiv(vertextShader, GL_INFO_LOG_LENGTH, &size);
			fprintf(stderr, "info size: %d\n", size);
			if(size > 0){
				char *pBuf;
				pBuf = (char *)malloc(size+1);
				glGetShaderInfoLog(vertextShader, size, &len, pBuf);
				fprintf(stderr, "%s\n", pBuf);
				free(pBuf);
			}
			ret = -1;
			goto error_exit;
		}
	}

	if(pFragmentShaderCode != NULL){
		GLint length = strlen(pFragmentShaderCode);
		const GLchar **ppCode = &pFragmentShaderCode;
		const GLint  *pLengths = &length;

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if(fragmentShader == 0){
			readGLError("failed to create fragment shader");
			ret = -1;
			goto error_exit;
		}
		glShaderSource(fragmentShader, 1, ppCode, pLengths);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			fprintf(stderr, "failed to compile fragment shader\n");
			glGetProgramiv(fragmentShader, GL_INFO_LOG_LENGTH, &size);
			fprintf(stderr, "info size: %d\n", size);
			if(size > 0){
				char *pBuf;
				pBuf = (char *)malloc(size+1);
				glGetShaderInfoLog(fragmentShader, size, &len, pBuf);
				fprintf(stderr, "%s\n", pBuf);
				free(pBuf);
			}
			ret = -1;
			goto error_exit;
		}
		else {
			fprintf(stderr, "success to compile fragment shader.\n");
		}
	}

	*pProgramId = glCreateProgram();
	if(*pProgramId == 0){
		fprintf(stderr, "failed to create program.\n");
		goto error_exit;
	}

	if(vertextShader != 0){
		glAttachShader(*pProgramId, vertextShader);
		glDeleteShader(vertextShader);
	}

	if(fragmentShader != 0){
		glAttachShader(*pProgramId, fragmentShader);
		glDeleteShader(fragmentShader);
	}

	glLinkProgram(*pProgramId);
	{
		GLint linked;
		glGetProgramiv(*pProgramId, GL_LINK_STATUS, &linked);
		if(linked == GL_FALSE){
			char *pInfoLog;
			fprintf(stderr, "failed to link shader\n");
			glGetProgramiv(*pProgramId, GL_INFO_LOG_LENGTH, &size );
			fprintf(stderr, "info size: %d\n", size);
			if ( size > 0 ) {
				pInfoLog = (char *)malloc(size);
				glGetProgramInfoLog( *pProgramId, size, &len, pInfoLog );
				printf("%s\n",pInfoLog);
				free(pInfoLog);
			}
			ret = -1;
			goto error_exit;
		}
		else {
			fprintf(stderr, "success to link shader to program.\n");
		}
	}
	return ret;

 error_exit:

	if(fragmentShader != 0){
		glDeleteShader(fragmentShader);
	}

	if(vertextShader != 0){
		glDeleteShader(vertextShader);
	}

	if(*pProgramId != 0){
		glDeleteProgram(*pProgramId);
	}

	return ret;
}

static int32_t destroyProgramShader(GLuint programId){

	if(programId != 0){
		glDeleteProgram(programId);
	}

	return 0;
}

static void initShader(GLDrawContext *pCtx){
	const char *pVertexShaderCode = NULL;
	const char *pFragmentShaderCode = NULL;

	const char *pVertexShaderPath;
	const char *pFragmentShaderPath;

	if(pCtx->glslversion == 120){
		pVertexShaderPath    = "./simple_120.vert";
		pFragmentShaderPath  = "./simple_120.frag";
	}
	else {
		pVertexShaderPath    = "./simple_150.vert";
		pFragmentShaderPath  = "./simple_150.frag";
	}

	pVertexShaderCode   = readShaderCode(pVertexShaderPath);
	pFragmentShaderCode = readShaderCode(pFragmentShaderPath);

	creareProgramShader(pVertexShaderCode, pFragmentShaderCode, &pCtx->programId);
	if(pFragmentShaderCode != NULL){
		free((void *)pFragmentShaderCode);
	}

	if(pVertexShaderCode != NULL){
		free((void *)pVertexShaderCode);
	}

	return;
}

static void redraw( Display *dpy, Window w, GLDrawContext *pCtx )
{

	const GLfloat scale = 0.40;
	//printf("Redraw event\n");

	glUseProgram(pCtx->programId);

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
	glFlush();

	glUseProgram(0);

	glXSwapBuffers( dpy, w );

}

static void resize( unsigned int width, unsigned int height )
{
	printf("Resize event\n");
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
	//glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
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
	GLenum err;

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

	err = glewInit();
	if(err != GLEW_OK){
		printf("Error: failed to glewInit(). %s\n", glewGetErrorString(err));
		exit(1);
	}

	return win;
}

static void event_loop( Display *dpy, Window win,  GLDrawContext *pGLDrawCtx )
{

	struct timespec now, prev;
	clock_gettime(CLOCK_MONOTONIC, &now);
	prev = now;
	for(;;) {

		while(XPending(dpy) > 0){
			XEvent event;
			XNextEvent( dpy, &event );
			switch (event.type) {
			case Expose:
				break;
			case ConfigureNotify:
				resize( event.xconfigure.width, event.xconfigure.height );
				break;
			}
		}

		redraw( dpy, win, pGLDrawCtx );
		clock_gettime(CLOCK_MONOTONIC, &now);
		if(now.tv_sec != prev.tv_sec){
			pGLDrawCtx->rotate += 6.0;
			pGLDrawCtx->rotate = (float)((int)pGLDrawCtx->rotate%360);
			prev = now;
		}
	}

	return;
}

int main( int argc, char *argv[] )
{
	int32_t ret;
	Display *dpy;
	Window win;
	GLDrawContext ctx;

	memset(&ctx, 0x00, sizeof(ctx));
	ctx.glslversion = 120;

	ret = parseOption(argc, argv, &ctx);
	if(ret != 0){
		showHelp();
		return 1;
	}

	if(ctx.glslversion != 150 &&
	   ctx.glslversion != 120){
		showHelp();
		return 1;
	}

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

	initShader(&ctx);

	glClearColor( 0.5, 0.5, 0.5, 1.0 );

	XMapWindow( dpy, win );

	event_loop( dpy, win, &ctx );

	destroyProgramShader(ctx.programId);

	return 0;
}
