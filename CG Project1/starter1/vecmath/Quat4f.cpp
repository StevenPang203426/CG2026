/*
 * Quat4f.cpp 实现说明（中文注释版）
 * - 本文件实现四元数代数、旋转表达以及插值曲线相关算法。
 * - normalize/conjugate/inverse 对应旋转计算中的标准四元数操作。
 * - log()/exp() 支持将旋转映射到切空间并回到单位球面。
 * - slerp/squad/cubicInterpolate 提供从线性到高阶的姿态插值能力。
 * - fromRotationMatrix()/fromRotatedBasis() 支持矩阵与基向量到四元数的转换。
 */

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cstdio>

#include "Quat4f.h"
#include "Vector3f.h"
#include "Vector4f.h"

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Quat4f Quat4f::ZERO = Quat4f( 0, 0, 0, 0 );

// static
const Quat4f Quat4f::IDENTITY = Quat4f( 1, 0, 0, 0 );

// 默认构造：将所有分量初始化为 0（不代表有效旋转，需手动设置）
Quat4f::Quat4f()
{
	m_elements[ 0 ] = 0;
	m_elements[ 1 ] = 0;
	m_elements[ 2 ] = 0;
	m_elements[ 3 ] = 0;
}

// 用 (w, x, y, z) 分量构造四元数，w 为实部，(x,y,z) 为虚部
Quat4f::Quat4f( float w, float x, float y, float z )
{
	m_elements[ 0 ] = w;
	m_elements[ 1 ] = x;
	m_elements[ 2 ] = y;
	m_elements[ 3 ] = z;
}

// 拷贝构造：从另一个 Quat4f 复制所有分量
Quat4f::Quat4f( const Quat4f& rq )
{
	m_elements[ 0 ] = rq.m_elements[ 0 ];
	m_elements[ 1 ] = rq.m_elements[ 1 ];
	m_elements[ 2 ] = rq.m_elements[ 2 ];
	m_elements[ 3 ] = rq.m_elements[ 3 ];
}

// 赋值运算符：将 rq 的分量逐一复制给自身
Quat4f& Quat4f::operator = ( const Quat4f& rq )
{
	if( this != ( &rq ) )
	{
		m_elements[ 0 ] = rq.m_elements[ 0 ];
		m_elements[ 1 ] = rq.m_elements[ 1 ];
		m_elements[ 2 ] = rq.m_elements[ 2 ];
		m_elements[ 3 ] = rq.m_elements[ 3 ];
	}
    return( *this );
}

// 从三维向量构造纯四元数（实部 w=0，虚部为向量分量），用于向量的四元数旋转运算
Quat4f::Quat4f( const Vector3f& v )
{
	m_elements[ 0 ] = 0;
	m_elements[ 1 ] = v[ 0 ];
	m_elements[ 2 ] = v[ 1 ];
	m_elements[ 3 ] = v[ 2 ];
}

// 从四维向量直接构造四元数（按 w, x, y, z 顺序复制）
Quat4f::Quat4f( const Vector4f& v )
{
	m_elements[ 0 ] = v[ 0 ];
	m_elements[ 1 ] = v[ 1 ];
	m_elements[ 2 ] = v[ 2 ];
	m_elements[ 3 ] = v[ 3 ];
}

// 按下标读取分量（0=w, 1=x, 2=y, 3=z），只读版本
const float& Quat4f::operator [] ( int i ) const
{
	return m_elements[ i ];
}

// 按下标读写分量（0=w, 1=x, 2=y, 3=z）
float& Quat4f::operator [] ( int i )
{
	return m_elements[ i ];
}

// 返回实部 w 的只读值
float Quat4f::w() const
{
	return m_elements[ 0 ];
}

// 返回虚部 x 分量的只读值
float Quat4f::x() const
{
	return m_elements[ 1 ];
}

// 返回虚部 y 分量的只读值
float Quat4f::y() const
{
	return m_elements[ 2 ];
}

