#include "CMaze.h"

extern CVector3 *g_vWorld;    
extern int g_NumberOfVerts ;

//create a new console
CMaze::CMaze()
{
	m_nRowX = 0; m_nColY = 0;

	m_fWidth  = 0.0f;	m_fLength = 0.0f;	m_fHeight = 0.0f;

	m_TexID = NULL;

	m_bMazeX = NULL;	m_bMazeY = NULL;
}


//read maze data and generate list  读取迷宫数据并生成列表
void CMaze::
DrawMaze_Init(char *FileMaze, int p_nRowX, int p_nColY, float p_x0, float p_y0, float p_z0, float p_fWidth, float p_fHeight, float p_fLength, GLuint p_TexID)
{
	m_nRowX = p_nRowX;
	m_nColY = p_nColY;

	m_x0 = p_x0;
	m_y0 = p_y0;
	m_z0 = p_z0;

	m_fWidth  = p_fWidth;   
	m_fHeight = p_fHeight;  
	m_fLength = p_fLength;  
	
	m_TexID = p_TexID;

	m_bMazeX = new bool [m_nRowX*m_nColY];
	m_bMazeY = new bool [m_nRowX*m_nColY];

	if(!m_bMazeX||!m_bMazeY) { Error("Can't allocate memory for maze"); return; }

	memset(m_bMazeX, 0, sizeof(bool)*m_nRowX*m_nColY);
	memset(m_bMazeY, 0, sizeof(bool)*m_nRowX*m_nColY);

	//读取ViewPort的尺寸  Read the size of the ViewPort
	GLint nView[4];
    glGetIntegerv(GL_VIEWPORT,nView);
	m_nViewWidth  = nView[2];
	m_nViewHeight = nView[3];

	//读取maze编码  Read maze encoding
	SetupWorld(FileMaze);
	//读取顶点信息  Read vertex information
	LoadVertices();
	//生成显示列表  Generate display list
	GenerateList();
}

//call list to draw maze, center at (0,0,0)
void CMaze::DrawMaze()
{
	if(m_TexID) glBindTexture(GL_TEXTURE_2D, m_TexID);
	glCallList(m_MazeList);
}

//call list to draw maze line map
void CMaze::DrawMazeMap()
{
	glCallList(m_MazeSmallList);
}

