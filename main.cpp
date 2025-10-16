#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

// Structure to represent a 2D point
struct Point {
    double x;
    double y;
    
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    
    // Display point
    void display() const {
        std::cout << "(" << x << ", " << y << ")";
    }
};

// Calculate Euclidean distance between two points
double distance(const Point& p1, const Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Structure to store point with its distance
struct PointDistance {
    Point point;
    double dist;
    
    PointDistance(const Point& p, double d) : point(p), dist(d) {}
    
    // For max heap (reverse comparison)
    bool operator<(const PointDistance& other) const {
        return dist < other.dist;
    }
};

// Find n nearest points to a given reference point using max heap
std::vector<Point> findNNearestPoints(const std::vector<Point>& points, 
                                       const Point& reference, 
                                       int n) {
    if (n <= 0 || points.empty()) {
        return std::vector<Point>();
    }
    
    // Limit n to the number of available points
    n = std::min(n, static_cast<int>(points.size()));
    
    // Use a max heap to maintain n nearest points
    std::priority_queue<PointDistance> maxHeap;
    
    for (const auto& point : points) {
        double dist = distance(point, reference);
        
        if (maxHeap.size() < static_cast<size_t>(n)) {
            maxHeap.push(PointDistance(point, dist));
        } else if (dist < maxHeap.top().dist) {
            maxHeap.pop();
            maxHeap.push(PointDistance(point, dist));
        }
    }
    
    // Extract points from heap
    std::vector<Point> result;
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top().point);
        maxHeap.pop();
    }
    
    // Reverse to get points in ascending order of distance
    std::reverse(result.begin(), result.end());
    
    return result;
}

int main() {
    // Example usage
    std::vector<Point> points = {
        Point(1, 3),
        Point(3, 4),
        Point(2, -1),
        Point(-1, 2),
        Point(5, 3),
        Point(4, 1),
        Point(-2, -3),
        Point(0, 0)
    };
    
    Point reference(0, 0);
    int n = 3;
    
    std::cout << "Points:" << std::endl;
    for (const auto& p : points) {
        std::cout << "  ";
        p.display();
        std::cout << " - Distance: " << distance(p, reference) << std::endl;
    }
    
    std::cout << "\nReference point: ";
    reference.display();
    std::cout << "\nFinding " << n << " nearest points...\n" << std::endl;
    
    std::vector<Point> nearest = findNNearestPoints(points, reference, n);
    
    std::cout << "The " << n << " nearest points are:" << std::endl;
    for (size_t i = 0; i < nearest.size(); ++i) {
        std::cout << i + 1 << ". ";
        nearest[i].display();
        std::cout << " - Distance: " << distance(nearest[i], reference) << std::endl;
    }
    
    // Additional example with custom input
    std::cout << "\n--- Another Example ---" << std::endl;
    std::vector<Point> points2 = {
        Point(1, 1),
        Point(2, 2),
        Point(3, 3),
        Point(4, 4),
        Point(5, 5)
    };
    
    Point reference2(0, 0);
    int n2 = 2;
    
    std::cout << "\nReference point: ";
    reference2.display();
    std::cout << "\nFinding " << n2 << " nearest points...\n" << std::endl;
    
    std::vector<Point> nearest2 = findNNearestPoints(points2, reference2, n2);
    
    std::cout << "The " << n2 << " nearest points are:" << std::endl;
    for (size_t i = 0; i < nearest2.size(); ++i) {
        std::cout << i + 1 << ". ";
        nearest2[i].display();
        std::cout << " - Distance: " << distance(nearest2[i], reference2) << std::endl;
    }
    
    return 0;
}