// 返回虚部 z 分量的只读值
float Quat4f::z() const
{
	return m_elements[ 3 ];
}

// 返回虚部 (x, y, z) 组成的三维向量
Vector3f Quat4f::xyz() const
{
	return Vector3f
	(
		m_elements[ 1 ],
		m_elements[ 2 ],
		m_elements[ 3 ]
	);
}

// 返回完整四元数 (w, x, y, z) 组成的四维向量
Vector4f Quat4f::wxyz() const
{
	return Vector4f
	(
		m_elements[ 0 ],
		m_elements[ 1 ],
		m_elements[ 2 ],
		m_elements[ 3 ]
	);
}

// 返回四元数的欧氏长度（模）
float Quat4f::abs() const
{
	return sqrt( absSquared() );
}

// 返回四元数长度的平方，避免开方以提升性能
float Quat4f::absSquared() const
{
	return
	(
		m_elements[ 0 ] * m_elements[ 0 ] +
		m_elements[ 1 ] * m_elements[ 1 ] +
		m_elements[ 2 ] * m_elements[ 2 ] +
		m_elements[ 3 ] * m_elements[ 3 ]
	);
}

// 原地将四元数归一化为单位长度，使其成为有效的旋转表达
void Quat4f::normalize()
{
	float reciprocalAbs = 1.f / abs();

	m_elements[ 0 ] *= reciprocalAbs;
	m_elements[ 1 ] *= reciprocalAbs;
	m_elements[ 2 ] *= reciprocalAbs;
	m_elements[ 3 ] *= reciprocalAbs;
}

// 返回归一化后的新四元数，自身不变
Quat4f Quat4f::normalized() const
{
	Quat4f q( *this );
	q.normalize();
	return q;
}

// 原地取共轭：翻转虚部符号（等价于对旋转取逆，仅对单位四元数成立）
void Quat4f::conjugate()
{
	m_elements[ 1 ] = -m_elements[ 1 ];
	m_elements[ 2 ] = -m_elements[ 2 ];
	m_elements[ 3 ] = -m_elements[ 3 ];
}

// 返回共轭四元数（翻转虚部），自身不变
Quat4f Quat4f::conjugated() const
{
	return Quat4f
	(
		 m_elements[ 0 ],
		-m_elements[ 1 ],
		-m_elements[ 2 ],
		-m_elements[ 3 ]
	);
}

// 原地计算四元数的逆：q^{-1} = q* / |q|^2
void Quat4f::invert()
{
	Quat4f inverse = conjugated() * ( 1.0f / absSquared() );

	m_elements[ 0 ] = inverse.m_elements[ 0 ];
	m_elements[ 1 ] = inverse.m_elements[ 1 ];
	m_elements[ 2 ] = inverse.m_elements[ 2 ];
	m_elements[ 3 ] = inverse.m_elements[ 3 ];
}

// 返回四元数的逆：q^{-1} = q* / |q|^2，自身不变
Quat4f Quat4f::inverse() const
{
	return conjugated() * ( 1.0f / absSquared() );
}


// 计算单位四元数的对数映射（将旋转从球面映射到切空间），结果为纯虚四元数
Quat4f Quat4f::log() const
{
	float len =
		sqrt
		(
			m_elements[ 1 ] * m_elements[ 1 ] +
			m_elements[ 2 ] * m_elements[ 2 ] +
			m_elements[ 3 ] * m_elements[ 3 ]
		);

	if( len < 1e-6 )
	{
		return Quat4f( 0, m_elements[ 1 ], m_elements[ 2 ], m_elements[ 3 ] );
	}
	else
	{
		float coeff = acos( m_elements[ 0 ] ) / len;
		return Quat4f( 0, m_elements[ 1 ] * coeff, m_elements[ 2 ] * coeff, m_elements[ 3 ] * coeff );
	}
}

