/*
 * Matrix4f.cpp 实现说明（中文注释版）
 * - 本文件实现 4x4 齐次矩阵完整能力：代数运算、逆、转置及图形学构造。
 * - 变换构造覆盖平移、旋转、缩放、lookAt、正交投影、透视投影等。
 * - 子矩阵接口（2x2/3x3）服务于行列式与逆矩阵分块计算。
 * - 投影相关函数同时考虑 OpenGL/DirectX 深度区间差异。
 * - randomRotation() 基于均匀采样生成随机旋转矩阵，用于随机姿态场景。
 */

#include "Matrix4f.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "Matrix2f.h"
#include "Matrix3f.h"
#include "Quat4f.h"
#include "Vector3f.h"
#include "Vector4f.h"

// 用标量 fill 填充矩阵所有 16 个元素
Matrix4f::Matrix4f( float fill )
{
	for( int i = 0; i < 16; ++i )
	{
		m_elements[ i ] = fill;
	}
}

// 按行主序参数构造 4x4 矩阵（内部以列主序存储）
Matrix4f::Matrix4f( float m00, float m01, float m02, float m03,
				   float m10, float m11, float m12, float m13,
				   float m20, float m21, float m22, float m23,
				   float m30, float m31, float m32, float m33 )
{
	m_elements[ 0 ] = m00;
	m_elements[ 1 ] = m10;
	m_elements[ 2 ] = m20;
	m_elements[ 3 ] = m30;

	m_elements[ 4 ] = m01;
	m_elements[ 5 ] = m11;
	m_elements[ 6 ] = m21;
	m_elements[ 7 ] = m31;

	m_elements[ 8 ] = m02;
	m_elements[ 9 ] = m12;
	m_elements[ 10 ] = m22;
	m_elements[ 11 ] = m32;

	m_elements[ 12 ] = m03;
	m_elements[ 13 ] = m13;
	m_elements[ 14 ] = m23;
	m_elements[ 15 ] = m33;
}

// 原地将所有元素除以标量 d
Matrix4f& Matrix4f::operator/=(float d)
{
	for(int ii=0;ii<16;ii++){
		m_elements[ii]/=d;
	}
	return *this;
}

// 用四个向量构造矩阵：setColumns=true 时四向量为列，否则为行
Matrix4f::Matrix4f( const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, bool setColumns )
{
	if( setColumns )
	{
		setCol( 0, v0 );
		setCol( 1, v1 );
		setCol( 2, v2 );
		setCol( 3, v3 );
	}
	else
	{
		setRow( 0, v0 );
		setRow( 1, v1 );
		setRow( 2, v2 );
		setRow( 3, v3 );
	}
}

// 拷贝构造：从另一个 Matrix4f 按字节复制所有元素
Matrix4f::Matrix4f( const Matrix4f& rm )
{
	memcpy( m_elements, rm.m_elements, sizeof(m_elements) );
}

// 赋值运算符：按字节复制 rm 的元素
Matrix4f& Matrix4f::operator = ( const Matrix4f& rm )
{
	if( this != &rm )
	{
		memcpy( m_elements, rm.m_elements, sizeof(m_elements)  );
	}
	return *this;
}

// 按行列下标读取元素（列主序），只读版本：(i, j) → m_elements[j*4+i]
const float& Matrix4f::operator () ( int i, int j ) const
{
	return m_elements[ j * 4 + i ];
}

// 按行列下标读写元素（列主序）：(i, j) → m_elements[j*4+i]
float& Matrix4f::operator () ( int i, int j )
{
	return m_elements[ j * 4 + i ];
}

// 返回第 i 行，以 Vector4f 形式提取
Vector4f Matrix4f::getRow( int i ) const
{
	return Vector4f
	(
		m_elements[ i ],
		m_elements[ i + 4 ],
		m_elements[ i + 8 ],
		m_elements[ i + 12 ]
	);
}

// 将向量 v 写入第 i 行
void Matrix4f::setRow( int i, const Vector4f& v )
{
	m_elements[ i ] = v.x();
	m_elements[ i + 4 ] = v.y();
	m_elements[ i + 8 ] = v.z();
	m_elements[ i + 12 ] = v.w();
}

