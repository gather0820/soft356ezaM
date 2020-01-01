#include "main.h"
#include<stdio.h>

void ChangeToFullScreen()
{
	DEVMODE dmSettings = {0};							// Device Mode variable  设备模式变量

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

	memset(&wndclass, 0, sizeof(WNDCLASS));				// Init the size of the class 输入类的大小
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Regular drawing capabilities 常规绘图功能
	wndclass.lpfnWndProc = WinProc;						// Pass our function pointer as the window procedure 将函数指针作为窗口过程传递
	wndclass.hInstance = hInstance;						// Assign our hInstance 分配hInstance
	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);		// An arrow for the cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window 光标的箭头
	wndclass.lpszClassName = "GameTutorials";			// Assign the class name

	RegisterClass(&wndclass);							// Register the class
	
	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0  
	rWindow.right	= width;							// Set Right Value To Requested Width 将右值设置为请求的宽度。
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height 将底值设置为请求的高度

	
		dwExStyle=WS_EX_APPWINDOW;			// Window Extended Style 窗口扩展样式
		dwStyle = WS_CAPTION;

	g_hInstance = hInstance;		// Assign our global hInstance to the window's hInstance将全局hInstance分配给窗口的hInstance

	AdjustWindowRect( &rWindow, dwStyle, false);		

	//创建窗口 Create the window
	hWnd = CreateWindowEx(dwExStyle,
						"GameTutorials", strWindowName, dwStyle, nX0, nY0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;								// If we could get a handle, return NULL 如果我们可以获得句柄，则返回NULL。

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Show the window 显示视窗
	UpdateWindow(hWnd);									// Draw the window 画视窗

	SetFocus(hWnd);										// Sets Keyboard Focus To The Window	 将键盘焦点设置到窗口

	return hWnd;
}

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure  设置结构的大小 
    pfd.nVersion = 1;									// Always set this to 1  始终将其设置为1 
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask  标准蒙版
    pfd.iPixelType = PFD_TYPE_RGBA;						// RGB and Alpha pixel type   RGB和Alpha像素类型 
    pfd.cDepthBits = SCREEN_DEPTH;						
    pfd.cAccumBits = 0;									// No special bitplanes needed 不需要特殊的位平面 
    pfd.cStencilBits = 0;								// We desire no stencil bits  不想要钢丝网 
 
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
void SizeOpenGLScreen(int width, int height)			// Initialize The GL Window 初始化GL窗口 
{
	if (height==0)										// Prevent A Divide By Zero error 防止被除零的错误 
	{
		height=1;										// Make the Height Equal One 使高度等于一 
	}

	glViewport(0,0,width,height);						// Make our viewport the whole window  使我们的视口成为整个窗口 

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix  选择投影矩阵 
	glLoadIdentity();									// Reset The Projection Matrix  重置投影矩阵 
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f, 900.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix  选择模型视图矩阵 
	glLoadIdentity();									// Reset The Modelview Matrix  重置模型视图矩阵
}

//INITIALIZE GL 
void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);								// This sets our global HDC  整体HDC设置（在结束前不释放HDC）
    if (!bSetupPixelFormat(g_hDC))						// This sets our pixel format/information  设置像素格式/信息
        PostQuitMessage (0);							// If there's an error, quit

	g_hRC = wglCreateContext(g_hDC);					// This creates a rendering context from our hdc  从HDC创建context
    wglMakeCurrent(g_hDC, g_hRC);// the rendering context = want to use 使刚创建的context成为我们想要使用的context

	SizeOpenGLScreen(width, height);					// Setup the screen translations and viewport 设置屏幕平移和视口
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;
	g_bFullScreen = false;
	hWnd = CreateMyWindow("Maze356", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	// If we never got a valid window handle, quit the program
	//如果我们从未获得有效的窗口句柄，退出程序
	if(hWnd == NULL) return TRUE;

	// INIT OpenGL
	Init(hWnd);	
	ShowCursor(false);
		
	// Run message loop and after it's done, return the result
	//运行消息循环，并在完成后返回结果
	return MainLoop();						
}
