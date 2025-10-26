# n_nearest_points

This repository showcases the difference between 2 different strategies for building the kd-tree.
The standard aproach picks the split axis based on the current node tree depth,
while the alternate approach implemented herepicks the split axis
based on the longer side of the rectangle confining the current points.

## Running

Compile and run `main.cpp`:
```
g++ -std=c++17 -O2 -pipe -o nnp main.cpp
./nnp <n> <k> <t> <gx> <gy>
```
Where:
* `<n>`: number of points (e.g., 100000)
* `<k>`: k nearest neighbors (e.g., 1000)
* `<t>`: number of test query points (e.g., 10000)
* `<gx>`: grid x dimension (e.g., 1.0)
* `<gy>`: grid y dimension (e.g., 1.0)

## Benchmarking

Set the configs inside `bench.py` and run it:
```
python3 bench.py
```
The results of the run can be found in `results.csv`.
The tree construction times and the time it takes to find `k` neighbours is measured for each config.

## Results

In the `results.example.csv` file you can find benchmark results ran for 3 different configs with varying grid sizes.
The first grid size is a 1x1 square, and the second two are 1x10 and 1x100 rectangles.
For each setup 5 tests were ran.

We can see that tree construction times don't differ between the two strategies,
which makes sense since both of them have the same time complexity.

As for the actual finding times, we can see that for the square grid the strategies are more or less the same, but for the 1x100 grid
the standard alteranating strategy is two times slower then the optimized longest split strategy.
