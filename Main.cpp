#include "main.h"					
#include "CVector3.h"               
#include "CCamera.h"				
#include "CMaze.h"
#include "CTexture.h"
#include "CLight.h"
#include <iostream>
using namespace std;
#pragma comment(lib, "glu32.lib")			

//skybox 
const int SKYBOX_WIDTH  =600;
const int SKYBOX_LENGTH =600;
const int SKYBOX_HEIGHT =600;

//window
#define SCREEN_WIDTH  800
#define SCREEN_LENGTH 600

bool  g_bFullScreen = TRUE;								
HWND  g_hWnd;											
RECT  g_rRect;											
int   g_cw, g_ch;                                       
HDC   g_hDC;											
HGLRC g_hRC;											

HINSTANCE g_hInstance;		
bool	keys[256];			
bool    keysP[256];
//当前场景  Current scene
int current_scene=1;        

CLight light1,light2;
//控制游戏的状态  Control the state of the game
bool bGameActive=true;      
//防止按键冲突  Prevent button conflicts
bool bShowConsole=false; 

//maze的变量 Variables of maze
//maze的每个cell的尺寸  The size of each cell of maze
const GLfloat CELL_WIDTH  = 20.0f;
const GLfloat CELL_LENGTH = 20.0f;
const GLfloat CELL_HEIGHT = 15.0f;
//maze的总体尺寸，初始化为Normal状态的尺寸  The overall size of the maze, initialized to the size of the Normal state
GLfloat fMaze_Width = 9.0f*CELL_WIDTH;
GLfloat fMaze_Length= 9.0f*CELL_LENGTH;
CMaze maze;

const int nMax_Menu = 4;
struct Struct_MazeMap
{
	char szMaze[256];
	int  nSizeX;
	int  nSizeY;
}MazeMap[4]=
{
	"Data/mazedata/Easy.txt", 5, 5,	 "Data/mazedata/Normal.txt", 10,10,
	"Data/mazedata/Hard.txt", 20,20, "Data/mazedata/Harder.txt", 30,30
};

CVector3 *g_vWorld=NULL;
//用于冲突检测  For conflict detection      
int g_NumberOfVerts =0;			

//纹理的变量  Variables of textur 
//纹理资源在DeInit()中释放  Texture resources are released in DeInit ().
//纹理ID  Texture ID
enum eTex_ID
{
	BGROUND_ID, 
	MENU_FONT_ID, MENU1_ID, MENU2_ID,

	BACK_ID, FRONT_ID, BOTTOM_ID, TOP_ID, LEFT_ID, RIGHT_ID, BOTTOMSNOW_ID,

	MAZE0_TEX_ID, MAZE1_TEX_ID, MAZE2_TEX_ID, MAZE3_TEX_ID,
};
//纹理的数量  Number of textur
const int nMAX_TEXTURE = MAZE3_TEX_ID +1; 

struct Struct_Texture
{
	CTexture texture[nMAX_TEXTURE];
	//存储纹理的id  Id that stores textures
	UINT g_Texture[MAX_TEXTURES];
	//纹理的索引  Index of textur
	int nIndexTex;          
	char szFileTexture[nMAX_TEXTURE][256];
}Texture;

//menu的变量  Variables of menu
enum eLevel_ID
{ EASY_ID, NORMAL_ID, HARD_ID, HARDER_ID};

struct Struct_Menu
{
	int nLevel;
    char szLevel[4][256];
	//是否显示地图  Whether to display the map.
	bool bMazeMap; 
	//当前选项  Current option
	int nOption;   
}Menu=
{
	NORMAL_ID,  //难度 Degree of difficulty 
	{ "Easy","Normal","Hard","Harder"},
	false, //map

};


CCamera g_Camera;

//函数声名 Function name
bool Scene01();  //初始菜单界面
bool Scene02();  //迷宫
bool Scene03();  //过关界面


