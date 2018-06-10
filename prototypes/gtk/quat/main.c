#include <stdbool.h>
#include <stdio.h>

#include <GL/glew.h>
#include <gtk/gtk.h>

typedef struct AppContext {
	GtkWidget *topWindow;
	GtkWidget *glArea;
} AppContext;

static const GLfloat whiteColor[] = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat blackColor[] = { 0.0, 0.0, 0.0, 1.0 };
static const GLfloat blueColor[]  = { 0.0, 0.0, 1.0, 1.0 };

static void cb_button_clicked (GtkWidget *button, gpointer user_data)
{
	gtk_main_quit();
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

static void draw_frame(void){
	const float L=0.5;
	static GLfloat vdata[8][3] = {
		{-L,-L,-L},{ L,-L,-L},{ L, L,-L},{-L, L, -L},
		{-L,-L, L},{ L,-L, L},{ L, L, L},{-L, L,  L}
	};
	static GLint tindices[6][4] = {
		{0,1,2,3},{0,4,5,1},{0,3,7,4},
		{1,5,6,2},{2,6,7,3},{5,4,7,6}
	};

	int i;

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blueColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blackColor);

	for(i=0;i<3;i++){
		glBegin(GL_QUADS);
		{
			glNormal3fv(&vdata[tindices[i][0]][0]);
			glVertex3fv(&vdata[tindices[i][0]][0]);
			glNormal3fv(&vdata[tindices[i][1]][0]);
			glVertex3fv(&vdata[tindices[i][1]][0]);
			glNormal3fv(&vdata[tindices[i][2]][0]);
			glVertex3fv(&vdata[tindices[i][2]][0]);
			glNormal3fv(&vdata[tindices[i][3]][0]);
			glVertex3fv(&vdata[tindices[i][3]][0]);
		}
		glEnd();
	}
	return;
}

static void draw_objects(void){
	const float X = 0.525731112119133606/2.0;
	const float Z = 0.850650808352039932/2.0;
	static GLfloat vdata[12][3] = {
		{  -X, 0.0,   Z}, {   X, 0.0,   Z}, {  -X, 0.0,  -Z},
		{   X, 0.0,  -Z}, { 0.0,   Z,   X}, { 0.0,   Z,  -X},
		{ 0.0,  -Z,   X}, { 0.0,  -Z,  -X}, {   Z,   X, 0.0},
		{  -Z,   X, 0.0}, {   Z,  -X, 0.0}, {  -Z,  -X, 0.0}
	};
	static GLint tindices[20][3] = {
		{  0,  4,  1}, {  0,  9,  4}, {  9,  5,  4}, {  4,  5,  8},
		{  4,  8,  1}, {  8, 10,  1}, {  8,  3, 10}, {  5,  3,  8},
		{  5,  2,  3}, {  2,  7,  3}, {  7, 10,  3}, {  7,  6, 10},
		{  7, 11,  6}, { 11,  0,  6}, {  0,  1,  6}, {  6,  1, 10},
		{  9,  0, 11}, {  9, 11,  2}, {  9,  2,  5}, {  7,  2, 11}
	};
	int i;

	glEnable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, whiteColor);
	for(i=0; i<20; i++){
		glBegin(GL_TRIANGLES);
		{
			glNormal3fv(&vdata[tindices[i][0]][0]);
			glVertex3fv(&vdata[tindices[i][0]][0]);
			glNormal3fv(&vdata[tindices[i][1]][0]);
			glVertex3fv(&vdata[tindices[i][1]][0]);
			glNormal3fv(&vdata[tindices[i][2]][0]);
			glVertex3fv(&vdata[tindices[i][2]][0]);
		}
		glEnd();
	}
	glDisable(GL_CULL_FACE);
	return;
}

static void draw(){
	static bool bFirstCall = true;
	static GLint displayList = 0;

	if(bFirstCall){
		bFirstCall = false;
		displayList = glGenLists(1);
		if( displayList == 0 ){
			GLenum error = glGetError();
			const GLubyte *errorStr = gluErrorString(error);
			printf("glGenLists() error: %d %s\n",
				   error, (const char*)errorStr);
		}
		glNewList(displayList, GL_COMPILE_AND_EXECUTE);
		draw_frame();
		draw_objects();
		glEndList();
	}
	else{
		glCallList(displayList);
	}

	return;
}

static void on_realize(GtkGLArea *area, gpointer user_data){
	const GLubyte *renderName,*versionStr;
	GdkGLContext  *glCtx;
	GdkWindow     *glWindow;
	GdkFrameClock *frameClock;
	const GLfloat light1[] = { 0.0, 0.0, 10.0, 1.0 };
	//printf("on_realize\n");

	// Make current
	gtk_gl_area_make_current(area);

	//glewInit();

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	gtk_gl_area_set_has_depth_buffer(area,TRUE);

	glClearColor(0.0, 0.0, 0.0, 1.0); // background color

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, blackColor);
	glEnable(GL_LIGHT0);

	//glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteColor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1);
	glEnable(GL_LIGHT1);

	glCtx      = gtk_gl_area_get_context(area);
	glWindow   = gdk_gl_context_get_window(glCtx);
	frameClock = gdk_window_get_frame_clock(glWindow);

	{
		int major,minor;
		gdk_gl_context_get_required_version(glCtx, &major, &minor);

		printf("version %d.%d\n", major, minor);
	}
	
	g_signal_connect_swapped( frameClock,
							  "update",
							  G_CALLBACK(gtk_gl_area_queue_render),
							  area);
	gdk_frame_clock_begin_updating(frameClock);

	return;
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx){

	//printf("on_render\n");

	//gtk_gl_area_make_current(area);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0, 1.0, 0.0, 1.0); // background color

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	draw();

	return TRUE;
}

static void on_resize(GtkGLArea *area, gint width, gint height, gpointer user_data){

	static float FOVY=20.0;

	printf("on_resize. (%d,%d)\n", width, height);

	glClearColor(1.0, 0.0, 0.0, 1.0); // background color

	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (GLfloat)width/(GLfloat)height, 2.0, -2.0);
	glMatrixMode(GL_MODELVIEW);

	return;
}

static GtkWidget* createGLArea(){
	GtkWidget *glArea = NULL;
	int major,minor;
	glArea = gtk_gl_area_new();

	g_signal_connect(glArea, "realize", G_CALLBACK(on_realize), NULL);
	g_signal_connect(glArea, "render", G_CALLBACK(on_render), NULL);
	g_signal_connect(glArea, "resize", G_CALLBACK(on_resize), NULL);

	return glArea;
}

int main(int argc, char **argv){
	AppContext appCtx;

	GtkWidget *menubar;
	GtkWidget *vbox,*hbox;
	gtk_init(&argc, &argv);

	appCtx.topWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(appCtx.topWindow), "Viewer");
	gtk_widget_set_size_request(appCtx.topWindow, 300, 200);
	gtk_container_set_border_width(GTK_CONTAINER(appCtx.topWindow), 2);

#if 0
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(appCtx.topWindow), vbox);

	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, TRUE, 0);

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), createFileMenu(&appCtx));

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), gtk_button_new_with_label("Quit"), FALSE, FALSE, 0);

	appCtx.glArea = createGLArea();

	gtk_box_pack_start(GTK_BOX(hbox), appCtx.glArea, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE,  0);
#else
	appCtx.glArea = createGLArea();
	gtk_container_add(GTK_CONTAINER(appCtx.topWindow),appCtx.glArea);
#endif
	g_signal_connect(appCtx.topWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(appCtx.topWindow);

	gtk_main();

	return 0;
}