// 计算纯虚四元数的指数映射（将切空间中的旋转映射回单位球面）
Quat4f Quat4f::exp() const
{
	float theta =
		sqrt
		(
			m_elements[ 1 ] * m_elements[ 1 ] +
			m_elements[ 2 ] * m_elements[ 2 ] +
			m_elements[ 3 ] * m_elements[ 3 ]
		);

	if( theta < 1e-6 )
	{
		return Quat4f( cos( theta ), m_elements[ 1 ], m_elements[ 2 ], m_elements[ 3 ] );
	}
	else
	{
		float coeff = sin( theta ) / theta;
		return Quat4f( cos( theta ), m_elements[ 1 ] * coeff, m_elements[ 2 ] * coeff, m_elements[ 3 ] * coeff );
	}
}

// 从单位四元数提取旋转轴与旋转角（弧度）：返回轴向量，通过 radiansOut 输出角度
Vector3f Quat4f::getAxisAngle( float* radiansOut )
{
	float theta = acos( w() ) * 2;
	float vectorNorm = sqrt( x() * x() + y() * y() + z() * z() );
	float reciprocalVectorNorm = 1.f / vectorNorm;

	*radiansOut = theta;
	return Vector3f
	(
		x() * reciprocalVectorNorm,
		y() * reciprocalVectorNorm,
		z() * reciprocalVectorNorm
	);
}

// 以旋转轴 axis 与旋转角 radians 设置单位四元数（Rodrigues 参数化）
void Quat4f::setAxisAngle( float radians, const Vector3f& axis )
{
	m_elements[ 0 ] = cos( radians / 2 );

	float sinHalfTheta = sin( radians / 2 );
	float vectorNorm = axis.abs();
	float reciprocalVectorNorm = 1.f / vectorNorm;

	m_elements[ 1 ] = axis.x() * sinHalfTheta * reciprocalVectorNorm;
	m_elements[ 2 ] = axis.y() * sinHalfTheta * reciprocalVectorNorm;
	m_elements[ 3 ] = axis.z() * sinHalfTheta * reciprocalVectorNorm;
}

// 以格式 < w + xi + yj + zk > 打印四元数到标准输出
void Quat4f::print()
{
	printf( "< %.4f + %.4f i + %.4f j + %.4f k >\n",
		m_elements[ 0 ], m_elements[ 1 ], m_elements[ 2 ], m_elements[ 3 ] );
}

// static
// 计算两个四元数的点积：w0*w1 + x0*x1 + y0*y1 + z0*z1
float Quat4f::dot( const Quat4f& q0, const Quat4f& q1 )
{
	return
	(
		q0.w() * q1.w() +
		q0.x() * q1.x() +
		q0.y() * q1.y() +
		q0.z() * q1.z()
	);
}

// static
// 在 q0 与 q1 之间做线性插值后归一化（NLERP），适合快速近似但不等速
Quat4f Quat4f::lerp( const Quat4f& q0, const Quat4f& q1, float alpha )
{
	return( ( q0 + alpha * ( q1 - q0 ) ).normalized() );
}

// static
// 在 a 与 b 之间做球面线性插值（SLERP），保证等角速度过渡；allowFlip=true 时取最短路径
Quat4f Quat4f::slerp( const Quat4f& a, const Quat4f& b, float t, bool allowFlip )
{
	float cosAngle = Quat4f::dot( a, b );

	float c1;
	float c2;

	// Linear interpolation for close orientations
	if( ( 1.0f - fabs( cosAngle ) ) < 0.01f )
	{
		c1 = 1.0f - t;
		c2 = t;
	}
	else
	{
		// Spherical interpolation
		float angle = acos( fabs( cosAngle ) );
		float sinAngle = sin( angle );
		c1 = sin( angle * ( 1.0f - t ) ) / sinAngle;
		c2 = sin( angle * t ) / sinAngle;
	}

	// Use the shortest path
	if( allowFlip && ( cosAngle < 0.0f ) )
	{
		c1 = -c1;
	}

	return Quat4f( c1 * a[ 0 ] + c2 * b[ 0 ], c1 * a[ 1 ] + c2 * b[ 1 ], c1 * a[ 2 ] + c2 * b[ 2 ], c1 * a[ 3 ] + c2 * b[ 3 ] );
}

