#include "Buttons.hpp"
#include "glm/fwd.hpp"
#include <cstdio>
#include "PlayMode.hpp"

Buttons::~Buttons() {}
Buttons::Buttons(const Narrative::Beat &options) {
	for (size_t i = 0; i < 4; i++) {
		buttons[i].clear();
		if (options.next[i] != -1) {
			TextRenderer::get_string(options.options[i].c_str(), new_buttons[i]);
		}
	}
}

void Buttons::show_options() {
	buttons = new_buttons;
	opacity = 1.0f;
}

void Buttons::fade_options(const Narrative::Beat &new_options) {
	for (size_t i = 0; i < 4; i++) {
		new_buttons[i].clear();
		if (new_options.next[i] != -1) {
			TextRenderer::get_string(new_options.options[i].c_str(), new_buttons[i]);
		}
	}

	opacity = 0.0f;

}

bool Buttons::handle_event(const SDL_Event &evt, const glm::uvec2 &window_size) {
	if (evt.type == SDL_MOUSEMOTION) {
		SDL_MouseMotionEvent mbe = evt.motion;
		glm::ivec2 mouse_pos = glm::uvec2(mbe.x, window_size.y - mbe.y);
		hovered_index = (mouse_pos.y / (TextRenderer::line_height * TextDisplay::LINE_SPACING)) - 0.75f;
		return true;
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		SDL_MouseButtonEvent mbe = evt.button;
		if (mbe.button != SDL_BUTTON_LEFT) return false;
		glm::ivec2 mouse_pos = glm::uvec2(mbe.x, window_size.y - mbe.y);

		clicked_index = (mouse_pos.y / (TextRenderer::line_height * TextDisplay::LINE_SPACING)) - 0.75f;
		if (clicked_index >= 0 && clicked_index < 4 && buttons[clicked_index].size() > 0) {
			clicked = true;
		}
		return true;
	}
	return false;
}

int Buttons::poll_clicked() {
	if (clicked) {
		clicked = false;
		return clicked_index;
	} else {
		return -1;
	}
}

void Buttons::update(float elapsed) {
	return;
}

void Buttons::render(const glm::uvec2 &drawable_size) {
	
	for (size_t i = 0; i < 4; i++) {
		glm::u8vec3 color = ((int)i == hovered_index) ?
			glm::u8vec3(0xBB, 0xBB, 0xBB) :
			glm::u8vec3(PlayMode::YOU_COLOR);
		TextRenderer::render(
			drawable_size, 
			buttons[i], 
			buttons[i].size()/6, 
			glm::u8vec4(color, (uint8_t)(255 * opacity)), 
			glm::vec2(
				TextRenderer::line_height, 
				(i+1) * TextRenderer::line_height * 
				TextDisplay::LINE_SPACING
			)
		);
	}

}
