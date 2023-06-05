#pragma once

struct Point {
	int y;
	int x;

	Point()
		:y(0), x(0)
	{
	}

	Point(int y, int x)
		: y(y), x(x)
	{
	}

	Point& operator=(const Point& other) {
		y = other.y;
		x = other.x;
		return *this;
	}

	Point(const Point& other)
		: y(other.y), x(other.x)
	{
	}

	bool operator==(const Point& other) const {
		return (x == other.x) && (y == other.y);
	}
};


namespace std {
	template <>
	struct hash<Point>
	{
		std::size_t operator()(const Point& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			return ((hash<int>()(k.y)
				^ (hash<int>()(k.x) << 1)) >> 1);
		}
	};
}