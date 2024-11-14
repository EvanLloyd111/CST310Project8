#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::radians
#include <cmath>                        // For sin and cos functions
#include <SOIL/SOIL.h>  // Include for texture loading
#include <iostream>



float arrowPosition = -1.5f; // Starting position of the arrow
float arrowSpeed = 0.02f;    // Speed of the arrow's movement
bool arrowMoving = true;     // Flag to control arrow movement

bool barActive = false; // Flag to activate the red bar after the arrow stops
bool barMoving = true;  // Flag to control the red bar's movement
float barPosition = 0.0f; // Initial position of the red bar
bool barDirection = true; // True for moving up, false for moving down
float barSpeed = 0.02f; // Initial speed for the red bar's movement

bool ballLaunched = false;      // Tracks if the ball has been launched
glm::vec3 ballVelocity(0.0f);   // Velocity vector for the ball
float launchSpeed = 7.0f;        // Maximum speed for the ball launch
glm::vec3 ballPosition(0.0f, 0.2f, -2.0f); // Initial position of the ball

// Goal boundaries (adjust if necessary)
const float goalMinX = -1.5f; // Left side of the goal
const float goalMaxX = 1.5f;  // Right side of the goal
const float goalZ = -10.0f;   // Depth of the goal

float goalieX = 0.0f;
float goalieSpeed = 0.05f;
bool movingRight = true;

bool checkGoal() {
    // Check if the ball is within the x boundaries of the goal
    if (ballPosition.x > goalMinX && ballPosition.x < goalMaxX) {
        // Check if the ball has passed the goal on the z-axis
        if (ballPosition.z < goalZ) {
            return true;
        }
    }
    return false;
}

void setupLighting() {
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat lightPosition[] = { 0.0f, 10.0f, 5.0f, 1.0f };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

GLuint grassTexture;


void loadTexture() {
    // Load texture from file
    grassTexture = SOIL_load_OGL_texture(
        "grass_texture.png",   // Path to your grass texture file
        SOIL_LOAD_AUTO,        // Automatically detect format
        SOIL_CREATE_NEW_ID,    // Create new texture ID
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y  // Generate mipmaps and invert Y-axis for correct texture orientation
    );

    if (grassTexture == 0) {
        std::cerr << "Error loading texture: " << SOIL_last_result() << std::endl;
        return;
    }

    // Set texture parameters
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void drawField() {
    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    glBindTexture(GL_TEXTURE_2D, grassTexture);  // Bind the grass texture

    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white to avoid tinting the texture
    glBegin(GL_QUADS);
    // Define the vertices and texture coordinates for the field
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0f, 0.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0f, 0.0f, -10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0f, 0.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0f, 0.0f, 10.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D); // Disable 2D texturing
}


void drawBall() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the ball
    glPushMatrix();
    glTranslatef(ballPosition.x, ballPosition.y, ballPosition.z); // Use ballPosition for dynamic movement
    glutSolidSphere(0.2f, 20, 20); // Small sphere for ball
    glPopMatrix();
}

void drawGoalPosts() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for goalposts
    float postWidth = 0.1f;
    float postHeight = 1.5f;
    float goalDepth = 0.5f;
    
    glPushMatrix();
    glTranslatef(-1.0f, postHeight / 2, -10.0f); // Left post
    glScalef(postWidth, postHeight, postWidth);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0f, postHeight / 2, -10.0f); // Right post
    glScalef(postWidth, postHeight, postWidth);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, postHeight, -10.0f); // Top post
    glScalef(2.0f, postWidth, postWidth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawGoalBox() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for goal box rectangles
    float thickness = 0.05f; // Adjust thickness as desired

    // Front line (rectangle)
    glBegin(GL_QUADS);
    glVertex3f(-1.5f, 0.1f, -8.0f);            // Bottom left
    glVertex3f(1.5f, 0.1f, -8.0f);             // Bottom right
    glVertex3f(1.5f, 0.1f + thickness, -8.0f); // Top right
    glVertex3f(-1.5f, 0.1f + thickness, -8.0f);// Top left
    glEnd();

    // Left line (rectangle)
    glBegin(GL_QUADS);
    glVertex3f(-1.5f, 0.1f, -8.0f);              // Front bottom
    glVertex3f(-1.5f, 0.1f, -10.0f);             // Back bottom
    glVertex3f(-1.5f + thickness, 0.1f, -10.0f); // Back top
    glVertex3f(-1.5f + thickness, 0.1f, -8.0f);  // Front top
    glEnd();

    // Right line (rectangle)
    glBegin(GL_QUADS);
    glVertex3f(1.5f, 0.1f, -8.0f);               // Front bottom
    glVertex3f(1.5f, 0.1f, -10.0f);              // Back bottom
    glVertex3f(1.5f - thickness, 0.1f, -10.0f);  // Back top
    glVertex3f(1.5f - thickness, 0.1f, -8.0f);   // Front top
    glEnd();
}

