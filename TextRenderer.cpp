#include "TextRenderer.hpp"
#include "gl_errors.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"

// The code in this file was puzzled together from the following sources:
// https://learnopengl.com/In-Practice/Text-Rendering
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_02#Creating_a_texture_atlas
// https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
// https://harfbuzz.github.io/ch03s03.html

void TextRenderer::load_font(const std::string &font_path) {
	if (FT_Init_FreeType(&ft_library)) {
		throw std::runtime_error("FT Library failed to initialize.");
	}
	if (FT_New_Face(ft_library, font_path.c_str(), 0, &ft_face)) {
		throw std::runtime_error("FT Face failed to initialize.");
	}
	if (FT_Set_Char_Size(ft_face, 0, ft_size << 6, 0, 0)) {
		throw std::runtime_error("FT Set Char Size failed.");
	}


	font = hb_ft_font_create(ft_face, NULL);
	buf = hb_buffer_create();
}

void TextRenderer::load_characters(const char *characters) {

	hb_buffer_clear_contents(buf);
	hb_buffer_add_utf8(buf, characters, -1, 0, -1);
	hb_buffer_guess_segment_properties(buf);
	hb_shape(font, buf, NULL, 0);

	positions.clear();

	size_t buf_len = hb_buffer_get_length(buf);
	hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buf, NULL);

	glm::uvec2 tex_size = glm::uvec2(0, 0);

	std::set<hb_codepoint_t> codepoints;

	// get texture atlas size
	for (size_t i = 0; i < buf_len; i++) {
		hb_codepoint_t cp = info[i].codepoint;

		// we've seen this codepoint before
		if (!codepoints.emplace(cp).second) {
			continue;
		}

		if (FT_Load_Glyph(ft_face, cp, FT_LOAD_RENDER)) {
			std::cerr << "Codepoint " << cp << "failed to load." << std::endl;
			continue;
		}

		tex_size.x += ft_face->glyph->bitmap.width;
		tex_size.y = std::max(tex_size.y, ft_face->glyph->bitmap.rows);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		tex_size.x,
		tex_size.y,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		0
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL_ERRORS();

	size_t x = 0;
	for (hb_codepoint_t cp : codepoints) {
		if (FT_Load_Glyph(ft_face, cp, FT_LOAD_RENDER)) {
			std::cerr << "Codepoint " << cp << "failed to load." << std::endl;
			continue;
		}

		glm::uvec2 size = glm::uvec2(
			ft_face->glyph->bitmap.width,
			ft_face->glyph->bitmap.rows
		);
		glm::uvec2 bearing = glm::uvec2(
			ft_face->glyph->bitmap_left,
			ft_face->glyph->bitmap_top
		);

		Character c = {
			size,
			glm::uvec2(x, 0),
			bearing
		};
		positions.emplace(cp, c);

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			x,
			0,
			size.x,
			size.y,
			GL_RED,
			GL_UNSIGNED_BYTE,
			ft_face->glyph->bitmap.buffer
		);
		x += size.x;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	GL_ERRORS();

}

void TextRenderer::load_gl() {
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(
		text_render_program->Position_vec2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLbyte *)0 + 0
	);
	glEnableVertexAttribArray(text_render_program->Position_vec2);

	glVertexAttribPointer(
		text_render_program->TexCoord_vec2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLbyte *)0 + 4*2
	);
	glEnableVertexAttribArray(text_render_program->TexCoord_vec2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GL_ERRORS();
}

TextRenderer::~TextRenderer() {}

TextRenderer::TextRenderer() {
	if (buf == nullptr) {
		throw std::runtime_error("Please initialize TextRenderer with a font.");
	}
}


void TextRenderer::get_string(const char *string, std::vector<Vertex> &vertices) {
	hb_buffer_clear_contents(buf);
	hb_buffer_add_utf8(buf, string, -1, 0, -1);
	hb_buffer_guess_segment_properties(buf);
	hb_shape(font, buf, NULL, 0);

	hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buf, NULL);
	hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(buf, NULL);

	size_t buf_len = hb_buffer_get_length(buf);

	glm::vec2 current_position = glm::vec2(0.0f, 0.0f);
	for (size_t i = 0; i < buf_len; i++) {
		hb_codepoint_t cp = info[i].codepoint;
		glm::vec2 advance = glm::vec2(
			(float)pos[i].x_advance / 64.0f,
			(float)pos[i].y_advance / 64.0f
		);
		glm::vec2 offset = glm::vec2(
			(float)pos[i].x_offset / 64.0f,
			(float)pos[i].y_offset / 64.0f
		);

		auto c_it = positions.find(cp);
		if (c_it == positions.end()) {
			throw std::runtime_error("get_string found character that didn't exist");
		}
		Character c = c_it->second;

		glm::vec2 real_pos = current_position + offset;
		glm::vec2 tex_pos = c.start;

		auto add_vert = [&real_pos, &c, &tex_pos, &vertices](float x, float y){
			Vertex v = {
				real_pos + glm::vec2(x, y - c.size.y) + c.bearing,
				tex_pos  + glm::vec2(x, c.size.y - y)
			};

			vertices.push_back(v);
		};

		add_vert(0.0f,     c.size.y);
		add_vert(0.0f,     0.0f);
		add_vert(c.size.x, 0.0f);

		add_vert(0.0f,     c.size.y);
		add_vert(c.size.x, 0.0f);
		add_vert(c.size.x, c.size.y);

		current_position += advance;

	}

}

void TextRenderer::render(
		const glm::uvec2 &drawable_size,
		const std::vector<Vertex> &vertices,
		size_t num_characters) {

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
			GL_ARRAY_BUFFER, 
			num_characters * 6 * sizeof(Vertex),
			vertices.data(),
			GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(text_render_program->program);

	glm::mat4 proj = glm::ortho(
		0.0f, (float)drawable_size.x, 0.0f, (float)drawable_size.y
	);

	glUniformMatrix4fv(
			text_render_program->OBJECT_TO_CLIP_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(proj)
	);

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, GLsizei(num_characters * 6));
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	GL_ERRORS();

}
