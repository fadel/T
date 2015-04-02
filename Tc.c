/*
 * Tc.c
 * T client. Merely issues commands to the T daemon.
 *
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "T.h"

#define ARG_NEWWIN "-n"
#define ARG_EXIT   "-x"

static int open_conn();
static TMessage parse_args(int argc, char *argv[]);
static void usage();

static int
open_conn()
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

static TMessage
parse_args(int argc, char *argv[])
{
    if (argc == 1) /* No args means new window */
        return MSG_NEWWIN;
    if (argc > 2) /* More than 1 arg means derp */
        return MSG_ERROR;
    if (strcmp(argv[1], ARG_NEWWIN) == 0)
        return MSG_NEWWIN;
    if (strcmp(argv[1], ARG_EXIT) == 0)
        return MSG_EXIT;

    return MSG_ERROR;
}

static void
usage()
{
    puts("usage:");
    puts("\tTc [ARG]");
    puts("\nwhere ARG is one of:");
    puts("\t" ARG_NEWWIN "\tRequests a new window; default if ommited");
    puts("\t" ARG_EXIT   "\tRequests daemon termination; only successful if there are no open windows");
}

int
main(int argc, char *argv[])
{
    TMessage msg = parse_args(argc, argv);
    if (msg == MSG_ERROR) {
        usage();
        return EXIT_FAILURE;
    }

    int sock_fd = open_conn();
    if (write(sock_fd, &msg, sizeof(msg)) < 0)
        err("Failed to send data", EXIT_FAILURE);
    if (recv(sock_fd, &msg, sizeof(msg), 0) < 0)
        err("Failed to recv data", EXIT_FAILURE);
    close(sock_fd);

    return msg == MSG_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
