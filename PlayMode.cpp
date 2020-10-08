#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include "TextRenderer.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <sstream>


PlayMode::PlayMode() : narrative(Narrative(data_path("story.txt"))) {}

PlayMode::~PlayMode() {}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (buttons != nullptr) {
		return buttons->handle_event(evt, window_size);
	}
	return false;

}

void PlayMode::update(float elapsed) {
	if (textDisplay != nullptr) {
		int clicked = buttons->poll_clicked();
		if (clicked >= 0) {
			textDisplay->add_line(
				narrative.get_current_beat().options[clicked].c_str(),
				YOU_COLOR,
				true
			);
			buttons->fade_options(narrative.choose(clicked).first);
			textDisplay->add_line(
				narrative.get_current_beat().prompt.c_str(),
				THEM_COLOR,
				true
			);
		}
		textDisplay->update(elapsed);
		bool is_animating = textDisplay->is_animating();
		if (was_animating && !is_animating) {
			buttons->show_options();
		}
		was_animating = is_animating;
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

	if (TextRenderer::font == nullptr) {
		TextRenderer::load_font(drawable_size, data_path("mononoki.ttf"));

		std::ifstream t(data_path("story.txt"));
		std::stringstream buffer;
		buffer << t.rdbuf();

		TextRenderer::load_characters(buffer.str().c_str());
		TextRenderer::load_gl();
	}
	if (textDisplay == nullptr) {
		textDisplay = new TextDisplay();
		textDisplay->add_line(
			narrative.get_current_beat().prompt.c_str(),
			THEM_COLOR,
			true
		);
	}
	if (buttons == nullptr) {
		buttons = new Buttons(narrative.get_current_beat());
	}
	// This code is based off the game0 render code.
	glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<TextRenderer::Vertex> verts;
	textDisplay->render(drawable_size);
	buttons->render(drawable_size);
	glDisable(GL_DEPTH_TEST);

	GL_ERRORS();
}
