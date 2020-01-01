#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <htmlhelp.h>     
#include <math.h>
#include <gl\gl.h>										
#include <gl\glu.h>										
#include "resource\resource.h"

#define SCREEN_WIDTH 800			
#define SCREEN_HEIGHT 600			
#define SCREEN_DEPTH 16				
#define MAX_TEXTURES 1000			
#define GL_CLAMP_TO_EDGE	0x812F	

extern bool  g_bFullScreen;									
extern HWND  g_hWnd;										
extern RECT  g_rRect;										
extern HDC   g_hDC;											
extern HGLRC g_hRC;											
extern HINSTANCE g_hInstance;								

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop();
void ChangeToFullScreen();
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

//为OpenGL和BackBuffered配置视窗 Configure Windows for OpenGL and BackBuffered
bool bSetupPixelFormat(HDC hdc);

//初始化屏幕平移和投影 Initialize screen panning and projection
void SizeOpenGLScreen(int width, int height);
void InitializeOpenGL(int width, int height);

//初始化整个程序  Initialize the entire program
void Init(HWND hWnd);

//Draw
void RenderScene();

//释放程序中所有内存  Free all memory in the program
void DeInit();



