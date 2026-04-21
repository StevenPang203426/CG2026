/*
 * Matrix3f.cpp 实现说明（中文注释版）
 * - 本文件实现 3x3 矩阵基础操作、逆矩阵、旋转与缩放构造。
 * - 通过 getSubmatrix2x2/setSubmatrix2x2 支撑代数余子式相关计算。
 * - determinant()/inverse() 以经典线性代数公式实现，并提供奇异性判断。
 * - rotation(axis, radians) 支持绕任意轴旋转，适用于局部姿态构造。
 * - rotation(Quat4f) 将四元数旋转转换为等价矩阵表达。
 */

#include "Matrix3f.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "Matrix2f.h"
#include "Quat4f.h"
#include "Vector3f.h"

// 用标量 fill 填充矩阵所有 9 个元素
Matrix3f::Matrix3f( float fill )
{
	for( int i = 0; i < 9; ++i )
	{
		m_elements[ i ] = fill;
	}
}

// 按行主序参数构造矩阵（内部以列主序存储）
Matrix3f::Matrix3f( float m00, float m01, float m02,
				   float m10, float m11, float m12,
				   float m20, float m21, float m22 )
{
	m_elements[ 0 ] = m00;
	m_elements[ 1 ] = m10;
	m_elements[ 2 ] = m20;

	m_elements[ 3 ] = m01;
	m_elements[ 4 ] = m11;
	m_elements[ 5 ] = m21;

	m_elements[ 6 ] = m02;
	m_elements[ 7 ] = m12;
	m_elements[ 8 ] = m22;
}

// 用三个向量构造矩阵：setColumns=true 时三向量为列，否则为行
Matrix3f::Matrix3f( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, bool setColumns )
{
	if( setColumns )
	{
		setCol( 0, v0 );
		setCol( 1, v1 );
		setCol( 2, v2 );
	}
	else
	{
		setRow( 0, v0 );
		setRow( 1, v1 );
		setRow( 2, v2 );
	}
}

// 拷贝构造：从另一个 Matrix3f 按字节复制所有元素
Matrix3f::Matrix3f( const Matrix3f& rm )
{
	memcpy( m_elements, rm.m_elements, sizeof(m_elements)  );
}

// 赋值运算符：按字节复制 rm 的元素
Matrix3f& Matrix3f::operator = ( const Matrix3f& rm )
{
	if( this != &rm )
	{
		memcpy( m_elements, rm.m_elements, sizeof(m_elements)  );
	}
	return *this;
}

// 按行列下标读取元素（列主序），只读版本：(i, j) → m_elements[j*3+i]
const float& Matrix3f::operator () ( int i, int j ) const
{
	return m_elements[ j * 3 + i ];
}

// 按行列下标读写元素（列主序）：(i, j) → m_elements[j*3+i]
float& Matrix3f::operator () ( int i, int j )
{
	return m_elements[ j * 3 + i ];
}

// 返回第 i 行，以 Vector3f 形式提取
Vector3f Matrix3f::getRow( int i ) const
{
	return Vector3f
	(
		m_elements[ i ],
		m_elements[ i + 3 ],
		m_elements[ i + 6 ]
	);
}

// 将向量 v 写入第 i 行
void Matrix3f::setRow( int i, const Vector3f& v )
{
	m_elements[ i ] = v.x();
	m_elements[ i + 3 ] = v.y();
	m_elements[ i + 6 ] = v.z();
}

// 返回第 j 列，以 Vector3f 形式提取
Vector3f Matrix3f::getCol( int j ) const
{
	int colStart = 3 * j;

	return Vector3f
	(
		m_elements[ colStart ],
		m_elements[ colStart + 1 ],
		m_elements[ colStart + 2 ]
	);
}

// 将向量 v 写入第 j 列
void Matrix3f::setCol( int j, const Vector3f& v )
{
	int colStart = 3 * j;

	m_elements[ colStart ] = v.x();
	m_elements[ colStart + 1 ] = v.y();
	m_elements[ colStart + 2 ] = v.z();
}

// 从 (i0, j0) 起提取 2x2 子矩阵，用于余子式计算
Matrix2f Matrix3f::getSubmatrix2x2( int i0, int j0 ) const
{
	Matrix2f out;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			out( i, j ) = ( *this )( i + i0, j + j0 );
		}
	}

	return out;
}

// 将 2x2 矩阵 m 写入从 (i0, j0) 起的子块
void Matrix3f::setSubmatrix2x2( int i0, int j0, const Matrix2f& m )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			( *this )( i + i0, j + j0 ) = m( i, j );
		}
	}
}

