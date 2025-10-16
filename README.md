# n_nearest_points

A simple C++ program that solves the problem of finding the n nearest points to a given reference point in 2D space.

## Overview

This program implements an efficient algorithm to find the n closest points to a reference point using Euclidean distance. The implementation uses a max heap data structure to achieve O(m log n) time complexity, where m is the total number of points and n is the number of nearest points to find.

## Features

- **Efficient Algorithm**: Uses a max heap to maintain only the n nearest points
- **2D Point Support**: Works with 2D coordinates (x, y)
- **Euclidean Distance**: Calculates distance using the standard Euclidean formula
- **Example Demonstrations**: Includes sample usage with different point sets

## Algorithm

The program uses the following approach:
1. For each point in the collection, calculate its distance from the reference point
2. Maintain a max heap of size n containing the n nearest points seen so far
3. If a new point is closer than the farthest point in the heap, replace it
4. Return the n points in ascending order of distance

## Building the Program

### Prerequisites
- C++ compiler with C++11 support (g++, clang++, etc.)
- Make (optional, for using Makefile)

### Compilation

Using Make:
```bash
make
```

Or manually with g++:
```bash
g++ -std=c++11 -Wall -Wextra -O2 -o n_nearest_points main.cpp
```

## Running the Program

After building, run the program:
```bash
./n_nearest_points
```

Or use Make:
```bash
make run
```

## Example Output

```
Points:
  (1, 3) - Distance: 3.16228
  (3, 4) - Distance: 5
  (2, -1) - Distance: 2.23607
  (-1, 2) - Distance: 2.23607
  ...

Reference point: (0, 0)
Finding 3 nearest points...

The 3 nearest points are:
1. (0, 0) - Distance: 0
2. (-1, 2) - Distance: 2.23607
3. (2, -1) - Distance: 2.23607
```

## Code Structure

- **Point struct**: Represents a 2D point with x and y coordinates
- **distance()**: Calculates Euclidean distance between two points
- **findNNearestPoints()**: Main algorithm to find n nearest points
- **main()**: Demonstrates the usage with example point sets

## Customization

To use the program with your own data:

1. Modify the `points` vector in `main()` with your point coordinates
2. Set the `reference` point to your desired origin
3. Set `n` to the number of nearest points you want to find
4. Recompile and run

Example:
```cpp
std::vector<Point> points = {
    Point(1, 2),
    Point(3, 4),
    Point(5, 6)
};
Point reference(0, 0);
int n = 2;

std::vector<Point> nearest = findNNearestPoints(points, reference, n);
```

## Cleaning Up

To remove the compiled binary:
```bash
make clean
```

## Time Complexity

- **Time**: O(m log n) where m is the total number of points and n is the number of nearest points
- **Space**: O(n) for storing the n nearest points in the heap

## License

This is a simple educational implementation for finding n nearest points.