#pragma once
class FiEngine {
	FiEngine();
	virtual ~FiEngine();

	static FiEngine* _instance;

public:
	bool init();
	void handleInput();
	void mainLoop();
	void end();

	static FiEngine* getInstance();
protected:
	virtual void draw();

private:


};