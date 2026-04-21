/*
 * Matrix2f.cpp 实现说明（中文注释版）
 * - 本文件实现 2x2 矩阵的所有成员函数与相关全局运算符。
 * - 数据按列主序存储：第 j 列第 i 行映射到 m_elements[j * 2 + i]。
 * - inverse() 使用 2x2 逆矩阵显式公式，并通过 epsilon 判断奇异矩阵。
 * - rotation(degrees) 构造二维旋转矩阵，角度输入会在实现中转换为弧度。
 * - Matrix2f * Vector2f 与 Matrix2f * Matrix2f 保持数学语义一致。
 */

#include "Matrix2f.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "Vector2f.h"

// 用标量 fill 填充矩阵所有元素
Matrix2f::Matrix2f( float fill )
{
	for( int i = 0; i < 4; ++i )
	{
		m_elements[ i ] = fill;
	}
}

// 按行主序参数构造矩阵：m00 m01 / m10 m11（内部以列主序存储）
Matrix2f::Matrix2f( float m00, float m01,
				   float m10, float m11 )
{
	m_elements[ 0 ] = m00;
	m_elements[ 1 ] = m10;

	m_elements[ 2 ] = m01;
	m_elements[ 3 ] = m11;
}

// 用两个向量构造矩阵：setColumns=true 时两向量为列，否则为行
Matrix2f::Matrix2f( const Vector2f& v0, const Vector2f& v1, bool setColumns )
{
	if( setColumns )
	{
		setCol( 0, v0 );
		setCol( 1, v1 );
	}
	else
	{
		setRow( 0, v0 );
		setRow( 1, v1 );
	}
}

// 拷贝构造：从另一个 Matrix2f 按字节复制所有元素
Matrix2f::Matrix2f( const Matrix2f& rm )
{
	memcpy( m_elements, rm.m_elements, sizeof(m_elements));
}

// 赋值运算符：按字节复制 rm 的元素
Matrix2f& Matrix2f::operator = ( const Matrix2f& rm )
{
	if( this != &rm )
	{
		memcpy( m_elements, rm.m_elements, sizeof(m_elements)   );
	}
	return *this;
}

// 按行列下标读取元素（列主序），只读版本：(i, j) → m_elements[j*2+i]
const float& Matrix2f::operator () ( int i, int j ) const
{
	return m_elements[ j * 2 + i ];
}

// 按行列下标读写元素（列主序）：(i, j) → m_elements[j*2+i]
float& Matrix2f::operator () ( int i, int j )
{
	return m_elements[ j * 2 + i ];
}

// 返回第 i 行，以 Vector2f 形式提取
Vector2f Matrix2f::getRow( int i ) const
{
	return Vector2f
	(
		m_elements[ i ],
		m_elements[ i + 2 ]
	);
}

// 将向量 v 写入第 i 行
void Matrix2f::setRow( int i, const Vector2f& v )
{
	m_elements[ i ] = v.x();
	m_elements[ i + 2 ] = v.y();
}

// 返回第 j 列，以 Vector2f 形式提取
Vector2f Matrix2f::getCol( int j ) const
{
	int colStart = 2 * j;

	return Vector2f
	(
		m_elements[ colStart ],
		m_elements[ colStart + 1 ]
	);
}

// 将向量 v 写入第 j 列
void Matrix2f::setCol( int j, const Vector2f& v )
{
	int colStart = 2 * j;

	m_elements[ colStart ] = v.x();
	m_elements[ colStart + 1 ] = v.y();
}

// 计算并返回矩阵的行列式（ad - bc）
float Matrix2f::determinant()
{
	return Matrix2f::determinant2x2
	(
		m_elements[ 0 ], m_elements[ 2 ],
		m_elements[ 1 ], m_elements[ 3 ]
	);
}

