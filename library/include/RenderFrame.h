/* -*- c++ -*- */
#ifndef _RENDERFRAME_H_
#define _RENDERFRAME_H_

#include <stdint.h>

class IRenderFrame {
 protected:
	virtual ~RenderFrame(){};
};


int32_t RenderFrameCreate(IRenderFrame **ppFrame);
int32_t RenderFrameDestroy(IRenderFrame *pFrame);


#endif /* _RENDERFRAME_H_ */
