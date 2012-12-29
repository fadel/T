/*
 * config.h
 *
 * Configuration file. Modify these to customize T.
 */

/* Minimum width/height, in characters */
#define MIN_WIDTH  20
#define MIN_HEIGHT 5

/* Font name */
#define FONT_NAME "Terminus 9"

/* Color palette definition */
/* PALETTE_SIZE must be 8, 16, 24 or between 25 and 255, inclusive */
#define PALETTE_SIZE 16

static const char *COLOR_PALETTE[PALETTE_SIZE] = {
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
#define FOREGROUND_COLOR "#757575" /* COLOR_PALETTE[7] */
#define BACKGROUND_COLOR "#151515" /* COLOR_PALETTE[0] */

/* Mouse auto-hide (TRUE or FALSE) */
#define MOUSE_AUTOHIDE TRUE

/* Visible bell (TRUE or FALSE) */
#define VISIBLE_BELL FALSE

/* Audible bell (TRUE or FALSE) */
#define AUDIBLE_BELL FALSE
