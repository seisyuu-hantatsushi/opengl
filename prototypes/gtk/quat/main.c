#include <stdio.h>

#include <GL/glew.h>
#include <gtk/gtk.h>

typedef struct AppContext {
	GtkWidget *topWindow;
	GtkWidget *glArea;
} AppContext;

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

static void on_realize(GtkGLArea *area, gpointer user_data){
	const GLubyte *renderName,*versionStr;

	//printf("on_realize\n");

	// Make current
	gtk_gl_area_make_current(area);

	glewInit();

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

	return;
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *ctx){
	printf("on_render\n");
	return TRUE;
}

static void on_reize(GtkGLArea *area, gint width, gint height, gpointer user_data){
	printf("on_reize. (%d,%d)\n", width, height);
	return;
}

static GtkWidget* createGLArea(){
	GtkWidget *glArea = NULL;
	glArea = gtk_gl_area_new();

	g_signal_connect(glArea, "realize", G_CALLBACK(on_realize), NULL);
	g_signal_connect(glArea, "render", G_CALLBACK(on_render), NULL);
	g_signal_connect(glArea, "resize", G_CALLBACK(on_reize), NULL);

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

	g_signal_connect(appCtx.topWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(appCtx.topWindow);

	gtk_main();

	return 0;
}