// static
// Squad（球面三次插值）：在 a~b 段利用切向量 tanA/tanB 构造 C1 连续的旋转曲线
Quat4f Quat4f::squad( const Quat4f& a, const Quat4f& tanA, const Quat4f& tanB, const Quat4f& b, float t )
{
	Quat4f ab = Quat4f::slerp( a, b, t );
	Quat4f tangent = Quat4f::slerp( tanA, tanB, t, false );
	return Quat4f::slerp( ab, tangent, 2.0f * t * ( 1.0f - t ), false );
}

// static
// 对四个关键帧 q0~q3 做三阶 de Casteljau 四元数插值，参数 t 在 [0,1] 区间内
Quat4f Quat4f::cubicInterpolate( const Quat4f& q0, const Quat4f& q1, const Quat4f& q2, const Quat4f& q3, float t )
{
	// geometric construction:
	//            t
	//   (t+1)/2     t/2
	// t+1        t	        t-1

	// bottom level
	Quat4f q0q1 = Quat4f::slerp( q0, q1, t + 1 );
	Quat4f q1q2 = Quat4f::slerp( q1, q2, t );
	Quat4f q2q3 = Quat4f::slerp( q2, q3, t - 1 );

	// middle level
	Quat4f q0q1_q1q2 = Quat4f::slerp( q0q1, q1q2, 0.5f * ( t + 1 ) );
	Quat4f q1q2_q2q3 = Quat4f::slerp( q1q2, q2q3, 0.5f * t );

	// top level
	return Quat4f::slerp( q0q1_q1q2, q1q2_q2q3, t );
}

// static
// 计算从旋转 a 到旋转 b 的对数差：log(a^{-1} * b)，用于构造 Squad 切向量
Quat4f Quat4f::logDifference( const Quat4f& a, const Quat4f& b )
{
	Quat4f diff = a.inverse() * b;
	diff.normalize();
	return diff.log();
}

// static
// 计算 Squad 所需的内部切向量：exp(-0.25 * (log(q_center^{-1} * q_before) + log(q_center^{-1} * q_after)))
Quat4f Quat4f::squadTangent( const Quat4f& before, const Quat4f& center, const Quat4f& after )
{
	Quat4f l1 = Quat4f::logDifference( center, before );
	Quat4f l2 = Quat4f::logDifference( center, after );

	Quat4f e;
	for( int i = 0; i < 4; ++i )
	{
		e[ i ] = -0.25f * ( l1[ i ] + l2[ i ] );
	}
	e = center * ( e.exp() );

	return e;
}

// static
// 从旋转矩阵 m 提取等价的单位四元数（Shepperd 方法，数值稳定）
Quat4f Quat4f::fromRotationMatrix( const Matrix3f& m )
{
	float x;
	float y;
	float z;
	float w;

	// Compute one plus the trace of the matrix
	float onePlusTrace = 1.0f + m( 0, 0 ) + m( 1, 1 ) + m( 2, 2 );

	if( onePlusTrace > 1e-5 )
	{
		// Direct computation
		float s = sqrt( onePlusTrace ) * 2.0f;
		x = ( m( 2, 1 ) - m( 1, 2 ) ) / s;
		y = ( m( 0, 2 ) - m( 2, 0 ) ) / s;
		z = ( m( 1, 0 ) - m( 0, 1 ) ) / s;
		w = 0.25f * s;
	}
	else
	{
		// Computation depends on major diagonal term
		if( ( m( 0, 0 ) > m( 1, 1 ) ) & ( m( 0, 0 ) > m( 2, 2 ) ) )
		{
			float s = sqrt( 1.0f + m( 0, 0 ) - m( 1, 1 ) - m( 2, 2 ) ) * 2.0f;
			x = 0.25f * s;
			y = ( m( 0, 1 ) + m( 1, 0 ) ) / s;
			z = ( m( 0, 2 ) + m( 2, 0 ) ) / s;
			w = ( m( 1, 2 ) - m( 2, 1 ) ) / s;
		}
		else if( m( 1, 1 ) > m( 2, 2 ) )
		{
			float s = sqrt( 1.0f + m( 1, 1 ) - m( 0, 0 ) - m( 2, 2 ) ) * 2.0f;
			x = ( m( 0, 1 ) + m( 1, 0 ) ) / s;
			y = 0.25f * s;
			z = ( m( 1, 2 ) + m( 2, 1 ) ) / s;
			w = ( m( 0, 2 ) - m( 2, 0 ) ) / s;
		}
		else
		{
			float s = sqrt( 1.0f + m( 2, 2 ) - m( 0, 0 ) - m( 1, 1 ) ) * 2.0f;
			x = ( m( 0, 2 ) + m( 2, 0 ) ) / s;
			y = ( m( 1, 2 ) + m( 2, 1 ) ) / s;
			z = 0.25f * s;
			w = ( m( 0, 1 ) - m( 1, 0 ) ) / s;
		}
	}

	Quat4f q( w, x, y, z );
	return q.normalized();
}

