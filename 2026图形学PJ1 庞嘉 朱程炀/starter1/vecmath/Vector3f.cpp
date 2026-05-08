/*
 * Vector3f.cpp 实现说明（中文注释版）
 * - 本文件实现三维向量的完整运算与插值功能。
 * - dot/cross 分别用于投影计算与法向/叉乘方向构造。
 * - homogenized() 提供从齐次语义向二维平面坐标的常见转换。
 * - lerp() 与 cubicInterpolate() 支持路径采样与动画关键帧过渡。
 * - 文件同时实现常量方向向量，统一场景中方向基准。
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Vector3f.h"
#include "Vector2f.h"

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector3f Vector3f::ZERO = Vector3f( 0, 0, 0 );

// static
const Vector3f Vector3f::UP = Vector3f( 0, 1, 0 );

// static
const Vector3f Vector3f::RIGHT = Vector3f( 1, 0, 0 );

// static
const Vector3f Vector3f::FORWARD = Vector3f( 0, 0, -1 );

// 用同一标量 f 填充 x、y、z 分量
Vector3f::Vector3f( float f )
{
    m_elements[0] = f;
    m_elements[1] = f;
    m_elements[2] = f;
}

// 用给定的 x、y、z 分量构造三维向量
Vector3f::Vector3f( float x, float y, float z )
{
    m_elements[0] = x;
    m_elements[1] = y;
    m_elements[2] = z;
}

// 用二维向量 xy 与标量 z 构造三维向量
Vector3f::Vector3f( const Vector2f& xy, float z )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = z;
}

// 用标量 x 与二维向量 yz 构造三维向量
Vector3f::Vector3f( float x, const Vector2f& yz )
{
	m_elements[0] = x;
	m_elements[1] = yz.x();
	m_elements[2] = yz.y();
}

// 拷贝构造：从另一个 Vector3f 复制分量
Vector3f::Vector3f( const Vector3f& rv )
{
    m_elements[0] = rv[0];
    m_elements[1] = rv[1];
    m_elements[2] = rv[2];
}

// 赋值运算符：将 rv 的分量逐一复制给自身
Vector3f& Vector3f::operator = ( const Vector3f& rv )
{
    if( this != &rv )
    {
        m_elements[0] = rv[0];
        m_elements[1] = rv[1];
        m_elements[2] = rv[2];
    }
    return *this;
}

// 按下标读取分量（0=x, 1=y, 2=z），只读版本
const float& Vector3f::operator [] ( int i ) const
{
    return m_elements[i];
}

// 按下标读写分量（0=x, 1=y, 2=z）
float& Vector3f::operator [] ( int i )
{
    return m_elements[i];
}

// 返回 x 分量的可修改引用
float& Vector3f::x()
{
    return m_elements[0];
}

// 返回 y 分量的可修改引用
float& Vector3f::y()
{
    return m_elements[1];
}

// 返回 z 分量的可修改引用
float& Vector3f::z()
{
    return m_elements[2];
}

// 返回 x 分量的只读值
float Vector3f::x() const
{
    return m_elements[0];
}

// 返回 y 分量的只读值
float Vector3f::y() const
{
    return m_elements[1];
}

// 返回 z 分量的只读值
float Vector3f::z() const
{
    return m_elements[2];
}

// 返回 (x, y) 的 swizzle 子向量
Vector2f Vector3f::xy() const
{
	return Vector2f( m_elements[0], m_elements[1] );
}

// 返回 (x, z) 的 swizzle 子向量
Vector2f Vector3f::xz() const
{
	return Vector2f( m_elements[0], m_elements[2] );
}

// 返回 (y, z) 的 swizzle 子向量
Vector2f Vector3f::yz() const
{
	return Vector2f( m_elements[1], m_elements[2] );
}

// 返回 (x, y, z) 的拷贝（swizzle 等价于自身）
Vector3f Vector3f::xyz() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[2] );
}

// 返回循环移位后的 swizzle 向量 (y, z, x)
Vector3f Vector3f::yzx() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[0] );
}

// 返回循环移位后的 swizzle 向量 (z, x, y)
Vector3f Vector3f::zxy() const
{
	return Vector3f( m_elements[2], m_elements[0], m_elements[1] );
}

// 返回向量的欧氏长度（模）
float Vector3f::abs() const
{
	return sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] );
}

// 返回向量长度的平方，避免开方以提升性能
float Vector3f::absSquared() const
{
    return
        (
            m_elements[0] * m_elements[0] +
            m_elements[1] * m_elements[1] +
            m_elements[2] * m_elements[2]
        );
}

// 原地将向量归一化为单位长度
void Vector3f::normalize()
{
	float norm = abs();
	m_elements[0] /= norm;
	m_elements[1] /= norm;
	m_elements[2] /= norm;
}

// 返回归一化后的新向量，自身不变
Vector3f Vector3f::normalized() const
{
	float norm = abs();
	return Vector3f
		(
			m_elements[0] / norm,
			m_elements[1] / norm,
			m_elements[2] / norm
		);
}

// 齐次除法：将 (x, y, z) 除以 z，返回对应的二维平面坐标
Vector2f Vector3f::homogenized() const
{
	return Vector2f
		(
			m_elements[ 0 ] / m_elements[ 2 ],
			m_elements[ 1 ] / m_elements[ 2 ]
		);
}

// 原地对每个分量取反
void Vector3f::negate()
{
	m_elements[0] = -m_elements[0];
	m_elements[1] = -m_elements[1];
	m_elements[2] = -m_elements[2];
}

// 隐式转换为 const float 指针，便于传递给图形 API
Vector3f::operator const float* () const
{
    return m_elements;
}

// 隐式转换为可写 float 指针
Vector3f::operator float* ()
{
    return m_elements;
}

// 以格式 < x, y, z > 打印向量到标准输出
void Vector3f::print() const
{
	printf( "< %.4f, %.4f, %.4f >\n",
		m_elements[0], m_elements[1], m_elements[2] );
}

// 原地分量加法：将 v 的各分量累加到自身
Vector3f& Vector3f::operator += ( const Vector3f& v )
{
	m_elements[ 0 ] += v.m_elements[ 0 ];
	m_elements[ 1 ] += v.m_elements[ 1 ];
	m_elements[ 2 ] += v.m_elements[ 2 ];
	return *this;
}

// 原地分量减法：从自身各分量中减去 v 的对应分量
Vector3f& Vector3f::operator -= ( const Vector3f& v )
{
	m_elements[ 0 ] -= v.m_elements[ 0 ];
	m_elements[ 1 ] -= v.m_elements[ 1 ];
	m_elements[ 2 ] -= v.m_elements[ 2 ];
	return *this;
}

// 原地标量乘法：将自身每个分量乘以 f
Vector3f& Vector3f::operator *= ( float f )
{
	m_elements[ 0 ] *= f;
	m_elements[ 1 ] *= f;
	m_elements[ 2 ] *= f;
	return *this;
}

// 原地标量除法：将自身每个分量除以 f
Vector3f& Vector3f::operator /= ( float f )
{
  m_elements[ 0 ] /= f;
  m_elements[ 1 ] /= f;
  m_elements[ 2 ] /= f;
  return *this;
}

// static
// 计算 v0·v1 的点积（内积）：x0*x1 + y0*y1 + z0*z1
float Vector3f::dot( const Vector3f& v0, const Vector3f& v1 )
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

// static
// 计算 v0×v1 的叉积，结果向量同时垂直于 v0 和 v1，常用于构造法向量
Vector3f Vector3f::cross( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f
        (
            v0.y() * v1.z() - v0.z() * v1.y(),
            v0.z() * v1.x() - v0.x() * v1.z(),
            v0.x() * v1.y() - v0.y() * v1.x()
        );
}

// static
// 在 v0 与 v1 之间按参数 alpha 做线性插值：alpha=0 返回 v0，alpha=1 返回 v1
Vector3f Vector3f::lerp( const Vector3f& v0, const Vector3f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

// static
// 对四个控制点 p0~p3 做三阶 de Casteljau 三次插值，t 为插值参数
Vector3f Vector3f::cubicInterpolate( const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float t )
{
	// geometric construction:
	//            t
	//   (t+1)/2     t/2
	// t+1        t	        t-1

	// bottom level
	Vector3f p0p1 = Vector3f::lerp( p0, p1, t + 1 );
	Vector3f p1p2 = Vector3f::lerp( p1, p2, t );
	Vector3f p2p3 = Vector3f::lerp( p2, p3, t - 1 );

	// middle level
	Vector3f p0p1_p1p2 = Vector3f::lerp( p0p1, p1p2, 0.5f * ( t + 1 ) );
	Vector3f p1p2_p2p3 = Vector3f::lerp( p1p2, p2p3, 0.5f * t );

	// top level
	return Vector3f::lerp( p0p1_p1p2, p1p2_p2p3, t );
}

// 两向量分量相加，返回新向量
Vector3f operator + ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2] );
}

// 两向量分量相减，返回新向量
Vector3f operator - ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2] );
}

// 两向量分量逐一相乘（Hadamard 积），返回新向量
Vector3f operator * ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2] );
}

// 两向量分量逐一相除，返回新向量
Vector3f operator / ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] / v1[0], v0[1] / v1[1], v0[2] / v1[2] );
}

// 一元取反：对每个分量乘以 -1
Vector3f operator - ( const Vector3f& v )
{
    return Vector3f( -v[0], -v[1], -v[2] );
}

// 标量左乘向量：f * (x, y, z)
Vector3f operator * ( float f, const Vector3f& v )
{
    return Vector3f( v[0] * f, v[1] * f, v[2] * f );
}

// 向量右乘标量：(x, y, z) * f
Vector3f operator * ( const Vector3f& v, float f )
{
    return Vector3f( v[0] * f, v[1] * f, v[2] * f );
}

// 向量除以标量：(x/f, y/f, z/f)
Vector3f operator / ( const Vector3f& v, float f )
{
    return Vector3f( v[0] / f, v[1] / f, v[2] / f );
}

// 逐分量比较两向量是否完全相等
bool operator == ( const Vector3f& v0, const Vector3f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() && v0.z() == v1.z() );
}

// 逐分量比较两向量是否不等
bool operator != ( const Vector3f& v0, const Vector3f& v1 )
{
    return !( v0 == v1 );
}
