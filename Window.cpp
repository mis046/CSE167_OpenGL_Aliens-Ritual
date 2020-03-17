#include "Window.h"
#include "Geometry.hpp"
#include "Particle.h"

/* 
 * Declare your variables below. Unnamed namespace is used here to avoid 
 * declaring global or static variables.
 */
namespace
{
    bool toonShadingOn = true;
    bool particleOn = true;

    int particleNumber = 500;
    const float cameraSpeed = 0.05f; // adjust accordingly

    int nextP = 0;
    vector<Geometry*> geometries;
    vector<Transform*> moveL;

    Transform* lines;

	int width, height;
	std::string windowTitle("167 Final Project");

	Cube* cube;
    Object* skybox;
    Transform* alien;
    Transform* alienArmy;
    Transform* duck;


    vector<vec3> linePoints;

    DirectionalLight* directionalLight;
    glm::vec3 dirColor = glm::vec3(0.55f, 0.55f, 0.5f);
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
    GLuint particleShader;
    GLuint phongShader;
    GLuint toonShader;

    vector<Particle*> particles;
    float particleSize = 0.1;
    float gravity = 0.001;

    bool mouseLeftPressed;
    bool mouseRightPressed;

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
    phongShader = LoadShaders("src/phongShader.vert", "src/phongShader.frag");
    toonShader = LoadShaders("src/toonShader.vert", "src/toonShader.frag");
    skyboxProgram = LoadShaders("src/skybox.vert", "src/skybox.frag");
    particleShader = LoadShaders("src/particleShader.vert", "src/particleShader.frag");
    program = toonShader;

	// Check the shader program.
	if (!program || !skyboxProgram || !particleShader || !toonShader || ! phongShader)
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

glm::vec3 Window::randV() {
    return 8.0f * vec3(float(rand() % 200 - 100) / 6000.0, float(rand() % 100) / 1000.0, float(rand() % 200 - 100) / 6000.0);
}

int Window::randLife() {
    return rand() % 1000+1000;
}

bool Window::initializeObjects()
{
    directionalLight = new DirectionalLight(dirDir, dirColor);
    
	cube = new Cube(skyboxProgram);
	// Set cube to be the first to display
	skybox = cube;
    
   
    mat4 I = glm::translate(glm::vec3(0));
    
    alienArmy = new Transform(I);
    duck = new Transform(I);

    // Geometries
    Geometry * alienGeometry = new Geometry("src/obj_smoothAlien.obj", &program);
    Geometry * duckGeometry = new Geometry("src/obj_duck.obj", &program);

    geometries.push_back(alienGeometry);
    geometries.push_back(duckGeometry);

    Material alienMaterial;
    alienMaterial.ambient = glm::vec3(0.05f);
    alienMaterial.diffuse = glm::vec3(1.0f);
    alienMaterial.specular = glm::vec3(0.2f);
    alienMaterial.shininess = 64.0f;
    alienMaterial.color = glm::vec3(0.8, 0.8, 0.8);
    alienGeometry->setMaterial(alienMaterial);
    Material duckMaterial;
    duckMaterial.ambient = glm::vec3(0.05f);
    duckMaterial.diffuse = glm::vec3(1.0f);
    duckMaterial.specular = glm::vec3(0.0f);
    duckMaterial.shininess = 1.0f;
    duckMaterial.color = glm::vec3(1.0, 1.0, 0.0);
    duckGeometry->setMaterial(duckMaterial);
    
    // Transformations
    alien = new Transform(I);
    Transform* body = new Transform(I);
    moveL.push_back(body);
    body->addChild(alienGeometry);
    alien->addChild(body);
    // Army
    for (int x = -2; x <= 2; x += 1) {
        for (int z = -1; z <= 1; z += 1) {
            Transform* clone = new Transform(translate(I, vec3(100*x, 0,  100*z)));
            clone->addChild(alien);
            alienArmy->addChild(clone);
        }
    }
    alienArmy->scale(0.03);
    alienArmy->rotate(glm::vec3(1.0, 0.0, 0.0), -1.55);

    duck->addChild(duckGeometry);
    duck->scale(0.03);
    duck->rotate(glm::vec3(1.0, 0.0, 0.0), -1.55);
    duck->moveTo(glm::vec3(-1, -4, -6));
    
    lines = new Transform(I);
    // Add lines
    vec3 p1(-12, 0, 0);
    vec3 p2(-4, 0, 0);
    vec3 p3(4, 0, 0);
    vec3 p4(-4, 0, 0);
    vec3 p5(-12, 0, 0);

    
    vector<vec3> controlPoints1;
    controlPoints1.push_back(p1);
    controlPoints1.push_back(vec3(-6, 0, 2));
    controlPoints1.push_back(vec3(-6, 0, 4));
    controlPoints1.push_back(p2);
    BezierCurve* l1 = new BezierCurve(controlPoints1, &program);
    lines->addChild(l1);
    vector<vec3> controlPoints2;
    controlPoints2.push_back(p2);
    controlPoints2.push_back(vec3(-2, 0, -2));
    controlPoints2.push_back(vec3(2, 0, 5));
    controlPoints2.push_back(p3);
    BezierCurve* l2 = new BezierCurve(controlPoints2, &program);
    lines->addChild(l2);
    vector<vec3> controlPoints3;
    controlPoints3.push_back(p3);
    controlPoints3.push_back(vec3(2, 0, 2));
    controlPoints3.push_back(vec3(0, 0, 1));
    controlPoints3.push_back(p4);
    BezierCurve* l3 = new BezierCurve(controlPoints3, &program);
    lines->addChild(l3);
    vector<vec3> controlPoints4;
    controlPoints4.push_back(p4);
    controlPoints4.push_back(vec3(-8, 0, 1));
    controlPoints4.push_back(vec3(-6, 0, -5));
    controlPoints4.push_back(p5);
    BezierCurve* l4 = new BezierCurve(controlPoints4, &program);
    lines->addChild(l4);
    vector<vec3> controlPoints5;
    controlPoints5.push_back(p5);
    controlPoints5.push_back(vec3(5, 0, 1));
    controlPoints5.push_back(vec3(1, 0, 5));
    controlPoints5.push_back(p1);
    BezierCurve* l5 = new BezierCurve(controlPoints5, &program);
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
       
    
    for (unsigned int i = 0; i < particleNumber; i++) {
        // Always up. left/right, up/down, in/out
        particles.push_back(new Particle(particleSize, particleShader, randLife(), randV(), gravity, duck->getPos()));
    }
    
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

    for (Particle * p : particles)
        delete p;
    
	// Delete the shader program.
	glDeleteProgram(phongShader);
    glDeleteProgram(toonShader);
    glDeleteProgram(skyboxProgram);
    glDeleteProgram(particleShader);
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
    for (Particle * p : particles) {
        p->update();
        // Check if need to remove it and replace with a new one
        if (p->lifeLeft <= 0.0) {
            p->resetPosition();
            p->velocity = randV();
            p->lifeLeft = randLife();
        }
    }
	// Perform any updates as necessary. 
//	skybox->update();
    
    // Alien Dancing
    for (Transform* t : moveL) {
        t->moveL();
    }

    if (nextP >= linePoints.size()-1) {
        nextP = 0;
    }
    else {
        nextP ++;
    }
    alienArmy->setM(translate(alienArmy->getM(), (linePoints.at(nextP) - alienArmy->getPos())));
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
    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, glm::value_ptr(eye));


