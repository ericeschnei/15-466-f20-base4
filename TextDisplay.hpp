#pragma once

#include <deque>
#include "TextRenderer.hpp"

struct TextDisplay {

	TextDisplay();
	~TextDisplay();

	struct Line {
		std::vector<TextRenderer::Vertex> verts;
		glm::u8vec4 color;
	};

	std::deque<Line> lines;
	std::deque<std::pair<Line, bool>> new_lines;

	// move_progress: 0.....MOVE_TIME->0
	// type_progress:                  0->TYPE_TIME 0->TYPE_TIME...
	float move_progress = 0.0f;
	float type_progress = 0.0f;
	size_t type_index = 0;

	static constexpr float MOVE_TIME = 0.5f;
	static constexpr float TYPE_TIME = 0.05f;

	static constexpr float LINE_SPACING = 1.5f;
	glm::vec2 OFFSET = glm::vec2(TextRenderer::line_height, TextRenderer::line_height * LINE_SPACING * 6);

	void add_line(
		const char *string,
		glm::u8vec4 color,
		bool type
	);

	void update(float elapsed);

	void render(const glm::uvec2 &drawable_size); 

	bool is_animating();

};