// 计算逆矩阵；行列式绝对值小于 epsilon 时视为奇异矩阵并通过 pbIsSingular 报告
Matrix2f Matrix2f::inverse( bool* pbIsSingular, float epsilon )
{
	float determinant = m_elements[ 0 ] * m_elements[ 3 ] - m_elements[ 2 ] * m_elements[ 1 ];

	bool isSingular = ( fabs( determinant ) < epsilon );
	if( isSingular )
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = true;
		}
		return Matrix2f();
	}
	else
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = false;
		}

		float reciprocalDeterminant = 1.0f / determinant;

		return Matrix2f
		(
			m_elements[ 3 ] * reciprocalDeterminant, -m_elements[ 2 ] * reciprocalDeterminant,
			-m_elements[ 1 ] * reciprocalDeterminant, m_elements[ 0 ] * reciprocalDeterminant
		);
	}
}

// 原地转置：交换非对角线元素
void Matrix2f::transpose()
{
	float m01 = ( *this )( 0, 1 );
	float m10 = ( *this )( 1, 0 );

	( *this )( 0, 1 ) = m10;
	( *this )( 1, 0 ) = m01;
}

// 返回转置后的新矩阵，自身不变
Matrix2f Matrix2f::transposed() const
{
	return Matrix2f
	(
		( *this )( 0, 0 ), ( *this )( 1, 0 ),
		( *this )( 0, 1 ), ( *this )( 1, 1 )
	);

}

// 隐式转换为可写 float 指针，便于传递给图形 API
Matrix2f::operator float* ()
{
	return m_elements;
}

// 按行主序打印矩阵到标准输出
void Matrix2f::print()
{
	printf( "[ %.4f %.4f ]\n[ %.4f %.4f ]\n",
		m_elements[ 0 ], m_elements[ 2 ],
		m_elements[ 1 ], m_elements[ 3 ] );
}

// static
// 计算 2x2 矩阵的行列式：m00*m11 - m01*m10
float Matrix2f::determinant2x2( float m00, float m01,
							   float m10, float m11 )
{
	return( m00 * m11 - m01 * m10 );
}

// static
// 返回所有元素均为 1 的矩阵
Matrix2f Matrix2f::ones()
{
	Matrix2f m;
	for( int i = 0; i < 4; ++i )
	{
		m.m_elements[ i ] = 1;
	}

	return m;
}

// static
// 返回 2x2 单位矩阵（对角线为 1，其余为 0）
Matrix2f Matrix2f::identity()
{
	Matrix2f m;

	m( 0, 0 ) = 1;
	m( 1, 1 ) = 1;

	return m;
}

// static
// 构造二维旋转矩阵，输入单位为弧度（注意：参数名为 degrees 但实现按弧度使用）
Matrix2f Matrix2f::rotation( float degrees )
{
	float c = cos( degrees );
	float s = sin( degrees );

	return Matrix2f
	(
		c, -s,
		s, c
	);
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

// 标量左乘矩阵：f * M，对每个元素乘以 f
Matrix2f operator * ( float f, const Matrix2f& m )
{
	Matrix2f output;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			output( i, j ) = f * m( i, j );
		}
	}

	return output;
}

// 矩阵右乘标量：M * f，复用左乘实现
Matrix2f operator * ( const Matrix2f& m, float f )
{
	return f * m;
}

// 矩阵左乘列向量：M * v，按标准矩阵-向量乘法计算
Vector2f operator * ( const Matrix2f& m, const Vector2f& v )
{
	Vector2f output( 0, 0 );

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			output[ i ] += m( i, j ) * v[ j ];
		}
	}

	return output;
}

// 两矩阵相乘：x * y，按标准矩阵乘法计算
Matrix2f operator * ( const Matrix2f& x, const Matrix2f& y )
{
	Matrix2f product; // zeroes

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			for( int k = 0; k < 2; ++k )
			{
				product( i, k ) += x( i, j ) * y( j, k );
			}
		}
	}

	return product;
}
