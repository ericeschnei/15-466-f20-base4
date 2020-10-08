#pragma once

#include "Narrative.hpp"
#include "SDL_events.h"
#include "TextDisplay.hpp"
#include "TextRenderer.hpp"

struct Buttons {

	Buttons(const Narrative::Beat &options);
	~Buttons();

	std::array<std::vector<TextRenderer::Vertex>, 4> buttons;
	std::array<std::vector<TextRenderer::Vertex>, 4> new_buttons;

	bool clicked = false;
	int clicked_index = -1;
	int hovered_index = -1;
	float opacity = 0.0f;

	bool handle_event(const SDL_Event &evt, const glm::uvec2 &window_size);
	int poll_clicked();

	void fade_options(const Narrative::Beat &new_options);
	void show_options();

	void update(float elapsed);
	void render(const glm::uvec2 &drawable_size);


};
