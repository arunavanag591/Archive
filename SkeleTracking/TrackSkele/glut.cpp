#include "glut.h"
#include "TestTracking.h"

void draw() {
   drawKinectData();
   glutSwapBuffers();
}

void execute() {
    glutMainLoop();
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width,height);
    glutCreateWindow("SkeleTracking");
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
	glewInit();
    return true;
}
