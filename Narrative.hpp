#pragma once

#include <string>
#include <array>
#include <unordered_map>

struct Narrative {

	struct Beat {
		std::string prompt;
		std::array<std::string, 4> options;
		std::array<int, 4> next;
	};

	size_t current_beat;
	std::unordered_map<int, Beat> beats;

	Narrative(const std::string &filename);

	std::pair<Beat, bool> choose(size_t choice);
	Beat get_current_beat();
};
