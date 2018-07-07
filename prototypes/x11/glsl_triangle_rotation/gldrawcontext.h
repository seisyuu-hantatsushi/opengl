#ifndef _GLDRAWCONTEXT_H_
#define _GLDRAWCONTEXT_H_

#include <GL/glew.h>
#include <GL/glx.h>

typedef struct GLDrawContext {
	uint32_t glslversion;
	GLfloat rotate;
	GLuint programId;

	void (*pResize)(unsigned int width, unsigned int height, struct GLDrawContext *pGLDrawCtx);
	void (*pRedraw)( Display *dpy, Window w, struct GLDrawContext *pCtx );

} GLDrawContext;

#endif /* _GLDRAWCONTEXT_H_ */
