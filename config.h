/*
 * config.h
 * Configuration file. Modify these to customize T.
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

/* The directory of the connection socket used by the T daemon. */
#define CONFIG_SOCKDIR "/tmp/Td"

/* The name of the connection socket used by the T daemon. */
#define CONFIG_SOCKNAME "Td.sock"

/* Minimum width/height, in characters */
#define CONFIG_MIN_WIDTH  20
#define CONFIG_MIN_HEIGHT 5

/* Font */
#define CONFIG_FONT_NAME "Monospace 9"

/* Color palette definition */
/* PALETTE_SIZE must be 8, 16, 24 or between 25 and 255, inclusive */
#define CONFIG_PALETTE_SIZE 16

#define CONFIG_PALLETE_0  "#000000" /* Black */
#define CONFIG_PALLETE_1  "#95513a" /* Red */
#define CONFIG_PALLETE_2  "#3a9551" /* Green */
#define CONFIG_PALLETE_3  "#7e953a" /* Yellow */
#define CONFIG_PALLETE_4  "#3d558d" /* Blue */
#define CONFIG_PALLETE_5  "#953a7e" /* Magenta */
#define CONFIG_PALLETE_6  "#3a7e95" /* Cyan */
#define CONFIG_PALLETE_7  "#d9d9d9" /* White */
#define CONFIG_PALLETE_8  "#262626" /* Black */
#define CONFIG_PALLETE_9  "#c17860" /* Red */
#define CONFIG_PALLETE_10 "#60c178" /* Green */
#define CONFIG_PALLETE_11 "#a9c160" /* Yellow */
#define CONFIG_PALLETE_12 "#5f7aba" /* Blue */
#define CONFIG_PALLETE_13 "#c160a9" /* Magenta */
#define CONFIG_PALLETE_14 "#60a9c1" /* Cyan */
#define CONFIG_PALLETE_15 "#ffffff" /* White */

/* Foreground and background. */
#define CONFIG_FOREGROUND_COLOR "#d9e6f2" /* COLOR_PALETTE[7] */
#define CONFIG_BACKGROUND_COLOR "#0d1926" /* COLOR_PALETTE[0] */

/* Scrollback lines (0 means disabled; negative means "infinite") */
#define CONFIG_SCROLLBACK_LINES 2000

/* Mouse auto-hide (TRUE or FALSE) */
#define CONFIG_MOUSE_AUTOHIDE TRUE

/* Audible bell (TRUE or FALSE) */
#define CONFIG_AUDIBLE_BELL FALSE

/* Visible bell (TRUE or FALSE) */
#define CONFIG_VISIBLE_BELL FALSE

/* Cursor blink mode. One of: VTE_CURSOR_BLINK_SYSTEM/ON/OFF */
#define CONFIG_CURSOR_BLINK_MODE VTE_CURSOR_BLINK_OFF
