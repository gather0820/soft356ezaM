#include "main.h"
#include<stdio.h>

void ChangeToFullScreen()
{
	DEVMODE dmSettings = {0};							// Device Mode variable  �豸ģʽ����

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;			
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;		
	dmSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
}

//CREATE WINDOW 

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	DWORD		dwExStyle;		
	int nX0=0; int nY0=0;

	memset(&wndclass, 0, sizeof(WNDCLASS));				// Init the size of the class ������Ĵ�С
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Regular drawing capabilities �����ͼ����
	wndclass.lpfnWndProc = WinProc;						// Pass our function pointer as the window procedure ������ָ����Ϊ���ڹ��̴���
	wndclass.hInstance = hInstance;						// Assign our hInstance ����hInstance
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);		// An arrow for the cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window ���ļ�ͷ
	wndclass.lpszClassName = "GameTutorials";			// Assign the class name

	RegisterClass(&wndclass);							// Register the class
	
	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0  
	rWindow.right	= width;							// Set Right Value To Requested Width ����ֵ����Ϊ����Ŀ�ȡ�
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height ����ֵ����Ϊ����ĸ߶�

	
		dwExStyle=WS_EX_APPWINDOW;			// Window Extended Style ������չ��ʽ
		dwStyle = WS_CAPTION;

	g_hInstance = hInstance;		// Assign our global hInstance to the window's hInstance��ȫ��hInstance��������ڵ�hInstance

	AdjustWindowRect( &rWindow, dwStyle, false);		

	//�������� Create the window
	hWnd = CreateWindowEx(dwExStyle,
						"GameTutorials", strWindowName, dwStyle, nX0, nY0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;								// If we could get a handle, return NULL ������ǿ��Ի�þ�����򷵻�NULL��

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Show the window ��ʾ�Ӵ�
	UpdateWindow(hWnd);									// Draw the window ���Ӵ�

	SetFocus(hWnd);										// Sets Keyboard Focus To The Window	 �����̽������õ�����

	return hWnd;
}

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure  ���ýṹ�Ĵ�С 
    pfd.nVersion = 1;									// Always set this to 1  ʼ�ս�������Ϊ1 
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask  ��׼�ɰ�
    pfd.iPixelType = PFD_TYPE_RGBA;						// RGB and Alpha pixel type   RGB��Alpha�������� 
    pfd.cDepthBits = SCREEN_DEPTH;						
    pfd.cAccumBits = 0;									// No special bitplanes needed ����Ҫ�����λƽ�� 
    pfd.cStencilBits = 0;								// We desire no stencil bits  ����Ҫ��˿�� 
 
	// Get a matching pixel format
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
	// Set the pixel format
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;									
}

// Window
void SizeOpenGLScreen(int width, int height)			// Initialize The GL Window ��ʼ��GL���� 
{
	if (height==0)										// Prevent A Divide By Zero error ��ֹ������Ĵ��� 
	{
		height=1;										// Make the Height Equal One ʹ�߶ȵ���һ 
	}

	glViewport(0,0,width,height);						// Make our viewport the whole window  ʹ���ǵ��ӿڳ�Ϊ�������� 

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix  ѡ��ͶӰ���� 
	glLoadIdentity();									// Reset The Projection Matrix  ����ͶӰ���� 
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f, 900.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix  ѡ��ģ����ͼ���� 
	glLoadIdentity();									// Reset The Modelview Matrix  ����ģ����ͼ����
}

//INITIALIZE GL 
void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);								// This sets our global HDC  ����HDC���ã��ڽ���ǰ���ͷ�HDC��
    if (!bSetupPixelFormat(g_hDC))						// This sets our pixel format/information  �������ظ�ʽ/��Ϣ
        PostQuitMessage (0);							// If there's an error, quit

	g_hRC = wglCreateContext(g_hDC);					// This creates a rendering context from our hdc  ��HDC����context
    wglMakeCurrent(g_hDC, g_hRC);// the rendering context = want to use ʹ�մ�����context��Ϊ������Ҫʹ�õ�context

	SizeOpenGLScreen(width, height);					// Setup the screen translations and viewport ������Ļƽ�ƺ��ӿ�
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;
	g_bFullScreen = false;
	hWnd = CreateMyWindow("Maze356", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	// If we never got a valid window handle, quit the program
	//������Ǵ�δ�����Ч�Ĵ��ھ�����˳�����
	if(hWnd == NULL) return TRUE;

	// INIT OpenGL
	Init(hWnd);	
	ShowCursor(false);
		
	// Run message loop and after it's done, return the result
	//������Ϣѭ����������ɺ󷵻ؽ��
	return MainLoop();						
}
