#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GL/glx.h>

static GLuint s_vertexShader; 

int readShaderCompile(GLuint shader, const char *pFile){
	int ret = 0;
	FILE *fp = NULL;
	char *pBuf;

	GLsizei size,len;
	GLint compiled;

	fp = fopen(pFile, "rb");
	if(fp == NULL){
		printf("failed to open file. %s. <%s>\n", pFile, strerror(errno));
		return -1;
	}

	//get file size
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	pBuf = (GLchar*)malloc(size);
	if(pBuf == NULL){
		printf("out of memory\n");
		ret = -1;
		goto error_exit;
	}
	
	fseek(fp,0,SEEK_SET);
	fread(pBuf, 1, size, fp);
	glShaderSource(shader, 1, (const GLchar**)&pBuf, &size);
	free(pBuf);

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE){
		printf("failed to compile %s\n", pFile);
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &size);
		if(size > 0){
			pBuf = (char *)malloc(size);
			glGetShaderInfoLog(shader, size, &len, pBuf);
			printf("%s\n", pBuf);
			free(pBuf);
		}
		ret = -1;
		goto error_exit;
	}
	
 error_exit:
	fclose(fp);

	return ret;
}


int main(int argc, char *argv[]){
	GLenum glResult;
	GLFWwindow *pWindow = NULL;
	GLuint program,linked;
	
	if(!glfwInit()){
		return 0;
	}
	
	pWindow = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);

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
	
	s_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	printf("s_vertexShader = %d\n", s_vertexShader);

	readShaderCompile(s_vertexShader, "triangle.vert");

	program = glCreateProgram();
	if(program == 0){
		printf("Failed to create program\n");
		goto error_destroy_window;
	}

	glAttachShader(program, s_vertexShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if(linked == GL_FALSE){
		char *pInfoLog;
		GLsizei size,len;
		printf("failed to link shader\n");
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &size );
		if ( size > 0 ) 
		{
			pInfoLog = (char *)malloc(size);
			glGetProgramInfoLog( program, size, &len, pInfoLog );
			printf("%s\n",pInfoLog);
			free(pInfoLog);
		}	
	}
	
	while(!glfwWindowShouldClose(pWindow)){
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
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

	glDeleteProgram(program);
	
 error_destroy_window:
	glfwDestroyWindow(pWindow);

	glDeleteShader(s_vertexShader);
	
 error_exit:	
	glfwTerminate();
	return 0;
}
