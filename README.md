# cli-timer

cli-timer is a fork of [ttytimer by mbarbar][ttytimer], which is a
fork of the [tty-clock by xorg62][tty-clock]. Unlike tty-clock, this
fork counts **up from zero**, instead of counting down from a number.

[ttytimer]: https://github.com/mbarbar/ttytimer
[tty-clock]: https://github.com/xorg62/tty-clock

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

---

> Created by Berkin AKKAYA