//函数定义 Function definition
//调整和初始化总账窗口的大小 adjust and initialize the size of the general ledger window
GLvoid ReLoadingSize(GLsizei width, GLsizei height)		// 
{
	if (height==0)										
	{
		height=1;
	}

	glViewport(0,0,width,height);

	//选择投影矩阵  Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);				
	glLoadIdentity();									
	gluOrtho2D(0, 13, 0, 10);

	//选择模型视图矩阵  Select The Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									
}

//读取文本文件的非空行 Read non-blank lines of a text file
void Readstr(FILE *f,char *string)		
{
	do
	{
		fgets(string, 256, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

//display error messages
void Error(char *text, ...)
{
	char buf[1024];
	va_list arg_list;
	va_start(arg_list, text);
	vsprintf (buf, text, arg_list) ;
	MessageBox(GetFocus(),buf,"Error",0);
	va_end(arg_list);
}

//初始化纹理，OpenGL的状态  Initialize the state of sound, texture, OpenGL
void Init(HWND hWnd)
{
	g_hWnd = hWnd;
	GetClientRect(g_hWnd, &g_rRect);
	g_cw = g_rRect.right - g_rRect.left;
	g_ch = g_rRect.bottom- g_rRect.top;
	InitializeOpenGL(g_cw, g_ch);

    ReLoadingSize(g_cw,g_ch);  

	glEnable(GL_TEXTURE_2D);
    GLfloat lightAmbient1[4]={0.3f,0.3f,0.3f,1.0f};
	GLfloat lightDiffuse1[4]={0.2f,0.2f,0.2f,1.0f};
	GLfloat lightPosition1[4]={0.0f,90.0f,0.0f};
	light1.Init(lightAmbient1,lightDiffuse1,lightPosition1,GL_LIGHT1);

    GLfloat lightAmbient2[4]={0.6f,0.6f,0.6f,1.0f};
	GLfloat lightDiffuse2[4]={0.8f,0.8f,0.8f,1.0f};
	GLfloat lightPosition2[4]={0.0f,90.0f,0.0f};
	light2.Init(lightAmbient2, lightDiffuse2, lightPosition2,GL_LIGHT2);
	light2.Disable();

	float fPosition=2.0f;
	int i=0;	

	FILE *filein;
	char oneline[256];
	int nNumSounds=0, nNumTextures=0;

	//加载坐标数据的文件  A file that loads coordinate data
	filein = fopen("Data/ini.txt", "rt");
	if(!filein) { Error("Can't open %s file","Data/ini.txt"); return; }

	//初始化纹理 Initialize textur
	Readstr(filein,oneline);
	sscanf(oneline, "NUMTEXTURES %d", &nNumTextures);
	for(i=0;i<nNumTextures;i++)
	{
		Readstr(filein,oneline);
		sscanf(oneline,"%s",Texture.szFileTexture[i]);
	}

	//纹理  textur
	Texture.texture[BGROUND_ID].LoadTexture(Texture.szFileTexture[BGROUND_ID],1);
	Texture.g_Texture[BGROUND_ID]=Texture.texture[BGROUND_ID].Get_TextureID();

	for(i=MENU_FONT_ID;i<nMAX_TEXTURE;i++)
	{
		Texture.texture[i].LoadTexture(Texture.szFileTexture[i],1);
		Texture.g_Texture[i]=Texture.texture[i].Get_TextureID();
		fPosition+=1.0f;
	}
	fPosition=nMAX_TEXTURE; 	
	Texture.nIndexTex=MAZE0_TEX_ID;

	fclose(filein);

	glEnable(GL_DEPTH_TEST);
	//设置为三维投影 Set to 3D projection
    SizeOpenGLScreen(g_cw,g_ch);
}

WPARAM MainLoop()
{
	MSG msg;
	while(1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					
				break;
            TranslateMessage(&msg);						
            DispatchMessage(&msg);						
        }
		else{ 
			RenderScene();								
        	SwapBuffers(g_hDC);							
        } 
	}
	DeInit();											
	return(msg.wParam);									
}

void RenderScene() 
{
	static bool bFirst=true;

	switch(current_scene)
	{	
	   case 1:
		{
			if(bFirst)
			{
				bFirst=false;
			}
			if(Scene01())
			{
				bFirst = true;
				current_scene=2;
			}
			break;
		}	
	
		case 2:
		{
			if(bFirst)
			{
				bFirst=false;
				//初始化镜头的位置 Initialize the position of the lens
				g_Camera.PositionCamera( 0, -90, 0,	0, -90, -10,	0, 1, 0);					
				g_Camera.Set_CameraRadius( 3.2f);
				fMaze_Width = CELL_WIDTH*(MazeMap[Menu.nLevel].nSizeY-1);
				fMaze_Length= CELL_LENGTH*(MazeMap[Menu.nLevel].nSizeX-1);
			 	maze.DrawMaze_Init(MazeMap[Menu.nLevel].szMaze,MazeMap[Menu.nLevel].nSizeX,MazeMap[Menu.nLevel].nSizeY,0.0f,-(SKYBOX_HEIGHT-CELL_HEIGHT)/6,0.0f,fMaze_Width,CELL_HEIGHT,fMaze_Length,Texture.g_Texture[MAZE0_TEX_ID]);				

				
			}
			if(Scene02())
			{
				bFirst = true;
				current_scene=3;
				
				maze.DrawMaze_Del();
				
				//重置camera Reset camera
				g_Camera.Set_ToReset();
			
			}
			break;
		}
		case 3:
		{
			if(Scene03())
			{
				bFirst=true;
				current_scene=2;
			
	            Menu.nLevel++;      if(Menu.nLevel>HARDER_ID) current_scene=4;
			}
			break;
		}
		case 4:
		{
			if(bFirst)
			{
				bFirst=false;
			}
			break;
		}
	}
}

void DeInit()
{
	if (g_hRC)											
	{
		//释放渲染内存,重置正常状态  Free rendering memory and reset normal state
		wglMakeCurrent(NULL, NULL);	
		wglDeleteContext(g_hRC);
	}
	
	if (g_hDC) 
		//从内存中释放HDC
		ReleaseDC(g_hWnd, g_hDC);
	UnregisterClass("GameTutorials", g_hInstance);
	PostQuitMessage (0);
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LONG    lRet = 0; 
    switch (uMsg)
	{ 
    case WM_SIZE:	
		if(!g_bFullScreen)	
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);					
		}
        break; 

 	 case WM_CHAR: 
		if(wParam=='`') bShowConsole=!bShowConsole;
		break;

	 case WM_KEYUP:
		keys[wParam] = false;					
	 break;

	case WM_KEYDOWN:
		keys[wParam] = true;					
		if(wParam==VK_ESCAPE) PostQuitMessage(0);
		break;

    case WM_DESTROY:									
        PostQuitMessage(0);								
        break; 
     
    default:											
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
  return lRet;	
}

//设置2D投影
void Start2DMode(void)
{
	glMatrixMode(GL_PROJECTION);	
	//存储投影矩阵
	glPushMatrix();
		////重置投影矩阵
		glLoadIdentity();
		glOrtho(0,g_cw,0,g_ch,-1,1);
		glMatrixMode(GL_MODELVIEW);	
}
void End2DMode(void)
{
	glMatrixMode(GL_PROJECTION);
	//恢复旧的投影矩阵。
	glPopMatrix();				
	glMatrixMode(GL_MODELVIEW);	
}

//scene01
void Scene01_DrawBackground();
bool Scene01_ProcessMenuKey();

bool Scene01()
{
	bool bSceneEnd=false;
	if(Scene01_ProcessMenuKey()) bSceneEnd=true;

	////清除屏幕和深度缓冲区  clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	Start2DMode();
		Scene01_DrawBackground();
	End2DMode();
	return bSceneEnd;
}

void Scene01_DrawBackground()
{	
	int nBackWidth=g_cw,     nBackLength=g_ch;
	int nBackX0 = 0,  nBackY0 = 0;

	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[BGROUND_ID]);
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);      		
		glTexCoord2f(0.0f, 0.0f); glVertex2i(nBackX0, nBackY0);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(nBackX0+nBackWidth, nBackY0);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(nBackX0+nBackWidth, nBackY0+nBackLength);          
		glTexCoord2f(0.0f, 1.0f); glVertex2i(nBackX0, nBackY0+nBackLength);
	glEnd();
}


