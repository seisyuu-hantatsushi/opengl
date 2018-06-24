
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
	GLuint programShader;
	GLuint programId;
} GLProgramShaderSet;

typedef struct GLAppContext {
	GLuint shaderProgram;
} GLAppContext;

typedef struct AppContext {

	guint gktVersion[3];

	GtkWidget *topWindow;
	GtkWidget *glArea;

	GLAppContext glAppCtx;

	GLProgramShaderSet programSet;

} AppContext;


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

	readsize = fread(pShaderCode, stbuf.st_size, sizeof(char), fp);
	if(readsize < stbuf.st_size){
		int lastError = errno;
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
	return 0;
}

static int32_t destroyProgramShader(GLProgramShaderSet *pProgramSet){
	return 0;
}

static void on_realize(GtkGLArea *area, gpointer user_data){
	AppContext *appCtx = (AppContext *)user_data;
	GdkGLContext *glCtx;
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


	free(pShaderCode);
	return;
}

static void on_unrealize(GtkGLArea *area, gpointer user_data){
	printf("on_unrealize\n");
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx, gpointer user_data){

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
