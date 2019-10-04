#ifndef WINDOW_H
#define WINDOW_H
#include <gtk/gtk.h>

int open_window(int argc, char **argv );
static void on_button_clicked(GtkButton *btn, gpointer data);
static void on_app_activate(GApplication *app, gpointer data);

#endif
