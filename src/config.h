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
#define CONFIG_FONT_NAME "Terminus 9"

/* Color palette definition */
/* PALETTE_SIZE must be 8, 16, 24 or between 25 and 255, inclusive */
#define CONFIG_PALETTE_SIZE 16

static const char *CONFIG_COLOR_PALETTE[CONFIG_PALETTE_SIZE] = {
	"#000000", /* 0 Black */
	"#a5372e", /* 1 Red */
	"#4a663c", /* 2 Green */
	"#7c5618", /* 3 Yellow */
	"#566060", /* 4 Blue */
	"#8e4948", /* 5 Magenta */
	"#6d5b42", /* 6 Cyan */
	"#8d8d8d", /* 7 White */
	"#2b2b2b", /* 8 Black */
	"#e06c5c", /* 9 Red */
	"#7b996c", /* 10 Green */
	"#b38849", /* 11 Yellow */
	"#889393", /* 12 Blue */
	"#c67c78", /* 13 Magenta */
	"#a28d73", /* 14 Cyan */
	"#c6c6c6", /* 15 White */
};

/* Foreground and background. */
#define CONFIG_FOREGROUND_COLOR "#757575" /* COLOR_PALETTE[7] */
#define CONFIG_BACKGROUND_COLOR "#151515" /* COLOR_PALETTE[0] */

/* Scrollback lines (0 means disabled; negative means "infinite") */
#define CONFIG_SCROLLBACK_LINES 2000

/* Mouse auto-hide (TRUE or FALSE) */
#define CONFIG_MOUSE_AUTOHIDE TRUE

/* Audible bell (TRUE or FALSE) */
#define CONFIG_AUDIBLE_BELL FALSE

/* Visible bell (TRUE or FALSE) */
#define CONFIG_VISIBLE_BELL FALSE
