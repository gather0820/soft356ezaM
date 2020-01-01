#pragma once
#include <windows.h>		
#include <math.h>			
#include <olectl.h>			
#include <stdio.h>
#include <gl\gl.h>			
#include <gl\glu.h>			

//һ������Ӱ�䷽ʽ�����ַ�ʽ�������߽������  Lines that eliminate boundaries
#define GL_CLAMP_TO_EDGE	0x812F	

class CTexture
{
	public:
		//�жϵ�ǰ��������Ƿ���Ч  Determine whether the current texture object is valid
		bool IsValid() const { return ID != NULL;}  
		//װ�������ͨ�ú�������ʱ����װ��bmp,tga,pcx  General functions for loading textures
		bool LoadTexture(const char* fname, bool make_mipmaps=false);
		//ʹ��ǰ���� Make current textur
		void MakeCurrent();
		//��������ID  Get textured ID
		GLuint Get_TextureID(){ if(ID) return ID; else return NULL; }
		//������������Ĳ���  Reset the parameters of the texture
		void Set_Filter(GLuint mag_filter,         GLuint min_filter,
						GLuint wrap_s = GL_REPEAT, GLuint wrap_t = GL_REPEAT,
						GLuint env_mode = GL_MODULATE);	
		//�ͷ�������Դ  Release texture resources
		void Free();

		CTexture(){ ID =0; }
		~CTexture()	{ Free(); }

	private:
		//�����ID Texture ID
      	unsigned int ID;		
	
		//��ʾ����  Display error
		void Error(char *text, ...);
		bool LoadTGA(const char* filename, bool make_mipmaps=false);
		bool LoadOthers(const char* filename, bool make_mipmaps=false);
	};