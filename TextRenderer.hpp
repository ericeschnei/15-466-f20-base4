#include "GL.hpp"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <deque>
#include <vector>

#include <hb.h>
#include <hb-ft.h>
#include <unordered_map>
#include "TextRenderProgram.hpp"

struct TextRenderer {

	inline static GLuint       texture;
	inline static GLuint       vao, vbo;

	inline static hb_buffer_t *buf;
	inline static hb_font_t   *font;

	inline static FT_Library   ft_library;
	inline static FT_Face      ft_face;
	inline static constexpr size_t ft_size = 100;

	static void load_font(const std::string &font_path);
	static void load_characters(const char *characters);
	static void load_gl();

	struct Vertex {
		glm::vec2 position;
		glm::vec2 tex;
	};

	static void get_string(
			const char *string, 
			std::vector<Vertex> &vertices);

	static void render(
		const glm::uvec2 &drawable_size,
		const std::vector<Vertex> &vertices,
		size_t num_characters);
	
	// a way to reference characters
	// stored in the texture
	struct Character {
		glm::vec2 size;
		glm::vec2 start;
		glm::vec2 bearing;
	};
	inline static std::unordered_map<hb_codepoint_t, Character> positions;
	
	TextRenderer();
	~TextRenderer();



};
