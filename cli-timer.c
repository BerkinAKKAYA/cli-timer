#include <ctype.h>
#include "cli-timer.h"

void init(void) {
	struct sigaction sig;
	clitimer->bg = COLOR_BLACK;

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, True);
	start_color();
	curs_set(False);
	clear();

	if (use_default_colors() == OK) {
		clitimer->bg = -1;
	}

	/* Init color pair */
	init_pair(0, clitimer->bg, clitimer->bg);
	init_pair(1, clitimer->bg, clitimer->option.color);
	init_pair(2, clitimer->option.color, clitimer->bg);
	refresh();

	/* Init signal handler */
	sig.sa_handler = signal_handler;
	sig.sa_flags   = 0;
	sigaction(SIGWINCH, &sig, NULL);
	sigaction(SIGTERM,  &sig, NULL);
	sigaction(SIGINT,   &sig, NULL);
	sigaction(SIGSEGV,  &sig, NULL);

	/* Init global struct */
	clitimer->running = True;
	if(!clitimer->geo.x) clitimer->geo.x = 0;
	if(!clitimer->geo.y) clitimer->geo.y = 0;
	clitimer->geo.w = SECFRAMEW;
	clitimer->geo.h = 7;

	/* Create clock win */
	clitimer->framewin = newwin(clitimer->geo.h, clitimer->geo.w, clitimer->geo.x, clitimer->geo.y);
	
	if (clitimer->bold) {
		wattron(clitimer->framewin, A_BLINK);
	}

	/* Create the date win */
	int beginY = clitimer->geo.x + clitimer->geo.h - 1;
	int beginX = clitimer->geo.y + (clitimer->geo.w / 2) - (strlen(clitimer->date.timestr) / 2) - 1;
	clitimer->datewin = newwin(DATEWINH, strlen(clitimer->date.timestr) + 2, beginY, beginX);

	clearok(clitimer->datewin, True);
	set_center();
	nodelay(stdscr, True);
	wrefresh(clitimer->datewin);
	wrefresh(clitimer->framewin);
}

void signal_handler(int signal) {
	switch(signal) {
		case SIGWINCH:
			endwin();
			init();
			break;
		case SIGINT:
		case SIGTERM:
			clitimer->running = False;
			break;
		case SIGSEGV:
			endwin();
			fprintf(stderr, "Segmentation fault.\n");
			exit(EXIT_FAILURE);
			break;
	}
}

void cleanup(void) {
	if (clitimer->ttyscr) delscreen(clitimer->ttyscr);
	if (clitimer) free(clitimer);
}

/* Update Time Array */
void update_hour(void) {
	unsigned int seconds = clitimer->date.second[0] * 10 + clitimer->date.second[1];
	unsigned int minutes = clitimer->date.minute[0] * 10 + clitimer->date.minute[1];
	unsigned int hours   = clitimer->date.hour[0]   * 10 + clitimer->date.hour[1];

	if (minutes == 59 && seconds == 59) hours = hours == 59 ? 0 : hours + 1;
	if (seconds == 59) minutes = minutes == 59 ? 0 : minutes + 1;
	seconds = seconds == 59 ? 0 : seconds + 1;

	/* Put it all back into clitimer. */
	clitimer->date.hour[0] = hours / 10;
	clitimer->date.hour[1] = hours % 10;

	clitimer->date.minute[0] = minutes / 10;
	clitimer->date.minute[1] = minutes % 10;

	clitimer->date.second[0] = seconds / 10;
	clitimer->date.second[1] = seconds % 10;
}

void draw_number(int n, int x, int y, unsigned int color) {
	int i, sy = y;

	for(i = 0; i < 30; ++i, ++sy) {
		if(sy == y + 6) {
			sy = y;
			++x;
		}

		if (clitimer->bold) wattron(clitimer->framewin, A_BLINK);
		else wattroff(clitimer->framewin, A_BLINK);

		wbkgdset(clitimer->framewin, COLOR_PAIR(number[n][i/2] * color));
		mvwaddch(clitimer->framewin, x, sy, ' ');
	}

	wrefresh(clitimer->framewin);
}

void draw_clock(void) {
	chtype dotcolor = COLOR_PAIR(1);
	unsigned int numcolor = 1;

	/* Change the colours to blink at certain times. */
	if (time(NULL) % 2 == 0) {
		dotcolor = COLOR_PAIR(2);
	}

	/* Draw hour numbers */
	draw_number(clitimer->date.hour[0], 1, 1, numcolor);
	draw_number(clitimer->date.hour[1], 1, 8, numcolor);

	/* 2 dot for number separation */
	wbkgdset(clitimer->framewin, dotcolor);
	mvwaddstr(clitimer->framewin, 2, 16, "  ");
	mvwaddstr(clitimer->framewin, 4, 16, "  ");

	/* Draw minute numbers */
	draw_number(clitimer->date.minute[0], 1, 20, numcolor);
	draw_number(clitimer->date.minute[1], 1, 27, numcolor);

	/* Draw the date */
	if (clitimer->bold) wattron(clitimer->datewin, A_BOLD);
	else wattroff(clitimer->datewin, A_BOLD);

	wbkgdset(clitimer->datewin, (COLOR_PAIR(2)));
	mvwprintw(clitimer->datewin, (DATEWINH / 2), 1, clitimer->date.timestr);
	wrefresh(clitimer->datewin);

	/* Draw second frame. */
	/* Again 2 dot for number separation */
	wbkgdset(clitimer->framewin, dotcolor);
	mvwaddstr(clitimer->framewin, 2, NORMFRAMEW, "  ");
	mvwaddstr(clitimer->framewin, 4, NORMFRAMEW, "  ");

	/* Draw second numbers */
	draw_number(clitimer->date.second[0], 1, 39, numcolor);
	draw_number(clitimer->date.second[1], 1, 46, numcolor);
}

