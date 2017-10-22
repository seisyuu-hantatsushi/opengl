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
	int32_t ret;
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

	ret = shaderProgramSetCreate(&cubeShader, &_binary_cube_vert_start[0], NULL);
	if(ret < 0){
		goto error_destroy_window;
	}

	while(!glfwWindowShouldClose(pWindow)){
		int view_width,view_height;

		glfwGetFramebufferSize(pWindow, &view_width, &view_height);
		glViewport(0, 0, view_width, view_height);


		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, (double)view_width / (double)view_height, 1.0, 100.0); //視野の設定
		  
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
		//gluLookAt(0.0,1.5,0.0,0.0,0.0,0.0,0.0,0.0,1.0);

		
		glUseProgram(shaderProgramGetProgram(cubeShader));

		glBegin(GL_LINES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(-2.0, 0.0, 0.0);
		glVertex3d(2.0, 0.0, 0.0);
		glColor3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, -2.0, 0.0);
		glVertex3d(0.0, 2.0, 0.0);
		glColor3d(0.0, 0.0, 1.0);
		glVertex3d(0.0, 0.0, 2.0);
		glVertex3d(0.0, 0.0, -2.0);
		
		glEnd();
		
		glBegin(GL_TRIANGLES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(1.0, 0.0, 1.0);
		glVertex3d(1.0, 0.0, 0.0);

		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(1.0, 0.0, 0.0);
		glVertex3d(1.0, 1.0, 0.0);

		glEnd();

		glUseProgram(0);
		glFlush();
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	printf("destroy program shader\n");
	shaderProgramSetDestroy(cubeShader);

 error_destroy_window:
	glfwDestroyWindow(pWindow);

 error_exit:
	glfwTerminate();
	return 0;

}
