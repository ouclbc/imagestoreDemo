//
// Created by ByteFlow on 2019/7/9.
//
#include "MyGLRenderContext.h"
#include "../sample/TextureMapSample.h"
#include <LogUtil.h>
// call this func when loading lib

MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    m_Sample = new TextureMapSample();
}

MyGLRenderContext::~MyGLRenderContext() {
    if (m_Sample) {
        delete m_Sample;
        m_Sample = nullptr;
    }

}
//生命周期的各个步骤调用GL的各个方法.??
void MyGLRenderContext::OnSurfaceCreated() {
    LOGCATE("MyGLRenderContext::OnSurfaceCreated");
    glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGCATE("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
    glViewport(0, 0, width, height);
}

void MyGLRenderContext::OnDrawFrame() {
    LOGCATE("MyGLRenderContext::OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (m_Sample) {
        //m_Sample->Init();
        //m_Sample->Draw();
        m_Sample->Display();
    }
}

MyGLRenderContext *MyGLRenderContext::GetInstance() {
    LOGCATE("MyGLRenderContext::GetInstance");
    if (m_pContext == nullptr) {
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    LOGCATE("MyGLRenderContext::DestroyInstance");
    if (m_pContext) {
        delete m_pContext;
        m_pContext = nullptr;
    }
}

void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData){
    LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format,
            width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    if (m_Sample) {
        //调用CopyNativeImage将bitmap copy到gpu.
        LOGCATE("SetImageData");
        m_Sample->LoadImage(&nativeImage);
    }

}
