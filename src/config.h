/*
 * config.h
 *
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

/* Minimum width/height, in characters */
#define CONFIG_MIN_WIDTH  20
#define CONFIG_MIN_HEIGHT 5

/* Font name */
#define CONFIG_FONT_NAME "Monospace 9"

/* Color palette definition */
/* PALETTE_SIZE must be 8, 16, 24 or between 25 and 255, inclusive */
#define CONFIG_PALETTE_SIZE 16

static const char *CONFIG_COLOR_PALETTE[CONFIG_PALETTE_SIZE] = {
	"#000000000000", /* 0 Black */
	"#959551513a3a", /* 1 Red */
	"#3a3a95955151", /* 2 Green */
	"#7e7e95953a3a", /* 3 Yellow */
	"#3d3d55558d8d", /* 4 Blue */
	"#95953a3a7e7e", /* 5 Magenta */
	"#3a3a7e7e9595", /* 6 Cyan */
	"#d9d9d9d9d9d9", /* 7 White */
	"#444444444444", /* 8 Black */
	"#c1c178786060", /* 9 Red */
	"#6060c1c17878", /* 10 Green */
	"#a9a9c1c16060", /* 11 Yellow */
	"#5f5f7a7ababa", /* 12 Blue */
	"#c1c16060a9a9", /* 13 Magenta */
	"#6060a9a9c1c1", /* 14 Cyan */
	"#ffffffffffff", /* 15 White */
};

/* Foreground and background. */
#define CONFIG_FOREGROUND_COLOR "#d9d9e6e6f2f2" /* COLOR_PALETTE[7] */
#define CONFIG_BACKGROUND_COLOR "#0d0d19192626" /* COLOR_PALETTE[0] */

/* Scrollback lines (0 means disabled; negative means "infinite") */
#define CONFIG_SCROLLBACK_LINES 2000

/* Mouse auto-hide (TRUE or FALSE) */
#define CONFIG_MOUSE_AUTOHIDE TRUE

/* Audible bell (TRUE or FALSE) */
#define CONFIG_AUDIBLE_BELL FALSE

/* Visible bell (TRUE or FALSE) */
#define CONFIG_VISIBLE_BELL FALSE
