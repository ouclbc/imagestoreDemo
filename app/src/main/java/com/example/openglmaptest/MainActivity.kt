package com.example.openglmaptest

import android.annotation.SuppressLint
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.openglmaptest.MyGLSurfaceView.IMAGE_FORMAT_RGBA
import java.io.IOException
import java.nio.ByteBuffer

//1.从本地drawable中load一张图片 转成Bitmap
//2.自定义MyGLSurfaceView 集成 GLSurfaceView
//3.native 方法
//4.c++中loadImage方法
//5.shader程序加载
//6.生成纹理链接编译着色器程序
//7.片元着色器 和顶点着色器
//8.调用OpenGL进行绘制
class MainActivity : AppCompatActivity() {
    private var mSurfaceView : MyGLSurfaceView? = null
    private var mShowText : TextView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mSurfaceView = findViewById<MyGLSurfaceView>(R.id.myGLSurfaceView)
        mShowText = findViewById<TextView>(R.id.tv_showText)
        mSurfaceView!!.nativeRender.native_OnInit()
    }
    //从本地加载一张图片
    @SuppressLint("ResourceType")
    private fun loadImage() {
        val inputStream = this.resources.openRawResource(R.drawable.tmap)//183*275
        val bitmap = BitmapFactory.decodeStream(inputStream)
        //将bitmap 转成byte数组 传给native层
        if (bitmap != null) {
            val bytes = bitmap.byteCount
            val buf = ByteBuffer.allocate(bytes)
            bitmap.copyPixelsToBuffer(buf)
            val byteArray = buf.array()
            mSurfaceView!!.nativeRender.native_SetImageData(
                IMAGE_FORMAT_RGBA,
                bitmap.width,
                bitmap.height,
                byteArray
            )
        }
        try {
            inputStream.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }

    }

    fun showTexture(view: View) {
        Toast.makeText(MainActivity@this,"show Image",Toast.LENGTH_LONG).show()
        loadImage()
        mShowText!!.visibility = View.GONE
        mSurfaceView!!.requestRender()
    }

    override fun onDestroy() {
        super.onDestroy()
        mSurfaceView?.nativeRender?.native_OnUnInit()
    }
}