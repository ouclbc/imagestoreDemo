//
// Created by ByteFlow on 2019/7/10.
//

#include <GLUtils.h>
#include <LogUtil.h>
#include "TextureMapSample.h"
//纹理映射贴图的一般步骤.
// 生成纹理，编译链接着色器程序
//  确定纹理坐标及对应的顶点坐标
//  加载图像数据到纹理，加载纹理坐标和顶点坐标到着色器程序
//	绘制
TextureMapSample::TextureMapSample()
{
	//生成一个纹理，将纹理 id 赋值给 m_TextureId
	m_TextureId = 0;
	mInit = false;
}

TextureMapSample::~TextureMapSample()
{
	//析构函数 释放内存资源.
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}
// 生成纹理，编译链接着色器程序
void TextureMapSample::Init()
{
	CreateTexture();
    //顶点着色器绘制图形.
	char vShaderStr[] =
			"#version 320 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec2 a_texCoord;   \n"
			"out vec2 v_texCoord;                       \n"
			"void main()                                \n"
			"{                                          \n"
			"   gl_Position = a_position;               \n"
			"   v_texCoord = a_texCoord;                \n"
			"}                                          \n";
    //光栅化是将图元转化为一组二维片段的过程,这些片段由片段着色器处理.
    //创建片段着色器.给二维片段着色.
	// texture() 为内置的采样函数，v_texCoord 为顶点着色器传进来的纹理坐标
	// 根据纹理坐标对纹理进行采样，输出采样的 rgba 值（4维向量）
	//这一段写法??? 可以写成shader小程序加载进来.
	char fShaderStr[] =
			"#version 320 es                                     \n"
			"precision mediump float;                            \n"
			"in vec2 v_texCoord;                                 \n"
			"layout(location = 0) out vec4 outColor;             \n"
			"uniform sampler2D s_TextureMap;                     \n"
			"void main()                                         \n"
			"{                                                   \n"
			"  outColor = texture(s_TextureMap, v_texCoord);     \n"
			"}                                                   \n";
	/*char fShaderStr[] =
			"#version 320 es                                                  \n"
			"precision highp float;                                           \n"
			"layout(rgba8, binding=1) writeonly uniform highp image2D dumpTex;\n"
			"uniform sampler2D s_TextureMap;                                  \n"
			"in vec2 v_texCoord;                                              \n"
			"void main()                                                      \n"
			"{                                                                \n"
			"  vec4 color = texture(s_TextureMap, v_texCoord);                \n"
			"  imageStore(dumpTex, ivec2(gl_FragCoord.xy),color);             \n"
			"}                                                                \n";*/

	char displayShader[] =
			"#version 320 es                                                   \n"
			"precision highp float;                                            \n"
			"out vec4 color;                                                   \n"
			"uniform layout(binding=1, rgba8ui) readonly uimage2D dumpTex;     \n"
			"void main()                                                       \n"
			"{                                                                 \n"
			"  vec4 s = imageLoad(dumpTex, ivec2(gl_FragCoord.xy) ;            \n"
			"  color = (1.0 / 255.0) * s;                                      \n"
			"}                                                                 \n";
    //add for imagestore

    //创建项目在项目上运行 顶点着色器和片段着色器.
	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
        /**glGetUniformLocation返回一个整数，表示程序对象中特定统一变量的位置（序号）。 name必须是不包含空格的空终止字符串。
		name必须是程序中的活动统一变量名，它不能是结构，也不能是结构数组或向量或矩阵的子组件。
		如果name与程序中的活动统一变量不对应，或者name以保留前缀“gl_”开头，则此函数返回-1。*/
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        LOGCATE("TextureMapSample::Init create program success");
	}
	else
	{
		LOGCATE("TextureMapSample::Init create program fail");
	}
	//m_ProgramDisplay = GLUtils::CreateProgram(vShaderStr, displayShader, m_VertexShader, m_FragmentShader);
}

