#include "WindowToolkit.h"

#include <iostream>

// ===================================================================

Engine::Window::WindowToolkit::WindowToolkit(std::string title, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	:windowTitle(title),windowPosX(x),windowPosY(y),windowWidth(width),windowHeight(height),deltaTime(1.0 / 60.0),lastFrameTime(0.0)
{
}

Engine::Window::WindowToolkit::~WindowToolkit()
{
	for (auto ui : userInterfaces)
	{
		if (ui != NULL)
		{
			delete ui;
		}
	}

	userInterfaces.clear();
}

void Engine::Window::WindowToolkit::setOGLVersion(unsigned int major, unsigned int minor)
{
	oglMajorV = major;
	oglMinorV = minor;
}

void Engine::Window::WindowToolkit::initGlew()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;
}

void Engine::Window::WindowToolkit::initializeOGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
}

void Engine::Window::WindowToolkit::setContextProfile(unsigned int contxtProfile)
{
	contextProfile = contxtProfile;
}

void Engine::Window::WindowToolkit::addUserInterface(Engine::Window::UserInterface * ui)
{
	userInterfaces.push_back(ui);
}

void Engine::Window::WindowToolkit::updateUI()
{
	for (auto ui : userInterfaces)
	{
		ui->render(deltaTime);
	}
}

double Engine::Window::WindowToolkit::getDeltaTime()
{
	return deltaTime;
}