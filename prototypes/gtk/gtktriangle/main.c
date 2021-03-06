/*
   (C) TANIGUCHI, Kazushige 2019
   taniguchi.kazushige@gmail.com
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <math.h>

#include <GL/glew.h>
#include <gtk/gtk.h>

#include <errno.h>

#include "matrix4x4.h"

typedef struct GLProgramShaderSet {
	GLuint vertextShader;
	GLuint fragmentShader;
	GLuint programId;
} GLProgramShaderSet;

typedef struct GLAppContext {
	GLProgramShaderSet programSet;
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLint mvpIndex;
	GLint positionIndex;
	GLint colorIndex;

	/* model-view-projection matrix */
	matrix4x4 mvp;

} GLAppContext;

typedef struct AppContext {

	guint gktVersion[3];

	GtkWidget *topWindow;
	GtkWidget *glArea;

	GLAppContext glAppCtx;

} AppContext;

static const GLfloat lightpos[] = { 0.0f, 0.0f, 5.0f, 1.0f }; /* 位置       */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.0f }; /* 直接光強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.0f }; /* 環境光強度 */

static void readGLError(const char *message){
	GLenum error = glGetError();
	if(error != GL_NO_ERROR){
		const GLubyte *errorStr = gluErrorString(error);
		fprintf(stderr, "%s %d:%s\n", message, error, errorStr);
	}
	return;
}

static GtkWidget* createFileMenu(AppContext *pCtx){
	GtkWidget *root,*menu,*menuitem;

	root = gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline(GTK_MENU_ITEM(root), TRUE);

	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(root), menu);

	menuitem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	g_signal_connect(menuitem, "activate", G_CALLBACK(gtk_main_quit), NULL);
	return root;
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