// 返回第 j 列，以 Vector4f 形式提取
Vector4f Matrix4f::getCol( int j ) const
{
	int colStart = 4 * j;

	return Vector4f
	(
		m_elements[ colStart ],
		m_elements[ colStart + 1 ],
		m_elements[ colStart + 2 ],
		m_elements[ colStart + 3 ]
	);
}

// 将向量 v 写入第 j 列
void Matrix4f::setCol( int j, const Vector4f& v )
{
	int colStart = 4 * j;

	m_elements[ colStart ] = v.x();
	m_elements[ colStart + 1 ] = v.y();
	m_elements[ colStart + 2 ] = v.z();
	m_elements[ colStart + 3 ] = v.w();
}

// 从 (i0, j0) 起提取 2x2 子矩阵
Matrix2f Matrix4f::getSubmatrix2x2( int i0, int j0 ) const
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

// 从 (i0, j0) 起提取 3x3 子矩阵，用于余子式计算
Matrix3f Matrix4f::getSubmatrix3x3( int i0, int j0 ) const
{
	Matrix3f out;

	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			out( i, j ) = ( *this )( i + i0, j + j0 );
		}
	}

	return out;
}

// 将 2x2 矩阵 m 写入从 (i0, j0) 起的子块
void Matrix4f::setSubmatrix2x2( int i0, int j0, const Matrix2f& m )
{
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			( *this )( i + i0, j + j0 ) = m( i, j );
		}
	}
}

// 将 3x3 矩阵 m 写入从 (i0, j0) 起的子块
void Matrix4f::setSubmatrix3x3( int i0, int j0, const Matrix3f& m )
{
	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			( *this )( i + i0, j + j0 ) = m( i, j );
		}
	}
}

