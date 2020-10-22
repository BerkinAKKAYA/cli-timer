# ttytimer

tty-stopwatch is a fork of ttytimer by mbarbar, which is a
fork of the tty-clock by xorg62. I added start/stop feature
and removed the code i don't personally need.

## Usage

```
usage : %s [-xbvih] [-C color] hh:mm:ss
        -C color      Set the clock color
                      | black  | red   | green   |
                      | yellow | blue  | magenta |
                      | cyan   | white |
        -v            Show ttytimer version
        -h            Show this page
```

### At runtime

| KEY   | COMMAND      |
| ----- | ------------ |
| qQ    | Quit         |
| rR    | Restart      |
| Space | Start & Stop |

## Installation

### Dependencies

-   `make`
-   ncurses 5
-   C compiler

### Instructions

```
make install
```

## TODO

1. Option to count up rather than just down.
2. Man page - including detailed description of time format.
