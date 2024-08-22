#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Variables to control cube's rotation and mouse interaction
bool mousePressed = false;
double lastX = 0.0, lastY = 0.0;
float rotationX = 0.0f, rotationY = 0.0f;
float cubeAngleX = 0.0f, cubeAngleY = 0.0f;
float velocityX = 0.0f, velocityY = 0.0f;  // Velocity for inertia effect
float frictionWhilePressed = 0.97f;  // Friction when the mouse is pressed
float frictionWhileNotPressed = 0.99f;  // Friction when the mouse is not pressed

// Score variable
float score = 0.0f;  // The score will increase as the cube rotates

// Perspective projection matrix
void setPerspective(float fovY, float aspect, float zNear, float zFar) {
    float top = zNear * tanf(fovY * 0.5f);
    float right = top * aspect;
    float left = -right;
    float bottom = -top;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
}

// Callback function to adjust the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    setPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

// Mouse button callback for click detection
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY); // Capture initial mouse position
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;

            // Set velocity based on last rotation changes
            velocityX += rotationX * 0.3f; // Adding to existing velocity
            velocityY += rotationY * 0.3f; // Adding to existing velocity

            // Reset rotation deltas
            rotationX = 0.0f;
            rotationY = 0.0f;
        }
    }
}

// Cursor position callback to handle rotation
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mousePressed) {
        double deltaX = xpos - lastX;
        double deltaY = ypos - lastY;

        // Update rotation angles based on mouse movement
        rotationX = (float)deltaY * 0.3f;
        rotationY = (float)deltaX * 0.3f;

        // Update the cube's rotation angles with the current mouse movements
        cubeAngleX += rotationX;
        cubeAngleY += rotationY;

        // Update last mouse position
        lastX = xpos;
        lastY = ypos;
    }
}

// Apply inertia for rotation with friction effects
void applyInertia() {
    // Determine friction based on whether the mouse is pressed
    float currentFriction = mousePressed ? frictionWhilePressed : frictionWhileNotPressed;

    // Increment the score based on rotation speed
    score += (fabs(velocityX) + fabs(velocityY)) * 0.3f;

    // Apply inertia to rotation angles
    cubeAngleX += velocityX;
    cubeAngleY += velocityY;

    // Apply friction (gradually slow down the velocity)
    velocityX *= currentFriction;
    velocityY *= currentFriction;

    // Stop applying inertia when velocity is very low
    if (fabs(velocityX) < 0.001f && fabs(velocityY) < 0.001f) {
        velocityX = 0.0f;
        velocityY = 0.0f;
    }
}

// Render the score on the screen as lines
void renderScore() {
    // Switch to orthographic projection for basic score rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);  // Adjust based on window size

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Set color for the score (white)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Render score as a simple bar or visual representation (example)
    float scoreLength = score * 0.01f;  // The length of the "score bar" increases with score

    glBegin(GL_QUADS);  // Draw a rectangle as a "score bar"
    glVertex2f(10, 10);
    glVertex2f(10 + scoreLength, 10);
    glVertex2f(10 + scoreLength, 20);
    glVertex2f(10, 20);
    glEnd();

    // Restore previous projection and model view matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Render the scene
void renderScene() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load identity matrix to reset transformations
    glLoadIdentity();

    // Set up the camera
    glTranslatef(0.0f, 0.0f, -5.0f);

    // Apply mouse-controlled rotation
    glRotatef(cubeAngleX, 1.0f, 0.0f, 0.0f); // Rotation around X-axis
    glRotatef(cubeAngleY, 0.0f, 1.0f, 0.0f); // Rotation around Y-axis

    // Apply inertia and friction
    applyInertia();

    // Draw the edges of the cube (in white)
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);  // White

    // Front face
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Back face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    // Connecting edges
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();

    // Render the score in the top-left corner
    renderScore();
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Get the primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        std::cerr << "Failed to get primary monitor" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Get the video mode of the monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        std::cerr << "Failed to get video mode of the monitor" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Create a fullscreen window
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Wireframe Cube", monitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initial setup for perspective projection
    setPerspective(45.0f, (float)mode->width / (float)mode->height, 0.1f, 100.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render the scene
        renderScene();

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up GLFW resources before exiting
    glfwTerminate();
    return 0;
}