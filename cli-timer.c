#include <ctype.h>
#include "cli-timer.h"

static bool time_is_zero(void) {
	return clitimer->date.hour[0]   == 0
		&& clitimer->date.hour[1]   == 0
		&& clitimer->date.minute[0] == 0
		&& clitimer->date.minute[1] == 0
		&& clitimer->date.second[0] == 0
		&& clitimer->date.second[1] == 0;
}

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

void update_hour(void) {
	unsigned int seconds = clitimer->date.second[0] * 10 + clitimer->date.second[1];
	unsigned int minutes = clitimer->date.minute[0] * 10 + clitimer->date.minute[1];
	unsigned int hours   = clitimer->date.hour[0]   * 10 + clitimer->date.hour[1];

	if (minutes == 0 && seconds == 0) hours = hours == 0 ? 59 : hours - 1;
	if (seconds == 0) minutes = minutes == 0 ? 59 : minutes - 1;
	seconds = seconds == 0 ? 59 : seconds - 1;

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
		if (time_is_zero()) numcolor = 2;
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

void clock_move(int x, int y, int w, int h) {
}

void set_second(void) {
	int new_w = SECFRAMEW;
	int y_adj;

	for(y_adj = 0; (clitimer->geo.y - y_adj) > (COLS - new_w - 1); ++y_adj);

	set_center();
}

void set_center(void) {
	int x = (LINES / 2 - (clitimer->geo.h / 2));
	int y = (COLS  / 2 - (clitimer->geo.w / 2));

	/* Erase border for a clean move */
	wbkgdset(clitimer->framewin, COLOR_PAIR(0));
	wborder(clitimer->framewin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	werase(clitimer->framewin);
	wrefresh(clitimer->framewin);

	wbkgdset(clitimer->datewin, COLOR_PAIR(0));
	wborder(clitimer->datewin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	werase(clitimer->datewin);
	wrefresh(clitimer->datewin);

	/* Frame win move */
	mvwin(clitimer->framewin, (clitimer->geo.x = x), (clitimer->geo.y = y));
	wresize(clitimer->framewin, clitimer->geo.h, clitimer->geo.w);

	/* Date win move */
	int mvX = clitimer->geo.x + clitimer->geo.h - 1;
	int mvY = clitimer->geo.y + (clitimer->geo.w / 2)- (strlen(clitimer->date.timestr) / 2) - 1;
	mvwin(clitimer->datewin, mvX, mvY);
	
	wresize(clitimer->datewin, DATEWINH, strlen(clitimer->date.timestr) + 2);

	wrefresh(clitimer->framewin);
	wrefresh(clitimer->datewin); 
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
	int i = 0;
	int c = wgetch(stdscr);

	// {  s, ns }
	struct timespec length = { 1, 0 };

	switch(c) {
		case 'q':
		case 'Q':
			clitimer->running = False;
			break;

		case 'r':
		case 'R':
			fill_clitimer_time(clitimer->initial_digits, clitimer->date.hour);
			fill_clitimer_time(clitimer->initial_digits + 2, clitimer->date.minute);
			fill_clitimer_time(clitimer->initial_digits + 4, clitimer->date.second);
			break;
        
		case ' ':
			clitimer->paused = !clitimer->paused;

		default:
			nanosleep(&length, NULL);

			for (i = 0; i < 8; ++i) {
				if (c == (i + '0')) {
					clitimer->option.color = i;
					init_pair(1, clitimer->bg, i);
					init_pair(2, i, clitimer->bg);
				}
			}

			break;
		}
}

/* Parses time into clitimer->date.hour/minute/second. Exits with
 * an error message on bad time format. Sets timestr to what was
 * parsed.
 * time format: hh:mm:ss, where all but the colons are optional.
 */
static void parse_time_arg(char *time) {
	int digits[N_TIME_DIGITS];
	for (int i=0; i<N_TIME_DIGITS; ++i) {
		digits[i] = -1;
	}

	int i=0, remaining=2;
	while (*time != '\0') {
		if (isdigit(*time)) {
			if (remaining == 0) {
				puts("Too many digits in time argument");
				exit(EXIT_FAILURE);
			}

			digits[i] = *time - '0';
			++i;
			--remaining;
		} else if (*time == ':') {
			i += remaining;
			remaining = 2;
		} else {
			puts("Invalid character in time argument");
			exit(EXIT_FAILURE);
		}

		++time;
	}

	fill_clitimer_time(digits, clitimer->date.hour);
	fill_clitimer_time(digits + 2, clitimer->date.minute);
	fill_clitimer_time(digits + 4, clitimer->date.second);
	memcpy(clitimer->initial_digits, digits, N_TIME_DIGITS * sizeof(int));

	clitimer->date.timestr[0] = clitimer->date.hour[0] + '0';
	clitimer->date.timestr[1] = clitimer->date.hour[1] + '0';
	clitimer->date.timestr[2] = ':';
	clitimer->date.timestr[3] = clitimer->date.minute[0] + '0';
	clitimer->date.timestr[4] = clitimer->date.minute[1] + '0';
	clitimer->date.timestr[5] = ':';
	clitimer->date.timestr[6] = clitimer->date.second[0] + '0';
	clitimer->date.timestr[7] = clitimer->date.second[1] + '0';
	clitimer->date.timestr[8] = '\0';
}

/* Converts the name of a colour to its ncurses number. Case insensitive. */
int color_name_to_number(const char *color) {
	if (strcasecmp(color, "black") == 0) return COLOR_BLACK;
	else if (strcasecmp(color, "red") == 0) return COLOR_RED;
	else if (strcasecmp(color, "green") == 0) return COLOR_GREEN;
	else if (strcasecmp(color, "yellow") == 0) return COLOR_YELLOW;
	else if (strcasecmp(color, "blue") == 0) return COLOR_BLUE;
	else if (strcasecmp(color, "magenta") == 0) return COLOR_MAGENTA;
	else if (strcasecmp(color, "cyan") == 0) return COLOR_CYAN;
	else if (strcasecmp(color, "white") == 0) return COLOR_WHITE;
	else return -1;
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
	parse_time_arg(argv[optind]);
	

	if (time_is_zero()) {
		puts("Time argument is zero");
		exit(EXIT_FAILURE);
	}

	init();
	attron(A_BLINK);
	while (clitimer->running) {
		draw_clock();
		key_event();
		if (!time_is_zero() && !clitimer->paused) {
			update_hour();
		}
	}

	endwin();
	return 0;
}