    glUniformMatrix4fv((glGetUniformLocation(program, "view")), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv((glGetUniformLocation(program, "projection")), 1, GL_FALSE, glm::value_ptr(projection));

    alienArmy->draw(mat4(1.0f));
    duck->draw(mat4(1.0f));

    // Draw particle
    if (particleOn) {
        for (Particle * p : particles)
            p->draw(projection, view);
    }
    
//    lines->draw(mat4(1.0f));

    glm::vec3 cameraFront = getCameraFront();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        eye += cameraSpeed * getCameraFront();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        eye -= cameraSpeed * getCameraFront();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        eye -= glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        eye += glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;
    }

    view = glm::lookAt(eye, eye + getCameraFront(), up);
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
                case GLFW_KEY_X:
                    alienArmy->scaleUp();
                    break;
                default:
                    break;
            }
        }
        else {
            switch (key)
            {
//            case GLFW_KEY_W:
//                eye += cameraSpeed * getCameraFront();
//                view = glm::lookAt(eye, eye + getCameraFront(), up);
//                break;
//            case GLFW_KEY_S:
//                eye -= cameraSpeed * getCameraFront();
//                view = glm::lookAt(eye, eye + getCameraFront(), up);
//                break;
//            case GLFW_KEY_A:
//                eye -= glm::normalize(glm::cross(getCameraFront(), up)) * cameraSpeed;
//                view = glm::lookAt(eye, eye + getCameraFront(), up);
//                break;
//            case GLFW_KEY_D:
//                    eye += glm::normalize(glm::cross(getCameraFront(), up)) * cameraSpeed;
//                view = glm::lookAt(eye, eye + getCameraFront(), up);
//                break;
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_X:
                alienArmy->scaleDown();
                break;
            case GLFW_KEY_P:
                if (particleOn) {
                    // Delete particles
                    for (Particle * p : particles) {
                        delete p;
                    }
                    particles.clear();
                }
                else {
                    // Initialize particles
                    for (unsigned int i = 0; i < particleNumber; i++) {
                        // Always up. left/right, up/down, in/out
                        particles.push_back(new Particle(particleSize, particleShader, randLife(), randV(), gravity, duck->getPos()));
                    }
                }
                particleOn = !particleOn;
                break;
            case GLFW_KEY_T:
                if (toonShadingOn)
                    program = phongShader;
                else
                    program = toonShader;
                toonShadingOn = !toonShadingOn;
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
     if (button == GLFW_MOUSE_BUTTON_RIGHT) {
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

        const float sensitivity = 0.2f;
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
        alienArmy->setM(temp * alienArmy->getM());
    
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
