#include "Window.h"

/* 
 * Declare your variables below. Unnamed namespace is used here to avoid 
 * declaring global or static variables.
 */
namespace
{
    int nextP = 0;
    vector<Geometry*> geometries;
    vector<Transform*> moveL;
    vector<Transform*> moveR;

    Transform* lines;

	int width, height;
	std::string windowTitle("GLFW Starter Project");

	Cube* cube;
    Object* skybox;
    Transform* robot;
    Transform* robotArmy;

    vector<vec3> linePoints;

    DirectionalLight* directionalLight;
    GLuint dirLightColorLocPhong;
    GLuint dirLightDirLocPhong;
    glm::vec3 dirColor = glm::vec3(1.0f, 0.3f, 0.0f);
    glm::vec3 dirDir = glm::vec3(100.0f, 100.0f, 100.0f);

	glm::vec3 eye(0, 5, 20); // Camera position.
	glm::vec3 up(0, 1, 0); // The up direction of the camera.
	float fovy = 60;
	float near = 1;
	float far = 2000;
	glm::mat4 view; // View matrix, defined by eye, center and up.
	glm::mat4 projection; // Projection matrix.

	GLuint program; // The shader program id.

    GLuint skyboxProgram;

    bool mouseLeftPressed, mouseRightPressed;

    // Rotation
     double pressedX; // Pressed location
     double pressedY; // Pressed location
     glm::vec3 lastPoint;
     // Cursor position
     double cursorX;
     double cursorY;

    // Used for turning the camera
    float lastX, lastY;
    float yawW = -90, pitchW;
};

bool Window::initializeProgram()
{
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("src/toonShader.vert", "src/toonShader.frag");
    skyboxProgram = LoadShaders("src/skybox.vert", "src/skybox.frag");

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
    glUseProgram(program);

    view = glm::lookAt(eye, eye + getCameraFront(), up);
    
    
	// Get the locations of uniform variables.
    glUseProgram(skyboxProgram);

    // Dir light
    glUseProgram(program);

	return true;
}

