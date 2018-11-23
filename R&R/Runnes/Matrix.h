#pragma once

#pragma warning(disable:4244)
#include "vector.h"
#include <math.h>
class CMatrix
{
public:
	CMatrix(void);

	float   m11,m12,m13,m14,
			m21,m22,m23,m24,
			m31,m32,m33,m34,
			m41,m42,m43,m44;

	CMatrix(float _m11, float _m12, float _m13, float _m14,
			float _m21, float _m22, float _m23, float _m24,
			float _m31, float _m32, float _m33, float _m34,
			float _m41, float _m42, float _m43, float _m44);
	
	static CMatrix Identity()
	{
		CMatrix ma;
		ma.m11=ma.m22=ma.m33=ma.m44=1.0f;

		return ma;
	}

	static CMatrix CreateRotationX(const float &degree)
	{
		CMatrix ma;
		const float Pi=3.141592654f;
		float f = (float)degree*Pi/ 180.0f;

		ma.m11=ma.m44=1.0f;

		ma.m22=ma.m33= cosf(f);
		ma.m23 = -sinf(f);
		ma.m32 = -ma.m23;

		return ma;
	}
	static CMatrix CreateRotationY(const float &degree)
	{
		CMatrix ma;
		const float Pi=3.141592654f;
		float f = (float)degree*Pi/ 180.0f;

		ma.m22=ma.m44=1.0f;

		ma.m11=ma.m33= cosf(f);
		ma.m13 = sinf(f);
		ma.m31 = -ma.m13;

		return ma;
	}

	~CMatrix(void);

	CMatrix operator* (const CMatrix &m) { 
		CMatrix c;

		c.m11=m11*m.m11 + m12*m.m21 + m13*m.m31 + m14*m.m41;
		c.m12=m11*m.m12 + m12*m.m22 + m13*m.m32 + m14*m.m42;
		c.m13=m11*m.m13 + m12*m.m23 + m13*m.m33 + m14*m.m43;
		c.m14=m11*m.m14 + m12*m.m24 + m13*m.m34 + m14*m.m44;

		c.m21=m21*m.m11 + m22*m.m21 + m23*m.m31 + m24*m.m41;
		c.m22=m21*m.m12 + m22*m.m22 + m23*m.m32 + m24*m.m42;
		c.m23=m21*m.m13 + m22*m.m23 + m23*m.m33 + m24*m.m43;
		c.m24=m21*m.m14 + m22*m.m24 + m23*m.m34 + m24*m.m44;

		c.m31=m31*m.m11 + m32*m.m21 + m33*m.m31 + m34*m.m41;
		c.m32=m31*m.m12 + m32*m.m22 + m33*m.m32 + m34*m.m42;
		c.m33=m31*m.m13 + m32*m.m23 + m33*m.m33 + m34*m.m43;
		c.m34=m31*m.m14 + m32*m.m24 + m33*m.m34 + m34*m.m44;

		c.m41=m41*m.m11 + m42*m.m21 + m43*m.m31 + m44*m.m41;
		c.m42=m41*m.m12 + m42*m.m22 + m43*m.m32 + m44*m.m42;
		c.m43=m41*m.m13 + m42*m.m23 + m43*m.m33 + m44*m.m43;
		c.m44=m41*m.m14 + m42*m.m24 + m43*m.m34 + m44*m.m44;

		return c;
	}

	CVector4 operator* (const CVector4 &v) { 
		CVector4 c;

		c.x=m11*v.x + m12*v.y + m13*v.z + m14*v.w;
		c.y=m21*v.x + m22*v.y + m23*v.z + m24*v.w;
		c.z=m31*v.x + m32*v.y + m33*v.z + m34*v.w;
		c.w=m41*v.x + m42*v.y + m43*v.z + m44*v.w;
		return c;
	}

	static CMatrix CreateRotationFromAxisAngle(const float &degree, const CVector3 &dir)
	{
		CMatrix ma;
		const float Pi=3.141592654f;
		//float f = (float)(degree-360.0f)*Pi/ 180.0f;
		float f = (float)degree*Pi/ 180.0f;
		float c = cosf(f);
		float s = sinf(f);

		///////////////////////////////////////////
		ma.m11 = dir.x*dir.x*(1.0-c)+c;
		ma.m12 = dir.x*dir.y*(1.0-c)+dir.z*s;
		ma.m13 = dir.x*dir.z*(1.0-c)+dir.y*s;
		//////////////////////////////////////////
		ma.m21 = dir.y*dir.x*(1.0-c)+dir.z*s;
		ma.m22 = dir.y*dir.y*(1.0-c)+c;
		ma.m23 = dir.y*dir.z*(1.0-c)+dir.x*s;
		//////////////////////////////////////////
		ma.m31 = dir.x*dir.z*(1.0-c)+dir.y*s;
		ma.m32 = dir.y*dir.z*(1.0-c)+dir.x*s;
		ma.m33 = dir.z*dir.z*(1.0-c)+c;

		ma.m44=1.0;

		return ma;
	}
};
