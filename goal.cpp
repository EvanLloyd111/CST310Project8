#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define GL_SILENCE_DEPRECATION

void setupLighting() {
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::radians
#include <cmath>                        // For sin and cos functions


float arrowPosition = -1.5f; // Starting position of the arrow
float arrowSpeed = 0.02f;    // Speed of the arrow's movement
bool arrowMoving = true;     // Flag to control arrow movement

bool barActive = false; // Flag to activate the red bar after the arrow stops
bool barMoving = true;  // Flag to control the red bar's movement
float barPosition = 0.0f; // Initial position of the red bar
bool barDirection = true; // True for moving up, false for moving down

bool ballLaunched = false;      // Tracks if the ball has been launched
glm::vec3 ballVelocity(0.0f);   // Velocity vector for the ball
float maxSpeed = 5.0f;          // Maximum speed for the ball launch
glm::vec3 ballPosition(0.0f, 0.2f, -2.0f); // Initial position of the ball

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

void drawField() {
    glColor3f(0.0f, 0.5f, 0.0f); // red for grass
    glPushMatrix();
    glScalef(10.0f, 0.1f, 20.0f); // Large thin rectangle for the field
    glutSolidCube(1.0f);
    glPopMatrix();
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
            // Bar movement logic (no changes)
            if (barDirection) {
                barPosition += 0.02f; // Move up
                if (barPosition >= 0.2f) {
                    barPosition = 0.2f;
                    barDirection = false;
                }
            } else {
                barPosition -= 0.02f; // Move down
                if (barPosition <= -0.2f) {
                    barPosition = -0.2f;
                    barDirection = true;
                }
            }
        }
    } else {
        // Ball movement logic after launch
        ballPosition += ballVelocity * 0.016f; // Adjust 0.016 for a smooth frame-rate independent speed
    }

    glutPostRedisplay();           // Request display update
    glutTimerFunc(16, update, 0);  // 60 FPS update rate
}

void launchBall() {
    // Calculate direction based on the arrow’s final position
    float angle = arrowPosition * glm::radians(45.0f); // Convert to a 45-degree range
    ballVelocity.x = std::sin(angle);
    ballVelocity.z = -std::cos(angle);

    // Calculate speed based on barPosition in the range [0, maxSpeed]
    float normalizedBarPosition = (barPosition + 0.2f) / 0.4f; // Normalize barPosition to [0, 1]
    float launchSpeed = normalizedBarPosition * maxSpeed;
    
    // Apply speed to direction vector
    ballVelocity *= launchSpeed;
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);

    drawField();
    drawBall();
    drawGoalPosts();
    
    glDisable(GL_LIGHTING); // Temporarily disable lighting for color consistency
    drawGoalBox();
    drawArrow();
    drawRedBar(); // Draw the red bar when active
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
    glutTimerFunc(16, update, 0);    // Start the update loop for arrow movement

    glutMainLoop();

    return 0;
}