void drawArrow() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the arrow
    glPushMatrix();
    glTranslatef(0.1f, 0.2f, -2.1f); // Position the back of the arrow directly in front of the ball

    glBegin(GL_TRIANGLES);

    // Back of the arrow (fixed position)
    glVertex3f(-0.1f, 0.0f, 0.0f); 

    // Moving front of the arrow (moves along x-axis for left-right movement)
    glVertex3f(arrowPosition, 0.2f, -0.5f);   // Top front
    glVertex3f(arrowPosition, -0.1f, -0.5f);  // Bottom front

    glEnd();
    glPopMatrix();
}

void drawRedBar() {
    if (barActive) {
        // Draw the outer box
        glColor3f(1.0f, 1.0f, 1.0f); // White box
        glPushMatrix();
        glTranslatef(-1.5f, 0.5f, -2.1f); // Position to the left of the ball
        glScalef(0.2f, 0.5f, 0.1f);
        glutWireCube(1.0f);
        glPopMatrix();

        // Draw the moving red bar
        glColor3f(1.0f, 0.0f, 0.0f); // Red color for the bar
        glPushMatrix();
        glTranslatef(-1.5f, 0.5f + barPosition, -2.1f); // Center the bar inside the box vertically
        glScalef(0.1f, 0.1f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}

void launchBall() {
    // Calculate direction based on the arrow\u2019s final position
    float angle = arrowPosition * glm::radians(45.0f); // Convert to a 45-degree range
    ballVelocity.x = std::sin(angle);
    ballVelocity.z = -std::cos(angle);

    // Set fixed horizontal speed
    ballVelocity.x *= launchSpeed;
    ballVelocity.z *= launchSpeed;

    // Determine vertical component based on bar position
    float normalizedBarPosition = (barPosition + 0.2f) / 0.4f; // Normalize barPosition to [0, 1]
    
    // If in the bottom 75% of the bar, set a low enough height for a goal
    if (normalizedBarPosition <= 0.75f) {
        ballVelocity.y = 1.0f * normalizedBarPosition; // Low height for scoring
    } else {
        ballVelocity.y = 1.5f; // High height for missing the goal
    }
}

void reset() {
    // Reset ball position and velocity
    ballPosition = glm::vec3(0.0f, 0.2f, -2.0f); // Initial position of the ball
    ballVelocity = glm::vec3(0.0f);              // No movement initially
    ballLaunched = false;                        // Ball has not been launched

    // Reset arrow position and movement
    arrowPosition = -1.5f;    // Initial position of the arrow
    arrowMoving = true;       // Arrow should be moving initially

    // Reset the red bar state
    barActive = false;        // Bar is initially inactive
    barMoving = true;         // Red bar moves once active
    barPosition = 0.0f;       // Start position of the red bar
    barDirection = true;      // Bar starts moving up
}

void drawGoalie() {
    glPushMatrix();
    glTranslatef(goalieX, 0.0f, -9.0f); // Position the goalie in front of the goal

    // Draw the legs (black rectangle)
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);  // Position the legs lower
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for legs
    glScalef(1.0f, 1.0f, 0.2f); // Scale to make it a thin rectangle
    glutSolidCube(0.5); // Draw the legs
    glPopMatrix();

    // Draw the jersey (green and white square)
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);  // Position the jersey higher
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for the jersey
    glScalef(1.0f, 1.0f, 0.4f); // Scale for jersey thickness
    glutSolidCube(0.5); // Draw the jersey
    glPopMatrix();

    // Add a white stripe for the jersey
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.3f);  // Position the stripe on the chest
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the stripe
    glScalef(1.0f, 0.2f, 0.2f); // Thin stripe
    glutSolidCube(0.5); // Draw the white stripe
    glPopMatrix();

    // Draw the head (skin-colored square)
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);  // Position the head higher
    glColor3f(1.0f, 0.8f, 0.6f); // Skin color for the head
    glutSolidCube(0.4); // Draw the head
    glPopMatrix();

    glPopMatrix();
}