// 计算并返回 4x4 矩阵的行列式（按第一行展开，递归调用 3x3 行列式）
float Matrix4f::determinant() const
{
	float m00 = m_elements[ 0 ];
	float m10 = m_elements[ 1 ];
	float m20 = m_elements[ 2 ];
	float m30 = m_elements[ 3 ];

	float m01 = m_elements[ 4 ];
	float m11 = m_elements[ 5 ];
	float m21 = m_elements[ 6 ];
	float m31 = m_elements[ 7 ];

	float m02 = m_elements[ 8 ];
	float m12 = m_elements[ 9 ];
	float m22 = m_elements[ 10 ];
	float m32 = m_elements[ 11 ];

	float m03 = m_elements[ 12 ];
	float m13 = m_elements[ 13 ];
	float m23 = m_elements[ 14 ];
	float m33 = m_elements[ 15 ];

	float cofactor00 =  Matrix3f::determinant3x3( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
	float cofactor01 = -Matrix3f::determinant3x3( m12, m13, m10, m22, m23, m20, m32, m33, m30 );
	float cofactor02 =  Matrix3f::determinant3x3( m13, m10, m11, m23, m20, m21, m33, m30, m31 );
	float cofactor03 = -Matrix3f::determinant3x3( m10, m11, m12, m20, m21, m22, m30, m31, m32 );

	return( m00 * cofactor00 + m01 * cofactor01 + m02 * cofactor02 + m03 * cofactor03 );
}

// 计算逆矩阵（伴随矩阵除以行列式）；行列式绝对值小于 epsilon 时视为奇异
Matrix4f Matrix4f::inverse( bool* pbIsSingular, float epsilon ) const
{
	float m00 = m_elements[ 0 ];
	float m10 = m_elements[ 1 ];
	float m20 = m_elements[ 2 ];
	float m30 = m_elements[ 3 ];

	float m01 = m_elements[ 4 ];
	float m11 = m_elements[ 5 ];
	float m21 = m_elements[ 6 ];
	float m31 = m_elements[ 7 ];

	float m02 = m_elements[ 8 ];
	float m12 = m_elements[ 9 ];
	float m22 = m_elements[ 10 ];
	float m32 = m_elements[ 11 ];

	float m03 = m_elements[ 12 ];
	float m13 = m_elements[ 13 ];
	float m23 = m_elements[ 14 ];
	float m33 = m_elements[ 15 ];

    float cofactor00 =  Matrix3f::determinant3x3( m11, m12, m13, m21, m22, m23, m31, m32, m33 );
    float cofactor01 = -Matrix3f::determinant3x3( m12, m13, m10, m22, m23, m20, m32, m33, m30 );
    float cofactor02 =  Matrix3f::determinant3x3( m13, m10, m11, m23, m20, m21, m33, m30, m31 );
    float cofactor03 = -Matrix3f::determinant3x3( m10, m11, m12, m20, m21, m22, m30, m31, m32 );

    float cofactor10 = -Matrix3f::determinant3x3( m21, m22, m23, m31, m32, m33, m01, m02, m03 );
    float cofactor11 =  Matrix3f::determinant3x3( m22, m23, m20, m32, m33, m30, m02, m03, m00 );
    float cofactor12 = -Matrix3f::determinant3x3( m23, m20, m21, m33, m30, m31, m03, m00, m01 );
    float cofactor13 =  Matrix3f::determinant3x3( m20, m21, m22, m30, m31, m32, m00, m01, m02 );

    float cofactor20 =  Matrix3f::determinant3x3( m31, m32, m33, m01, m02, m03, m11, m12, m13 );
    float cofactor21 = -Matrix3f::determinant3x3( m32, m33, m30, m02, m03, m00, m12, m13, m10 );
    float cofactor22 =  Matrix3f::determinant3x3( m33, m30, m31, m03, m00, m01, m13, m10, m11 );
    float cofactor23 = -Matrix3f::determinant3x3( m30, m31, m32, m00, m01, m02, m10, m11, m12 );

    float cofactor30 = -Matrix3f::determinant3x3( m01, m02, m03, m11, m12, m13, m21, m22, m23 );
    float cofactor31 =  Matrix3f::determinant3x3( m02, m03, m00, m12, m13, m10, m22, m23, m20 );
    float cofactor32 = -Matrix3f::determinant3x3( m03, m00, m01, m13, m10, m11, m23, m20, m21 );
    float cofactor33 =  Matrix3f::determinant3x3( m00, m01, m02, m10, m11, m12, m20, m21, m22 );

	float determinant = m00 * cofactor00 + m01 * cofactor01 + m02 * cofactor02 + m03 * cofactor03;

	bool isSingular = ( fabs( determinant ) < epsilon );
	if( isSingular )
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = true;
		}
		return Matrix4f();
	}
	else
	{
		if( pbIsSingular != NULL )
		{
			*pbIsSingular = false;
		}

		float reciprocalDeterminant = 1.0f / determinant;

		return Matrix4f
			(
				cofactor00 * reciprocalDeterminant, cofactor10 * reciprocalDeterminant, cofactor20 * reciprocalDeterminant, cofactor30 * reciprocalDeterminant,
				cofactor01 * reciprocalDeterminant, cofactor11 * reciprocalDeterminant, cofactor21 * reciprocalDeterminant, cofactor31 * reciprocalDeterminant,
				cofactor02 * reciprocalDeterminant, cofactor12 * reciprocalDeterminant, cofactor22 * reciprocalDeterminant, cofactor32 * reciprocalDeterminant,
				cofactor03 * reciprocalDeterminant, cofactor13 * reciprocalDeterminant, cofactor23 * reciprocalDeterminant, cofactor33 * reciprocalDeterminant
			);
	}
}

// 原地转置：交换非对角线元素
void Matrix4f::transpose()
{
	float temp;

	for( int i = 0; i < 3; ++i )
	{
		for( int j = i + 1; j < 4; ++j )
		{
			temp = ( *this )( i, j );
			( * this )( i, j ) = ( *this )( j, i );
			( *this )( j, i ) = temp;
		}
	}
}

// 返回转置后的新矩阵，自身不变
Matrix4f Matrix4f::transposed() const
{
	Matrix4f out;
	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			out( j, i ) = ( *this )( i, j );
		}
	}

	return out;
}

// 隐式转换为可写 float 指针，便于传递给图形 API
Matrix4f::operator float* ()
{
	return m_elements;
}

// 隐式转换为 const float 指针，便于传递给图形 API（只读版本）
Matrix4f::operator const float* ()const
{
	return m_elements;
}


