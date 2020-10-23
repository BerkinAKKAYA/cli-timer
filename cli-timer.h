#ifndef clitimer_H_INCLUDED
#define clitimer_H_INCLUDED

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ncurses.h>
#include <unistd.h>
#include <getopt.h>

/* Macro */
#define NORMFRAMEW 35
#define SECFRAMEW  54
#define DATEWINH   3
#define N_TIME_DIGITS 6 /* hh:mm:ss. */

typedef enum { False, True } Bool;

/* Global clitimer struct */
typedef struct {
	/* while() boolean */
	Bool running;
	Bool paused;
	Bool bold;

	/* terminal variables */ 
	SCREEN *ttyscr;
	int bg;

	/* Running option */
	struct {
		Bool box;
		int color;
	} option;

	/* Clock geometry */
	struct {
		int x, y, w, h;
		/* For rebound use (see clock_rebound())*/
		int a, b;
	} geo;

	/* Date content ([2] = number by number) */
	int initial_digits[N_TIME_DIGITS];
	struct {
		unsigned int hour[2];
		unsigned int minute[2];
		unsigned int second[2];
		char timestr[9];  /* hh:mm:ss */
	} date;

	/* time.h utils */
	struct tm *tm;
	time_t lt;

	/* Clock member */
	WINDOW *framewin;
	WINDOW *datewin;
} clitimer_t;

/* Prototypes */
void init(void);
void signal_handler(int signal);
void update_hour(void);
void draw_number(int n, int x, int y, unsigned int color);
void draw_clock(void);
void clock_move(int x, int y, int w, int h);
void set_second(void);
void set_center(void);
void set_box(Bool b);
void key_event(void);

/* Global variable */
clitimer_t *clitimer;

/* Number matrix */
const Bool number[][15] = {
	{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1}, /* 0 */
	{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 1 */
	{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1}, /* 2 */
	{1,1,1,0,0,1,1,1,1,0,0,1,1,1,1}, /* 3 */
	{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1}, /* 4 */
	{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1}, /* 5 */
	{1,1,1,1,0,0,1,1,1,1,0,1,1,1,1}, /* 6 */
	{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 7 */
	{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1}, /* 8 */
	{1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}, /* 9 */
};

#endif /* clitimer_H_INCLUDED */