void updateGoalie(int value) {
    if (movingRight) {
        goalieX += goalieSpeed;
        if (goalieX > 1.0f) { // Change direction at the right boundary
            movingRight = false;
        }
    } else {
        goalieX -= goalieSpeed;
        if (goalieX < -1.0f) { // Change direction at the left boundary
            movingRight = true;
        }
    }
    glutPostRedisplay(); // Request a redraw
    glutTimerFunc(16, updateGoalie, 0); // Call this function again after 16ms
}

void update(int value) {
    if (!ballLaunched) {
        if (arrowMoving) {
            // Arrow movement logic (no changes)
            arrowPosition += arrowSpeed;
            if (arrowPosition >= 0.5f) {
                arrowPosition = 0.5f;
                arrowSpeed = -arrowSpeed;
            } else if (arrowPosition <= -0.5f) {
                arrowPosition = -0.5f;
                arrowSpeed = -arrowSpeed;
            }
        } else if (barActive && barMoving) {
            // Bar movement logic using barSpeed
            if (barDirection) {
                barPosition += barSpeed; // Move up
                if (barPosition >= 0.2f) {
                    barPosition = 0.2f;
                    barDirection = false;
                }
            } else {
                barPosition -= barSpeed; // Move down
                if (barPosition <= -0.2f) {
                    barPosition = -0.2f;
                    barDirection = true;
                }
            }
        }
    } else {
        // Ball movement logic after launch
        ballPosition += ballVelocity * 0.016f; // Adjust 0.016 for a smooth frame-rate independent speed

        // Check if the ball has passed through the goal
        if (checkGoal()) {
            // Increase speed by 25%
            arrowSpeed *= 1.25f;
            barSpeed *= 1.15f;

            // Reset the scene
            reset();
        }
    }

    glutPostRedisplay();           // Request display update
    glutTimerFunc(16, update, 0);  // 60 FPS update rate
}


void handleMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!barActive) {
            // Stop arrow movement and activate the red bar
            arrowMoving = false;
            barActive = true;
            barMoving = true; // Ensure the bar starts moving once activated
        } else if (!ballLaunched) {
            if (barMoving) {
                // Toggle the red bar's movement on left-click without reversing direction
                barMoving = !barMoving;
            } else {
                // Final click to launch the ball
                ballLaunched = true;
                launchBall();
            }
        }
    }
}


void handleKeyboard(unsigned char key, int x, int y) {
    if (key == 'R' || key == 'r') {
        reset();              // Call reset to reinitialize the game state
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);
    loadTexture();  // Load texture when the program starts

    drawField();
    drawBall();
    drawGoalPosts();
    
    glDisable(GL_LIGHTING); // Temporarily disable lighting for color consistency
    drawGoalBox();
    drawArrow();
    drawRedBar(); // Draw the red bar when active
    drawGoalie(); // Draw the moving goalie

    glEnable(GL_LIGHTING);  // Re-enable lighting if necessary for other objects

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 50.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);

    glutCreateWindow("3D Soccer Field with Moving Arrow");

    setupLighting();
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(handleMouse);      // Register mouse event handler
    glutKeyboardFunc(handleKeyboard); // Register keyboard event handler
    glutTimerFunc(16, update, 0);    // Start the update loop for arrow movement
    glutTimerFunc(16, updateGoalie, 0); // Start the update loop for goalie movement

    glutMainLoop();

    return 0;
}