bool Scene01_ProcessMenuKey()
{
	bool bPlayMenuSound=false;
	bool bEnd=false;
	//难度切换 EASY_ID, NORMAL_ID, HARD_ID, HARDER_ID
	if (keys[VK_F1]) {     
		Menu.nLevel = EASY_ID;
	}
	if (keys[VK_F2]) {
		Menu.nLevel = NORMAL_ID;
	}
	if (keys[VK_F3]) {
		Menu.nLevel = HARD_ID;
	}
	if (keys[VK_F4]) {
		Menu.nLevel = HARDER_ID;
	}

	    if (keys[VK_RETURN]||keys[VK_SPACE]) 
		{
			keys[VK_RETURN]=keys[VK_SPACE]=false;
			bEnd = true;
		}
	
		return bEnd;
}


//scene02
void Scene02_DrawSkyBox(float x, float y, float z, float width, float height, float length);
void Scene02_DrawPoint();
void Scene02_ProcessKey();

bool Scene02()
{
	bool bSceneEnd=false;
	//处理键盘 Processing keyboard
	Scene02_ProcessKey();

	//清除屏幕和深度缓冲区
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	CVector3 vPos = g_Camera.Position();
	//如果超出边界则，不渲染 If the boundary is exceeded, do not render
	if(vPos.x<-fMaze_Width/2-5.0f||vPos.z> fMaze_Length/2+5.0f) bSceneEnd = true;

	if(!bShowConsole) g_Camera.Update();

	//collision 检测  Collision detection
	g_Camera.CheckCameraCollision(g_vWorld, g_NumberOfVerts);

	
	// 获取摄像机的当前位置  Get the current position of the camera
	vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;
	CVector3 vView = g_Camera.View();

	//设置camera的位置，以防穿越对象  Set the location of the camera to prevent traversing the object
	{
		vNewPos.y = -97;
		float temp = vNewPos.y - vPos.y;
		vView.y += temp;
		//设置新的摄像头位置  Set the new camera position.
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
								vView.x,	vView.y,	vView.z,	0, 1, 0);
	}
	
	g_Camera.Look();
	//渲染skybox  Render skybox
	Scene02_DrawSkyBox(0, 0, 0, SKYBOX_WIDTH, SKYBOX_HEIGHT, SKYBOX_LENGTH);	

	//渲染maze  Render maze
	maze.DrawMaze();

	glLoadIdentity();


	//渲染小地图  Render Mini Map
	if(Menu.bMazeMap)
	{
		glLoadIdentity();		
		glTranslatef(g_cw*3/4+(0.25f-0.23f)*g_cw/2,(0.25f-0.23f)*g_ch/2,0.0f);	
		maze.DrawMazeMap();
		Scene02_DrawPoint();
	}	

	return bSceneEnd;
}

