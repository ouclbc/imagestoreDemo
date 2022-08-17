#include <LogUtil.h>
#include "jni.h"
#include "render/MyGLRenderContext.h"
#define NATIVE_RENDER_CLASS_NAME "com/example/openglmaptest/MyNativeRender"
// call this func when loading lib

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnInit
 * Signature: ()V
 */
//JNIEnv实际上是C++类实例，JNI函数以成员函数形式存在
JNIEXPORT void JNICALL native_OnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance();
    LOGCATE("实现了动态注册 native_OnInit");
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnUnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnUnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::DestroyInstance();
}

/*
* Class:     com_byteflow_app_MyNativeRender
* Method:    native_OnSurfaceCreated
* Signature: ()V
*/
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    LOGCATE("native_OnSurfaceCreated");
    MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnDrawFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance()->OnDrawFrame();
    LOGCATE("native_OnDrawFrame");

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_SetImageData
 * Signature: (III[B)V
 * 转成uint8_t
 */
JNIEXPORT void JNICALL native_SetImageData(JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData) {
    LOGCATE(" native_SetImageData");
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
    MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}


#ifdef __cplusplus
}
#endif

// 将native 方法添加映射.
static JNINativeMethod g_RenderMethods[]{
        {"native_OnInit",           "()V",      (void *) (native_OnInit)},
        {"native_OnUnInit",         "()V",      (void *) (native_OnUnInit)},
        {"native_OnSurfaceCreated", "()V",      (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
        {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)}
};

//动态注册方法.
static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}
//注销动态注册.
static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }
}
//???? 这个方法是系统调用的
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    LOGCATE("========= JNI_OnLoad");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods, sizeof(g_RenderMethods) / sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);
}
