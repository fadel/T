/*
 * T.c
 * T is a lean Terminal emulator.
 *
 * This file is part of T.
 *
 * T is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * T is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * T. If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define PROGRAM_NAME "T"

#define SOCKPATH CONFIG_SOCKDIR "/" CONFIG_SOCKNAME
#define SOCKPATH_MAXLEN sizeof((SOCKPATH))

#define MSGBUF_MAXLEN 64

#define ARG_DAEMON "-d"
#define ARG_NEWWIN "-n"
#define ARG_EXIT   "-x"

typedef enum {
    MSG_NEWWIN,
    MSG_EXIT,
    MSG_OK,
    MSG_ERROR
} TMessage;

static int window_count = 0;

/* UI event callbacks */
static void window_destroy_callback(GtkWidget *widget, gpointer data);
static gboolean key_press_callback(GtkWidget *widget, GdkEvent *event, gpointer data);
static void terminal_child_exited_callback(VteTerminal *terminal, gpointer data);
static void terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data);

/* UI helper functions */
static void change_font_size(VteTerminal *terminal, int delta);
static void set_preferences(VteTerminal *terminal);
static void setup_terminal(GtkWindow *window, VteTerminal *terminal);

/* (local) Networking & utility functions */
static int num_open_windows();
static void new_window();
static void wrn(const char *message);
static void err(const char *message, int ecode);
static int conn_socket();
static int bind_socket();
static void cleanup_socket(int sock_fd);
static TMessage handle_message(TMessage msg);
static gboolean socket_g_io_watch(GIOChannel *source, GIOCondition condition, gpointer data);
static int send_message(TMessage msg);
static int run_daemon();
static void usage();

static void
window_destroy_callback(GtkWidget *widget, gpointer data)
{
    /* Keeping track of the open windows */
    window_count--;
}

static gboolean
key_press_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GdkEventKey *key_event = (GdkEventKey *) event;
    VteTerminal *terminal = VTE_TERMINAL(widget);

    gboolean has_shift = (key_event->state & GDK_SHIFT_MASK) != 0;
    gboolean has_control = (key_event->state & GDK_CONTROL_MASK) != 0;

    if (!has_control)
        return FALSE;

    switch (key_event->keyval) {
    case GDK_KEY_minus:
        change_font_size(terminal, -1);
        return TRUE;
    case GDK_KEY_plus:
        change_font_size(terminal, 1);
        return TRUE;
    }

    if (!has_shift)
        return FALSE;

    /* Handles ctrl + shift + {c,v,n} */
    switch (key_event->keyval) {
    case GDK_KEY_C:
    case GDK_KEY_c:
        /* turn this event into a Control+Insert and delegate it */
        key_event->keyval = GDK_KEY_Insert;
        key_event->state &= ~GDK_SHIFT_MASK;
        break;
    case GDK_KEY_V:
    case GDK_KEY_v:
        /* turn this event into a Shift+Insert and delegate it */
        key_event->keyval = GDK_KEY_Insert;
        key_event->state &= ~GDK_CONTROL_MASK;
        break;
    case GDK_KEY_N:
    case GDK_KEY_n:
        new_window();
        return TRUE;
    }

    return FALSE;
}

static void
terminal_child_exited_callback(VteTerminal *terminal, gpointer data)
{
    /* The forked process exited; issue destruction of the terminal window */
    gtk_widget_destroy(GTK_WIDGET(GTK_WINDOW(data)));
}

static void
terminal_window_title_changed_callback(VteTerminal *terminal, gpointer data)
{
    GtkWindow *window = GTK_WINDOW(data);
    gtk_window_set_title(window, vte_terminal_get_window_title(terminal));
}

static void
change_font_size(VteTerminal *terminal, int delta)
{
    PangoFontDescription *desc = (PangoFontDescription *) vte_terminal_get_font(terminal);
    gint font_size = pango_font_description_get_size(desc) + delta * PANGO_SCALE;
    pango_font_description_set_size(desc, font_size);
    vte_terminal_set_font(terminal, desc);
}

