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

// 用同一标量 f 填充 x、y 分量
Vector2f::Vector2f( float f )
{
    m_elements[0] = f;
    m_elements[1] = f;
}

// 用给定的 x、y 分量构造二维向量
Vector2f::Vector2f( float x, float y )
{
    m_elements[0] = x;
    m_elements[1] = y;
}

// 拷贝构造：从另一个 Vector2f 复制分量
Vector2f::Vector2f( const Vector2f& rv )
{
    m_elements[0] = rv[0];
    m_elements[1] = rv[1];
}

// 赋值运算符：将 rv 的分量逐一复制给自身
Vector2f& Vector2f::operator = ( const Vector2f& rv )
{
 	if( this != &rv )
	{
        m_elements[0] = rv[0];
        m_elements[1] = rv[1];
    }
    return *this;
}

// 按下标读取分量（0=x, 1=y），只读版本
const float& Vector2f::operator [] ( int i ) const
{
    return m_elements[i];
}

// 按下标读写分量（0=x, 1=y）
float& Vector2f::operator [] ( int i )
{
    return m_elements[i];
}

// 返回 x 分量的可修改引用
float& Vector2f::x()
{
    return m_elements[0];
}

// 返回 y 分量的可修改引用
float& Vector2f::y()
{
    return m_elements[1];
}

// 返回 x 分量的只读值
float Vector2f::x() const
{
    return m_elements[0];
}

// 返回 y 分量的只读值
float Vector2f::y() const
{
    return m_elements[1];
}

// 返回 (x, y) 的拷贝（swizzle 等价于自身）
Vector2f Vector2f::xy() const
{
    return *this;
}

// 返回分量交换后的向量 (y, x)
Vector2f Vector2f::yx() const
{
    return Vector2f( m_elements[1], m_elements[0] );
}

// 返回 (x, x) 的 swizzle 向量
Vector2f Vector2f::xx() const
{
    return Vector2f( m_elements[0], m_elements[0] );
}

// 返回 (y, y) 的 swizzle 向量
Vector2f Vector2f::yy() const
{
    return Vector2f( m_elements[1], m_elements[1] );
}

// 返回垂直于自身的法向量 (-y, x)，常用于 2D 边法线计算
Vector2f Vector2f::normal() const
{
    return Vector2f( -m_elements[1], m_elements[0] );
}

// 返回向量的欧氏长度（模）
float Vector2f::abs() const
{
    return sqrt(absSquared());
}

// 返回向量长度的平方，避免开方以提升性能
float Vector2f::absSquared() const
{
    return m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1];
}

// 原地将向量归一化为单位长度
void Vector2f::normalize()
{
    float norm = abs();
    m_elements[0] /= norm;
    m_elements[1] /= norm;
}

// 返回归一化后的新向量，自身不变
Vector2f Vector2f::normalized() const
{
    float norm = abs();
    return Vector2f( m_elements[0] / norm, m_elements[1] / norm );
}

// 原地对每个分量取反
void Vector2f::negate()
{
    m_elements[0] = -m_elements[0];
    m_elements[1] = -m_elements[1];
}

// 隐式转换为 const float 指针，便于传递给图形 API
Vector2f::operator const float* () const
{
    return m_elements;
}

// 隐式转换为可写 float 指针
Vector2f::operator float* ()
{
    return m_elements;
}

// 以格式 < x, y > 打印向量到标准输出
void Vector2f::print() const
{
	printf( "< %.4f, %.4f >\n",
		m_elements[0], m_elements[1] );
}

// 原地分量加法：将 v 的各分量累加到自身
Vector2f& Vector2f::operator += ( const Vector2f& v )
{
	m_elements[ 0 ] += v.m_elements[ 0 ];
	m_elements[ 1 ] += v.m_elements[ 1 ];
	return *this;
}

// 原地分量减法：从自身各分量中减去 v 的对应分量
Vector2f& Vector2f::operator -= ( const Vector2f& v )
{
	m_elements[ 0 ] -= v.m_elements[ 0 ];
	m_elements[ 1 ] -= v.m_elements[ 1 ];
	return *this;
}

// 原地标量乘法：将自身每个分量乘以 f
Vector2f& Vector2f::operator *= ( float f )
{
	m_elements[ 0 ] *= f;
	m_elements[ 1 ] *= f;
	return *this;
}

// static
// 计算 v0·v1 的点积（内积）：x0*x1 + y0*y1
float Vector2f::dot( const Vector2f& v0, const Vector2f& v1 )
{
    return v0[0] * v1[0] + v0[1] * v1[1];
}

// static
// 计算二维叉积，结果以三维向量的 z 分量表示（x0*y1 - y0*x1），可用于判断转向
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
// 在 v0 与 v1 之间按参数 alpha 做线性插值：alpha=0 返回 v0，alpha=1 返回 v1
Vector2f Vector2f::lerp( const Vector2f& v0, const Vector2f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

//////////////////////////////////////////////////////////////////////////
// Operator overloading
//////////////////////////////////////////////////////////////////////////

// 两向量分量相加，返回新向量
Vector2f operator + ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() + v1.x(), v0.y() + v1.y() );
}

// 两向量分量相减，返回新向量
Vector2f operator - ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() - v1.x(), v0.y() - v1.y() );
}

// 两向量分量逐一相乘（Hadamard 积），返回新向量
Vector2f operator * ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

// 两向量分量逐一相除，返回新向量
Vector2f operator / ( const Vector2f& v0, const Vector2f& v1 )
{
    return Vector2f( v0.x() * v1.x(), v0.y() * v1.y() );
}

// 一元取反：对每个分量乘以 -1
Vector2f operator - ( const Vector2f& v )
{
    return Vector2f( -v.x(), -v.y() );
}

// 标量左乘向量：f * (x, y)
Vector2f operator * ( float f, const Vector2f& v )
{
    return Vector2f( f * v.x(), f * v.y() );
}

// 向量右乘标量：(x, y) * f
Vector2f operator * ( const Vector2f& v, float f )
{
    return Vector2f( f * v.x(), f * v.y() );
}

// 向量除以标量：(x/f, y/f)
Vector2f operator / ( const Vector2f& v, float f )
{
    return Vector2f( v.x() / f, v.y() / f );
}

// 逐分量比较两向量是否完全相等
bool operator == ( const Vector2f& v0, const Vector2f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() );
}

// 逐分量比较两向量是否不等
bool operator != ( const Vector2f& v0, const Vector2f& v1 )
{
    return !( v0 == v1 );
}
