# Advent of Code

All of my Advent of Code attempts (starting from 2023, the earlier solutions are lost to time...). Check out AoC here: https://adventofcode.com/.

It's nothing too serious, mostly me just playing around with whatever language I want for the year. Sometimes
I focus on a general solution that can handle whatever input it gets (see 2023), or sometimes I set hard limits
on the size of the inputs, and focus on performance instead (see 2024).

# Project Structure

The problems are structured in a directory by year and then by day number, like `2024/15`. This
is the structure the `setup_day` and `run` script expects.

Generally each day has `prog.c`, which contains the code. The input to the day's problem should be
stored in the directory as text files, though they aren't committed as per Advent of Code's wishes.

The `run` command expects the main input file to be named `input.txt`, and a secondary example input
to be named `input_test.txt`, but there can be any number of input files.

Shared utility code lives under `/utils`.

# Running Solutions

To run a given solution, run the `run` command for the directory:

```
./run 2024/15 -b -e
```

The above command will compile the program in `2024/15`, and the `-e` flag tells it to pass
in the `input_test.txt` file to use as the problem input. Run `run -h` for more info.

# Quick Start

To set up a new day easily, I've got `./setup_day` which given a day number sets up a new directory with a `prog.c` file (copied from `_starter_prog.c`), and an empty `input.txt` and `input_test.txt` file.

Right now, the directory year is hardcoded! I have to update that at some point, maybe next year.