static void
set_preferences(VteTerminal *terminal)
{
    /*
     * Options set here can (and should) be configured in config.h
     */

    /*
     * These are static because this function is called every time a new window
     * is created and we don't want to parse the colors all over again
     */
    static gboolean colors_parsed = FALSE;
    static GdkColor palette[CONFIG_PALETTE_SIZE], bg_color, fg_color;
    static const char *CONFIG_COLOR_PALETTE[CONFIG_PALETTE_SIZE] = {
        CONFIG_PALLETE_0,
        CONFIG_PALLETE_1,
        CONFIG_PALLETE_2,
        CONFIG_PALLETE_3,
        CONFIG_PALLETE_4,
        CONFIG_PALLETE_5,
        CONFIG_PALLETE_6,
        CONFIG_PALLETE_7,
        CONFIG_PALLETE_8,
        CONFIG_PALLETE_9,
        CONFIG_PALLETE_10,
        CONFIG_PALLETE_11,
        CONFIG_PALLETE_12,
        CONFIG_PALLETE_13,
        CONFIG_PALLETE_14,
        CONFIG_PALLETE_15,
    };

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
    vte_terminal_set_cursor_blink_mode(terminal, CONFIG_CURSOR_BLINK_MODE);
    vte_terminal_set_font_from_string(terminal, CONFIG_FONT_NAME);
    vte_terminal_set_mouse_autohide(terminal, CONFIG_MOUSE_AUTOHIDE);
    vte_terminal_set_scrollback_lines(terminal, CONFIG_SCROLLBACK_LINES);
    vte_terminal_set_visible_bell(terminal, CONFIG_VISIBLE_BELL);
}

static void
setup_terminal(GtkWindow *window, VteTerminal *terminal)
{
    char *argv[] = { vte_get_user_shell(), NULL };
    GPid child_pid;

    vte_terminal_fork_command_full(terminal,
            VTE_PTY_DEFAULT,
            NULL, /* wd; NULL for cwd */
            argv, /* the program to fork into and its args */
            NULL, /* env vars */
            G_SPAWN_DO_NOT_REAP_CHILD,
            NULL, /* setup func */
            NULL, /* custom data to setup func */
            &child_pid,
            NULL); /* TODO: error handling */

    g_signal_connect(terminal, "child-exited", G_CALLBACK(terminal_child_exited_callback), window);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(key_press_callback), window);
    g_signal_connect(terminal, "window-title-changed", G_CALLBACK(terminal_window_title_changed_callback), window);

    set_preferences(terminal);
}

static int
num_open_windows()
{
    return window_count;
}

static void
new_window()
{
    GtkWindow *window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);
    VteTerminal *terminal = (VteTerminal *) vte_terminal_new();
    setup_terminal(window, terminal);

    GdkGeometry hints;
    hints.base_width  = vte_terminal_get_char_width(terminal);
    hints.base_height = vte_terminal_get_char_height(terminal);
    hints.min_width   = hints.base_width * CONFIG_MIN_WIDTH;
    hints.min_height  = hints.base_height * CONFIG_MIN_HEIGHT;
    hints.width_inc   = hints.base_width;
    hints.height_inc  = hints.base_height;
    gtk_window_set_geometry_hints(window, GTK_WIDGET(terminal), &hints, GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);
    gtk_window_set_icon_name(window, "utilities-terminal");

    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_false), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(window_destroy_callback), NULL);

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(terminal));

    gtk_widget_show(GTK_WIDGET(terminal));
    gtk_widget_show(GTK_WIDGET(window));

    window_count++;
}

static void
wrn(const char *message)
{
    fprintf(stderr, "%s: %s\n", PROGRAM_NAME, message);
}

static void
err(const char *message, int ecode)
{
    wrn(message);
    exit(ecode);
}

static int
conn_socket()
{
    struct sockaddr_un sock_addr;

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0)
        err("Failed to create socket", EXIT_FAILURE);

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, SOCKPATH, sizeof(sock_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
        err("Failed to connect to daemon", EXIT_FAILURE);

    return sock_fd;
}

