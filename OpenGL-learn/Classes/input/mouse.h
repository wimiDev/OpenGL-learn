#pragma once
#include <GLFW/glfw3.h>

class Mouse {
	Mouse();

	virtual ~Mouse();

	virtual void registerMove(GLFWwindow *window, void(*callback)(double xpos, double ypos));
	virtual void registerScroll(GLFWwindow *window, void(*callback)(double xoffset, double yoffset));
};