#include "CCamera.h"
								
// Our global float that stores the elapsed time between the current and last frame
float g_FrameInterval = 0.0f;
//This function calculates the frame rate and time intervals between frames
//此函数用于计算帧速率和帧之间的时间间隔
void CalculateFrameRate()
{
    static float LastTime			= 0.0f;	
    float currentTime = timeGetTime() * 0.001f;	 
 	g_FrameInterval = currentTime - LastTime;
	LastTime = currentTime;
}


CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 1.0);
	CVector3 vView = CVector3(0.0, 0.0, 2.5);
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);

	m_vPosition	= vZero;
	m_vView		= vView;
	m_vUpVector	= vUp;	

	m_ScreenMiddleX = 400;
	m_ScreenMiddleY = 300;

	m_kSpeed = 50.0f;
	m_currentRotX=0.0f;
}

void CCamera::Set_ToReset()
{
	CVector3 vZero = CVector3(0.0, 0.0, 1.0);		
	CVector3 vView = CVector3(0.0, 0.0, 2.5);		 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		

	m_vPosition	= vZero;
	m_vView		= vView;
	m_vUpVector	= vUp;	

	m_ScreenMiddleX = 400;
	m_ScreenMiddleY = 300;

	m_kSpeed = 50.0f;
	m_currentRotX=0.0f;
}

//his function sets the camera's position and view and up vVector.该函数设置摄影机的位置和视图以及上方向向量。
void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	m_vPosition = vPosition;	
	m_vView     = vView;		
	m_vUpVector = vUpVector;	
}


//This allows us to look around using the mouse, like in most first person games.
void CCamera::SetViewByMouse()
{
	POINT mousePos;									
 	float angleY = 0.0f;							
	float angleZ = 0.0f;						
	GetCursorPos(&mousePos);						
	
	if( (mousePos.x == m_ScreenMiddleX) && (mousePos.y == m_ScreenMiddleY) ) return;

	SetCursorPos(m_ScreenMiddleX, m_ScreenMiddleY);							

	angleY = (float)( (m_ScreenMiddleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (m_ScreenMiddleY - mousePos.y) ) / 500.0f;		

	m_currentRotX -= angleZ;  

	if(m_currentRotX > 1.0f)
		m_currentRotX = 1.0f;
	else if(m_currentRotX < -1.0f)
		m_currentRotX = -1.0f;
	else
	{
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	}
}

void CCamera::SetViewByKeyboard()
{	
	float speed = m_kSpeed * g_FrameInterval;

	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
		MoveCamera(speed);				
	}

	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			
		MoveCamera(-speed);				
	}

	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {
		StrafeCamera(-speed);
	}

	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
		StrafeCamera(speed);
	}	
}


//This rotates the view around the position using an axis-angle rotation这将使用轴角旋转围绕该位置旋转视图
void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;
	CVector3 vView = m_vView - m_vPosition;		

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	m_vView = m_vPosition + vNewView;
}


void CCamera::StrafeCamera(float speed)
{	
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}


//这将根据速度向前或向后移动相机This will move the camera forward or backward depending on the speed
void CCamera::MoveCamera(float speed)
{
	//获取当前视图矢量(正在看的方向)Get the current view vector (the direction you are looking at)
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;
	m_vPosition.y += vVector.y * speed;
	m_vPosition.z += vVector.z * speed;
	m_vView.x += vVector.x * speed;	
	m_vView.y += vVector.y * speed;	
	m_vView.z += vVector.z * speed;	
}


//This checks all the polygons in our list and offsets the camera if collided
//pVertices和numOfVerts以绘制quads的形式给出即可
void CCamera::CheckCameraCollision(CVector3 *pVertices, int numOfVerts)
{	
	for(int i = 0; i < numOfVerts; i += 4)
	{
		CVector3 vTemp[4] = { pVertices[i], pVertices[i+1], pVertices[i+2],pVertices[i+3] };
		CVector3 vTriangle[3];
		for(int j=0; j<2; j++)
		{
			if(j==0)
			{ vTriangle[0] = vTemp[0]; vTriangle[1] = vTemp[1]; vTriangle[2] = vTemp[2]; };
			if(j==1)
			{ vTriangle[0] = vTemp[2]; vTriangle[1] = vTemp[3]; vTriangle[2] = vTemp[0]; };

			CVector3 vNormal = Normal(vTriangle);
			float distance = 0.0f;
			int classification = ClassifySphere(m_vPosition, vNormal, vTriangle[0], m_radius, distance);

			if(classification == INTERSECTS) 
			{
				CVector3 vOffset = vNormal * distance;
				CVector3 vIntersection = m_vPosition - vOffset;

				if(InsidePolygon(vIntersection, vTriangle, 3) ||
				   EdgeSphereCollision(m_vPosition, vTriangle, 3, m_radius / 2))
				{
					vOffset = GetCollisionOffset(vNormal, m_radius, distance);
					m_vPosition = m_vPosition + vOffset;
					m_vView = m_vView + vOffset;
				}
			}
		}
	}
}