//坐标原点位于box的中心  The coordinate origin is located at the center of the box.
void Scene02_DrawSkyBox(float x, float y, float z, float width, float height, float length)
{
	// 居中   In the middle （x,y,z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

    glColor3f(1.0f,1.0f,1.0f);
	//将天空贴图的背面纹理绑定到立方体的背面 Bind the back texture of the sky map to the back of the cube
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[BACK_ID]);
	glBegin(GL_QUADS);	
		//为背面指定纹理坐标和顶点  Specify texture coordinates and vertices for the back face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width , y , z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width , y + height , z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x , y + height , z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x , y , z);
	glEnd();

	//Front
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[FRONT_ID]);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x , y , z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x , y + height , z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width , y + height , z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width , y , z + length);
	glEnd();

	// Top
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[TOP_ID]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width , y + height , z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width , y + height , z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x , y + height , z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x , y + height , z);
	glEnd();

	// Left
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[LEFT_ID]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x  ,y + height , z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x , y + height , z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x , y , z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x , y , z);		
	glEnd();

	// Right
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[RIGHT_ID]);
	glBegin(GL_QUADS);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width , y , z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width , y , z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width , y + height , z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width , y + height , z);
	glEnd();

	//Bottom
	glBindTexture(GL_TEXTURE_2D, Texture.g_Texture[BOTTOM_ID]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x , y , z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x , y , z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width , y , z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width , y , z);
	glEnd();
}

