#include "view_window.h"



BaseViewWindow::BaseViewWindow(
	int width,
	int height
) : _width(width), _height(height) {
	_mapMovementKeys();
	
	_cam = Camera(
		vec3({ -1,0,0 }),
		vec3({ 250,0,0 }),
		_width,
		_height,
		PI / 3);
	_cam_manager.attach(&_cam);
}

void BaseViewWindow::launch(const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	_is_running = true;
	_main_thread = std::thread(&BaseViewWindow::_windowProgram, this, title, monitor, share);
}

void BaseViewWindow::close()
{
	glfwSetWindowShouldClose(_window, true);
	return;
}

void BaseViewWindow::_windowProgram(const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
	}

	_window = glfwCreateWindow(_width, _height, title, monitor, share);
	if (!_window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
	}
	glfwSetWindowPos(_window,0,0);
	glfwMakeContextCurrent(_window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, _keyCallback);

	_enableMouseControls();
	//Center cursor so camera does not jerk on startup
	glfwSetCursorPos(_window,0,0);
	glfwSetCursorPosCallback(_window, _cursorPosCallback);
	_cam_manager.start();

	const GLubyte* _renderer = glGetString(GL_RENDERER);
	const GLubyte* _version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", _renderer);
	printf("OpenGL version supported %s\n", _version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	_main();

	_main_thread.detach();
	_cam_manager.stop();
	_is_running = false;
	glfwDestroyWindow(_window);
	glfwTerminate();

	
	return;
}

void BaseViewWindow::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto win = static_cast<BaseViewWindow*>(glfwGetWindowUserPointer(window));

	win->_key_manager.callKeyFunc(key, action);
}

void BaseViewWindow::_cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	auto win = static_cast<BaseViewWindow*>(glfwGetWindowUserPointer(window));
	win->_cam_manager.rotate(xpos, ypos);
}

void BaseViewWindow::_enableMouseControls()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else return;

}

void BaseViewWindow::_mapMovementKeys()  {
	//map_keys is to condense things
	auto map_keys = [this](int action) {
		//mval determines whether motion in a direction should start or stop
		int mval = (action == GLFW_PRESS) - (action == GLFW_RELEASE);
		vec3& dir = this->_cam_manager.motion_dir;
		this->_key_manager.mapKey(GLFW_KEY_W, action, [&dir, mval]() {*dir[2] += +mval; });
		this->_key_manager.mapKey(GLFW_KEY_A, action, [&dir, mval]() {*dir[0] += -mval; });
		this->_key_manager.mapKey(GLFW_KEY_S, action, [&dir, mval]() {*dir[2] += -mval; });
		this->_key_manager.mapKey(GLFW_KEY_D, action, [&dir, mval]() {*dir[0] += +mval; });
		this->_key_manager.mapKey(GLFW_KEY_LEFT_SHIFT, action, [&dir, mval]() {*dir[1] += -mval; });
		this->_key_manager.mapKey(GLFW_KEY_SPACE, action, [&dir, mval]() {*dir[1] += mval; });
	};
	map_keys(GLFW_PRESS);
	map_keys(GLFW_RELEASE);
	return;
}

void KeyManager::callKeyFunc(int key, int action)
{
	pair<int, int> keyaction({ key,action });

	if (keymap.contains(keyaction)) keymap.at(keyaction)();

	return;
}

void CameraManager::_update_loop()
{
	while (true) {
		if (_should_close) return;
		_cam->translate(motion_dir * movespeed);
	}
	return;
}

CameraManager::~CameraManager()
{
	_should_close = true;
	_updater_thread.detach();
}

void CameraManager::start()
{
	_should_close = false;
	motion_dir = { 0,0,0 };
	_cursor_pos = { 0,0 };
	_updater_thread = thread(&CameraManager::_update_loop, this);
	return;
}

void CameraManager::stop()
{
	_should_close = true;
	_updater_thread.detach();
	_cam->reset();
	return;
}

void CameraManager::rotate(double x_new, double y_new)
{
	double dx = (_cursor_pos[0][0] - x_new) * camspeed;
	double dy = (_cursor_pos[0][1] - y_new) * camspeed;
	_cursor_pos = { (float)x_new,(float)y_new };
	_cam->rotate(dy, dx);
	return;
}

