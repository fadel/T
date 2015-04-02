/*
 * Td.c
 * T daemon. Saves resources by uniting all T windows under the same process.
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

#include "T.h"

#define MSGBUF_MAXLEN 64

static int bind_socket();
static TMessage handle_message(TMessage msg);
static gboolean socket_g_io_watch(GIOChannel *source, GIOCondition condition, gpointer data);

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

int
main(int argc, char *argv[])
{
    int sock_fd;

    gtk_init(&argc, &argv);

    sock_fd = bind_socket();
    if (listen(sock_fd, SOMAXCONN) < 0)
        err("Failed to listen to socket", EXIT_SUCCESS);

    /* poll the socket in the main loop */
    GIOChannel *sock_chan = g_io_channel_unix_new(sock_fd);
    g_io_add_watch(sock_chan, G_IO_IN, socket_g_io_watch, NULL);

    gtk_main();

    /* clean up */
    close(sock_fd);
    unlink(SOCKPATH);

    return EXIT_SUCCESS;
}