//This updates the camera's view and strafe vector
void CCamera::Update() 
{
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);
	m_vStrafe = Normalize(vCross);
	SetViewByMouse();
	SetViewByKeyboard();
	CalculateFrameRate();
}


//This updates the camera according to the 
void CCamera::Look()
{
	// Give openGL our camera position, then camera view, then camera up vector
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

//This returns the absolute value of the number passed in
float CCamera::Absolute(float num)
{
	if(num < 0)
		return (0 - num);
	return num;
}


//This returns a perpendicular vector from 2 given vectors by taking the cross product.											
CVector3 CCamera::Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;									
}


//This returns the magnitude of a normal (or any other vector)
float CCamera::Magnitude(CVector3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
}


//This returns a normalize vector (A vector exactly of length 1)
CVector3 CCamera::Normalize(CVector3 vNormal)
{
	float magnitude = Magnitude(vNormal);	
	vNormal.x /= magnitude;	
	vNormal.y /= magnitude;	
	vNormal.z /= magnitude;	
	return vNormal;	
}

//This returns the normal of a polygon (The direction the polygon is facing)
CVector3 CCamera::Normal(CVector3 vPolygon[])					
{													
	CVector3 vVector1 = vPolygon[2] - vPolygon[0];
	CVector3 vVector2 = vPolygon[1] - vPolygon[0];
	CVector3 vNormal = Cross(vVector1, vVector2);	
	vNormal = Normalize(vNormal);	
	return vNormal;										
}


