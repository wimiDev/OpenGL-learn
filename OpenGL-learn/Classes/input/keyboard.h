#pragma once
#include <GLFW/glfw3.h>

using namespace std;

class KeyBoard {

	KeyBoard();

	virtual ~KeyBoard();

	public:

		virtual void registerKeyboardEvent(GLFWwindow *window, void(*onKeyPree)(unsigned char key), void(*onKeyUp)(unsigned char key)=nullptr);

};