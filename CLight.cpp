#include "CLight.h"

void CLight::
   Init(GLfloat lightAmbient[3], GLfloat lightDiffuse[3],  GLfloat lightPosition[3], GLuint  lightNumber)
{
	glLightfv(lightNumber, GL_AMBIENT, lightAmbient);		//ambient light
	glLightfv(lightNumber, GL_DIFFUSE, lightDiffuse);		//diffuse light
	glLightfv(lightNumber, GL_POSITION, lightPosition);	 	//position slight
	m_lightNumber=lightNumber;
	Enable();
}
