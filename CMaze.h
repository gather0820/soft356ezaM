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
		int m_nViewWidth, m_nViewHeight;        //�洢ViewPort�ĳߴ�  Store the size of the ViewPort.
		float m_x0, m_y0, m_z0;                 //maze������λ��  The central position of maze.
		float m_fWidth, m_fLength, m_fHeight;   //maze������ߴ�  Overall size of maze.
		int m_nRowX, m_nColY;                   //��X��Y������cell������  The number of cell in the XQuery Y direction.
		GLuint m_TexID;                         //maze���������  Texture objects for maze.
		GLuint m_MazeList, m_MazeSmallList;     //maze����ʾ�б�  Display list of maze

		CMaze();
		~CMaze(){ DrawMaze_Del(); }

		void DrawMaze_Init(char *FileMaze, int p_nRowX, int p_nColY, float p_x0, float p_y0, float pz0, float p_fWidth=5.0f, float p_fHeight=5.0f, float p_fLength=5.0f, GLuint p_TexID=NULL);
		void DrawMaze();                        //��Ⱦmaze  Render maze
		void DrawMazeMap();                   //��Ⱦmaze��ͼ  Render maze map
		void DrawMaze_Del();
		void Set_WallTexture(GLuint p_TexID){ m_TexID = p_TexID; }

	private:
		bool * m_bMazeX, * m_bMazeY;
		void Readstr(FILE *f,char *string);    //���ı��ļ��ж�ȡһ��  Read a line from a text file.
		void SetupWorld(char *FileMaze);       //��ȡmaze����  Read maze data.
		void GenerateList();                   //������ʾ�б�  Generate display list.
		void Start2DMode();                    //����2D��ʾģʽ  Start 2D display mode.
		void End2DMode();                      //����2D��ʾģʽ  End 2D display mode.
		void LoadVertices();                   //װ������collision����Ķ���  Load vertices for collision verification.
		void Error(char *text, ...);	       //����  Error
};