//This returns the distance between 2 3D points
float CCamera::Distance(CVector3 vPoint1, CVector3 vPoint2)
{
	double distance = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
						    (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
						    (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );
	return (float)distance;
}


//This returns the point on the line vA_vB that is closest to the point vPoint
CVector3 CCamera::ClosestPointOnLine(CVector3 vA, CVector3 vB, CVector3 vPoint)
{
	CVector3 vVector1 = vPoint - vA;
    CVector3 vVector2 = Normalize(vB - vA);

    float d = Distance(vA, vB);
    float t = Dot(vVector2, vVector1);

    if (t <= 0) 
		return vA;
    if (t >= d) 
		return vB;
    CVector3 vVector3 = vVector2 * t;
    CVector3 vClosestPoint = vA + vVector3;
	return vClosestPoint;
}

//This returns the distance between a plane and the origin				
float CCamera::PlaneDistance(CVector3 Normal, CVector3 Point)
{	
	float distance = 0;							
	distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

	return distance;									
}


//This checks to see if a line intersects a plane
bool CCamera::IntersectedPlane(CVector3 vPoly[], CVector3 vLine[], CVector3 &vNormal, float &originDistance)
{
	float distance1=0, distance2=0;	
	vNormal = Normal(vPoly);
	originDistance = PlaneDistance(vNormal, vPoly[0]);
	distance1 = ((vNormal.x * vLine[0].x)  +					
		         (vNormal.y * vLine[0].y)  +					
				 (vNormal.z * vLine[0].z)) + originDistance;	

	distance2 = ((vNormal.x * vLine[1].x)  +					
		         (vNormal.y * vLine[1].y)  +					
				 (vNormal.z * vLine[1].z)) + originDistance;	
	if(distance1 * distance2 >= 0)			
	   return false;
	return true;					
}


//This computers the dot product of 2 vectors
float CCamera::Dot(CVector3 vVector1, CVector3 vVector2) 
{
	return ( (vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z) );
}


//This checks to see if a point is inside the ranges of a polygon
double CCamera::AngleBetweenVectors(CVector3 Vector1, CVector3 Vector2)
{							
	float dotProduct = Dot(Vector1, Vector2);				
	float vectorsMagnitude = Magnitude(Vector1) * Magnitude(Vector2) ;
	double angle = acos( dotProduct / vectorsMagnitude );
	if(_isnan(angle))
		return 0;
	return( angle );
}

//This returns the intersection point of the line that intersects the plane								
CVector3 CCamera::IntersectionPoint(CVector3 vNormal, CVector3 vLine[], double distance)
{
	CVector3 vPoint, vLineDir;	
	double Numerator = 0.0, Denominator = 0.0, dist = 0.0;

	vLineDir = vLine[1] - vLine[0];	
	vLineDir = Normalize(vLineDir);	

	Numerator = - (vNormal.x * vLine[0].x +	
				   vNormal.y * vLine[0].y +
				   vNormal.z * vLine[0].z + distance);
	Denominator = Dot(vNormal, vLineDir);
	if( Denominator == 0.0)
		return vLine[0];	
	dist = Numerator / Denominator;	
	vPoint.x = (float)(vLine[0].x + (vLineDir.x * dist));
	vPoint.y = (float)(vLine[0].y + (vLineDir.y * dist));
	vPoint.z = (float)(vLine[0].z + (vLineDir.z * dist));

	return vPoint;
}

//This checks to see if a point is inside the ranges of a polygon
bool CCamera::InsidePolygon(CVector3 vIntersection, CVector3 Poly[], long verticeCount)
{
	const double MATCH_FACTOR = 0.99;		
	double Angle = 0.0;						
	CVector3 vA, vB;						
	
	for (int i = 0; i < verticeCount; i++)	
	{	
		vA = Poly[i] - vIntersection;		
		vB = Poly[(i + 1) % verticeCount] - vIntersection;
		Angle += AngleBetweenVectors(vA, vB);
	}
	if(Angle >= (MATCH_FACTOR * (2.0 * PI)) )
		return true;				
	return false;							
}

//This checks if a line is intersecting a polygon
bool CCamera::IntersectedPolygon(CVector3 vPoly[], CVector3 vLine[], int verticeCount)
{
	CVector3 vNormal;
	float originDistance = 0;
	if(!IntersectedPlane(vPoly, vLine,   vNormal,   originDistance))
		return false;
	CVector3 vIntersection = IntersectionPoint(vNormal, vLine, originDistance);

	if(InsidePolygon(vIntersection, vPoly, verticeCount))
		return true;	

	return false;							
}

//This tells if a sphere is BEHIND, in FRONT, or INTERSECTS a plane, also it's distance
int CCamera::ClassifySphere(CVector3 &vCenter, 
				   CVector3 &vNormal, CVector3 &vPoint, float radius, float &distance)
{
	float d = (float)PlaneDistance(vNormal, vPoint);

	distance = (vNormal.x * vCenter.x + vNormal.y * vCenter.y + vNormal.z * vCenter.z + d);
	if(Absolute(distance) < radius)
		return INTERSECTS;
	else if(distance >= radius)
		return FRONT;
	return BEHIND;
}

//This returns true if the sphere is intersecting any of the edges of the polygon
bool CCamera::EdgeSphereCollision(CVector3 &vCenter,CVector3 vPolygon[], int vertexCount, float radius)
{
	CVector3 vPoint;
	for(int i = 0; i < vertexCount; i++)
	{
		vPoint = ClosestPointOnLine(vPolygon[i], vPolygon[(i + 1) % vertexCount], vCenter);
		float distance = Distance(vPoint, vCenter);
		if(distance < radius)
			return true;
	}
	return false;
}

//This returns true if our sphere collides with the polygon passed in
bool CCamera::SpherePolygonCollision(CVector3 vPolygon[], 
							CVector3 &vCenter, int vertexCount, float radius)
{
	CVector3 vNormal = Normal(vPolygon);

	float distance = 0.0f;

	int classification = ClassifySphere(vCenter, vNormal, vPolygon[0], radius, distance);

	if(classification == INTERSECTS) 
	{
		CVector3 vOffset = vNormal * distance;
		CVector3 vPosition = vCenter - vOffset;

		if(InsidePolygon(vPosition, vPolygon, 3))
			return true;	
		else
		{
			if(EdgeSphereCollision(vCenter, vPolygon, vertexCount, radius))
			{
				return true;	
			}
		}
	}
	return false;
}


//This returns the offset to move the center of the sphere off the collided polygon
CVector3 CCamera::GetCollisionOffset(CVector3 &vNormal, float radius, float distance)
{
	CVector3 vOffset = CVector3(0, 0, 0);
	if(distance > 0)
	{
		float distanceOver = radius - distance;
		vOffset = vNormal * distanceOver;
	}
	else 
	{
		float distanceOver = radius + distance;
		vOffset = vNormal * -distanceOver;
	}
	return vOffset;
}