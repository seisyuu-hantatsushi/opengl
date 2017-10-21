#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GL/glx.h>

#include "shaderprogramset.h"

extern char _binary_cube_vert_start[];
extern char _binary_cube_vert_end[];
extern char _binary_cube_vert_size[];

int main(int argc, char *argv[]){
	GLenum glResult;
	GLFWwindow *pWindow = NULL;

	ShaderProgramSetHandler cubeShader = NULL;

	if(!glfwInit()){
		return 0;
	}

	pWindow = glfwCreateWindow(640, 480, "Cube", NULL, NULL);

	glfwMakeContextCurrent(pWindow);

	glResult = glewInit();
	if(glResult != GLEW_OK){
		printf("Error: %s\n", glewGetErrorString(glResult));
		return 0;
	}

	printf("VENDOR=%s\n", glGetString(GL_VENDOR));
	printf("GPU=%s\n", glGetString(GL_RENDERER));
	printf("OpenGL=%s\n", glGetString(GL_VERSION));
	printf("GLSL=%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	_binary_cube_vert_end[0] = '\0';
	printf("%s", &_binary_cube_vert_start[0]);

	shaderProgramSetCreate(&cubeShader, &_binary_cube_vert_start[0], NULL);

	while(!glfwWindowShouldClose(pWindow)){
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgramGetProgram(cubeShader));
		glBegin(GL_TRIANGLES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex2d(0.9*cos(0.0), 0.9*sin(0.0));
		glColor3d(0.0, 1.0, 0.0);
		glVertex2d(0.9*cos(2.0*M_PI/3.0), 0.9*sin(2.0*M_PI/3.0));
		glColor3d(0.0, 0.0, 1.0);
		glVertex2d(0.9*cos(2.0*M_PI*2.0/3.0), 0.9*sin(2.0*M_PI*2.0/3.0));
		glEnd();
		glUseProgram(0);
		glFlush();
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	shaderProgramSetDestroy(cubeShader);

 error_destroy_window:
	glfwDestroyWindow(pWindow);

 error_exit:
	glfwTerminate();
	return 0;

}
