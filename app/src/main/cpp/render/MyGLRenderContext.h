//
// Created by ByteFlow on 2019/7/9.
//

#ifndef NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
#define NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
#include <GLES3/gl3.h>
#include "../sample/GLSampleBase.h"

class MyGLRenderContext
{
	MyGLRenderContext();

	~MyGLRenderContext();

public:

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

	void SetImageData(int format, int width, int height, uint8_t *pData);

	static MyGLRenderContext* GetInstance();
    static void DestroyInstance();

private:
	static MyGLRenderContext *m_pContext;
	GLSampleBase *m_Sample;
};


#endif //NDK_OPENGLES_3_0_MYGLRENDERCONTEXT_H
