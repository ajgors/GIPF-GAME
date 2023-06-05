#pragma once
#include <string>
#include "Board.h"

namespace utils {

	bool contains(const std::string& str, const std::string& substr)
	{
		return str.find(substr) != std::string::npos;
	}


	bool containsAllElements(const std::vector<std::string>& vector1, const std::vector<std::string>& vector2)
	{
		for (const std::string& element : vector2) {
			if (std::find(vector1.begin(), vector1.end(), element) == vector1.end()) {
				return false;
			}
		}
		return true;
	}


	struct Vector2DHash {
		std::size_t operator()(const std::vector<std::vector<char>>& vec) const {
			std::size_t hash = 0;
			for (int y = 1; y < vec.size() - 1; y++) {
				for (int x = 1; x < vec[y].size() - 1; x++) {
					if (vec[y][x] == SPACE_BETWEN_FIELD || vec[y][x] == START_CHAR) continue;
					hash ^= std::hash<int>()(vec[y][x]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				}
			}
			return hash;
		}
	};
}