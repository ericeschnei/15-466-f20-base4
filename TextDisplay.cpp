#include "TextDisplay.hpp"
#include "SDL_video.h"
#include <cmath>
#include <iostream>
#include <cstdio>

TextDisplay::TextDisplay() {};

void TextDisplay::add_line(const char *string, glm::u8vec4 color, bool type) {
	std::vector<TextRenderer::Vertex> verts;
	TextRenderer::get_string(string, verts);

	Line new_line = {verts, color};
	new_lines.emplace_back(new_line, type);

}

void TextDisplay::update(float elapsed) {

	while (lines.size() > 50) {
		lines.pop_front();
	}

	if (move_progress > 0.0f) {
		move_progress += elapsed;
		
		if (move_progress > MOVE_TIME) {
			float remaining = move_progress - MOVE_TIME;
			move_progress = 0.0f;

			auto new_line = new_lines.front();
			new_lines.pop_front();
			lines.push_back(new_line.first);
			
			if (new_line.second) {
				type_index = 0;
				update(remaining);
				return;
			} else {
				type_index = new_line.first.verts.size() / 6;
			}
			
			move_progress = 0.0f;
		} 
	} else if (lines.size() > 0 && type_index < lines.back().verts.size() / 6) {
		type_progress += elapsed;
		while (type_progress > TYPE_TIME) {
			type_progress -= TYPE_TIME;
			type_index++;
			if (type_index == lines.back().verts.size() / 6) {
				float remaining = type_progress;
				type_progress = 0.0f;
				update(remaining);
				return;
			}
		}
	} else if (new_lines.size() > 0) {
		move_progress = elapsed;
		update(0.0f);
		return;
	}

}


void TextDisplay::render(const glm::uvec2 &drawable_size) {

	float transition_amt = (1.0f - std::pow((1.0f - (move_progress / MOVE_TIME)), 4.0f)) * LINE_SPACING * TextRenderer::line_height;

	if (lines.size() > 1) {
		for (size_t i = 0; i < lines.size() - 1; i++) {
			size_t j = lines.size() - i - 1;

			TextRenderer::render(
					drawable_size,
					lines[i].verts,
					lines[i].verts.size() / 6,
					lines[i].color,
					OFFSET + glm::vec2(0.0f, transition_amt + (float)j * LINE_SPACING * TextRenderer::line_height)
			);
		}
	}
	if (lines.size() > 0) {	
		TextRenderer::render(
			drawable_size,
			lines.back().verts,
			type_index,
			lines.back().color,
			OFFSET + glm::vec2(0.0f, transition_amt)
		);
	}

}

bool TextDisplay::is_animating() {
	if (move_progress > 0.0f) return true;
	if (lines.size() > 0 && type_index < lines.back().verts.size()/6) return true;
	if (new_lines.size() > 0) return true;
	return false;
}