bool Window::initializeObjects()
{
    directionalLight = new DirectionalLight(dirDir, dirColor);
    
	cube = new Cube(skyboxProgram);
	// Set cube to be the first to display
	skybox = cube;
    
   
    mat4 I = glm::translate(glm::vec3(0));
    
    robotArmy = new Transform(I);
    
    // Geometries
    Geometry * bunny = new Geometry("src/sphere.obj", program);
    geometries.push_back(bunny);
    Material m;
    m.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    m.shininess = 2.0f;
    m.color = glm::vec3(1.0, 1.0, 1.0);
    for (Geometry * g : geometries) {
        g->setMaterial(m);
    }
    
    // Offsets
    vec3 arm_offset = vec3(1.2, 0.0, 0.0);
    vec3 arm_scale = vec3(1.25);
    vec3 left_leg_offset = vec3(-0.4, -1.3, 0.0);
    vec3 right_leg_offset = vec3(0.4, -1.3, 0.0);
    vec3 leg_scale = vec3(1.5, 1.5, 1.25);
    vec3 head_offset = vec3(0.0, 1.1, 0.0);
    vec3 head_scale = vec3(1.1);
    vec3 a_l_offset = vec3(-0.4, 1.2, 0.0);
    vec3 a_r_offset = vec3(0.4, 1.2, 0.0);
    vec3 a_scale = vec3(0.5);
    vec3 e_l_offset = vec3(-0.4, 1.8, 0.8);
    vec3 e_r_offset = vec3(0.4, 1.8, 0.8);
    vec3 e_scale = vec3(2.0);
    
    // Transformations
    robot = new Transform(I);
    Transform* body = new Transform(I);
    body->addChild(bunny);
    
    // Connect
    robot->addChild(body);
    
    // Army
    for (int x = -2; x <= 2; x ++) {
        for (int z = -2; z <= 2; z++) {
            Transform* clone = new Transform(translate(I, vec3(- x*3.5, 0,  -z*3.5)));
            clone->addChild(robot);
            robotArmy->addChild(clone);
        }
    }
    
    /*
    lines = new Transform(I);
    // Add lines
    vec3 p1(-12, 0, 0);
    vec3 p2(-4, 4, 0);
    vec3 p3(4, 8, 0);
    vec3 p4(-4, 0, 0);
    vec3 p5(-12, -8, 0);

    
    vector<vec3> controlPoints1;
    controlPoints1.push_back(p1);
    controlPoints1.push_back(vec3(-6, 0, 0));
    controlPoints1.push_back(vec3(-6, 2, 0));
    controlPoints1.push_back(p2);
    BezierCurve* l1 = new BezierCurve(controlPoints1, program);
    lines->addChild(l1);
    vector<vec3> controlPoints2;
    controlPoints2.push_back(p2);
    controlPoints2.push_back(vec3(-2, 0, -2));
    controlPoints2.push_back(vec3(2, 0, 5));
    controlPoints2.push_back(p3);
    BezierCurve* l2 = new BezierCurve(controlPoints2, program);
    lines->addChild(l2);
    vector<vec3> controlPoints3;
    controlPoints3.push_back(p3);
    controlPoints3.push_back(vec3(2, 6, 2));
    controlPoints3.push_back(vec3(0, 6, 0));
    controlPoints3.push_back(p4);
    BezierCurve* l3 = new BezierCurve(controlPoints3, program);
    lines->addChild(l3);
    vector<vec3> controlPoints4;
    controlPoints4.push_back(p4);
    controlPoints4.push_back(vec3(-8, -6, 1));
    controlPoints4.push_back(vec3(-6, 6, -5));
    controlPoints4.push_back(p5);
    BezierCurve* l4 = new BezierCurve(controlPoints4, program);
    lines->addChild(l4);
    vector<vec3> controlPoints5;
    controlPoints5.push_back(p5);
    controlPoints5.push_back(vec3(5, 0, 1));
    controlPoints5.push_back(vec3(1, 0, 5));
    controlPoints5.push_back(p1);
    BezierCurve* l5 = new BezierCurve(controlPoints5, program);
    lines->addChild(l5);
    
    for (vec3 v : l1->getPoints())
        linePoints.push_back(v);
    for (vec3 v : l2->getPoints())
        linePoints.push_back(v);
    for (vec3 v : l3->getPoints())
        linePoints.push_back(v);
    for (vec3 v : l4->getPoints())
        linePoints.push_back(v);
    for (vec3 v : l5->getPoints())
        linePoints.push_back(v);
     */
       
    glUseProgram(skyboxProgram);
    glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
    
    for (Geometry* g : geometries)
        delete g;
    
	delete cube;
    delete directionalLight;
    delete lines;

	// Delete the shader program.
	glDeleteProgram(program);
    glDeleteProgram(skyboxProgram);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int w, int h)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
#ifdef _WIN32
	width = w;
	height = h;
#endif
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, near, far);
}

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	skybox->update();
    
    for (Transform* t : moveL) {
        t->moveL();
    }
    for (Transform* t : moveR) {
        t->moveR();
    }
//
//    if (nextP >= linePoints.size()-1) {
//        nextP = 0;
//    }
//    else {
//        nextP ++;
//    }
//    robotArmy->setM(translate(robotArmy->getM(), (linePoints.at(nextP) - robotArmy->getPos())));
}