// 画一个红点表示位置  draw a red point to represent the position of actor
void Scene02_DrawPoint()
{
	CVector3 vTemp = g_Camera.Position();
	CVector3 vNew;

	vNew.x = (vTemp.x * SCREEN_WIDTH/fMaze_Width+SCREEN_WIDTH/2)*0.23f;
	vNew.y = (-vTemp.z * SCREEN_LENGTH/fMaze_Length+SCREEN_LENGTH/2)*0.23f;
	//如果超出边界则，不渲染  If the boundary is exceeded, do not render
	if(vNew.x<(-fMaze_Width/2*SCREEN_WIDTH/fMaze_Width+SCREEN_WIDTH/2)*0.23f
	 ||vNew.x>(fMaze_Width/2*SCREEN_WIDTH/fMaze_Width+SCREEN_WIDTH/2)*0.23f
	 ||vNew.y<(-fMaze_Length/2 * SCREEN_LENGTH/fMaze_Length+SCREEN_LENGTH/2)*0.23f
	 ||vNew.y>(fMaze_Length/2 * SCREEN_LENGTH/fMaze_Length+SCREEN_LENGTH/2)*0.23f)
	 return;

	//使用二维投影  Use 2D projection
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	Start2DMode();
		glColor3f(1.0f,0.0f,0.0f);
		//根据maze的复杂程度设置点的大小  Set the size of the point according to the complexity of the maze
		if(Menu.nLevel>NORMAL_ID) glPointSize(3);
		else glPointSize(5);
		glBegin(GL_POINTS);
			glVertex2f(vNew.x, vNew.y);
		glEnd();
    End2DMode();
	glEnable(GL_TEXTURE_2D);
	glPopAttrib();
}

void Scene02_ProcessKey()
{
	//space
	if (keys[VK_SPACE]&&!keysP[VK_SPACE])
	{
		keysP[VK_SPACE]=true;
		Texture.nIndexTex++;
		if(Texture.nIndexTex>MAZE3_TEX_ID) Texture.nIndexTex=MAZE0_TEX_ID;				
		maze.Set_WallTexture(Texture.g_Texture[Texture.nIndexTex]);
	}
	if (!keys[VK_SPACE])
	{
		keysP[VK_SPACE]=false;
	}

	//map
	if (keys[VK_TAB]&&!keysP[VK_TAB])
	{
		keysP[VK_TAB]=true;
		Menu.bMazeMap=!Menu.bMazeMap;
	}
	if (!keys[VK_TAB])
	{
		keysP[VK_TAB]=false;
	}

}

//scene03
void Scene03_DrawBackground();

bool Scene03()
{	
	static GLfloat fTransY=0.0f;
	bool bSceneEnd=true;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	Start2DMode();
		glPushMatrix();
		glTranslatef(0.0f,-float(g_ch),0.0f);
		glTranslatef(0.0f,fTransY,0.0f);
		Scene03_DrawBackground();
		glPopMatrix();
	End2DMode();

	return bSceneEnd;
}

//background of scene03
void Scene03_DrawBackground()
{	
	int nBackWidth=g_cw,     nBackLength=g_ch;
	int nBackX0 = 0,  nBackY0 = 0;
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_QUADS); 		
		glTexCoord2f(0.0f, 0.0f); glVertex2i(nBackX0, nBackY0);           
		glTexCoord2f(1.0f, 0.0f); glVertex2i(nBackX0+nBackWidth, nBackY0);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(nBackX0+nBackWidth, nBackY0+nBackLength);      
		glTexCoord2f(0.0f, 1.0f); glVertex2i(nBackX0, nBackY0+nBackLength);
	glEnd();

	int nTextX0 = 250,      nTextY0 = g_ch/2-30;
    
}