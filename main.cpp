#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <memory>
#include <numeric>
#include <random>
#include <vector>
#include <iomanip>

using std::cout;
using std::size_t;
using std::vector;


struct Point {
	double x{};
	double y{};

	double coord(int axis) const {
		return axis == 0 ? x : y;
	}
};

static inline double sqdist(const Point &a, const Point &b) {
	auto dx = a.x - b.x;
	auto dy = a.y - b.y;
	return dx * dx + dy * dy;
}

enum class SplitStrategy { Alternate, LongestSide };

class KDTree {
	struct Node {
		size_t idx;
		int axis;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;
	};



	const vector<Point> &pts;
	std::unique_ptr<Node> root;
	SplitStrategy strategy_;

	int get_split_axis(const vector<size_t> &idxs, size_t l, size_t r, int depth) const {
		if (strategy_ == SplitStrategy::Alternate) {
			return depth % 2;
		}

		auto minx = std::numeric_limits<double>::infinity();
		auto maxx = -std::numeric_limits<double>::infinity();
		auto miny = std::numeric_limits<double>::infinity();
		auto maxy = -std::numeric_limits<double>::infinity();

		for (auto i = l; i < r; ++i) {
			const auto &p = pts[idxs[i]];
			if (p.x < minx) minx = p.x;
			if (p.x > maxx) maxx = p.x;
			if (p.y < miny) miny = p.y;
			if (p.y > maxy) maxy = p.y;
		}

		auto width = maxx - minx;
		auto height = maxy - miny;

		return (height > width) ? 1 : 0;
	}

	std::unique_ptr<Node> build(vector<size_t> &idxs, size_t l, size_t r, int depth) {
		if (l >= r) return nullptr;

		// O(n) complexity
		auto axis = get_split_axis(idxs, l, r, depth);

		auto mid = l + (r - l) / 2;
		auto comp = [&](size_t a, size_t b){
			const double va = pts[a].coord(axis);
			const double vb = pts[b].coord(axis);

			if (va != vb) return va < vb;
			return a < b;
		};

		// Find median index and set it at idxs[mid]
		// Also O(n) complexity
		std::nth_element(
			idxs.begin() + l,
			idxs.begin() + mid,
			idxs.begin() + r,
			comp
		);

		auto node = std::make_unique<Node>();
		node->idx = idxs[mid];
		node->axis = axis;
		node->left = build(idxs, l, mid, depth + 1);
		node->right = build(idxs, mid + 1, r, depth + 1);

		return node;
	}
	struct Item {
		double distance;
		size_t index;
	};

	static bool worse(const Item &a, const Item &b) {
		if (a.distance != b.distance) return a.distance < b.distance;
		return a.index < b.index;
	}

	static auto create_heap() {
		return std::priority_queue<Item, vector<Item>, std::function<bool(const Item&, const Item&)>>(
			[](const Item &a, const Item &b) { return KDTree::worse(a, b); }
		);
	}

public:
	KDTree(const vector<Point> &points, SplitStrategy strategy = SplitStrategy::Alternate)
		: pts(points), strategy_(strategy) {
		if (!pts.empty()) {
			vector<size_t> idxs(pts.size());

			// Fill idxs with 0, 1, 2, ..., n-1
			std::iota(idxs.begin(), idxs.end(), 0);

			root = build(idxs, 0, idxs.size(), 0);
		}
	}

	vector<size_t> knn(const Point &query_point, size_t k) const {
		const auto n = pts.size();
		if (k == 0 || n == 0) return {};
		if (k > n) k = n;

		auto heap = create_heap();

		std::function<void(const Node*)> visit = [&](const Node* node){
			if (!node) return;

			const auto idx = node->idx;
			const auto distance = sqdist(pts[idx], query_point);
			const auto current = Item{distance, idx};

			if (heap.size() < k) {
				heap.emplace(current);
			} else if (worse(current, heap.top())) {
				heap.pop();
				heap.emplace(current);
			}

			const auto axis = node->axis;
			const auto splitVal = pts[idx].coord(axis);
			const auto queryVal = query_point.coord(axis);

			const bool goLeft = queryVal <= splitVal;
			const auto* first = goLeft ? node->left.get() : node->right.get();
			const auto* second = goLeft ? node->right.get() : node->left.get();

			visit(first);

			const auto delta = queryVal - splitVal;
			const auto delta2 = delta * delta;
			if (heap.size() < k || delta2 < heap.top().distance) {
				visit(second);
			}
		};

		visit(root.get());

		auto result = vector<size_t>(heap.size());
		for (size_t i = result.size(); i-- > 0;) {
			result[i] = heap.top().index;
			heap.pop();
		}
		return result;
	}