// 计算并返回矩阵的行列式（按第一行展开）
float Matrix3f::determinant() const
{
	return Matrix3f::determinant3x3
	(
		m_elements[ 0 ], m_elements[ 3 ], m_elements[ 6 ],
		m_elements[ 1 ], m_elements[ 4 ], m_elements[ 7 ],
		m_elements[ 2 ], m_elements[ 5 ], m_elements[ 8 ]
	);
}

// 计算逆矩阵（伴随矩阵除以行列式）；行列式绝对值小于 epsilon 时视为奇异
Matrix3f Matrix3f::inverse( bool* pbIsSingular, float epsilon ) const
{
	float m00 = m_elements[ 0 ];
	float m10 = m_elements[ 1 ];
	float m20 = m_elements[ 2 ];

	float m01 = m_elements[ 3 ];
	float m11 = m_elements[ 4 ];
	float m21 = m_elements[ 5 ];

	float m02 = m_elements[ 6 ];
	float m12 = m_elements[ 7 ];
	float m22 = m_elements[ 8 ];

	float cofactor00 =  Matrix2f::determinant2x2( m11, m12, m21, m22 );
	float cofactor01 = -Matrix2f::determinant2x2( m10, m12, m20, m22 );
	float cofactor02 =  Matrix2f::determinant2x2( m10, m11, m20, m21 );

	float cofactor10 = -Matrix2f::determinant2x2( m01, m02, m21, m22 );
	float cofactor11 =  Matrix2f::determinant2x2( m00, m02, m20, m22 );
	float cofactor12 = -Matrix2f::determinant2x2( m00, m01, m20, m21 );

	float cofactor20 =  Matrix2f::determinant2x2( m01, m02, m11, m12 );
	float cofactor21 = -Matrix2f::determinant2x2( m00, m02, m10, m12 );
	float cofactor22 =  Matrix2f::determinant2x2( m00, m01, m10, m11 );

	float determinant = m00 * cofactor00 + m01 * cofactor01 + m02 * cofactor02;

	bool isSingular = ( fabs( determinant ) < epsilon );
	if( isSingular )
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = true;
		}
		return Matrix3f();
	}
	else
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = false;
		}

		float reciprocalDeterminant = 1.0f / determinant;

		return Matrix3f
		(
			cofactor00 * reciprocalDeterminant, cofactor10 * reciprocalDeterminant, cofactor20 * reciprocalDeterminant,
			cofactor01 * reciprocalDeterminant, cofactor11 * reciprocalDeterminant, cofactor21 * reciprocalDeterminant,
			cofactor02 * reciprocalDeterminant, cofactor12 * reciprocalDeterminant, cofactor22 * reciprocalDeterminant
		);
	}
}

// 原地转置：交换非对角线元素
void Matrix3f::transpose()
{
	float temp;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = i + 1; j < 3; ++j )
		{
			temp = ( *this )( i, j );
			( *this )( i, j ) = ( *this )( j, i );
			( *this )( j, i ) = temp;
		}
	}
}

// 返回转置后的新矩阵，自身不变
Matrix3f Matrix3f::transposed() const
{
	Matrix3f out;
	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			out( j, i ) = ( *this )( i, j );
		}
	}

	return out;
}

// 隐式转换为可写 float 指针，便于传递给图形 API
Matrix3f::operator float* ()
{
	return m_elements;
}

// 按行主序打印矩阵到标准输出
void Matrix3f::print()
{
	printf( "[ %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f ]\n",
		m_elements[ 0 ], m_elements[ 3 ], m_elements[ 6 ],
		m_elements[ 1 ], m_elements[ 4 ], m_elements[ 7 ],
		m_elements[ 2 ], m_elements[ 5 ], m_elements[ 8 ] );
}

// static
// 用 Sarrus 法则计算 3x3 矩阵的行列式
float Matrix3f::determinant3x3( float m00, float m01, float m02,
							   float m10, float m11, float m12,
							   float m20, float m21, float m22 )
{
	return
		(
			  m00 * ( m11 * m22 - m12 * m21 )
			- m01 * ( m10 * m22 - m12 * m20 )
			+ m02 * ( m10 * m21 - m11 * m20 )
		);
}

// static
// 返回所有元素均为 1 的 3x3 矩阵
Matrix3f Matrix3f::ones()
{
	Matrix3f m;
	for( int i = 0; i < 9; ++i )
	{
		m.m_elements[ i ] = 1;
	}

	return m;
}

// static
// 返回 3x3 单位矩阵（对角线为 1，其余为 0）
Matrix3f Matrix3f::identity()
{
	Matrix3f m;

	m( 0, 0 ) = 1;
	m( 1, 1 ) = 1;
	m( 2, 2 ) = 1;

	return m;
}