// 按行主序打印矩阵到标准输出
void Matrix4f::print()
{
	printf( "[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n[ %.4f %.4f %.4f %.4f ]\n",
		m_elements[ 0 ], m_elements[ 4 ], m_elements[ 8 ], m_elements[ 12 ],
		m_elements[ 1 ], m_elements[ 5 ], m_elements[ 9 ], m_elements[ 13 ],
		m_elements[ 2 ], m_elements[ 6 ], m_elements[ 10], m_elements[ 14 ],
		m_elements[ 3 ], m_elements[ 7 ], m_elements[ 11], m_elements[ 15 ] );
}

// static
// 返回所有元素均为 1 的 4x4 矩阵
Matrix4f Matrix4f::ones()
{
	Matrix4f m;
	for( int i = 0; i < 16; ++i )
	{
		m.m_elements[ i ] = 1;
	}

	return m;
}

// static
// 返回 4x4 单位矩阵（对角线为 1，其余为 0）
Matrix4f Matrix4f::identity()
{
	Matrix4f m;

	m( 0, 0 ) = 1;
	m( 1, 1 ) = 1;
	m( 2, 2 ) = 1;
	m( 3, 3 ) = 1;

	return m;
}

// static
// 构造沿 (x, y, z) 平移的 4x4 齐次变换矩阵
Matrix4f Matrix4f::translation( float x, float y, float z )
{
	return Matrix4f
	(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);
}

// static
// 构造沿向量 rTranslation 平移的 4x4 齐次变换矩阵
Matrix4f Matrix4f::translation( const Vector3f& rTranslation )
{
	return Matrix4f
	(
		1, 0, 0, rTranslation.x(),
		0, 1, 0, rTranslation.y(),
		0, 0, 1, rTranslation.z(),
		0, 0, 0, 1
	);
}

// static
// 构造绕 X 轴旋转 radians 弧度的 4x4 齐次旋转矩阵
Matrix4f Matrix4f::rotateX( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix4f
	(
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	);
}

// static
// 构造绕 Y 轴旋转 radians 弧度的 4x4 齐次旋转矩阵
Matrix4f Matrix4f::rotateY( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix4f
	(
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1
	);
}

