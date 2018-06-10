#include <stdbool.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gtk/gtk.h>

typedef struct GLAppContext {
	// The Vertex Array Object
	GLuint vao;
	GLuint shaderProgram;
	struct {
		GLuint modelViewProjectionMatrix;
		GLuint normalMatrix;
		GLuint lightSourcePosition;
		GLuint materialColor;
	} location;
} GLAppContext;

typedef struct AppContext {

	guint gktVersion[3];

	GtkWidget *topWindow;
	GtkWidget *glArea;

	GLAppContext glAppCtx;

} AppContext;

static const char vertex_shader[] =
"#version 150\n"
"\n"
"in vec3 position;\n"
"in vec3 normal;\n"
"\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"uniform mat4 NormalMatrix;\n"
"uniform vec4 LightSourcePosition;\n"
"uniform vec4 MaterialColor;\n"
"\n"
"smooth out vec4 Color;\n"
"\n"
"void main(void)\n"
"{\n"
"    // Transform the normal to eye coordinates\n"
"    vec3 N = normalize(vec3(NormalMatrix * vec4(normal, 1.0)));\n"
"\n"
"    // The LightSourcePosition is actually its direction for directional light\n"
"    vec3 L = normalize(LightSourcePosition.xyz);\n"
"\n"
"    // Multiply the diffuse value by the vertex color (which is fixed in this case)\n"
"    // to get the actual color that we will use to draw this vertex with\n"
"    float diffuse = max(dot(N, L), 0.0);\n"
"    Color = diffuse * MaterialColor;\n"
"\n"
"    // Transform the position to clip coordinates\n"
"    gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);\n"
"}";

static const char fragment_shader[] =
"#version 150\n"
"\n"
"smooth in vec4 Color;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = Color;\n"
"}";

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

static void on_realize(GtkGLArea *area, gpointer user_data){
	AppContext *appCtx = (AppContext *)user_data;
	GdkGLContext *glCtx;
	GError *pError;

	printf("on_realize\n");

	// Make current
	gtk_gl_area_make_current(area);

	pError = gtk_gl_area_get_error(area);
	if(pError != NULL){
		fprintf(stderr, "Failed to make current. %s\n", pError->message);
		return;
	}
	glCtx = gtk_gl_area_get_context(area);

	glewExperimental = GL_TRUE;
	glewInit();

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	glEnable(GL_CULL_FACE);
	readGLError("Failed to enable GL_CULL_FACE");
	glEnable(GL_DEPTH_TEST);
	readGLError("Failed to enable GL_DEPTH_TEST");

	{
		GLuint vao;
		glGenVertexArrays (1, &vao);
		readGLError("Failed to generate vertex arrays");
		glBindVertexArray(vao);
		readGLError("Failed to bind vertex arrays");
		appCtx->glAppCtx.vao = vao;
	}

	{
		char msg[512];
		GLuint vertexShader,fragmentShader;
		GLuint program;
		const GLchar *shader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if(vertexShader != 0){
			readGLError("Failed to create vertex shader");
		}
		shader = vertex_shader;
		glShaderSource(vertexShader, 1, &shader, NULL);
		glCompileShader(vertexShader);
		glGetShaderInfoLog(vertexShader, sizeof msg, NULL, msg);
		if(msg[0] != '\0'){
			g_print ("vertex shader info: %s\n", msg);
		}

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if(fragmentShader != 0){
			readGLError("Failed to create framgment shader");
		}
		shader = fragment_shader;
		glShaderSource(fragmentShader, 1, &shader, NULL);
		glCompileShader(fragmentShader);
		glGetShaderInfoLog(fragmentShader, sizeof msg, NULL, msg);
		if(msg[0] != '\0'){
			g_print ("fragment shader info: %s\n", msg);
			msg[0] = '\0';
		}

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glBindAttribLocation(program, 0, "position");
		glBindAttribLocation(program, 1, "normal");
		glLinkProgram(program);
		glGetProgramInfoLog(program, sizeof(msg), NULL, msg);
		if(msg[0] != '\0'){
			g_print ("program info: %s\n", msg);
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		appCtx->glAppCtx.shaderProgram = program;
		appCtx->glAppCtx.location.modelViewProjectionMatrix =
			glGetUniformLocation(program, "ModelViewProjectionMatrix");
		readGLError("Failed to get ModelViewProjectionMatrix location");
		appCtx->glAppCtx.location.normalMatrix =
			glGetUniformLocation(program, "NormalMatrix");
		readGLError("Failed to get NormalMatrix location");
		
	}

	return;
}

static void on_unrealize(GtkWidget *area, gpointer user_data){
	printf("on_unrealize\n");
	return;
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx){

	//printf("on_render\n");

	return TRUE;
}

static void on_resize(GtkGLArea *area, gint width, gint height, gpointer user_data){
	printf("on_resize\n");
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
}
