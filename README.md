# Slitherlink Solver

## About
My attempt at a slitherlink solver using some sample puzzles from [krazydad.com](https://www.krazydad.com/play/slitherlink) (first two puzzles per difficulty level for 4 different grid sizes).
Big thanks to **krazydad** himself for providing these with their encoded solutions.

Eventually I aim to expand this to work for non-square grids. It is still a WIP for square grids too.

## Quick use
I've been running this quickly on my local Linux machine with:

```
gcc ./slitherlink.c -o ./slitherlink.o && ./slitherlink.o
```

This will run through the 24 sample puzzles and print them if the solution is incomplete or doesn't match the known answer.

If your terminal isn't wide enough to print the board and its shaded version side-by-side, you can disable the shaded version by modifying line 50 in `slitherlink.c` to:

```
print_grid(&grid, puzzles[i].size, false, false)
```
