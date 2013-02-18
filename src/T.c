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

static int window_count;

/* Event callbacks */
static gboolean window_delete_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data);
static void window_destroy_callback(GtkWidget *widget, gpointer data);
static void terminal_child_exited_callback(VteTerminal *terminal, gpointer data);
static void terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data);

/* Helper setup functions */
static void setup_pty(VteTerminal *terminal, GPid *child_pid);
static void set_app_preferences(VteTerminal *terminal);
static void setup_terminal(GtkWindow *window, VteTerminal *terminal);
static void setup_window(GtkWindow *window);
static void spawn_window(void);

static gboolean
window_delete_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    /* TODO: always false? */
    return FALSE;
}

static void
window_destroy_callback(GtkWidget *widget, gpointer data)
{
    window_count--;

    if (window_count == 0)
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
        /*
         * Before, this was:
         * vte_terminal_copy_clipboard(terminal);
         * break;
         * But now we just turn this event into a Control+Insert and delegate it
         */
        key_event->keyval = GDK_KEY_Insert;
        key_event->state &= ~GDK_SHIFT_MASK;
        return FALSE;
    case GDK_KEY_V:
    case GDK_KEY_v:
        /*
         * Before, this was:
         * vte_terminal_paste_clipboard(terminal);
         * break;
         * But now we just turn this event into a Shift+Insert and delegate it
         */
        key_event->keyval = GDK_KEY_Insert;
        key_event->state &= ~GDK_CONTROL_MASK;
        return FALSE;
    case GDK_KEY_N:
    case GDK_KEY_n:
        spawn_window();
        break;
    }

    return TRUE;
}

static void
terminal_child_exited_callback(VteTerminal *terminal, gpointer data)
{
    GtkWindow *window;

    window = (GtkWindow *) data;
    gtk_widget_destroy(GTK_WIDGET(window));
}

static void
terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data)
{
    GtkWindow *window;

    window = (GtkWindow *) data;
    gtk_window_set_title(window, vte_terminal_get_window_title(terminal));
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
     * Options set here can (and should) be configured through config.h
     */

    /*
     * These are static because this function is called every time a new window
     * is created and we don't want to parse the colors all over again
     */
    static gboolean colors_parsed = FALSE;
    static GdkColor palette[CONFIG_PALETTE_SIZE], bg_color, fg_color;

    if (!colors_parsed) {
        int i;

        gdk_color_parse(CONFIG_FOREGROUND_COLOR, &fg_color);
        gdk_color_parse(CONFIG_BACKGROUND_COLOR, &bg_color);
        for (i = 0; i < CONFIG_PALETTE_SIZE; ++i)
            gdk_color_parse(CONFIG_COLOR_PALETTE[i], &palette[i]);

        colors_parsed = TRUE;
    }

    /* Set preferences */
    vte_terminal_set_audible_bell(terminal, CONFIG_AUDIBLE_BELL);
    vte_terminal_set_colors(terminal, &fg_color, &bg_color, palette, CONFIG_PALETTE_SIZE);
    vte_terminal_set_font_from_string(terminal, CONFIG_FONT_NAME);
    vte_terminal_set_mouse_autohide(terminal, CONFIG_MOUSE_AUTOHIDE);
    vte_terminal_set_scrollback_lines(terminal, CONFIG_SCROLLBACK_LINES);
    vte_terminal_set_visible_bell(terminal, CONFIG_VISIBLE_BELL);
}

static void
setup_terminal(GtkWindow *window, VteTerminal *terminal)
{
    GPid child_pid;

    setup_pty(terminal, &child_pid);
    vte_terminal_watch_child(terminal, child_pid);

    g_signal_connect(terminal, "child-exited", G_CALLBACK(terminal_child_exited_callback), window);
    g_signal_connect(terminal, "window-title-changed", G_CALLBACK(terminal_window_title_changed_callback), window);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(terminal_key_press_callback), window);

    set_app_preferences(terminal);
}

static void
setup_window(GtkWindow *window)
{
    GdkGeometry hints;
    VteTerminal *terminal;

    terminal = (VteTerminal *) vte_terminal_new();
    setup_terminal(window, terminal);

    hints.base_width  = vte_terminal_get_char_width(terminal);
    hints.base_height = vte_terminal_get_char_height(terminal);
    hints.min_width   = hints.base_width * CONFIG_MIN_WIDTH;
    hints.min_height  = hints.base_height * CONFIG_MIN_HEIGHT;
    hints.width_inc   = hints.base_width;
    hints.height_inc  = hints.base_height;
    gtk_window_set_geometry_hints(window, GTK_WIDGET(terminal), &hints, GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

    g_signal_connect(window, "delete-event", G_CALLBACK(window_delete_event_callback), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(window_destroy_callback), NULL);
    gtk_window_set_icon_name(window, "utilities-terminal");

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(terminal));

    gtk_widget_show(GTK_WIDGET(terminal));
    gtk_widget_show(GTK_WIDGET(window));
}

static void
spawn_window(void)
{
    GtkWindow *window;

    window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);
    setup_window(window);

    window_count++;
}

int
main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    /* Initial state: single window */
    window_count = 0;
    spawn_window();

    /*
     * TODO
     * Find out a nice and clean way of opening more windows when the user
     * does not have focus on a T window. Maybe fire spawn_window() when we
     * detect a running T process?
     */

    gtk_main();

    return 0;
}
