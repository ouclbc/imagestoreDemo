//
// Created by ByteFlow on 2019/7/10.
//

#ifndef NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H
#define NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H


#include "GLSampleBase.h"
#include "../util/ImageDef.h"
#include "../util/threemath.h"
#include "../util/shader.h"

class TextureMapSample : public GLSampleBase
{
public:
	TextureMapSample();

	~TextureMapSample();

	void LoadImage(NativeImage *pImage);

	virtual void Init();

	virtual void Draw();

	virtual void Display();

private:
	void CreateTexture();

	void DeleteTexture();

	GLuint m_TextureId;
	GLint m_SamplerLoc;
	NativeImage m_RenderImage;
	bool mInit = false;
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	GLuint tex;
	GLuint otex;
	GLuint obuf;
	Matrix4x4 modelMat;
	std::shared_ptr<Shader> dShader;
	std::shared_ptr<Shader> tShader;
};


#endif //NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H
