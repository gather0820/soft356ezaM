#include "CTexture.h"


//装载纹理的通用函数 General functions for loading textures
bool CTexture::LoadTexture(const char* fname, bool make_mipmaps)
{
    //判别文件的类型
 	const char * p;
	p = strchr(fname, '.');
	if(!_stricmp(p, ".tga")) return LoadTGA (fname, make_mipmaps);
    return LoadOthers (fname, make_mipmaps);
	return false;
}

//装载未压缩的TGA纹理  Load uncompressed TGA textures
bool CTexture::	LoadTGA(const char* filename, bool make_mipmaps)	
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	
	GLubyte		TGAcompare[12];
	GLubyte		header[6];	
	GLuint		bytesPerPixel;
	GLuint		imageSize;	
	GLuint		temp;		
    GLuint		type;		
	GLuint      width, height;
	GLubyte* data;		
	unsigned int bpp;	

	//open TGA
	FILE* file = fopen(filename, "rb");		

	// Detection file
	if(file==NULL													   ||	
	   fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	
	   memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0			   ||	
	   fread(header,1,sizeof(header),file)!=sizeof(header))					
		{
		if(file==NULL)							
			{
			Error("%s does not exist.", filename);
			return false;							
			}
		else
			{
			fclose(file);				
			Error("Could not load %s correctly, general failure.", filename);
			return false;						
			}
		}

	width = header[1] * 256 + header[0];		
	height= header[3] * 256 + header[2];		
    
 	if(width	<=0	||							
	   height<=0	||							
		(header[4]!=24 && header[4]!=32))		
		{
		fclose(file);							
		Error("%s's height or width is less than zero, or the TGA is not 24 or 32 bits.", filename);
		return false;							
		}

	bpp	 = header[4];							
	bytesPerPixel= bpp/8;						
	imageSize	 = width*height*bytesPerPixel;	

	data= new GLubyte [imageSize];				

	if(data==NULL ||							
	   fread(data, 1, imageSize, file)!=imageSize)	
		{
		if(data!=NULL)							
			free(data);							
		
		Error("Storate memory for %s does not exist or is corrupted.", filename);
		
		fclose(file);							
		return false;							
		}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)
		{									
		temp	 =data[i];					
		data[i]	 = data[i + 2];				
		data[i+2]= temp;					
		}                           
	fclose (file);								
 
	// Build A Texture From The Data
	//Generate OpenGL texture IDs
	glGenTextures(1, &ID);						
	//Bind the texture to a texture object
	glBindTexture(GL_TEXTURE_2D, ID);			 
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	if(bpp==24)	type=GL_RGB;								
	if(bpp==32)	type=GL_RGBA;								

    if(make_mipmaps) 
	{ 
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
		if(gluBuild2DMipmaps(GL_TEXTURE_2D, 3,width, height, type, GL_UNSIGNED_BYTE, data))
			Error((char*)gluErrorString(glGetError()));
	}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);

	if(data){ delete data; data = NULL; }

 	return true;
}

//装载其他的纹理包括BMP, JPG, GIF, EMF, ICO, WMF  Load other textures including BMP, JPG, GIF, EMF, ICO, WMF
bool CTexture::LoadOthers(const char* filename, bool make_mipmaps)
{
	HDC			hdcTemp;											
	HBITMAP		hbmpTemp;											
	IPicture	*pPicture;											
	OLECHAR		wszPath[MAX_PATH+1];								
	char		szPath[MAX_PATH+1];									
	long		lWidth;												
	long		lHeight;											
	long		lWidthPixels;										
	long		lHeightPixels;										
	GLint		glMaxTexDim ;										

	if (strstr(filename, "http://"))								
	{
		strcpy(szPath, filename);									
	}
	else															
	{
		GetCurrentDirectory(MAX_PATH, szPath);						
		strcat(szPath, "\\");										
		strcat(szPath, filename);									
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);	
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))														
		return FALSE;													

	hdcTemp = CreateCompatibleDC(GetDC(0));								
	if(!hdcTemp)														
	{
		pPicture->Release();											
		return FALSE;													
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);					
	
	pPicture->get_Width(&lWidth);										
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);										
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	if (lWidthPixels <= glMaxTexDim)
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else  
		lWidthPixels = glMaxTexDim;
 
	if (lHeightPixels <= glMaxTexDim)
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else  
		lHeightPixels = glMaxTexDim;
	
	BITMAPINFO	bi = {0};												
	DWORD		*pBits = 0;												

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);				
	bi.bmiHeader.biBitCount		= 32;									
	bi.bmiHeader.biWidth		= lWidthPixels;							
	bi.bmiHeader.biHeight		= lHeightPixels;						
	bi.bmiHeader.biCompression	= BI_RGB;								
	bi.bmiHeader.biPlanes		= 1;									

	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)													
	{
		DeleteDC(hdcTemp);											
		pPicture->Release();										
		return FALSE;												
	}

	SelectObject(hdcTemp, hbmpTemp);								
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)			
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);						
		BYTE  temp		= pPixel[0];								
		pPixel[0]		= pPixel[2];								
		pPixel[2]		= temp;										

		if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0))		
			pPixel[3]	=   0;										
		else														
			pPixel[3]	= 255;										
	}

	glGenTextures(1, &ID);											

	// Typical Texture Generation Using Data From The Bitmap
	// Bind To The Texture ID
	glBindTexture(GL_TEXTURE_2D, ID);								
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, lWidthPixels, lHeightPixels, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);												
	DeleteDC(hdcTemp);													

	pPicture->Release();												
	return TRUE;														
}

//使用纹理  Use textur
void CTexture::MakeCurrent(void)
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void CTexture::Free(void)
{
	if(ID) { glDeleteTextures(1,&ID); ID = NULL; }
}

//Error
void CTexture::
   Error(char *text, ...)
{
	char buf[1024];
	va_list arg_list;
	va_start(arg_list, text);
	vsprintf (buf, text, arg_list) ;
	MessageBox(GetFocus(),buf,"Error",0);
	va_end(arg_list);
}
