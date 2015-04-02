/*
 * T.h
 * Common definitions and utilities.
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

#ifndef T_H
#define T_H

#include "config.h"

#define PROGRAM_NAME "T"

#define SOCKPATH CONFIG_SOCKDIR "/" CONFIG_SOCKNAME
#define SOCKPATH_MAXLEN sizeof((SOCKPATH))

typedef enum {
    MSG_NEWWIN,
    MSG_EXIT,
    MSG_OK,
    MSG_ERROR
} TMessage;

int num_open_windows();
void new_window();

void wrn(const char *message);
void err(const char *message, int ecode);

#endif /* T_H */
