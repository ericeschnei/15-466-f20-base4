#pragma once

#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "TextDisplay.hpp"
#include "Narrative.hpp"
#include "Buttons.hpp"

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	TextDisplay *textDisplay = nullptr;
	Narrative narrative;
	Buttons *buttons = nullptr;
	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	bool was_animating = false;
	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	static constexpr glm::u8vec4 YOU_COLOR = glm::u8vec4(0xDE, 0x3C, 0x4B, 0xFF);
	static constexpr glm::u8vec4 THEM_COLOR = glm::u8vec4(0xE2, 0x84, 0x13, 0xFF);

};
