#pragma once
#include <windows.h>		
#include <gl\gl.h>			
#include <gl\glu.h>

class CLight				
{
	public:
		void Init(GLfloat A1, GLfloat A2, GLfloat A3, GLfloat A4,
				  GLfloat D1, GLfloat D2, GLfloat D3, GLfloat D4,
				  GLfloat P1, GLfloat P2, GLfloat P3, GLfloat P4,
				  GLuint lightNumber);
		void Init(GLfloat lightAmbient[4],
				  GLfloat lightDiffuse[4],
				  GLfloat lightPosition[4],
				  GLuint  lightNumber);
		void Enable() { glEnable(GL_LIGHTING);  glEnable(m_lightNumber);}
		void Disable(){ glDisable(GL_LIGHTING); glDisable(m_lightNumber);}
		CLight(){ m_lightNumber=GL_LIGHT0; }

	private:
		GLuint  m_lightNumber;
};