// static
// 构造绕 X 轴旋转 radians 弧度的 3x3 旋转矩阵
Matrix3f Matrix3f::rotateX( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix3f
	(
		1, 0, 0,
		0, c, -s,
		0, s, c
	);
}

// static
// 构造绕 Y 轴旋转 radians 弧度的 3x3 旋转矩阵
Matrix3f Matrix3f::rotateY( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix3f
	(
		c, 0, s,
		0, 1, 0,
		-s, 0, c
	);
}

// static
// 构造绕 Z 轴旋转 radians 弧度的 3x3 旋转矩阵
Matrix3f Matrix3f::rotateZ( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix3f
	(
		c, -s, 0,
		s, c, 0,
		0, 0, 1
	);
}

// static
// 构造各轴独立缩放的对角矩阵（sx, sy, sz 分别为三轴缩放因子）
Matrix3f Matrix3f::scaling( float sx, float sy, float sz )
{
	return Matrix3f
	(
		sx, 0, 0,
		0, sy, 0,
		0, 0, sz
	);
}

// static
// 构造三轴等比缩放的对角矩阵（s 为统一缩放因子）
Matrix3f Matrix3f::uniformScaling( float s )
{
	return Matrix3f
	(
		s, 0, 0,
		0, s, 0,
		0, 0, s
	);
}

// static
// 构造绕任意轴 rDirection 旋转 radians 弧度的 3x3 旋转矩阵（Rodrigues 公式）
Matrix3f Matrix3f::rotation( const Vector3f& rDirection, float radians )
{
	Vector3f normalizedDirection = rDirection.normalized();

	float cosTheta = cos( radians );
	float sinTheta = sin( radians );

	float x = normalizedDirection.x();
	float y = normalizedDirection.y();
	float z = normalizedDirection.z();

	return Matrix3f
		(
			x * x * ( 1.0f - cosTheta ) + cosTheta,			y * x * ( 1.0f - cosTheta ) - z * sinTheta,		z * x * ( 1.0f - cosTheta ) + y * sinTheta,
			x * y * ( 1.0f - cosTheta ) + z * sinTheta,		y * y * ( 1.0f - cosTheta ) + cosTheta,			z * y * ( 1.0f - cosTheta ) - x * sinTheta,
			x * z * ( 1.0f - cosTheta ) - y * sinTheta,		y * z * ( 1.0f - cosTheta ) + x * sinTheta,		z * z * ( 1.0f - cosTheta ) + cosTheta
		);
}

// static
// 将单位四元数 rq 转换为等价的 3x3 旋转矩阵
Matrix3f Matrix3f::rotation( const Quat4f& rq )
{
	Quat4f q = rq.normalized();

	float xx = q.x() * q.x();
	float yy = q.y() * q.y();
	float zz = q.z() * q.z();

	float xy = q.x() * q.y();
	float zw = q.z() * q.w();

	float xz = q.x() * q.z();
	float yw = q.y() * q.w();

	float yz = q.y() * q.z();
	float xw = q.x() * q.w();

	return Matrix3f
		(
			1.0f - 2.0f * ( yy + zz ),		2.0f * ( xy - zw ),				2.0f * ( xz + yw ),
			2.0f * ( xy + zw ),				1.0f - 2.0f * ( xx + zz ),		2.0f * ( yz - xw ),
			2.0f * ( xz - yw ),				2.0f * ( yz + xw ),				1.0f - 2.0f * ( xx + yy )
		);
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

// 矩阵左乘列向量：M * v，按标准矩阵-向量乘法计算
Vector3f operator * ( const Matrix3f& m, const Vector3f& v )
{
	Vector3f output( 0, 0, 0 );

	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			output[ i ] += m( i, j ) * v[ j ];
		}
	}

	return output;
}

// 两矩阵相乘：x * y，按标准矩阵乘法计算
Matrix3f operator * ( const Matrix3f& x, const Matrix3f& y )
{
	Matrix3f product; // zeroes

	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			for( int k = 0; k < 3; ++k )
			{
				product( i, k ) += x( i, j ) * y( j, k );
			}
		}
	}

	return product;
}
// Scalar multiplication
// 矩阵右乘标量：M * f，对每个元素乘以 f
Matrix3f operator * (const Matrix3f& m, float f) {
    Matrix3f product(m); // zeroes

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            product(i, j) *= f;
        }
    }
    return product;
}
// 标量左乘矩阵：f * M，对每个元素乘以 f
Matrix3f operator * (float f, const Matrix3f& m) {
    Matrix3f product(m); // zeroes
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            product(i, j) *= f;
        }
    }
    return product;
}
