#include "Matrix.h"

CMatrix::CMatrix(void)
{
	m11=m12=m13=m14=m21=m22=m23=m24=m31=m32=m33=m34=m41=m42=m43=m44=0.0f;
}
CMatrix::CMatrix(float _m11, float _m12, float _m13, float _m14,
				 float _m21, float _m22, float _m23, float _m24,
				 float _m31, float _m32, float _m33, float _m34,
				 float _m41, float _m42, float _m43, float _m44)
{
	m11=_m11; m12=_m12; m13=_m13; m14=_m14;
	m21=_m21; m22=_m22; m23=_m23; m24=_m24;
	m31=_m31; m32=_m32; m33=_m33; m34=_m34;
	m14=_m14; m24=_m24; m34=_m34; m44=_m44;
}
CMatrix::~CMatrix(void)
{

}
