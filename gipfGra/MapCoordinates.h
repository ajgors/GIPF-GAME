#pragma once
#include <vector>
#include <unordered_map>
#include "Point.h"
#include <string>

struct Coordinates {
	std::unordered_map<std::string, Point> m_coordinates;
	std::unordered_map<Point, std::string> pointToCordinate;
	std::vector<Point> startPoints;
};