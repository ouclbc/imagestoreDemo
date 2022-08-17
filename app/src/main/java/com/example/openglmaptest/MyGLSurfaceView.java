package com.example.openglmaptest;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
//1. MyGLSurfaceView 设置MyRender
//2. 自定义MyRender 实现 Render 关联MyNativeRender
public class MyGLSurfaceView extends GLSurfaceView {
    private final String TAG = "MyGLSurfaceView";
    private MyNativeRender mNativeRender;
    public static final int IMAGE_FORMAT_RGBA = 0x01;

    public MyGLSurfaceView(Context context) {
        super(context);
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.setEGLContextClientVersion(3);
        mNativeRender = new MyNativeRender();
        MyGLRender myGLRender = new MyGLRender();
        // 关联Render
        setRenderer(myGLRender);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }
    public MyNativeRender getNativeRender(){
        return mNativeRender;
    }
    // MyGLRender 调用一些native方法实现 渲染操作.
    class MyGLRender implements Renderer{
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.d(TAG, "onSurfaceCreated() called with: gl = [" + gl + "], config = [" + config + "]");
            mNativeRender.native_OnSurfaceCreated();

        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.d(TAG, "onSurfaceChanged() called with: gl = [" + gl + "], width = [" + width + "], height = [" + height + "]");
            mNativeRender.native_OnSurfaceChanged(width, height);
            //通知这个方法GPU纹理绘制改变了.
            Log.e(TAG," 通知onSurfaceChangedGPU纹理绘制改变!");
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            Log.d(TAG, "onDrawFrame() called with: gl = [" + gl + "]");
            mNativeRender.native_OnDrawFrame();
            Log.e(TAG,"MyGLSurface通知native进行onDrawFrame绘制!");
        }
    }
}