void TextureMapSample::Draw()
{
	LOGCATE("TextureMapSample::Draw()");

	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

	//设置激活的纹理单元
	glActiveTexture(GL_TEXTURE0);
	//将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	//加载 RGBA 格式的图像数据
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	//glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);
    //4个纹理贴图对应的OpenGL坐标系
	/**P0------------P3
	 *      |       |
	 * ----0,0-----
	 *      |       |
	 *P1----0,0-----P2
	 */
	GLfloat verticesCoords[] = {
			-1.0f,  0.82f, 0.0f,  // Position 0
			-1.0f, -0.82f, 0.0f,  // Position 1
			 1.0f, -0.82f, 0.0f,  // Position 2
			 1.0f,  0.82f, 0.0f,  // Position 3
	};
    // 4 个纹理坐标：左上角为（0,0），右下角为（1,1）
	GLfloat textureCoords[] = {
			0.0f,  0.0f,        // TexCoord 0
			0.0f,  1.0f,        // TexCoord 1
			1.0f,  1.0f,        // TexCoord 2
			1.0f,  0.0f         // TexCoord 3
	};

	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	// Use the program object
	glUseProgram (m_ProgramObj);

	//  glVertexAttribPointer 指定了渲染时索引值为 index 的顶点属性数组的数据格式和位置。 Load the vertex position 加载顶点的位置
	glVertexAttribPointer (0, 3, GL_FLOAT,
							GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
	// Load the texture coordinate 加载纹理坐标
	glVertexAttribPointer (1, 2, GL_FLOAT,
							GL_FALSE, 2 * sizeof (GLfloat), textureCoords);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	// Bind the RGBA map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);

	// Set the RGBA map sampler to texture unit to 0
	glUniform1i(m_SamplerLoc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	LOGCATE(" TextureMapSample::onDraw over()");

}
// 通过NativeImage 进行关联.
void TextureMapSample::LoadImage(NativeImage *pImage)
{
	LOGCATE(" TextureMapSample::LoadImage pImage = %p", pImage->ppPlane[0]);
	if (pImage)
	{
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		m_RenderImage.format = pImage->format;
        // 底层加载Image 图片方法
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
	}

}

void TextureMapSample::CreateTexture()
{
	//create RGBA texture 设置激活的纹理单元（texture unit）
	glActiveTexture(GL_TEXTURE0);
	// 绑定纹理目标(一个或多个)到纹理单元
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	//设置纹理 S 轴（横轴）的拉伸方式为截取
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//设置纹理采样方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}
void TextureMapSample::Display()
{
	if (m_RenderImage.ppPlane[0]){
		LOGCATE("data is ok\n");
	} else{
		LOGCATE("failed to load data\n");
		return;
	}
	if(!mInit)
	{
        char sampleVertex[] =
                "#version 320 es                                                   \n"
                "layout(location=0) in vec3 iPos;                                  \n"
                "layout(location=1) in vec2 iTexcoord;                             \n"
                "uniform mat4 model;                                               \n"
                "uniform mat4 view;                                                \n"
                "uniform mat4 proj;                                                \n"
                "out vec2 texcoords;                                               \n"
                "void main()                                                       \n"
                "{                                                                 \n"
                "  texcoords = iTexcoord;                                          \n"
                "  gl_Position = vec4(iPos,1);               \n"
                "}                                                                 \n";
		char samplefrag[] =
				"#version 320 es                                                                   \n"
				"precision highp float;                                                            \n"
				"layout(binding = 0, rgba8) readonly uniform mediump image2D colors;               \n"
				"layout(binding = 1, rgba8) writeonly uniform mediump image2D output_buffer;       \n"
				"in vec2 texcoords;                                                                \n"
				"out vec4 color;                                                                   \n"
				"void main()                                                                       \n"
				"{                                                                                 \n"
				"  ivec2 size = imageSize(output_buffer);                                          \n"
				"  vec4 col = imageLoad(colors,ivec2(gl_FragCoord.xy));                            \n"
				"  imageStore(output_buffer,ivec2(int(float(size.x) * texcoords.x), int(float(size.y) * texcoords.y)),col);                       \n"
				"  color = col;                                                                    \n"
				"}                                                                                 \n";

        char imageVertex[] =
                "#version 320 es                                                   \n"
                "layout(location=0) in vec3 iPos;                                  \n"
                "layout(location=1) in vec2 iTexcoords;                            \n"
                "uniform mat4 model;                                               \n"
                "uniform mat4 view;                                                \n"
                "uniform mat4 proj;                                                \n"
                "out vec2 texcoords;                                               \n"
                "void main()                                                       \n"
                "{                                                                 \n"
                "  texcoords = iTexcoords;                                        \n"
                "  gl_Position = proj * view * model * vec4(iPos,1);               \n"
                "}                                                                 \n";
		char imagefrag[] =
				"#version 320 es                                     \n"
                "precision mediump float;                            \n"
				"uniform sampler2D image;                            \n"
				"in vec2 texcoords;                                  \n"
				"out vec4 outColor;                                  \n"
				"void main()                                         \n"
				"{                                                   \n"
				"  outColor = texture(image,texcoords);              \n"
				"}                                                   \n";
		mInit = true;
		float vertices[] = {
				-1.0f,1.0f,0.0f,      0.0f,0.0f,
				-1.0f,-1.0f,0.0f,     0.0f,1.0f,
				1.0f,-1.0f,0.0f,	  1.0f,1.0f,
				1.0f,1.0f,0.0f,	      1.0f,0.0f,
		};
		GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
		glClearColor(0,0,0,1);

		glGenVertexArrays(1,&m_VAO);
		glGenBuffers(1,&m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		//位置属性(x,y,z)
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(GLvoid*)0);
		glEnableVertexAttribArray(0);
		//纹理坐标位置TexCoord attribute（x,y）
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(GL_FLOAT),(GLvoid*)(3* sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		//单位矩阵
		modelMat = Matrix4x4::Identity();
		Matrix4x4 viewMat =Matrix4x4::Identity();
		Matrix4x4 projMat = Ortho(-2,2,-2,2,0,10);

		dShader = std::make_shared<Shader>(sampleVertex,samplefrag);
		dShader->Use();
		//dShader->SetMatrix("model",modelMat.Get());
		//dShader->SetMatrix("view",viewMat.Get());
		//dShader->SetMatrix("proj",projMat.Get());

		tShader = std::make_shared<Shader>(imageVertex,imagefrag);
		tShader->Use();
		modelMat.Scale(0.5);
		tShader->SetMatrix("model",modelMat.Get());
		tShader->SetMatrix("view",viewMat.Get());
		tShader->SetMatrix("proj",projMat.Get());

		//输入图像RGBA
		glGenTextures(1,&tex);
		glBindTexture(GL_TEXTURE_2D,tex);
		glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,m_RenderImage.width,m_RenderImage.height);
		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						0,0,
						m_RenderImage.width,m_RenderImage.height,
						GL_RGBA,GL_UNSIGNED_BYTE,
						m_RenderImage.ppPlane[0]);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		//输出图像
		glGenTextures(1,&otex);
		glBindTexture(GL_TEXTURE_2D,otex);
		glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,m_RenderImage.width,m_RenderImage.height);
		glBindTexture(GL_TEXTURE_2D,0);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D,0);
	}

	glClear(GL_COLOR_BUFFER_BIT);

	dShader->Use();
	glBindImageTexture(0,tex,0,GL_FALSE,0,GL_READ_ONLY,GL_RGBA8);
	glBindImageTexture(1,otex,0,GL_FALSE,0,GL_WRITE_ONLY,GL_RGBA8);
	glBindVertexArray(m_VAO);
	//glDrawArrays(GL_TRIANGLES,0,6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	tShader->Use();
	glBindTexture(GL_TEXTURE_2D,otex);
	glBindVertexArray(m_VAO);
	//glDrawArrays(GL_TRIANGLES,0,6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	GLUtils::CheckGLError("glDrawArrays");;
	glFlush();
}

void TextureMapSample::DeleteTexture()
{
	if (m_TextureId)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glDeleteTextures(1, &m_TextureId);
		m_TextureId = 0;
	}

}
