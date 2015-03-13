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

/* Font */
#define CONFIG_FONT_NAME "Monospace 9"

/* Color palette definition */
/* PALETTE_SIZE must be 8, 16, 24 or between 25 and 255, inclusive */
#define CONFIG_PALETTE_SIZE 16

static const char *CONFIG_COLOR_PALETTE[CONFIG_PALETTE_SIZE] = {
	"#000000", /* 0 Black */
	"#95513a", /* 1 Red */
	"#3a9551", /* 2 Green */
	"#7e953a", /* 3 Yellow */
	"#3d558d", /* 4 Blue */
	"#953a7e", /* 5 Magenta */
	"#3a7e95", /* 6 Cyan */
	"#d9d9d9", /* 7 White */
	"#262626", /* 8 Black */
	"#c17860", /* 9 Red */
	"#60c178", /* 10 Green */
	"#a9c160", /* 11 Yellow */
	"#5f7aba", /* 12 Blue */
	"#c160a9", /* 13 Magenta */
	"#60a9c1", /* 14 Cyan */
	"#ffffff", /* 15 White */
};

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