	vector<vector<size_t>> knn_batch(const vector<Point> &queries, size_t k) const {
		auto results = vector<vector<size_t>>{};
		results.reserve(queries.size());
		for (const auto &q : queries) {
			results.push_back(knn(q, k));
		}
		return results;
	}
};

int main(int argc, char **argv) {
	if (argc != 6) return 1;

	// Usage:
	//   n = number of points
	//   k = number of nearest neighbors to find
	//   t = number of test points
	//   grid_x = size of the grid in the x dimension
	//   grid_y = size of the grid in the y dimension
	auto n = static_cast<size_t>(std::strtoull(argv[1], nullptr, 10));
	auto k = static_cast<size_t>(std::strtoull(argv[2], nullptr, 10));
	auto t = static_cast<size_t>(std::strtoull(argv[3], nullptr, 10));
	auto grid_x = static_cast<size_t>(std::strtoull(argv[4], nullptr, 10));
	auto grid_y = static_cast<size_t>(std::strtoull(argv[5], nullptr, 10));

	auto rd = std::random_device{};
	auto seed = (static_cast<unsigned long long>(rd()) << 32) ^ rd();
	auto rng = std::mt19937_64(seed);
	auto distx = std::uniform_real_distribution<double>(0.0, static_cast<double>(grid_x));
	auto disty = std::uniform_real_distribution<double>(0.0, static_cast<double>(grid_y));

	auto S = vector<Point>{};
	S.reserve(n);
	for (auto i = size_t{0}; i < n; ++i) {
		S.push_back(Point{distx(rng), disty(rng)});
	}

	auto t0 = std::chrono::steady_clock::now();

	KDTree kd_alt(S, SplitStrategy::Alternate);
	auto t1 = std::chrono::steady_clock::now();

	KDTree kd_long(S, SplitStrategy::LongestSide);
	auto t2 = std::chrono::steady_clock::now();

	auto dt_alt = t1 - t0;
	auto dt_long = t2 - t1;
	auto build_alt = std::chrono::duration<double>(dt_alt).count();
	auto build_long = std::chrono::duration<double>(dt_long).count();

	auto Q = vector<Point>{};
	Q.reserve(t);
	for (auto i = size_t{0}; i < t; ++i) {
		Q.push_back(Point{distx(rng), disty(rng)});
	}

	// Alternating axis
	auto q0 = std::chrono::steady_clock::now();
	auto results_alt = kd_alt.knn_batch(Q, k);
	auto q1 = std::chrono::steady_clock::now();
	auto sec_alt = std::chrono::duration<double>(q1 - q0).count();

	// Longer axis
	auto q2 = std::chrono::steady_clock::now();
	auto results_long = kd_long.knn_batch(Q, k);
	auto q3 = std::chrono::steady_clock::now();
	auto sec_long = std::chrono::duration<double>(q3 - q2).count();

	for (size_t i = 0; i < results_alt.size(); ++i) {
		cout << "ALT " << i << ":";
		for (auto idx : results_alt[i]) cout << ' ' << idx;
		cout << '\n';
		
		cout << "LNG " << i << ":";
		for (auto idx : results_long[i]) cout << ' ' << idx;
		cout << "\n\n";
	}

	cout << "query_alt query_long build_alt build_long\n";
	cout << std::fixed << std::setprecision(6)
		 << sec_alt << " " << sec_long << " "
		 << build_alt << " " << build_long << "\n";
	return 0;
}

