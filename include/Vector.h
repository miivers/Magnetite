#ifndef _VECTOR_H_
#define _VECTOR_H_
//#include "prerequisites.h"

class Vector3 {
public:
	float x,y,z,w;
	
	Vector3( float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f );
	
	Vector3 operator- () const;
	Vector3 operator+ (const Vector3& right) const;
	Vector3 operator- (const Vector3& right) const;
	Vector3 operator/ (const Vector3& right) const;

	bool operator < (const Vector3& right);

	Vector3& operator += (const Vector3& right);
	Vector3& operator -= (const Vector3& right);
	
	Vector3 operator* ( float scal ) const;
	Vector3 operator/ ( float scal ) const;

	Vector3 operator* ( const Vector3& rhs ) const;

	bool operator== ( const Vector3& rhs ) const;

	float length();
	Vector3 normalize();

	float dotProduct( const Vector3& rhs );

	float smallestDimension();
	float biggestDimension();

	float operator[] ( int index );
};

#endif
