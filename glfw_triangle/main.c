#include <GLFW/glfw3.h>

int main(int argc, char *argv[]){
	GLFWwindow *pWindow = NULL;

	if(!glfwInit()){
		return 0;
	}

	pWindow = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);
	
	while(!glfwWindowShouldClose(pWindow)){
	}

	glfwDestroyWindow(pWindow);

 error_exit:	
	glfwTerminate();
	return 0;
}
