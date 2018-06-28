
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

typedef struct GLProgramShaderSet {
	GLuint vertextShader;
	GLuint fragmentShader;
	GLuint programId;
} GLProgramShaderSet;

typedef struct GLAppContext {
	GLProgramShaderSet programSet;
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

static int32_t creareProgramShader(const char *pVertexShaderCode,
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
			fprintf(stderr, "failed to compile\n");
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

static void on_realize(GtkGLArea *area, gpointer user_data){
	AppContext *appCtx = (AppContext *)user_data;
	GLAppContext *glAppCtx = &appCtx->glAppCtx;
	GError *pError;
	GLenum glResult;
	const char *pShaderCode;
	size_t readsize;

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

	pShaderCode = readShaderCode("./triangle.vert");

	printf("%s\n", pShaderCode);

	creareProgramShader(pShaderCode, NULL, &glAppCtx->programSet);

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

	free(pShaderCode);
	return;
}

static void on_unrealize(GtkGLArea *area, gpointer user_data){
	printf("on_unrealize\n");
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx, gpointer user_data){
	AppContext *appCtx = (AppContext *)(user_data);
	GLAppContext *glAppCtx = &appCtx->glAppCtx;

	printf("on_render\n");

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(glAppCtx->programSet.programId);
	glBegin(GL_TRIANGLES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex2d(0.9*cos(0.0), 0.9*sin(0.0));
	glColor3d(0.0, 1.0, 0.0);
	glVertex2d(0.9*cos(2.0*M_PI/3.0), 0.9*sin(2.0*M_PI/3.0));
	glColor3d(0.0, 0.0, 1.0);
	glVertex2d(0.9*cos(2.0*M_PI*2.0/3.0), 0.9*sin(2.0*M_PI*2.0/3.0));
	glEnd();
	glUseProgram(0);
	glFlush();

	return TRUE;
}

static void on_resize(GtkGLArea *area, gint width, gint height, gpointer user_data){
	AppContext *appCtx =
		(AppContext *)(user_data);
	printf("on_resize\n");

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

	gtk_window_set_title(GTK_WINDOW(appCtx.topWindow), "Gears");
	gtk_widget_set_size_request(appCtx.topWindow, 300, 200);
	gtk_container_set_border_width(GTK_CONTAINER(appCtx.topWindow), 2);

	appCtx.glArea = createGLArea(&appCtx);
	gtk_container_add(GTK_CONTAINER(appCtx.topWindow),appCtx.glArea);

	g_signal_connect(appCtx.topWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(appCtx.topWindow);

	gtk_main();

	return 0;
}
