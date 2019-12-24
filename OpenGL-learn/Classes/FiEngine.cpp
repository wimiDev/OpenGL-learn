#include "FiEngine.h"

FiEngine::~FiEngine()
{

}

FiEngine * FiEngine::getInstance()
{
	if (_instance == nullptr) {
		_instance = new FiEngine();
	}
	return _instance;
}

bool FiEngine::init()
{
	return false;
}

void FiEngine::handleInput()
{
}

void FiEngine::draw()
{
}

void FiEngine::mainLoop()
{
}

void FiEngine::end()
{
}
