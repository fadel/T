/*
 * T.c
 *
 * T is a lean Terminal emulator.
 */

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <vte/vte.h>

#include "config.h"

static GtkWindow *main_window;

/* Event callbacks */
static gboolean window_delete_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data);
static void window_destroy_callback(GtkWidget *widget, gpointer data);
static void terminal_child_exited_callback(VteTerminal *terminal, gpointer data);
static void terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data);

/* Helper setup functions */
static void setup_pty(VteTerminal *terminal, GPid *child_pid);
static void set_app_preferences(VteTerminal *terminal);
static void setup_terminal(VteTerminal *terminal);
static void setup_main_window(void);

static gboolean
window_delete_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	/* TODO: always false? */
	return FALSE;
}

static void
window_destroy_callback(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static gboolean
terminal_key_press_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	GdkEventKey *key_event = (GdkEventKey *) event;
	VteTerminal *terminal = (VteTerminal *) widget;

	if ((key_event->state & GDK_SHIFT_MASK) == 0
		|| (key_event->state & GDK_CONTROL_MASK) == 0)
		return FALSE;

	switch (key_event->keyval) {
	case GDK_KEY_C:
	case GDK_KEY_c:
		vte_terminal_copy_clipboard(terminal);
		break;
	case GDK_KEY_V:
	case GDK_KEY_v:
		vte_terminal_paste_clipboard(terminal);
		break;
	case GDK_KEY_N:
	case GDK_KEY_n:
		fork();
		break;
	}

	return TRUE;
}

static void
terminal_child_exited_callback(VteTerminal *terminal, gpointer data)
{
	gtk_main_quit();
}

static void
terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data)
{
	gtk_window_set_title(main_window, vte_terminal_get_window_title(terminal));
}

static void
setup_pty(VteTerminal *terminal, GPid *child_pid)
{
	VtePty* pty;
	char *argv[] = { NULL, NULL };

	/* TODO: error handling */
	argv[0] = vte_get_user_shell();
	pty = vte_terminal_pty_new(terminal, VTE_PTY_DEFAULT, NULL);
	vte_terminal_set_pty_object(terminal, pty);
	vte_terminal_fork_command_full(terminal, VTE_PTY_DEFAULT, ".", argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, child_pid, NULL);
}

static void
set_app_preferences(VteTerminal *terminal)
{
	/*
	 * Options set here can be configured through config.h
	 */

	int i;
	GdkColor palette[PALETTE_SIZE], bg_color, fg_color;

	/* Set preferences */
	vte_terminal_set_mouse_autohide(terminal, MOUSE_AUTOHIDE);
	vte_terminal_set_visible_bell(terminal, VISIBLE_BELL);
	vte_terminal_set_audible_bell(terminal, AUDIBLE_BELL);
	vte_terminal_set_font_from_string(terminal, FONT_NAME);

	gdk_color_parse(FOREGROUND_COLOR, &fg_color);
	gdk_color_parse(BACKGROUND_COLOR, &bg_color);
	for (i = 0; i < PALETTE_SIZE; ++i)
		gdk_color_parse(COLOR_PALETTE[i], &palette[i]);
	vte_terminal_set_colors(terminal, &fg_color, &bg_color, palette, PALETTE_SIZE);
}

static void
setup_terminal(VteTerminal *terminal)
{
	GPid child_pid;

	setup_pty(terminal, &child_pid);
	vte_terminal_watch_child(terminal, child_pid);

	g_signal_connect(terminal, "child-exited", G_CALLBACK(terminal_child_exited_callback), NULL);
	g_signal_connect(terminal, "window-title-changed", G_CALLBACK(terminal_window_title_changed_callback), NULL);
	g_signal_connect(terminal, "key-press-event", G_CALLBACK(terminal_key_press_callback), NULL);

	set_app_preferences(terminal);
}

static void
setup_main_window(void)
{
	GdkGeometry hints;
	VteTerminal *terminal;

	terminal = (VteTerminal *) vte_terminal_new();
	setup_terminal(terminal);

	hints.base_width  = vte_terminal_get_char_width(terminal);
	hints.base_height = vte_terminal_get_char_height(terminal);
	hints.min_width   = hints.base_width * MIN_WIDTH;
	hints.min_height  = hints.base_height * MIN_HEIGHT;
	hints.width_inc   = hints.base_width;
	hints.height_inc  = hints.base_height;
	gtk_window_set_geometry_hints(main_window, GTK_WIDGET(terminal), &hints, GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

	g_signal_connect(main_window, "delete-event", G_CALLBACK(window_delete_event_callback), NULL);
	g_signal_connect(main_window, "destroy", G_CALLBACK(window_destroy_callback), NULL);
	gtk_window_set_icon_name(main_window, "utilities-terminal");

	gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(terminal));

	gtk_widget_show(GTK_WIDGET(terminal));
	gtk_widget_show(GTK_WIDGET(main_window));
}

int
main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	main_window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);
	setup_main_window();
	gtk_main();

	return 0;
}
