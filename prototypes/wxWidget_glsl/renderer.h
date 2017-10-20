/* -*- C++ -*- */
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <stdint.h>

class IRenderer {
protected:
	virtual ~IRenderer(){};
};

int32_t rendererCreate(IRenderer **ppRenderer);
int32_t rendererDestroy(IRenderer *pRenderer);

#endif /* _RENDERER_H_ */
