#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GL/glx.h>

#include "shaderprogramset.h"

struct ShaderProgramSet {
	GLuint vertShaderId;
	GLuint fragShaderId;
	GLuint programId;
};

static int32_t printShaderCompileInfo(GLuint shaderId) {
	GLint status = 0;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		GLint count;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &count);
		if(count > 0){
			GLchar *pErrorStr = malloc(sizeof(GLchar)*(count+1));
			if(pErrorStr != NULL){
				glGetShaderInfoLog(shaderId, count, NULL, pErrorStr);
				printf("Shader Compile Status\n");
				printf("%s\n", pErrorStr);
				free(pErrorStr);
			}
		}
		return -1;
	}
	else {
		printf("compiled shader source.\n");
	}

	return 0;
}

static int32_t createShader(GLenum shaderType, const char *pShaderSrc, GLuint *pShaderId) {
	int32_t ret;
	GLuint shaderId;
	GLenum glErr;

	shaderId = glCreateShader(shaderType);
	if(shaderId == 0){
		return -1;
	}

	glShaderSource(shaderId, 1, &pShaderSrc, NULL);
	glErr = glGetError();
	if(glErr != GL_NO_ERROR){
		printf("load shader code to shader. %s\n", gluErrorString(glErr));
		ret = -1;
		goto error_exit;
	}

	glCompileShader(shaderId);
	ret = printShaderCompileInfo(shaderId);
	if(ret < 0){
		goto error_exit;
	}

	*pShaderId = shaderId;
	return 0;

 error_exit:
	glDeleteShader(shaderId);
	return ret;
}

static int32_t deleteShader(GLuint shaderId){
	glDeleteShader(shaderId);
	return 0;
}

static int32_t printShaderLinkInfo(GLuint programId){
	GLint status = 0;

	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		GLint count;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &count);
		if(count > 0){
			GLchar *pErrorStr = (GLchar *)malloc(sizeof(GLchar)*(count+1));
			if(pErrorStr != NULL){
				glGetProgramInfoLog(programId, count, NULL, pErrorStr);
				fprintf(stderr, "Program Link Status\n");
				fprintf(stderr, "%s\n", pErrorStr);
				free(pErrorStr);
			}
		}
		return -1;
	}
	else {
		printf("program linked\n");
	}

	return 0;
}

static int32_t createProgramObject(struct ShaderProgramSet *pSet){
	int32_t ret;
	GLuint programId = 0;

	programId = glCreateProgram();
	if(programId == 0){
		printf("Failed to create program. %s\n", gluErrorString(glGetError()));
		goto error_exit;
	}

	if(pSet->vertShaderId != 0){
		glAttachShader(programId, pSet->vertShaderId);
	}

	if(pSet->fragShaderId != 0){
		glAttachShader(programId, pSet->fragShaderId);
	}

	glLinkProgram(programId);
	ret = printShaderLinkInfo(programId);
	if(ret < 0){
		goto error_exit;
	}

	pSet->programId = programId;

	return 0;

 error_exit:

	if (programId != 0){
		glDeleteProgram(programId);
	}

	return -1;
}

static int32_t deleteProgramObject(struct ShaderProgramSet *pSet){

	if(pSet->programId != 0){
		glDeleteProgram(pSet->programId);
		pSet->programId = 0;
	}

	return 0;
}


int32_t shaderProgramSetCreate(ShaderProgramSetHandler *pHandler, const char *pVertShaderSrc, const char *pFragShaderSrc){
	int32_t ret;
	struct ShaderProgramSet *pNewSet = NULL;

	pNewSet = (struct ShaderProgramSet *)malloc(sizeof(struct ShaderProgramSet));
	if(pNewSet == NULL){
		return -1;
	}
	memset(pNewSet, 0x00, sizeof(*pNewSet));

	// vertex shaderを作成
	if(pVertShaderSrc != NULL){
		ret = createShader(GL_VERTEX_SHADER, pVertShaderSrc, &pNewSet->vertShaderId);
		if(ret < 0){
			goto error_exit;
		}
	}

	// fragment shaderを作成
	if(pFragShaderSrc != NULL){
		ret = createShader(GL_FRAGMENT_SHADER, pFragShaderSrc, &pNewSet->fragShaderId);
	}

	// shader programを作成
	ret = createProgramObject(pNewSet);
	if(ret < 0){
		goto error_exit;
	}

	*pHandler = (ShaderProgramSetHandler*)pNewSet;

	return 0;

 error_exit:
	if(pNewSet->vertShaderId != 0){
		deleteShader(pNewSet->vertShaderId);
	}

	if(pNewSet->fragShaderId != 0){
		deleteShader(pNewSet->fragShaderId);
	}

	free(pNewSet);
	return -1;
}

int32_t shaderProgramSetDestroy(ShaderProgramSetHandler handler){
	int32_t ret;
	struct ShaderProgramSet *pSet =
		(struct ShaderProgramSet *)handler;

	deleteProgramObject(pSet);

	if(pSet->fragShaderId != 0){
		deleteShader(pSet->fragShaderId);
		pSet->fragShaderId = 0;
	}

	if(pSet->vertShaderId != 0){
		deleteShader(pSet->vertShaderId);
		pSet->vertShaderId = 0;
	}

	free(pSet);

	return -1;
}

GLuint shaderProgramGetProgram(ShaderProgramSetHandler handler){
	struct ShaderProgramSet *pSet =
		(struct ShaderProgramSet *)handler;
	if(handler == NULL){
		return 0;
	}
	return pSet->programId;
}
