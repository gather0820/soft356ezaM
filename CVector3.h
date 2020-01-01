#pragma once
struct CVector3
{
public:
	CVector3() {}
	// This is our constructor that allows us to initialize our data upon creating an instance
	//构造函数：允许在创建实例时初始化数据
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Here we overload the + operator so we can add vectors together 
	//重载 + 运算符，将向量相加
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result. 返回相加向量结果 
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	//重载-运算符，减去向量
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result  返回减去的向量结果
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the * operator so we can multiply by scalars
	//重载*运算符，乘以标量
	CVector3 operator*(float num)
	{
		// Return the scaled vector 返回相乘的向量结果
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	////重载 / 运算符，除以标量
	CVector3 operator/(float num)
	{
		// Return the scale vector 返回相除的向量结果
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};
