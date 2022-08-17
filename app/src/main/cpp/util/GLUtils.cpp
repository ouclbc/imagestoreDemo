#include "GLUtils.h"
#include "LogUtil.h"
#include <stdlib.h>

GLuint GLUtils::LoadShader(GLenum shaderType, const char *pSource)
{
    GLuint shader = 0;
	FUN_BEGIN_TIME("GLUtils::LoadShader")
//	    1.创建着色器
        shader = glCreateShader(shaderType);
        if (shader)
        {
            //  关联着色器
            glShaderSource(shader, 1, &pSource, NULL);
            //   编译gl文件检测编译是否成功.
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen)
                {
                    char* buf = (char*) malloc((size_t)infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        LOGCATE("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType, buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
	FUN_END_TIME("GLUtils::LoadShader")
	return shader;
}

GLuint GLUtils::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle)
{
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::CreateProgram")
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle) return program;

        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle) return program;
        // 创建程序.
        program = glCreateProgram();
        if (program)
        {
             //关联着色器到程序对象
            glAttachShader(program, vertexShaderHandle);
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle);
            CheckGLError("glAttachShader");
            //链接程序
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            // 删除顶点着色器
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            // 删除片元着色器
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;
            // 检测链接状态是否正确.
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGCATE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
//                glDeleteProgram(program);
                program = 0;
            }
        }
    FUN_END_TIME("GLUtils::CreateProgram")
    LOGCATE("GLUtils::CreateProgram program = %d", program);
	return program;
}

void GLUtils::DeleteProgram(GLuint &program)
{
    LOGCATE("GLUtils::DeleteProgram");
    if (program)
    {
        // 关闭着色器时只需要传入参数0即可
        glUseProgram(0);
        // 删除程序
//        glDeleteProgram(program);
        program = 0;
    }
}

void GLUtils::CheckGLError(const char *pGLOperation)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGCATE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }

}
