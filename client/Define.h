#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

// --
// Define variable
// --

enum
{

	// Display Mode
	WINDOW_DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH,

	// Window Position
	WINDOW_POSITION_X = 50,
	WINDOW_POSITION_Y = 50,

	// Window Size
	WINDOW_SIZE_X = 800,
	WINDOW_SIZE_Y = 800,

	// Type of Shapes
	SHAPE_SQUARE = 0,
	SHAPE_MAX,

	// Shader Program
	SHADER_TEXTURE_PROGRAM = 0,

	NETWORK_PORT = 4'000,
	NETWORK_BUFFER_SIZE = 256,

	
};


constexpr const char* WINDOW_TITLE = "Chess";

constexpr const char* SHADER_TEXTURE_VERTEX = "glsl\\Vertex_texture.glsl";
constexpr const char* SHADER_TEXTURE_FRAGMENT = "glsl\\Fragment_texture.glsl";

constexpr const char* IMAGE_BOARD = "image\\board.png";
constexpr const char* IMAGE_QUEEN = "image\\queen.png";
