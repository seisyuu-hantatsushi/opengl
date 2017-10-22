#ifndef _SHADER_PROGRAM_SET_H_
#define _SHADER_PROGRAM_SET_H_

#include <stdint.h>

typedef void * ShaderProgramSetHandler;

int32_t shaderProgramSetCreate(ShaderProgramSetHandler *pHandler, const char *pVertShaderSrc, const char *pFragShaderSrc);
int32_t shaderProgramSetDestroy(ShaderProgramSetHandler handler);

GLuint shaderProgramGetProgram(ShaderProgramSetHandler handler);

#endif /* _SHADER_PROGRAM_SET_H_ */