//read a line from the file 从文件中读取一行
void CMaze::Readstr(FILE *f,char *string)		
{
	do
	{
		fgets(string, 255, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}


//read maze data from file, then store in buffer of m_bMazeX and m_bMazeY
//从文件中读取迷宫数据，然后存储在m_bMazeX和m_bMazeY的缓冲区中
void CMaze::SetupWorld(char *FileMaze)
{
	FILE *filein;
	char oneline[255];

	filein = fopen(FileMaze, "rt");				// File To Load World Data From  要从中加载坐标数据的文件
	if(!filein) { Error("Can't open %s file",FileMaze); return; }
	int loop1=0, loop2=0;

	for (loop1 = 0; loop1 < m_nRowX; loop1++)
	{		
		Readstr(filein,oneline);
		for (loop2 = 0; loop2 < m_nColY; loop2++)
		{
			m_bMazeX[loop1* m_nColY +loop2] = oneline[loop2]=='1'? 1:0;	
		}
	}

	for (loop1 = 0; loop1 < m_nRowX; loop1++)
	{		
		Readstr(filein,oneline);
		for (loop2 = 0; loop2 < m_nColY; loop2++)
		{
			m_bMazeY[loop1* m_nColY +loop2] = oneline[loop2]=='1'? 1:0;	
		}
	}
	fclose(filein);
}


//generate list of maze  生成迷宫列表
void CMaze::GenerateList()
{
	int loop1=0, loop2=0;
	float x0 = 0.0f, y0 =0.0f, z0 =0.0f;
	float fx=0.0f, fy=0.0f, fz=0.0f;
	float fCellWidth, fCellHeight, fCellLength;

	//坐标原点位于maze的中心  The coordinate origin is located at the center of the maze.
	x0 = m_fWidth/2.0f;
	y0 = m_fHeight/2.0f;
	z0 = m_fLength/2.0f;

	fCellWidth  = m_fWidth/(float)(m_nColY-1);
	fCellHeight = m_fHeight;
	fCellLength = m_fLength/(float)(m_nRowX-1);

	m_MazeList = glGenLists(1);

	glNewList( m_MazeList, GL_COMPILE );
	glColor3f(0.6f,0.6f,0.6f);
		for (loop1 = 0; loop1 < m_nRowX; loop1++)
			for (loop2 = 0; loop2 < m_nColY; loop2++)
			{
					fx = fCellWidth*loop2-x0+m_x0;
					fy = 0.0f-y0+m_y0;
					fz = loop1 * fCellLength-z0+m_z0;

				if(!m_bMazeX[loop1*m_nColY+loop2])
				{					
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(fx,             fy,  fz );	
					glTexCoord2f(1.0f, 0.0f); glVertex3f(fx+fCellWidth,  fy,  fz);	
					glTexCoord2f(1.0f, 1.0f); glVertex3f(fx+fCellWidth,  fy+fCellHeight,   fz);	
					glTexCoord2f(0.0f, 1.0f); glVertex3f(fx,             fy+fCellHeight,   fz);	
					glEnd();
				}
				if(!m_bMazeY[loop1*m_nColY+loop2])
				{
					glBegin(GL_QUADS);			
					glTexCoord2f(0.0f, 0.0f); glVertex3f(fx, fy, fz);	
					glTexCoord2f(1.0f, 0.0f); glVertex3f(fx, fy, fz+fCellLength );		
					glTexCoord2f(1.0f, 1.0f); glVertex3f(fx, fy+fCellHeight,   fz+fCellLength);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(fx, fy+fCellHeight,   fz);
					glEnd();
				}
			}		
	glEndList();

	const float WidthFactor=0.23f, LengthFactor=0.23f;
	fCellWidth  = m_nViewWidth/(float)(m_nColY-1)*WidthFactor;
	fCellLength = m_nViewHeight/(float)(m_nRowX-1)*LengthFactor;

	m_MazeSmallList = glGenLists(1);

	glNewList( m_MazeSmallList, GL_COMPILE );	
		glColor3f(0.8f,0.8f,0.0f);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		Start2DMode();
		for (loop1 = 0; loop1 < m_nRowX; loop1++)
			for (loop2 = 0; loop2 < m_nColY; loop2++)
			{
					fx = fCellWidth*loop2;
					fy = (m_nRowX-loop1-1) * fCellLength;

				if(!m_bMazeX[loop1*m_nColY+loop2])
				{
					glBegin(GL_LINES);			
					glVertex2f(fx,             fy );
					glVertex2f(fx+fCellWidth,  fy);	
					glEnd();
				}
				if(!m_bMazeY[loop1*m_nColY+loop2])
				{
					glBegin(GL_LINES);			
					glVertex2f(fx,  fy            );
					glVertex2f(fx,  fy-fCellLength);
					glEnd();
				}
			}	
		End2DMode();
		glEnable(GL_TEXTURE_2D);
		glPopAttrib();
	glEndList();
}

//set a 2-D orthographic projection    设置二维正交投影
void CMaze::Start2DMode(void)
{
	//Select the projection matrix
	glMatrixMode(GL_PROJECTION);	
	//Store
	glPushMatrix();		
	//Reset
	glLoadIdentity();							
	glOrtho(0,m_nViewWidth,0,m_nViewHeight,-1,1);
	glMatrixMode(GL_MODELVIEW);					
}

//retrive the 3-D projection   检索三维投影
void CMaze::End2DMode(void)
{
	//Select
	glMatrixMode(GL_PROJECTION);	
	//Restore
	glPopMatrix();
	//Select
	glMatrixMode(GL_MODELVIEW);						
}

//delete memory and list  删除内存和列表
void CMaze::DrawMaze_Del()
{
	if(glIsList(m_MazeSmallList)) glDeleteLists(m_MazeSmallList,1);
	if(glIsList(m_MazeList)) glDeleteLists(m_MazeList,1);

	if(m_bMazeX) { delete m_bMazeX; m_bMazeX=NULL; }
	if(m_bMazeY) { delete m_bMazeY; m_bMazeY=NULL; }
}


//error
void CMaze::
   Error(char *text, ...)
{
	char buf[1024];
	va_list arg_list;
	va_start(arg_list, text);
	vsprintf (buf, text, arg_list) ;
	MessageBox(GetFocus(),buf,"Error",0);
	va_end(arg_list);
}

//获得用于边缘检测的顶点数组，存储在g_vWorld中  Get an array of vertices for edge detection and store them in g_vWorld
void CMaze::LoadVertices()
{
	int   loop1=0, loop2=0;
	int   i=0;
	float x0 = 0.0f, y0 =0.0f, z0 =0.0f;
	float fx=0.0f, fy=0.0f, fz=0.0f;
	float fCellWidth=0.0f, fCellHeight=0.0f, fCellLength=0.0f;

	x0 = m_fWidth/2.0f;
	y0 = m_fHeight/2.0f;
	z0 = m_fLength/2.0f;

	fCellWidth  = m_fWidth/(float)(m_nColY-1);
	fCellHeight = m_fHeight;
	fCellLength = m_fLength/(float)(m_nRowX-1);
	g_vWorld     = new CVector3 [ g_NumberOfVerts ];
 
	g_NumberOfVerts=0;

		for (loop1 = 0; loop1 < m_nRowX; loop1++)
			for (loop2 = 0; loop2 < m_nColY; loop2++)
			{
				if(!m_bMazeX[loop1*m_nColY+loop2]) g_NumberOfVerts += 4;
				if(!m_bMazeY[loop1*m_nColY+loop2]) g_NumberOfVerts += 4;
			}

		g_vWorld = new CVector3 [g_NumberOfVerts];

		for (loop1 = 0; loop1 < m_nRowX; loop1++)
			for (loop2 = 0; loop2 < m_nColY; loop2++)
			{
					fx = fCellWidth*loop2-x0+m_x0;
					fy = 0.0f-y0+m_y0;
					fz = loop1 * fCellLength-z0+m_z0;

				if(i>g_NumberOfVerts) break;
				if(!m_bMazeX[loop1*m_nColY+loop2])
				{									
					g_vWorld[i++] = CVector3(fx,              fy,  fz );
					g_vWorld[i++] = CVector3(fx+fCellWidth,   fy,  fz);	            
					g_vWorld[i++] = CVector3(fx+fCellWidth,   fy+fCellHeight,   fz);
					g_vWorld[i++] = CVector3(fx,              fy+fCellHeight,   fz);				
				}
				
				if(!m_bMazeY[loop1*m_nColY+loop2])
				{
					g_vWorld[ i++ ] = CVector3(fx, fy, fz+fCellLength );
					g_vWorld[ i++ ] = CVector3(fx, fy, fz);	            
					g_vWorld[ i++ ] = CVector3(fx, fy+fCellHeight,   fz);
					g_vWorld[ i++ ] = CVector3(fx, fy+fCellHeight,   fz+fCellLength);	
				}
			}	
}