void Window::displayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(skyboxProgram);
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	skybox->draw();
    
    glUseProgram(program);

    glUniform3fv(glGetUniformLocation(program, "dirLightColor"), 1, glm::value_ptr(directionalLight->color));
    glUniform3fv(glGetUniformLocation(program, "dirLightDir"), 1, glm::value_ptr(directionalLight->direction));

    glUniformMatrix4fv((glGetUniformLocation(program, "view")), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv((glGetUniformLocation(program, "projection")), 1, GL_FALSE, glm::value_ptr(projection));
    
    robotArmy->draw(mat4(1.0f));
    
//    lines->draw(mat4(1.0f));

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */

	 // Check for a key press.
	if (action == GLFW_PRESS)
	{
        // Uppercase key presses (shift held down + key press)
        if (mods == GLFW_MOD_SHIFT) {
            switch (key) {
                case GLFW_KEY_S:
                    robotArmy->scaleUp();
                    break;
                default:
                    break;
            }
        }
        else {
            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_S:
                robotArmy->scaleDown();
                break;
            default:
                break;
            }
        }
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // cout << "button " << button << " action " << action << " mods " << mods << "\n";
    // Button: 0 left click, 1 right click
    // Action: 1 press down, 0 release
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftPressed = true;
        }
        else if (action == GLFW_RELEASE)
            mouseLeftPressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mouseRightPressed = true;
            pressedX = cursorX;
            pressedY = cursorY;
            lastPoint = trackBallMapping(pressedX, pressedY);
        }
        else if (action == GLFW_RELEASE)
            mouseRightPressed = false;
    }
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    cursorX = xpos;
    cursorY = ypos;
    
    if (mouseLeftPressed) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top

        const float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        yawW   -= xoffset;
        pitchW -= yoffset;
        
        if(pitchW > 89.0f)
          pitchW =  89.0f;
        if(pitchW < -89.0f)
          pitchW = -89.0f;
        
        view = lookAt(eye, eye + getCameraFront(), up);
    }
    else if (mouseRightPressed) {
       float m_ROTSCALE = 0.5f;
        // Trackball here
        glm::vec3 curPoint = trackBallMapping(xpos, ypos); // Map the mouse position to a logical
        glm::vec3 direction = curPoint - lastPoint;
        
        float velocity = length(direction);
        
        // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
        glm::vec3 rotAxis;
        rotAxis = cross( lastPoint, curPoint );
//        cout << lastPoint.x << " " << lastPoint.y << " " << lastPoint.z << "\n";
//        cout << curPoint.x << " " << curPoint.y << " " << curPoint.z << "\n";
//        cout << "\n";
        
        float rot_angle = velocity * m_ROTSCALE;
        
        glm::mat4 temp = glm::rotate(mat4(1.0f), rot_angle, rotAxis);
        robotArmy->setM(temp * robotArmy->getM());
    
        lastPoint = curPoint;
        
        pressedX = xpos;
        pressedY = ypos;
    }
    
    lastX = xpos;
    lastY = ypos;
    
}

glm::vec3 Window::getCameraFront() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yawW)) * cos(glm::radians(pitchW));
    direction.y = sin(glm::radians(pitchW));
    direction.z = sin(glm::radians(yawW)) * cos(glm::radians(pitchW));
    glm::vec3 cameraFront = glm::normalize(direction);
    
    return cameraFront;
}

glm::vec3 Window::trackBallMapping(float pointX, float pointY) {
    float realWidth = width / 2.0f;
    float realHeight = height / 2.0f;
    
//    cout << "width " << realWidth << " height " << realHeight << "\n";
//    cout << "pointX " << pointX << " pointY " << pointY << "\n";
    
    glm::vec3 v;
    float d;
    v.x = (2.0f*pointX - realWidth) / realWidth;
    v.y = (realHeight - 2.0f*pointY) / realHeight;
    v.z = 0.0;
    d = length(v);
//    cout << "distance " << d << "\n";
    d = (d<1.0) ? d : 1.0;
    v.z = sqrtf(1.001 - d*d);
    normalize(v); // Still need to normalize, since we only capped d, not v.
//    cout << "x " << v.x << " y " << v.y << " z " << v.z <<  "\n";
    return v;
}
