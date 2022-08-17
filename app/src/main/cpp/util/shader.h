#ifndef __SHADER_HEAD__
#define __SHADER_HEAD__

#include <GLES3/gl32.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "LogUtil.h"

using namespace std;

class Shader {
public:
    Shader() : m_vShader(0),
               m_fShader(0),
               m_program(0),
               m_programValid(false) {}

    Shader(const char *vertexShader, const char *fragmentShader) {

        m_vShader = LoadShader(GL_VERTEX_SHADER, vertexShader);
        m_fShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShader);
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vShader);
        glAttachShader(m_program, m_fShader);
        glLinkProgram(m_program);
        int success;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        m_programValid = success;
        if (!success) {
            char buf[256];
            glGetProgramInfoLog(m_program, 256, NULL, buf);
            LOGCATE("link program error..\n %s", buf);
        }
    }

    Shader(string vertexShaderPath, string fragmentShaderPath, string geometryShaderPath) {
        m_vShader = CreateShader(GL_VERTEX_SHADER, vertexShaderPath);
        m_fShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
        m_gShader = CreateShader(GL_GEOMETRY_SHADER, geometryShaderPath);
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vShader);
        glAttachShader(m_program, m_fShader);
        glAttachShader(m_program, m_gShader);
        glLinkProgram(m_program);
        int success;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        m_programValid = success;
        if (!success) {
            char buf[256];
            glGetProgramInfoLog(m_program, 256, NULL, buf);
            LOGCATE("link program error..\n %s", buf);
        }

    }

    ~Shader() {
        glDeleteShader(m_vShader);
        glDeleteShader(m_fShader);
        glDeleteShader(m_gShader);
        glDeleteProgram(m_program);
    }

    Shader(const Shader &other) = delete;

    void operator=(const Shader &other) = delete;

    void Use() {
        glUseProgram(m_program);
    }

    void SetInt(string name, int value) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader.\n", name.c_str());
            return;
        }

        glUniform1i(loc, value);
    }

    void SetFloat(string name, float value) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader.\n", name.c_str());
            return;
        }

        glUniform1f(loc, value);
    }

    void SetMatrix(string name, float *data) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader. program:%d\n", name.c_str(), m_program);
            return;

        }

        glUniformMatrix4fv(loc, 1, GL_FALSE, data);

    }

    void SetMatrix(string name, int count, float *data) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader. program:%d\n", name.c_str(), m_program);
            return;

        }

        glUniformMatrix4fv(loc, count, GL_FALSE, data);

    }

    void SetVec3(string name, float v1, float v2, float v3) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader. program:%d\n", name.c_str(), m_program);
            return;

        }

        glUniform3f(loc, v1, v2, v3);
    }

    void SetVec2(string name, float v1, float v2) {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            LOGCATE("can't find %s in shader. program:%d\n", name.c_str(), m_program);
            return;

        }

        glUniform2f(loc, v1, v2);
    }

    int GetAttribLocation(string name) {
        return glGetAttribLocation(m_program, name.c_str());
    }

private:
    GLuint m_program;
    GLuint m_vShader;
    GLuint m_fShader;
    GLuint m_gShader;

    bool m_programValid;

    char *readShader(const char *file) {
        FILE *fp = fopen(file, "rb");
        if (fp == NULL) {
            LOGCATE("open %s failed.", file);
            return NULL;
        }

        if (fseek(fp, 0, SEEK_END) == 0) {
            int len = ftell(fp);

            char *content = (char *) malloc((len + 1) * sizeof(char));
            if (content == NULL) {
                LOGCATE("malloc fail...\n");
                exit(1);
            }
            memset(content, 0, len + 1);

            fseek(fp, 0, SEEK_SET);
            fread(content, 1, len, fp);
            fclose(fp);
            return content;
        } else {
            LOGCATE("fseek error..\n");
            return NULL;
        }
    }

    GLuint CreateShader(GLenum type, string path) {
        GLuint shader = glCreateShader(type);
        if (shader == 0) {
            LOGCATE("create shader %d fail.\n", type);
        }

        //LOGCATE("Compile Shader file:%s\n",path.c_str());
        char *filebuf = readShader(path.c_str());
        if (filebuf == NULL) {
            return 0;
        }

        glShaderSource(shader, 1, (const GLchar **) &filebuf, NULL);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            LOGCATE("compile shader fail.\n");
            char info[256];
            glGetShaderInfoLog(shader, 256, NULL, info);
            LOGCATE("%s\n", info);
        }

        free(filebuf);

        return shader;
    }

    GLuint LoadShader(GLenum type, const char *pSource) {
        GLuint shader = glCreateShader(type);
        if (shader == 0) {
            LOGCATE("create shader %d fail.\n", type);
        }

        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            LOGCATE("compile shader fail.\n");
            char info[256];
            glGetShaderInfoLog(shader, 256, NULL, info);
            LOGCATE("%s\n", info);
        }
        return shader;
    }
};

#endif
