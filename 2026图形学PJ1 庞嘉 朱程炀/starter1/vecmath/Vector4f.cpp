/*
 * Vector4f.cpp 实现说明（中文注释版）
 * - 本文件实现四维向量的构造、swizzle 访问与算术运算。
 * - homogenize()/homogenized() 用于齐次坐标透视除法场景。
 * - 通过与 Vector2f/Vector3f 的组合构造，简化跨维度拼接。
 * - dot/lerp 为颜色、位置与参数空间插值提供基础算子。
 * - 提供 const float* / float* 转换，便于与图形 API 直接对接。
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Vector4f.h"
#include "Vector2f.h"
#include "Vector3f.h"

// 用同一标量 f 填充 x、y、z、w 分量
Vector4f::Vector4f( float f )
{
	m_elements[ 0 ] = f;
	m_elements[ 1 ] = f;
	m_elements[ 2 ] = f;
	m_elements[ 3 ] = f;
}

// 用给定的 fx、fy、fz、fw 分量构造四维向量
Vector4f::Vector4f( float fx, float fy, float fz, float fw )
{
	m_elements[0] = fx;
	m_elements[1] = fy;
	m_elements[2] = fz;
	m_elements[3] = fw;
}

// 从长度为 4 的 float 数组逐元素复制构造
Vector4f::Vector4f( float buffer[ 4 ] )
{
	m_elements[ 0 ] = buffer[ 0 ];
	m_elements[ 1 ] = buffer[ 1 ];
	m_elements[ 2 ] = buffer[ 2 ];
	m_elements[ 3 ] = buffer[ 3 ];
}

// 用二维向量 xy 与标量 z、w 构造四维向量
Vector4f::Vector4f( const Vector2f& xy, float z, float w )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = z;
	m_elements[3] = w;
}

// 用标量 x、二维向量 yz、标量 w 构造四维向量
Vector4f::Vector4f( float x, const Vector2f& yz, float w )
{
	m_elements[0] = x;
	m_elements[1] = yz.x();
	m_elements[2] = yz.y();
	m_elements[3] = w;
}

// 用标量 x、y 与二维向量 zw 构造四维向量
Vector4f::Vector4f( float x, float y, const Vector2f& zw )
{
	m_elements[0] = x;
	m_elements[1] = y;
	m_elements[2] = zw.x();
	m_elements[3] = zw.y();
}

// 用两个二维向量 xy 和 zw 拼接构造四维向量
Vector4f::Vector4f( const Vector2f& xy, const Vector2f& zw )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = zw.x();
	m_elements[3] = zw.y();
}

// 用三维向量 xyz 与标量 w 构造四维向量（常用于齐次坐标）
Vector4f::Vector4f( const Vector3f& xyz, float w )
{
	m_elements[0] = xyz.x();
	m_elements[1] = xyz.y();
	m_elements[2] = xyz.z();
	m_elements[3] = w;
}

// 用标量 x 与三维向量 yzw 构造四维向量
Vector4f::Vector4f( float x, const Vector3f& yzw )
{
	m_elements[0] = x;
	m_elements[1] = yzw.x();
	m_elements[2] = yzw.y();
	m_elements[3] = yzw.z();
}

// 拷贝构造：从另一个 Vector4f 复制分量
Vector4f::Vector4f( const Vector4f& rv )
{
	m_elements[0] = rv.m_elements[0];
	m_elements[1] = rv.m_elements[1];
	m_elements[2] = rv.m_elements[2];
	m_elements[3] = rv.m_elements[3];
}

// 赋值运算符：将 rv 的分量逐一复制给自身
Vector4f& Vector4f::operator = ( const Vector4f& rv )
{
	if( this != &rv )
	{
		m_elements[0] = rv.m_elements[0];
		m_elements[1] = rv.m_elements[1];
		m_elements[2] = rv.m_elements[2];
		m_elements[3] = rv.m_elements[3];
	}
	return *this;
}

// 按下标读取分量（0=x, 1=y, 2=z, 3=w），只读版本
const float& Vector4f::operator [] ( int i ) const
{
	return m_elements[ i ];
}

// 按下标读写分量（0=x, 1=y, 2=z, 3=w）
float& Vector4f::operator [] ( int i )
{
	return m_elements[ i ];
}

// 返回 x 分量的可修改引用
float& Vector4f::x()
{
	return m_elements[ 0 ];
}

// 返回 y 分量的可修改引用
float& Vector4f::y()
{
	return m_elements[ 1 ];
}

// 返回 z 分量的可修改引用
float& Vector4f::z()
{
	return m_elements[ 2 ];
}

// 返回 w 分量的可修改引用
float& Vector4f::w()
{
	return m_elements[ 3 ];
}

// 返回 x 分量的只读值
float Vector4f::x() const
{
	return m_elements[0];
}

// 返回 y 分量的只读值
float Vector4f::y() const
{
	return m_elements[1];
}

// 返回 z 分量的只读值
float Vector4f::z() const
{
	return m_elements[2];
}

// 返回 w 分量的只读值
float Vector4f::w() const
{
	return m_elements[3];
}

// 返回 (x, y) 的 swizzle 子向量
Vector2f Vector4f::xy() const
{
	return Vector2f( m_elements[0], m_elements[1] );
}

// 返回 (y, z) 的 swizzle 子向量
Vector2f Vector4f::yz() const
{
	return Vector2f( m_elements[1], m_elements[2] );
}

// 返回 (z, w) 的 swizzle 子向量
Vector2f Vector4f::zw() const
{
	return Vector2f( m_elements[2], m_elements[3] );
}

// 返回 (w, x) 的 swizzle 子向量
Vector2f Vector4f::wx() const
{
	return Vector2f( m_elements[3], m_elements[0] );
}

// 返回前三维 (x, y, z) 的 swizzle 子向量
Vector3f Vector4f::xyz() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[2] );
}

// 返回 (y, z, w) 的 swizzle 子向量
Vector3f Vector4f::yzw() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[3] );
}

// 返回 (z, w, x) 的 swizzle 子向量
Vector3f Vector4f::zwx() const
{
	return Vector3f( m_elements[2], m_elements[3], m_elements[0] );
}

// 返回 (w, x, y) 的 swizzle 子向量
Vector3f Vector4f::wxy() const
{
	return Vector3f( m_elements[3], m_elements[0], m_elements[1] );
}

// 返回 (x, y, w) 的 swizzle 子向量
Vector3f Vector4f::xyw() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[3] );
}

// 返回 (y, z, x) 的 swizzle 子向量
Vector3f Vector4f::yzx() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[0] );
}

// 返回 (z, w, y) 的 swizzle 子向量
Vector3f Vector4f::zwy() const
{
	return Vector3f( m_elements[2], m_elements[3], m_elements[1] );
}

// 返回 (w, x, z) 的 swizzle 子向量
Vector3f Vector4f::wxz() const
{
	return Vector3f( m_elements[3], m_elements[0], m_elements[2] );
}

// 返回向量的欧氏长度（模）
float Vector4f::abs() const
{
	return sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
}

// 返回向量长度的平方，避免开方以提升性能
float Vector4f::absSquared() const
{
	return( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
}

// 原地将向量归一化为单位长度
void Vector4f::normalize()
{
	float norm = sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] + m_elements[3] * m_elements[3] );
	m_elements[0] = m_elements[0] / norm;
	m_elements[1] = m_elements[1] / norm;
	m_elements[2] = m_elements[2] / norm;
	m_elements[3] = m_elements[3] / norm;
}

// 返回归一化后的新向量，自身不变
Vector4f Vector4f::normalized() const
{
	float length = abs();
	return Vector4f
		(
			m_elements[0] / length,
			m_elements[1] / length,
			m_elements[2] / length,
			m_elements[3] / length
		);
}

// 原地执行齐次除法：将 x、y、z 除以 w，并将 w 置为 1（w 为 0 时不做处理）
void Vector4f::homogenize()
{
	if( m_elements[3] != 0 )
	{
		m_elements[0] /= m_elements[3];
		m_elements[1] /= m_elements[3];
		m_elements[2] /= m_elements[3];
		m_elements[3] = 1;
	}
}

// 返回齐次除法后的新向量（x/w, y/w, z/w, 1），w 为 0 时返回自身，自身不变
Vector4f Vector4f::homogenized() const
{
	if( m_elements[3] != 0 )
	{
		return Vector4f
			(
				m_elements[0] / m_elements[3],
				m_elements[1] / m_elements[3],
				m_elements[2] / m_elements[3],
				1
			);
	}
	else
	{
		return Vector4f
			(
				m_elements[0],
				m_elements[1],
				m_elements[2],
				m_elements[3]
			);
	}
}

// 原地对每个分量取反
void Vector4f::negate()
{
	m_elements[0] = -m_elements[0];
	m_elements[1] = -m_elements[1];
	m_elements[2] = -m_elements[2];
	m_elements[3] = -m_elements[3];
}

// 隐式转换为 const float 指针，便于传递给图形 API
Vector4f::operator const float* () const
{
	return m_elements;
}

// 隐式转换为可写 float 指针
Vector4f::operator float* ()
{
	return m_elements;
}

// 以格式 < x, y, z, w > 打印向量到标准输出
void Vector4f::print() const
{
	printf( "< %.4f, %.4f, %.4f, %.4f >\n",
		m_elements[0], m_elements[1], m_elements[2], m_elements[3] );
}

// static
// 计算 v0·v1 的四维点积：x0*x1 + y0*y1 + z0*z1 + w0*w1
float Vector4f::dot( const Vector4f& v0, const Vector4f& v1 )
{
	return v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z() + v0.w() * v1.w();
}

// static
// 在 v0 与 v1 之间按参数 alpha 做线性插值：alpha=0 返回 v0，alpha=1 返回 v1
Vector4f Vector4f::lerp( const Vector4f& v0, const Vector4f& v1, float alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

// 两向量分量相加，返回新向量
Vector4f operator + ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() + v1.x(), v0.y() + v1.y(), v0.z() + v1.z(), v0.w() + v1.w() );
}

// 两向量分量相减，返回新向量
Vector4f operator - ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z(), v0.w() - v1.w() );
}

// 两向量分量逐一相乘（Hadamard 积），返回新向量
Vector4f operator * ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() * v1.x(), v0.y() * v1.y(), v0.z() * v1.z(), v0.w() * v1.w() );
}

// 两向量分量逐一相除，返回新向量
Vector4f operator / ( const Vector4f& v0, const Vector4f& v1 )
{
	return Vector4f( v0.x() / v1.x(), v0.y() / v1.y(), v0.z() / v1.z(), v0.w() / v1.w() );
}

// 一元取反：对每个分量乘以 -1
Vector4f operator - ( const Vector4f& v )
{
	return Vector4f( -v.x(), -v.y(), -v.z(), -v.w() );
}

// 标量左乘向量：f * (x, y, z, w)
Vector4f operator * ( float f, const Vector4f& v )
{
	return Vector4f( f * v.x(), f * v.y(), f * v.z(), f * v.w() );
}

// 向量右乘标量：(x, y, z, w) * f
Vector4f operator * ( const Vector4f& v, float f )
{
	return Vector4f( f * v.x(), f * v.y(), f * v.z(), f * v.w() );
}

// 向量除以标量：(x/f, y/f, z/f, w/f)
Vector4f operator / ( const Vector4f& v, float f )
{
    return Vector4f( v[0] / f, v[1] / f, v[2] / f, v[3] / f );
}

// 逐分量比较两向量是否完全相等
bool operator == ( const Vector4f& v0, const Vector4f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() && v0.z() == v1.z() && v0.w() == v1.w() );
}

// 逐分量比较两向量是否不等
bool operator != ( const Vector4f& v0, const Vector4f& v1 )
{
    return !( v0 == v1 );
}