void set_second(void) {
	int new_w = SECFRAMEW;
	int y_adj;

	for(y_adj = 0; (clitimer->geo.y - y_adj) > (COLS - new_w - 1); ++y_adj);

	set_center();
}

void set_center(void) {
	clitimer->geo.x = (LINES / 2 - (clitimer->geo.h / 2));
	clitimer->geo.y = (COLS  / 2 - (clitimer->geo.w / 2));

	int dateX = clitimer->geo.x + clitimer->geo.h - 1;
	int dateY = clitimer->geo.y + (clitimer->geo.w / 2) - (strlen(clitimer->date.timestr) / 2) - 1;

	mvwin(clitimer->framewin, clitimer->geo.x, clitimer->geo.y);
	mvwin(clitimer->datewin, dateX, dateY);
	
	/*
	wbkgdset(clitimer->framewin, COLOR_PAIR(0));
	wbkgdset(clitimer->datewin, COLOR_PAIR(0));
	wborder(clitimer->framewin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wborder(clitimer->datewin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	werase(clitimer->framewin);
	werase(clitimer->datewin);
	wrefresh(clitimer->framewin);
	wrefresh(clitimer->datewin);

	wresize(clitimer->framewin, clitimer->geo.h, clitimer->geo.w);
	wresize(clitimer->datewin, DATEWINH, strlen(clitimer->date.timestr) + 2);

	wrefresh(clitimer->framewin);
	wrefresh(clitimer->datewin);
	*/
}

/* Fills two elements from digits into time, handling the -1 case. */
static void fill_clitimer_time(int *digits, unsigned int *time) {
	if (digits[1] == -1) {
		time[0] = 0;
		if (digits[0] == -1) time[1] = 0;
		else time[1] = digits[0];
	} else {
		time[0] = digits[0];
		time[1] = digits[1];
	}
}

void key_event(void) {
	struct timespec length = { 1, 0 };
	int c = wgetch(stdscr);

	switch(c) {
		case 'q':
		case 'Q':
			clitimer->running = False;
			break;

		case 'r':
		case 'R':
			fill_clitimer_time(clitimer->initial_digits,   clitimer->date.hour);
			fill_clitimer_time(clitimer->initial_digits+2, clitimer->date.minute);
			fill_clitimer_time(clitimer->initial_digits+4, clitimer->date.second);
			break;
        
		case ' ':
			clitimer->paused = !clitimer->paused;

		default:
			nanosleep(&length, NULL);

			for (int i=0; i<8; ++i) {
				if (c == (i + '0')) {
					clitimer->option.color = i;
					init_pair(1, clitimer->bg, i);
					init_pair(2, i, clitimer->bg);
				}
			}

			break;
		}
}

/* Fill the time array with zeros */
static void initialize_time_digits(char *time) {
	int digits[N_TIME_DIGITS];

	for (int i=0; i<N_TIME_DIGITS; ++i) {
		digits[i] = 0;
	}

	fill_clitimer_time(digits, clitimer->date.hour);
	fill_clitimer_time(digits + 2, clitimer->date.minute);
	fill_clitimer_time(digits + 4, clitimer->date.second);
	memcpy(clitimer->initial_digits, digits, N_TIME_DIGITS * sizeof(int));
}

int main(int argc, char **argv) {
	/* Alloc clitimer */
	clitimer = malloc(sizeof(clitimer_t));
	assert(clitimer != NULL);
	memset(clitimer, 0, sizeof(clitimer_t));

	/* Default color */
	clitimer->option.color = COLOR_CYAN;

	/* Run cleanup on exit */
	atexit(cleanup);
	initialize_time_digits(argv[optind]);

	init();
	attron(A_BLINK);
	while (clitimer->running) {
		draw_clock();
		key_event();

		if (!clitimer->paused) {
			update_hour();
		}
	}


	printf(
			"Session Ended in %d%d Hours, %d%d Minutes, %d%d Seconds",
			clitimer->date.hour[0], clitimer->date.hour[1],
			clitimer->date.minute[0], clitimer->date.minute[1],
			clitimer->date.second[0], clitimer->date.second[1]
	);
	endwin();
	return 0;
}
