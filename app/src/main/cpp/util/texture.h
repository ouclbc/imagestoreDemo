#ifndef __TEXTURE_HEAD__
#define __TEXTURE_HEAD__


#include <GLES3/gl32.h>


class Texture2D
{
	public:
		Texture2D(const char *path);
		//Texture2D(const Texture2D &other) = delete;
		//void operator=(const Texture2D &other) = delete;
		~Texture2D()
		{
			if(tex_ > 0)
			{
				glDeleteTextures(1,&tex_);
			}
		}

		GLuint Get()
		{
			return tex_;
		}
			
		static GLuint LoadTexture(const char *path);

	private:
		GLuint tex_;
};

#endif
