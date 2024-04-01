#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>

#include "CShaderMgr.h"
#include "CShapeDataMgr.h"
#include "Define.h"
#include "Struct.h"
#include "Resource.h"
#include "CNetworkMgr.h"


// --
// declare callback function 
// --

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
	GLvoid SpecialKeys(int key, int x, int y);
	GLvoid SpecialKeysUp(int key, int x, int y);
	GLvoid Motion(int x, int y);
	GLvoid KeyboardUp(unsigned char key, int x, int y);
}




class CWindowMgr
{
private:


	// default member variable
	SView		view;
	glm::mat4	proj = glm::mat4(1.f);
	glm::vec3	background_color = glm::vec3(0.f, 0.f, 0.f);


	// ---
	// process member variable
	// ---

	std::vector<CShape> shapes;

	std::unordered_map<int, Queen> queen_table;
	


public:

	// --
	// Basic func
	// --

	void init(int& argc, char** argv)
	{
		network_mgr.setServerAddress();
		network_mgr.init();


		// init GLUT
		glutInit(&argc, argv);
		glutInitDisplayMode(WINDOW_DISPLAYMODE);
		glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
		glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
		glutCreateWindow(WINDOW_TITLE);

		// init GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Unable to initialize GLEW" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
			std::cout << "GLEW Initialized\n";

		CShaderMgr::getInst()->init();



		// set cb func
		glutDisplayFunc(cb::Display);
		glutReshapeFunc(cb::Reshape);
		glutMouseFunc(cb::Mouse);
		glutKeyboardFunc(cb::Keyboard);
		glutKeyboardUpFunc(cb::KeyboardUp);
		glutTimerFunc(10, cb::GameLoop, 1);
		glutMotionFunc(cb::Motion);
		glutSpecialFunc(cb::SpecialKeys);
		glutSpecialUpFunc(cb::SpecialKeysUp);

		// --
		// create shape data
		// --
		CShape board;
		board.setData(SHAPE_SQUARE);
		board.setTexture(IMAGE_BOARD);
		board.setColor(1.f, 1.f, 1.f);
		board.scale(0, 2.f, 2.f, 2.f);
		shapes.push_back(board);


		for (auto& shape : shapes)
			shape.updateBuffer();

		
		// --
		// set basic variable
		// --


		proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);

		// 초기 카메라 위치 조정
		view.eye = glm::vec3(0.0f, 5.0f, 0.0f);
		view.at = glm::vec3(0.f, 0.f, 0.0f);
		view.up = glm::vec3(0.f, 0.f, -1.f);

		// --
		// explain
		// --

		std::cout << "---------------------" << std::endl;
		std::cout << "커서 키: 말 이동" << std::endl;
		std::cout << "---------------------" << std::endl;

	}

	// --
	// define cb func
	// --

	void Display()
	{
		glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int mode = GL_TRIANGLES;

		glEnable(GL_DEPTH_TEST);

		for (auto& shape : shapes)
			shape.draw(view, proj, mode);

		for (auto& queen : queen_table)
			queen.second.draw(view, proj, mode);

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Motion(const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
	
	}

	void KeyboardUp(const unsigned char _key, const int _x, const int _y)
	{
		
	}

	void SpecialKeys(const int _key, const int _x, const int _y)
	{
		std::vector<char> send{ 'D' };
		switch (_key)
		{
		case GLUT_KEY_UP:
			send.push_back('U');
			break;

		case GLUT_KEY_DOWN:
			send.push_back('D');
			break;

		case GLUT_KEY_LEFT:
			send.push_back('L');
			break;

		case GLUT_KEY_RIGHT:
			send.push_back('R');
			break;
		}

		network_mgr.doSend(send);
	}

	void SpecialKeysUp(const int _key, const int _x, const int _y)
	{
		std::vector<char> send{ 'U' };
		switch (_key)
		{
		case GLUT_KEY_UP:
			send.push_back('U');
			break;
		case GLUT_KEY_DOWN:
			send.push_back('D');
			break;
		case GLUT_KEY_LEFT:
			send.push_back('L');
			break;
		case GLUT_KEY_RIGHT:
			send.push_back('R');
			break;
		}
		network_mgr.doSend(send);
	}

	// --
	// process func
	// --

	void processMessage()
	{
		std::vector<char> message{ network_mgr.popMessage() };

		while (!message.empty()) {
			int id = static_cast<int>(message[1]);

			if (queen_table.find(id) == queen_table.end()) {
				queen_table[id] = Queen{};
			}

			Queen& target = queen_table[id];

			switch (message[2]) {
			case 'Q':
				PostQuitMessage(0);
				break;
			case 'M':
				target.move(message[3], message[4]);
				break;
			default:
				break;
			}
			message = network_mgr.popMessage();
		}
	}

	void updateState()
	{

	}


	void run()
	{
		glutMainLoop();
	}

};

CWindowMgr window_mgr;


// (CALLBACK) Display screen
GLvoid cb::Display()
{
	window_mgr.Display();
}

// (CALLBACK) Reset Viewport
GLvoid cb::Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

// (CALLBACK) Mouse click event
GLvoid cb::Mouse(int button, int state, int x, int y)
{
	window_mgr.Mouse(button, state, x, y);
}

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	window_mgr.Keyboard(key, x, y);
}

GLvoid cb::Motion(int x, int y)
{
	window_mgr.Motion(x, y);
}

GLvoid cb::SpecialKeys(int key, int x, int y)
{
	window_mgr.SpecialKeys(key, x, y);
}

GLvoid cb::SpecialKeysUp(int key, int x, int y)
{
	window_mgr.SpecialKeysUp(key, x, y);
}

GLvoid cb::KeyboardUp(unsigned char key, int x, int y)
{
	window_mgr.KeyboardUp(key, x, y);
}

// (CALLBACK) Main Loop
GLvoid cb::GameLoop(int value)
{
	SleepEx(0, TRUE);
	// process
	window_mgr.processMessage();

	// Game State Update
	window_mgr.updateState();

	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}

int main(int argc, char** argv)
{
	window_mgr.init(argc, argv);
	window_mgr.run();
}