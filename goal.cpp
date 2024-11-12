#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define GL_SILENCE_DEPRECATION

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
    glColor3f(0.0f, 0.5f, 0.0f); // Green for grass
    glPushMatrix();
    glScalef(10.0f, 0.1f, 20.0f); // Large thin rectangle for the field
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawBall() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the ball
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, -2.0f); // Near the camera
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
    glColor3f(1.0f, 1.0f, 1.0f); // White lines for the goal box

    glBegin(GL_LINES);
    glVertex3f(-1.5f, 0.05f, -8.0f);
    glVertex3f(1.5f, 0.05f, -8.0f); // Front line

    glVertex3f(-1.5f, 0.05f, -8.0f);
    glVertex3f(-1.5f, 0.05f, -10.0f); // Left line

    glVertex3f(1.5f, 0.05f, -8.0f);
    glVertex3f(1.5f, 0.05f, -10.0f); // Right line

    glVertex3f(-1.5f, 0.05f, -10.0f);
    glVertex3f(1.5f, 0.05f, -10.0f); // Back line
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);

    drawField();
    drawBall();
    drawGoalPosts();
    drawGoalBox();

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
    glutCreateWindow("3D Soccer Field");
    
    setupLighting();
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}

