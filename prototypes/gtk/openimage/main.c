#include <stdio.h>
#include <gtk/gtk.h>

typedef struct AppContext {
	GtkWidget *topWindow;
	GtkWidget *imageWindow;
} AppContext;

static void openFile(GtkWidget *menuitem, gpointer user_data){
	AppContext *pCtx = (AppContext *)user_data;
	GtkWidget *dialog;
	gint ret;

	dialog = gtk_file_chooser_dialog_new("Open File",
										 GTK_WINDOW(pCtx->topWindow),
										 GTK_FILE_CHOOSER_ACTION_OPEN,
										 ("_Cancel"),
										 GTK_RESPONSE_CANCEL,
										 ("_Open"),
										 GTK_RESPONSE_ACCEPT,
										 NULL);

	ret = gtk_dialog_run(GTK_DIALOG(dialog));
	if(ret == GTK_RESPONSE_ACCEPT){
		char *filepath;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		filepath = gtk_file_chooser_get_filename(chooser);
		printf("open file: %s\n", filepath);
		gtk_image_set_from_file(GTK_IMAGE(pCtx->imageWindow),filepath);
		g_free(filepath);
	}

	gtk_widget_destroy(dialog);

	return;
}

static GtkWidget* createFileMenu(AppContext *pCtx){
	GtkWidget *root,*menu,*menuitem;

	root = gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline(GTK_MENU_ITEM(root), TRUE);

	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(root), menu);

	menuitem = gtk_menu_item_new_with_label("Open");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	g_signal_connect(menuitem, "activate", G_CALLBACK(openFile), pCtx);

	menuitem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	g_signal_connect(menuitem, "activate", G_CALLBACK(gtk_main_quit), NULL);

	return root;
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

	gtk_box_pack_start(GTK_BOX(hbox), gtk_button_new_with_label("Quit"), TRUE, FALSE, 0);

	appCtx.imageWindow = gtk_image_new();
	gtk_box_pack_start(GTK_BOX(hbox), appCtx.imageWindow, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE,  0);

	g_signal_connect(appCtx.topWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(appCtx.topWindow);

	gtk_main();

	return 0;
}