static int
bind_socket()
{
    struct sockaddr_un sock_addr;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, SOCKPATH, sizeof(sock_addr.sun_path) - 1);

    /* Remove old socket */
    if (!access(sock_addr.sun_path, F_OK) && unlink(sock_addr.sun_path) < 0)
        err("Failed to remove socket file at " SOCKPATH, EXIT_FAILURE);
    if (!access(CONFIG_SOCKDIR, F_OK) && rmdir(CONFIG_SOCKDIR) < 0)
        err("Failed to remove socket directory at " CONFIG_SOCKDIR, EXIT_FAILURE);

    /* Create a new directory */
    if (mkdir(CONFIG_SOCKDIR, S_IRWXU) < 0)
        err("Failed to create socket directory at " CONFIG_SOCKDIR, EXIT_FAILURE);

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0)
        err("Failed to create socket", EXIT_FAILURE);

    if (bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
        err("Failed to bind the socket to the file at " SOCKPATH, EXIT_FAILURE);

    return sock_fd;
}

static void
cleanup_socket(int sock_fd)
{
    if (close(sock_fd) < 0)
        err("Failed to close socket", EXIT_FAILURE);

    if (unlink(SOCKPATH) < 0)
        err("Failed to remove socket file at " SOCKPATH, EXIT_FAILURE);
    if (rmdir(CONFIG_SOCKDIR) < 0)
        err("Failed to remove socket directory at " CONFIG_SOCKDIR, EXIT_FAILURE);
}

static TMessage
handle_message(TMessage msg)
{
    switch (msg) {
    case MSG_NEWWIN:
        new_window();
        return MSG_OK;

    case MSG_EXIT:
        if (num_open_windows() == 0) {
            gtk_main_quit();
            return MSG_OK;
        }
        return MSG_ERROR;

    default:
        return MSG_ERROR;
    }
}

static gboolean
socket_g_io_watch(GIOChannel *source, GIOCondition condition, gpointer data)
{
    int sock_fd = g_io_channel_unix_get_fd(source);
    int client_fd = accept(sock_fd, NULL, NULL);
    if (client_fd < 0) {
        wrn("accept() failed");
        return TRUE;
    }

    TMessage msg;
    int len = recv(client_fd, &msg, sizeof(msg), 0);
    if (len < 0) {
        wrn("recv() failed");
        goto socket_g_io_watch_cleanup;
    }

    msg = handle_message(msg);
    write(client_fd, &msg, sizeof(msg));

socket_g_io_watch_cleanup:
    close(client_fd);
    return TRUE;
}

static void
usage()
{
    puts("usage:");
    puts("\t" PROGRAM_NAME " [ARG]");
    puts("\nwhere ARG is one of:");
    puts("\t" ARG_NEWWIN "\tRequests a new window; default if ommited");
    puts("\t" ARG_EXIT   "\tRequests daemon termination; only successful if there are no open windows");
    puts("\t" ARG_DAEMON "\tRun in daemon mode");
}

static int
send_message(TMessage msg)
{
    int sock_fd = conn_socket();
    if (write(sock_fd, &msg, sizeof(msg)) < 0)
        err("Failed to send data", EXIT_FAILURE);
    if (recv(sock_fd, &msg, sizeof(msg), 0) < 0)
        err("Failed to recv data", EXIT_FAILURE);
    close(sock_fd);

    return msg == MSG_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int
run_daemon(int *argc, char ***argv)
{
    int sock_fd;
    gtk_init(argc, argv);

    sock_fd = bind_socket();
    if (listen(sock_fd, SOMAXCONN) < 0)
        err("Failed to listen to socket", EXIT_SUCCESS);

    /* poll the socket in the GTK+ main loop */
    GIOChannel *sock_chan = g_io_channel_unix_new(sock_fd);
    g_io_add_watch(sock_chan, G_IO_IN, socket_g_io_watch, NULL);

    gtk_main();

    /* clean up */
    cleanup_socket(sock_fd);
    g_io_channel_unref(sock_chan);

    return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{
    if (argc > 2) /* More than 1 arg means derp */
        usage();
    if (argc == 1 || strcmp(argv[1], ARG_NEWWIN) == 0)
        return send_message(MSG_NEWWIN);
    else if (strcmp(argv[1], ARG_EXIT) == 0)
        return send_message(MSG_EXIT);
    else if (strcmp(argv[1], ARG_DAEMON) == 0)
        return run_daemon(&argc, &argv);
    else /* derp */
        usage();

    return EXIT_FAILURE;
}
