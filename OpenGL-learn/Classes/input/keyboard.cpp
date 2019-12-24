#include "keyboard.h"

KeyBoard::~KeyBoard()
{

}

void KeyBoard::registerKeyboardEvent(GLFWwindow * window, void(*onKeyPree)(unsigned char key), void(*onKeyUp)(unsigned char key)=nullptr)
{
	for (int idx = GLFW_KEY_0; idx <= GLFW_KEY_9; idx++) {
		if (glfwGetKey(window, idx) == GLFW_PRESS)
			onKeyPree(idx);
	}

	for (int idx = GLFW_KEY_A; idx <= GLFW_KEY_Z; idx++) {
		if (glfwGetKey(window, idx) == GLFW_PRESS)
			onKeyPree(idx);
	}

	for (int idx = GLFW_KEY_RIGHT; idx <= GLFW_KEY_UP; idx++) {
		if (glfwGetKey(window, idx) == GLFW_PRESS)
			onKeyPree(idx);
	}

}

