/*
 * Vector2f.cpp 实现说明（中文注释版）
 * - 本文件实现二维向量的构造、访问、代数运算与几何工具函数。
 * - normal() 返回与原向量垂直的向量 (-y, x)，常用于二维法线计算。
 * - normalize()/normalized() 提供原地与非原地归一化两种使用方式。
 * - cross(v0, v1) 以三维向量形式返回二维叉积结果（z 分量携带面积信息）。
 * - 全局运算符实现分量级运算与标量缩放，适配表达式式写法。
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Vector2f.h"
#include "Vector3f.h"

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector2f Vector2f::ZERO = Vector2f( 0, 0 );

// static
const Vector2f Vector2f::UP = Vector2f( 0, 1 );

// static
const Vector2f Vector2f::RIGHT = Vector2f( 1, 0 );

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f::Vector2f( float f )
{
    m_elements[0] = f;
    m_elements[1] = f;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f::Vector2f( float x, float y )
{
    m_elements[0] = x;
    m_elements[1] = y;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f::Vector2f( const Vector2f& rv )
{
    m_elements[0] = rv[0];
    m_elements[1] = rv[1];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f& Vector2f::operator = ( const Vector2f& rv )
{
 	if( this != &rv )
	{
        m_elements[0] = rv[0];
        m_elements[1] = rv[1];
    }
    return *this;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
const float& Vector2f::operator [] ( int i ) const
{
    return m_elements[i];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float& Vector2f::operator [] ( int i )
{
    return m_elements[i];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float& Vector2f::x()
{
    return m_elements[0];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float& Vector2f::y()
{
    return m_elements[1];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float Vector2f::x() const
{
    return m_elements[0];
}	

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float Vector2f::y() const
{
    return m_elements[1];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::xy() const
{
    return *this;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::yx() const
{
    return Vector2f( m_elements[1], m_elements[0] );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::xx() const
{
    return Vector2f( m_elements[0], m_elements[0] );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::yy() const
{
    return Vector2f( m_elements[1], m_elements[1] );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::normal() const
{
    return Vector2f( -m_elements[1], m_elements[0] );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float Vector2f::abs() const
{
    return sqrt(absSquared());
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float Vector2f::absSquared() const
{
    return m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
void Vector2f::normalize()
{
    float norm = abs();
    m_elements[0] /= norm;
    m_elements[1] /= norm;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::normalized() const
{
    float norm = abs();
    return Vector2f( m_elements[0] / norm, m_elements[1] / norm );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
void Vector2f::negate()
{
    m_elements[0] = -m_elements[0];
    m_elements[1] = -m_elements[1];
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f::operator const float* () const
{
    return m_elements;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f::operator float* ()
{
    return m_elements;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
void Vector2f::print() const
{
	printf( "< %.4f, %.4f >\n",
		m_elements[0], m_elements[1] );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f& Vector2f::operator += ( const Vector2f& v )
{
	m_elements[ 0 ] += v.m_elements[ 0 ];
	m_elements[ 1 ] += v.m_elements[ 1 ];
	return *this;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f& Vector2f::operator -= ( const Vector2f& v )
{
	m_elements[ 0 ] -= v.m_elements[ 0 ];
	m_elements[ 1 ] -= v.m_elements[ 1 ];
	return *this;
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f& Vector2f::operator *= ( float f )
{
	m_elements[ 0 ] *= f;
	m_elements[ 1 ] *= f;
	return *this;
}

// static
// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
float Vector2f::dot( const Vector2f& v0, const Vector2f& v1 )
{
    return v0[0] * v1[0] + v0[1] * v1[1];
}

// static
// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector3f Vector2f::cross( const Vector2f& v0, const Vector2f& v1 )
{
	return Vector3f
		(
			0,
			0,
			v0.x() * v1.y() - v0.y() * v1.x()
		);
}

// static
// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f Vector2f::lerp( const Vector2f& v0, const Vector2f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

//////////////////////////////////////////////////////////////////////////
// Operator overloading
//////////////////////////////////////////////////////////////////////////

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator + ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() + v1.x(), v0.y() + v1.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator - ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() - v1.x(), v0.y() - v1.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator * ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator / ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator - ( const Vector2f& v )
{
    return Vector2f( -v.x(), -v.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator * ( float f, const Vector2f& v )
{
    return Vector2f( f * v.x(), f * v.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator * ( const Vector2f& v, float f )
{
    return Vector2f( f * v.x(), f * v.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
Vector2f operator / ( const Vector2f& v, float f )
{
    return Vector2f( v.x() / f, v.y() / f );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
bool operator == ( const Vector2f& v0, const Vector2f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() );
}

// 中文注释：该函数为实现层逻辑，负责完成对应数学运算或对象状态变更；输入输出含义与签名保持一致。
bool operator != ( const Vector2f& v0, const Vector2f& v1 )
{
    return !( v0 == v1 );
}