// static
// 构造绕 Z 轴旋转 radians 弧度的 4x4 齐次旋转矩阵
Matrix4f Matrix4f::rotateZ( float radians )
{
	float c = cos( radians );
	float s = sin( radians );

	return Matrix4f
	(
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

// static
// 构造绕任意轴 rDirection 旋转 radians 弧度的 4x4 齐次旋转矩阵（Rodrigues 公式）
Matrix4f Matrix4f::rotation( const Vector3f& rDirection, float radians )
{
	Vector3f normalizedDirection = rDirection.normalized();

	float cosTheta = cos( radians );
	float sinTheta = sin( radians );

	float x = normalizedDirection.x();
	float y = normalizedDirection.y();
	float z = normalizedDirection.z();

	return Matrix4f
	(
		x * x * ( 1.0f - cosTheta ) + cosTheta,			y * x * ( 1.0f - cosTheta ) - z * sinTheta,		z * x * ( 1.0f - cosTheta ) + y * sinTheta,		0.0f,
		x * y * ( 1.0f - cosTheta ) + z * sinTheta,		y * y * ( 1.0f - cosTheta ) + cosTheta,			z * y * ( 1.0f - cosTheta ) - x * sinTheta,		0.0f,
		x * z * ( 1.0f - cosTheta ) - y * sinTheta,		y * z * ( 1.0f - cosTheta ) + x * sinTheta,		z * z * ( 1.0f - cosTheta ) + cosTheta,			0.0f,
		0.0f,											0.0f,											0.0f,											1.0f
	);
}

// static
// 将单位四元数 q 转换为等价的 4x4 齐次旋转矩阵
Matrix4f Matrix4f::rotation( const Quat4f& q )
{
	Quat4f qq = q.normalized();

	float xx = qq.x() * qq.x();
	float yy = qq.y() * qq.y();
	float zz = qq.z() * qq.z();

	float xy = qq.x() * qq.y();
	float zw = qq.z() * qq.w();

	float xz = qq.x() * qq.z();
	float yw = qq.y() * qq.w();

	float yz = qq.y() * qq.z();
	float xw = qq.x() * qq.w();

	return Matrix4f
	(
		1.0f - 2.0f * ( yy + zz ),		2.0f * ( xy - zw ),				2.0f * ( xz + yw ),				0.0f,
		2.0f * ( xy + zw ),				1.0f - 2.0f * ( xx + zz ),		2.0f * ( yz - xw ),				0.0f,
		2.0f * ( xz - yw ),				2.0f * ( yz + xw ),				1.0f - 2.0f * ( xx + yy ),		0.0f,
		0.0f,							0.0f,							0.0f,							1.0f
	);
}

// static
// 构造各轴独立缩放的 4x4 对角齐次矩阵（sx, sy, sz 分别为三轴缩放因子）
Matrix4f Matrix4f::scaling( float sx, float sy, float sz )
{
	return Matrix4f
	(
		sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, sz, 0,
		0, 0, 0, 1
	);
}

// static
// 构造三轴等比缩放的 4x4 对角齐次矩阵（s 为统一缩放因子）
Matrix4f Matrix4f::uniformScaling( float s )
{
	return Matrix4f
	(
		s, 0, 0, 0,
		0, s, 0, 0,
		0, 0, s, 0,
		0, 0, 0, 1
	);
}

// static
// 用三个均匀分布随机数 u0/u1/u2 生成均匀分布的随机旋转矩阵
Matrix4f Matrix4f::randomRotation( float u0, float u1, float u2 )
{
	return Matrix4f::rotation( Quat4f::randomRotation( u0, u1, u2 ) );
}

// static
// 构造观察矩阵：将世界坐标系变换到以 eye 为原点、朝向 center、up 为上方的相机空间
Matrix4f Matrix4f::lookAt( const Vector3f& eye, const Vector3f& center, const Vector3f& up )
{
	// z is negative forward
	Vector3f z = ( eye - center ).normalized();
	Vector3f y = up;
	Vector3f x = Vector3f::cross( y, z );

	// the x, y, and z vectors define the orthonormal coordinate system
	// the affine part defines the overall translation
	Matrix4f view;

	view.setRow( 0, Vector4f( x, -Vector3f::dot( x, eye ) ) );
	view.setRow( 1, Vector4f( y, -Vector3f::dot( y, eye ) ) );
	view.setRow( 2, Vector4f( z, -Vector3f::dot( z, eye ) ) );
	view.setRow( 3, Vector4f( 0, 0, 0, 1 ) );

	return view;
}

// static
// 构造正交投影矩阵（以宽高指定视口，directX=true 时深度区间为 [0,1]，否则为 [-1,1]）
Matrix4f Matrix4f::orthographicProjection( float width, float height, float zNear, float zFar, bool directX )
{
	Matrix4f m;

	m( 0, 0 ) = 2.0f / width;
	m( 1, 1 ) = 2.0f / height;
	m( 3, 3 ) = 1.0f;

	m( 0, 3 ) = -1;
	m( 1, 3 ) = -1;

	if( directX )
	{
		m( 2, 2 ) = 1.0f / ( zNear - zFar );
		m( 2, 3 ) = zNear / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = 2.0f / ( zNear - zFar );
		m( 2, 3 ) = ( zNear + zFar ) / ( zNear - zFar );
	}

	return m;
}

// static
// 构造正交投影矩阵（以六个裁剪面指定视锥，directX=true 时深度区间为 [0,1]，否则为 [-1,1]）
Matrix4f Matrix4f::orthographicProjection( float left, float right, float bottom, float top, float zNear, float zFar, bool directX )
{
	Matrix4f m;

	m( 0, 0 ) = 2.0f / ( right - left );
	m( 1, 1 ) = 2.0f / ( top - bottom );
	m( 3, 3 ) = 1.0f;

	m( 0, 3 ) = ( left + right ) / ( left - right );
	m( 1, 3 ) = ( top + bottom ) / ( bottom - top );

	if( directX )
	{
		m( 2, 2 ) = 1.0f / ( zNear - zFar );
		m( 2, 3 ) = zNear / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = 2.0f / ( zNear - zFar );
		m( 2, 3 ) = ( zNear + zFar ) / ( zNear - zFar );
	}

	return m;
}

// static
// 构造透视投影矩阵（以六个裁剪面指定视锥，directX=true 时深度区间为 [0,1]，否则为 [-1,1]）
Matrix4f Matrix4f::perspectiveProjection( float fLeft, float fRight,
										 float fBottom, float fTop,
										 float fZNear, float fZFar,
										 bool directX )
{
	Matrix4f projection; // zero matrix

	projection( 0, 0 ) = ( 2.0f * fZNear ) / ( fRight - fLeft );
	projection( 1, 1 ) = ( 2.0f * fZNear ) / ( fTop - fBottom );
	projection( 0, 2 ) = ( fRight + fLeft ) / ( fRight - fLeft );
	projection( 1, 2 ) = ( fTop + fBottom ) / ( fTop - fBottom );
	projection( 3, 2 ) = -1;

	if( directX )
	{
		projection( 2, 2 ) = fZFar / ( fZNear - fZFar);
		projection( 2, 3 ) = ( fZNear * fZFar ) / ( fZNear - fZFar );
	}
	else
	{
		projection( 2, 2 ) = ( fZNear + fZFar ) / ( fZNear - fZFar );
		projection( 2, 3 ) = ( 2.0f * fZNear * fZFar ) / ( fZNear - fZFar );
	}

	return projection;
}

// static
// 构造透视投影矩阵（以垂直视角 fovY、宽高比 aspect 指定，directX=true 时深度区间为 [0,1]，否则为 [-1,1]）
Matrix4f Matrix4f::perspectiveProjection( float fovYRadians, float aspect, float zNear, float zFar, bool directX )
{
	assert(zNear >= 0);
	assert(zFar > 0);
	Matrix4f m; // zero matrix

	float yScale = 1.f / tanf( 0.5f * fovYRadians );
	float xScale = yScale / aspect;

	m( 0, 0 ) = xScale;
	m( 1, 1 ) = yScale;
	m( 3, 2 ) = -1;

	if( directX )
	{
		m( 2, 2 ) = zFar / ( zNear - zFar );
		m( 2, 3 ) = zNear * zFar / ( zNear - zFar );
	}
	else
	{
		m( 2, 2 ) = ( zFar + zNear ) / ( zNear - zFar );
		m( 2, 3 ) = 2.f * zFar * zNear / ( zNear - zFar );
	}

	return m;
}

// static
// 构造远截面为无穷远的透视投影矩阵（极限情况下的截锥体）
Matrix4f Matrix4f::infinitePerspectiveProjection( float fLeft, float fRight,
												 float fBottom, float fTop,
												 float fZNear, bool directX )
{
	Matrix4f projection;

	projection( 0, 0 ) = ( 2.0f * fZNear ) / ( fRight - fLeft );
	projection( 1, 1 ) = ( 2.0f * fZNear ) / ( fTop - fBottom );
	projection( 0, 2 ) = ( fRight + fLeft ) / ( fRight - fLeft );
	projection( 1, 2 ) = ( fTop + fBottom ) / ( fTop - fBottom );
	projection( 3, 2 ) = -1;

	// infinite view frustum
	// just take the limit as far --> inf of the regular frustum
	if( directX )
	{
		projection( 2, 2 ) = -1.0f;
		projection( 2, 3 ) = -fZNear;
	}
	else
	{
		projection( 2, 2 ) = -1.0f;
		projection( 2, 3 ) = -2.0f * fZNear;
	}

	return projection;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

// 矩阵左乘列向量：M * v，按标准 4x4 矩阵-向量乘法计算
Vector4f operator * ( const Matrix4f& m, const Vector4f& v )
{
	Vector4f output( 0, 0, 0, 0 );

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			output[ i ] += m( i, j ) * v[ j ];
		}
	}

	return output;
}

// 两矩阵相乘：x * y，按标准 4x4 矩阵乘法计算
Matrix4f operator * ( const Matrix4f& x, const Matrix4f& y )
{
	Matrix4f product; // zeroes

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			for( int k = 0; k < 4; ++k )
			{
				product( i, k ) += x( i, j ) * y( j, k );
			}
		}
	}

	return product;
}
// 矩阵右乘标量：M * f，对每个元素乘以 f
Matrix4f operator * (const Matrix4f& m, float f) {
	Matrix4f product(m); // zeroes

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
            product(i, j) *= f;
		}
	}
	return product;
}
// 标量左乘矩阵：f * M，对每个元素乘以 f
Matrix4f operator * (float f, const Matrix4f& m) {
	Matrix4f product(m); // zeroes

	for( int i = 0; i < 4; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
            product(i, j) *= f;
		}
	}
	return product;
}
