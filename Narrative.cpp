#include "Narrative.hpp"
#include <fstream>
#include <string>
#include <cstdio>

Narrative::Narrative(const std::string &filepath) {

	std::ifstream file(filepath);

	if (!file.is_open()) {
		throw std::runtime_error("Narrative file could not be opened.");
	}
	
	size_t index = 0;

	while (file.good()) {
		Beat beat;
		std::getline(file, beat.prompt);
		if (beat.prompt.length() == 0) break;

		for (size_t i = 0; i < 4; i++) {
			std::string next;
			std::getline(file, next, ' ');
			beat.next[i] = std::stoi(next);
			std::getline(file, beat.options[i]);
		}

		beats.emplace(index, beat);
		index += 5;
	}

	file.close();

}

std::pair<Narrative::Beat, bool> Narrative::choose(size_t choice) {
	
	// get current beat
	Beat current = get_current_beat();
	int next = current.next[choice];
	if (next == -1) {
		return std::pair<Beat, bool>(current, false);
	} else {
		current_beat = next;
		current = get_current_beat();
		return std::pair<Beat, bool>(current, true);
	}

}

Narrative::Beat Narrative::get_current_beat() {
	auto current_it = beats.find(current_beat);
	if (current_it == beats.end()) {
		throw std::runtime_error("We've jumped into unknown narrative territory");
	}
	return current_it->second;

}
