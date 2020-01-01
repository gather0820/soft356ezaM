#pragma once
#include <windows.h>		
#include <math.h>			
#include <olectl.h>			
#include <stdio.h>
#include <gl\gl.h>			
#include <gl\glu.h>			

//一种纹理影射方式，这种方式可消除边界的线条  Lines that eliminate boundaries
#define GL_CLAMP_TO_EDGE	0x812F	

class CTexture
{
	public:
		//判断当前纹理对象是否有效  Determine whether the current texture object is valid
		bool IsValid() const { return ID != NULL;}  
		//装载纹理的通用函数，暂时可以装载bmp,tga,pcx  General functions for loading textures
		bool LoadTexture(const char* fname, bool make_mipmaps=false);
		//使当前纹理 Make current textur
		void MakeCurrent();
		//获得纹理的ID  Get textured ID
		GLuint Get_TextureID(){ if(ID) return ID; else return NULL; }
		//重新设置纹理的参数  Reset the parameters of the texture
		void Set_Filter(GLuint mag_filter,         GLuint min_filter,
						GLuint wrap_s = GL_REPEAT, GLuint wrap_t = GL_REPEAT,
						GLuint env_mode = GL_MODULATE);	
		//释放纹理资源  Release texture resources
		void Free();

		CTexture(){ ID =0; }
		~CTexture()	{ Free(); }

	private:
		//纹理的ID Texture ID
      	unsigned int ID;		
	
		//显示错误  Display error
		void Error(char *text, ...);
		bool LoadTGA(const char* filename, bool make_mipmaps=false);
		bool LoadOthers(const char* filename, bool make_mipmaps=false);
	};