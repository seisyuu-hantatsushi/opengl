#include <math.h>
#include <GLFW/glfw3.h>

int main(int argc, char *argv[]){
	GLFWwindow *pWindow = NULL;

	if(!glfwInit()){
		return 0;
	}

	pWindow = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);

	glfwMakeContextCurrent(pWindow);
	
	while(!glfwWindowShouldClose(pWindow)){
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
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	glfwDestroyWindow(pWindow);

 error_exit:	
	glfwTerminate();
	return 0;
}
