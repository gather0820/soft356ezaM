#pragma once
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "CVector3.h" 
#include "main.h"

class CMaze
{
    public:
		int m_nViewWidth, m_nViewHeight;        //存储ViewPort的尺寸  Store the size of the ViewPort.
		float m_x0, m_y0, m_z0;                 //maze的中心位置  The central position of maze.
		float m_fWidth, m_fLength, m_fHeight;   //maze的总体尺寸  Overall size of maze.
		int m_nRowX, m_nColY;                   //在X，Y方向上cell的数量  The number of cell in the XQuery Y direction.
		GLuint m_TexID;                         //maze的纹理对象  Texture objects for maze.
		GLuint m_MazeList, m_MazeSmallList;     //maze的显示列表  Display list of maze

		CMaze();
		~CMaze(){ DrawMaze_Del(); }

		void DrawMaze_Init(char *FileMaze, int p_nRowX, int p_nColY, float p_x0, float p_y0, float pz0, float p_fWidth=5.0f, float p_fHeight=5.0f, float p_fLength=5.0f, GLuint p_TexID=NULL);
		void DrawMaze();                        //渲染maze  Render maze
		void DrawMazeMap();                   //渲染maze地图  Render maze map
		void DrawMaze_Del();
		void Set_WallTexture(GLuint p_TexID){ m_TexID = p_TexID; }

	private:
		bool * m_bMazeX, * m_bMazeY;
		void Readstr(FILE *f,char *string);    //从文本文件中读取一行  Read a line from a text file.
		void SetupWorld(char *FileMaze);       //读取maze数据  Read maze data.
		void GenerateList();                   //生成显示列表  Generate display list.
		void Start2DMode();                    //启动2D显示模式  Start 2D display mode.
		void End2DMode();                      //结束2D显示模式  End 2D display mode.
		void LoadVertices();                   //装载用于collision检验的顶点  Load vertices for collision verification.
		void Error(char *text, ...);	       //报错  Error
};