// static
// 从旋转后的正交基向量组 (x, y, z) 构造等价的单位四元数（先构造旋转矩阵再转换）
Quat4f Quat4f::fromRotatedBasis( const Vector3f& x, const Vector3f& y, const Vector3f& z )
{
	return fromRotationMatrix( Matrix3f( x, y, z ) );
}

// static
// 用三个均匀分布随机数 (u0, u1, u2) ∈ [0,1] 生成均匀分布的随机单位四元数
Quat4f Quat4f::randomRotation( float u0, float u1, float u2 )
{
	float z = u0;
	float theta = static_cast< float >( 2.f * M_PI * u1 );
	float r = sqrt( 1.f - z * z );
	float w = static_cast< float >( M_PI * u2 );

	return Quat4f
	(
		cos( w ),
		sin( w ) * cos( theta ) * r,
		sin( w ) * sin( theta ) * r,
		sin( w ) * z
	);
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

// 两四元数各分量相加（非旋转合成，用于 LERP 等代数操作）
Quat4f operator + ( const Quat4f& q0, const Quat4f& q1 )
{
	return Quat4f
	(
		q0.w() + q1.w(),
		q0.x() + q1.x(),
		q0.y() + q1.y(),
		q0.z() + q1.z()
	);
}

// 两四元数各分量相减
Quat4f operator - ( const Quat4f& q0, const Quat4f& q1 )
{
	return Quat4f
	(
		q0.w() - q1.w(),
		q0.x() - q1.x(),
		q0.y() - q1.y(),
		q0.z() - q1.z()
	);
}

// 四元数乘法（Hamilton 积）：实现旋转的合成，q0 * q1 表示先做 q1 再做 q0 的旋转
Quat4f operator * ( const Quat4f& q0, const Quat4f& q1 )
{
	return Quat4f
	(
		q0.w() * q1.w() - q0.x() * q1.x() - q0.y() * q1.y() - q0.z() * q1.z(),
		q0.w() * q1.x() + q0.x() * q1.w() + q0.y() * q1.z() - q0.z() * q1.y(),
		q0.w() * q1.y() - q0.x() * q1.z() + q0.y() * q1.w() + q0.z() * q1.x(),
		q0.w() * q1.z() + q0.x() * q1.y() - q0.y() * q1.x() + q0.z() * q1.w()
	);
}

// 标量左乘四元数：f * q，对每个分量乘以 f
Quat4f operator * ( float f, const Quat4f& q )
{
	return Quat4f
	(
		f * q.w(),
		f * q.x(),
		f * q.y(),
		f * q.z()
	);
}

// 四元数右乘标量：q * f，对每个分量乘以 f
Quat4f operator * ( const Quat4f& q, float f )
{
	return Quat4f
	(
		f * q.w(),
		f * q.x(),
		f * q.y(),
		f * q.z()
	);
}