static int32_t createProgramShader(const char *pVertexShaderCode,
								   const char *pFragmentShaderCode,
								   GLProgramShaderSet *pProgramSet){
	int ret = 0;
	GLint compiled;
	GLsizei size,len;

	pProgramSet->vertextShader  = 0;
	pProgramSet->fragmentShader = 0;
	pProgramSet->programId      = 0;

	if(pVertexShaderCode != NULL){
		GLint length = strlen(pVertexShaderCode);
		const GLchar **ppCode = &pVertexShaderCode;
		const GLint  *pLengths = &length;

		pProgramSet->vertextShader = glCreateShader(GL_VERTEX_SHADER);
		if(pProgramSet->vertextShader == 0){
			readGLError("failed to create vertex shader");
			ret = -1;
			goto error_exit;
		}
		glShaderSource(pProgramSet->vertextShader, 1, ppCode, pLengths);
		glCompileShader(pProgramSet->vertextShader);
		glGetShaderiv(pProgramSet->vertextShader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			fprintf(stderr, "failed to compile vertex shader\n");
			glGetProgramiv(pProgramSet->vertextShader, GL_INFO_LOG_LENGTH, &size);
			if(size > 0){
				char *pBuf;
				pBuf = (char *)malloc(size+1);
				glGetShaderInfoLog(pProgramSet->vertextShader, size, &len, pBuf);
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

		pProgramSet->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if(pProgramSet->fragmentShader == 0){
			readGLError("failed to create fragment shader");
			ret = -1;
			goto error_exit;
		}
		glShaderSource(pProgramSet->fragmentShader, 1, ppCode, pLengths);
		glCompileShader(pProgramSet->fragmentShader);
		glGetShaderiv(pProgramSet->fragmentShader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			fprintf(stderr, "failed to compile fragment shader\n");
			glGetProgramiv(pProgramSet->fragmentShader, GL_INFO_LOG_LENGTH, &size);
			if(size > 0){
				char *pBuf;
				pBuf = (char *)malloc(size+1);
				glGetShaderInfoLog(pProgramSet->fragmentShader, size, &len, pBuf);
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

	pProgramSet->programId = glCreateProgram();
	if(pProgramSet->programId == 0){
		fprintf(stderr, "failed to create program.\n");
		goto error_exit;
	}

	if(pProgramSet->vertextShader != 0){
		glAttachShader(pProgramSet->programId, pProgramSet->vertextShader);
	}

	if(pProgramSet->fragmentShader != 0){
		glAttachShader(pProgramSet->programId, pProgramSet->fragmentShader);
	}

	glLinkProgram(pProgramSet->programId);
	{
		GLuint linked;
		glGetProgramiv(pProgramSet->programId, GL_LINK_STATUS, &linked);
		if(linked == GL_FALSE){
			char *pInfoLog;
			fprintf(stderr, "failed to link shader\n");
			glGetProgramiv( pProgramSet->programId, GL_INFO_LOG_LENGTH, &size );
			if ( size > 0 ) {
				pInfoLog = (char *)malloc(size);
				glGetProgramInfoLog( pProgramSet->programId, size, &len, pInfoLog );
				printf("%s\n",pInfoLog);
				free(pInfoLog);
			}
		}
		else {
			fprintf(stderr, "success to link shader to program.\n");
		}
	}

	return ret;

 error_exit:

	if(pProgramSet->fragmentShader != 0){
		glDeleteShader(pProgramSet->fragmentShader);
	}

	if(pProgramSet->vertextShader != 0){
		glDeleteShader(pProgramSet->vertextShader);
	}

	if(pProgramSet->programId != 0){
		glDeleteProgram(pProgramSet->programId);
	}

	return ret;
}

static int32_t destroyProgramShader(GLProgramShaderSet *pProgramSet){
	return 0;
}

struct vertex_info {
  GLfloat position[3];
  GLfloat color[4];
};

static void on_realize(GtkGLArea *area, gpointer user_data){
	AppContext *appCtx = (AppContext *)user_data;
	GLAppContext *glAppCtx = &appCtx->glAppCtx;
	GError *pError;
	GLenum glResult;
	const char *pVertexShaderCode=NULL, *pFragmentShaderCode=NULL;
	size_t readsize;

	/* the vertex data is constant */
	struct vertex_info vertex_data[] = {
		{ {cos(M_PI/2),                  sin(M_PI/2),                  -2.0 }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {cos(M_PI/2+1.0*2.0*M_PI/3.0), sin(M_PI/2+1.0*2.0*M_PI/3.0), -2.0 }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {cos(M_PI/2+2.0*2.0*M_PI/3.0), sin(M_PI/2+2.0*2.0*M_PI/3.0), -2.0 }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	printf("on_realize\n");

	// Make current
	gtk_gl_area_make_current(area);

	glewExperimental = GL_TRUE;
	glResult = glewInit();
	if(glResult != GLEW_OK){
		printf("Error: %s\n", glewGetErrorString(glResult));
		return;
	}

	printf("Renderer: %s\n",       glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n",   glGetString(GL_SHADING_LANGUAGE_VERSION));

	pVertexShaderCode   = readShaderCode("./simple.vert");
	pFragmentShaderCode = readShaderCode("./simple.frag");

	createProgramShader(pVertexShaderCode, pFragmentShaderCode, &glAppCtx->programSet);

	glAppCtx->mvpIndex      = glGetUniformLocation(glAppCtx->programSet.programId, "mvp");
	glAppCtx->positionIndex = glGetAttribLocation(glAppCtx->programSet.programId,  "position");
	glAppCtx->colorIndex    = glGetAttribLocation(glAppCtx->programSet.programId,  "color");

	glGenVertexArrays(1, &glAppCtx->vertexArrayObject);
	glBindVertexArray(glAppCtx->vertexArrayObject);

	glGenBuffers(1, &glAppCtx->vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, glAppCtx->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glAppCtx->positionIndex);
	glVertexAttribPointer(glAppCtx->positionIndex, 3, GL_FLOAT, GL_FALSE,
						  sizeof(struct vertex_info),
						  (GLvoid*)(G_STRUCT_OFFSET(struct vertex_info, position)));

	glEnableVertexAttribArray(glAppCtx->colorIndex);
	glVertexAttribPointer(glAppCtx->colorIndex, 4, GL_FLOAT, GL_FALSE,
						  sizeof(struct vertex_info),
						  (GLvoid*)(G_STRUCT_OFFSET(struct vertex_info, color)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glDeleteBuffers(1, &glAppCtx->vertexBufferObject);

	/* 初期設定 */
	glClearColor(0.3, 0.3, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	/* 光源の初期設定 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	/* Open GLはカメラの初期位置は原点にあり, -z方向を向いていて, 上はy方向. */

	free(pFragmentShaderCode);
	free(pVertexShaderCode);
	return;
}

static void on_unrealize(GtkGLArea *area, gpointer user_data){
	printf("on_unrealize\n");
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx, gpointer user_data){
	AppContext *appCtx = (AppContext *)(user_data);
	GLAppContext *glAppCtx = &appCtx->glAppCtx;

	printf("on_render\n");

	/* 画面クリア */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 光源の位置を設定 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glUseProgram(glAppCtx->programSet.programId);
	glUniformMatrix4fv(glAppCtx->mvpIndex, 1, GL_FALSE, &(glAppCtx->mvp[0]));

	glBindVertexArray(glAppCtx->vertexArrayObject);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);
	glFlush();

	return TRUE;
}

static void on_resize(GtkGLArea *area, gint width, gint height, gpointer user_data){
	AppContext *appCtx =
		(AppContext *)(user_data);
	GLAppContext *glAppCtx = &appCtx->glAppCtx;

	printf("on_resize\n");
	glViewport(0, 0, width, height);

	/* Model View Project Matrixを計算 */
	perspectiveProjectMatrix(glAppCtx->mvp, 45.0, (float)width/(float)height, 1.0f, 10.0f);

	return;
}

static void glAppContextInit(GLAppContext *glAppCtx){

	return;
}

static GtkWidget* createGLArea(AppContext *appCtx){
	GtkWidget *glArea = NULL;

	glArea = gtk_gl_area_new();

	g_signal_connect(glArea, "realize",   G_CALLBACK(on_realize),   appCtx);
	g_signal_connect(glArea, "unrealize", G_CALLBACK(on_unrealize), appCtx);
	g_signal_connect(glArea, "render",    G_CALLBACK(on_render),    appCtx);
	g_signal_connect(glArea, "resize",    G_CALLBACK(on_resize),    appCtx);

	return glArea;
}

int main(int argc, char **argv){

	AppContext appCtx;

	GtkWidget *menubar;
	GtkWidget *vbox,*hbox;

	gtk_init(&argc, &argv);

	appCtx.gktVersion[0] = gtk_get_major_version();
	appCtx.gktVersion[1] = gtk_get_minor_version();
	appCtx.gktVersion[2] = gtk_get_micro_version();
	printf("gtk version: %u.%u.%u\n",
		   appCtx.gktVersion[0],appCtx.gktVersion[1],appCtx.gktVersion[2]);
	appCtx.topWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(appCtx.topWindow), "Triangle");
	gtk_widget_set_size_request(appCtx.topWindow, 300, 300);
	gtk_container_set_border_width(GTK_CONTAINER(appCtx.topWindow), 2);

	appCtx.glArea = createGLArea(&appCtx);
	gtk_container_add(GTK_CONTAINER(appCtx.topWindow),appCtx.glArea);

	g_signal_connect(appCtx.topWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(appCtx.topWindow);

	gtk_main();

	return 0;
}
