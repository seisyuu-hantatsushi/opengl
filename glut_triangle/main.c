#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3d(1.0, 0.0, 0.0);
 	glVertex2d(0.9*cos(0.0), 0.9*sin(0.0));
	glColor3d(0.0, 1.0, 0.0);
	glVertex2d(0.9*cos(2.0*M_PI/3.0), 0.9*sin(2.0*M_PI/3.0));
	glColor3d(0.0, 0.0, 1.0);
	glVertex2d(0.9*cos(2.0*M_PI*2.0/3.0), 0.9*sin(2.0*M_PI*2.0/3.0));
	glEnd();
	glFlush();
	return;
}

void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	return;
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	init();
	glutMainLoop(); 
	return 0;
}
