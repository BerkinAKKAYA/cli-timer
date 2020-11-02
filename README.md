# cli-timer

cli-timer is a fork of [ttytimer by mbarbar][ttytimer], which is a
fork of the [tty-clock by xorg62][tty-clock]. I added start/stop feature
and removed the code i don't personally need, such as rebound function,
man files, [toot][toot]... My goal is to just play with it and learn C.

[ttytimer]: https://github.com/mbarbar/ttytimer
[tty-clock]: https://github.com/xorg62/tty-clock
[toot]: https://github.com/vareille/toot

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
