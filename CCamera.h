#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <gl\gl.h>									
#include <gl\glu.h>									
#include "CVector3.h" 

#pragma comment(lib, "winmm.lib")

#define BEHIND		0
#define INTERSECTS	1
#define FRONT		2

#ifndef PI 
#define PI 3.14
#endif

class CCamera {

   public:
	CCamera();	
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}
	
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);
	void RotateView(float angle, float X, float Y, float Z);

	// 这将通过鼠标移动来移动相机的视图(第一人称视图)
	void SetViewByMouse(); 
	//这将检查键盘移动
	void SetViewByKeyboard();
	void StrafeCamera(float speed);
	void MoveCamera(float speed);

	//通知窗体的大小
	void NotifyScreenSize(int nx0, int ny0 ,int nWidth, int nHeight){ m_ScreenMiddleX = nx0 + nWidth/2; m_ScreenMiddleY = ny0 + nHeight/2; }

	// Initialize the sphere radius of the camera
	void Set_CameraRadius(float radius) {	m_radius = radius;	};
	void Set_SpeedUp()  { m_kSpeed += 0.5f; if(m_kSpeed > 20.0f) m_kSpeed = 20.0f; }
	void Set_SpeedDown(){ m_kSpeed -= 0.5f; if(m_kSpeed < 2.0f)  m_kSpeed = 2.0f; }
	void Set_ToReset();
	void CheckCameraCollision(CVector3 *pVertices, int numOfVerts);
	void Update();
	void Look();

private:
	CVector3 m_vPosition;		
	CVector3 m_vView;			
	CVector3 m_vUpVector;	
	//strafe vector
	CVector3 m_vStrafe;				
	float m_radius;										

	//屏幕的一半size
	int m_ScreenMiddleX, m_ScreenMiddleY;
    float  m_kSpeed;	
    float  m_currentRotX;
	//Camera中的数学方法
float Absolute(float num);
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);
//return size of a normal
float Magnitude(CVector3 vNormal);
//returns a normalize vector
CVector3 Normalize(CVector3 vNormal);
//returns the normal of polygon
CVector3 Normal(CVector3 vPolygon[]);
//returns the distance between 2/3D points
float Distance(CVector3 vPoint1, CVector3 vPoint2);
CVector3 ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint);
//returns the distance the plane is from the origin (0, 0, 0)
float PlaneDistance(CVector3 Normal, CVector3 Point);
//takes a triangle (plane) and line and returns true if they intersected
bool IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance);
//returns the dot product between 2 vectors
float Dot(CVector3 vVector1, CVector3 vVector2);
//returns the angle between 2 vectors
double AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2);
//returns an intersection point of a polygon and a line (assuming intersects the plane)
CVector3 IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance);
//returns true if the intersection point is inside of the polygon
bool InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount);
//test the collision between lines and faces
bool IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount);
int ClassifySphere(CVector3 &vCenter, 
				   CVector3 &vNormal, CVector3 &vPoint, float radius, float &distance);
bool EdgeSphereCollision(CVector3 &vCenter, 
						 CVector3 vPolygon[], int vertexCount, float radius);
bool SpherePolygonCollision(CVector3 vPolygon[], 
							CVector3 &vCenter, int vertexCount, float radius);
CVector3 GetCollisionOffset(CVector3 &vNormal, float radius, float distance);
};


