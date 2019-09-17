/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

#ifndef __PolyVox_Vector_H__
#define __PolyVox_Vector_H__

#include "Impl/ErrorHandling.h"
#include "Impl/PlatformDefinitions.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>

namespace PolyVox
{
	/**
	 * Represents a vector in space.
	 *
	 * This is a generl purpose vector class designed to represent both positions and directions. It is templatised
	 * on both size and data type but note that some of the operations do not make sense with integer types. For
	 * example it does not make conceptual sense to try and normalise an integer Vector.
	 *
	 * Every Vector must have at at least two elements, and the first four elements of any vector are known as the
	 * X, Y, Z and W elements. Note that W is last even though it comes before X in the alphabet. These elements can
	 * be accessed through getX(), setX(), getY(), setY(), getZ(), setZ(), getW() and setW(), while other elements
	 * can be accessed through getElemen() and setElement().
	 *
	 * This class includes a number of common mathematical operations (addition, subtraction, etc) as well as vector
	 * specific operations such as the dot and cross products. Note that this class is also templatised on an
	 * OperationType which is used for many internal calculations and some results. For example, the square of a
	 * vector's length will always be an integer if all the elements are integers, but the value might be outside
	 * that which can be represented by the StorageType. You don't need to worry about this as long as you are using
	 * the built in typedefs for common configurations.
	 *
	 * Typedefs are provided for 2, 3 and 4 dimensional vector with int8_t, uint8_t, int16_t, uint6_t, int32_t,
	 * uint32_t, float and double types. These typedefs are used as follows:
	 *
	 * \code
	 * Vector2DInt32 test(1,2); //Declares a 2 dimensional Vector of type int32_t.
	 * \endcode
	 */
	template <uint32_t Size, typename StorageType, typename OperationType = StorageType>
	class Vector
	{
	public:
		/// Constructor
		Vector(void);
		/// Constructor.
		Vector(StorageType tFillValue);
		/// Constructor.
		Vector(StorageType x, StorageType y);
		/// Constructor.
		Vector(StorageType x, StorageType y, StorageType z);
		/// Constructor.
		Vector(StorageType x, StorageType y, StorageType z, StorageType w);
		/// Copy Constructor.
		Vector(const Vector<Size, StorageType, OperationType>& vector);
		/// Copy Constructor which performs casting.
		template <typename CastType> explicit Vector(const Vector<Size, CastType>& vector);
		/// Destructor.
		~Vector(void);

		/// Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator=(const Vector<Size, StorageType, OperationType>& rhs);
		/// Equality Operator.
		bool operator==(const Vector<Size, StorageType, OperationType>& rhs) const;
		/// Inequality Operator.
		bool operator!=(const Vector<Size, StorageType, OperationType>& rhs) const;
		/// Addition and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator+=(const Vector<Size, StorageType, OperationType> &rhs);
		/// Subtraction and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator-=(const Vector<Size, StorageType, OperationType> &rhs);
		/// Multiplication and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator*=(const Vector<Size, StorageType, OperationType> &rhs);
		/// Division and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator/=(const Vector<Size, StorageType, OperationType> &rhs);
		/// Multiplication and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator*=(const StorageType& rhs);
		/// Division and Assignment Operator.
		Vector<Size, StorageType, OperationType>& operator/=(const StorageType& rhs);

		/// Element Access.
		StorageType getElement(uint32_t index) const;
		/// Get the x component of the vector.
		StorageType getX(void) const;
		/// Get the y component of the vector.
		StorageType getY(void) const;
		/// Get the z component of the vector.
		StorageType getZ(void) const;
		/// Get the w component of the vector.
		StorageType getW(void) const;

		/// Element Access.
		void setElement(uint32_t index, StorageType tValue);
		/// Element Access.
		void setElements(StorageType x, StorageType y);
		/// Element Access.
		void setElements(StorageType x, StorageType y, StorageType z);
		/// Element Access.
		void setElements(StorageType x, StorageType y, StorageType z, StorageType w);
		/// Set the x component of the vector.
		void setX(StorageType tX);
		/// Set the y component of the vector.
		void setY(StorageType tY);
		/// Set the z component of the vector.
		void setZ(StorageType tZ);
		/// Set the w component of the vector.
		void setW(StorageType tW);

		/// Get the length of the vector.
		float length(void) const;
		/// Get the squared length of the vector.
		OperationType lengthSquared(void) const;
		/// Find the angle between this vector and that which is passed as a parameter.
		float angleTo(const Vector<Size, StorageType, OperationType>& vector) const;
		/// Find the cross product between this vector and the vector passed as a parameter.
		Vector<Size, StorageType, OperationType> cross(const Vector<Size, StorageType, OperationType>& vector) const;
		/// Find the dot product between this vector and the vector passed as a parameter.
		OperationType dot(const Vector<Size, StorageType, OperationType>& rhs) const;
		/// Normalise the vector.
		void normalise(void);

	private:
		// Values for the vector
		StorageType m_tElements[Size];
	};

	// Non-member overloaded operators. 
	/// Addition operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator+(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs);
	/// Subtraction operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator-(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs);
	/// Multiplication operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator*(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs);
	/// Division operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator/(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs);
	/// Multiplication operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator*(const Vector<Size, StorageType, OperationType>& lhs, const StorageType& rhs);
	/// Division operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator/(const Vector<Size, StorageType, OperationType>& lhs, const StorageType& rhs);
	/// Stream insertion operator.
	template <uint32_t Size, typename StorageType, typename OperationType>
	std::ostream& operator<<(std::ostream& os, const Vector<Size, StorageType, OperationType>& vector);

	//Some handy typedefs

	/// A 2D Vector of floats.
	typedef Vector<2, float, float> Vector2DFloat;
	/// A 2D Vector of doubles.
	typedef Vector<2, double, double> Vector2DDouble;
	/// A 2D Vector of signed 8-bit values.
	typedef Vector<2, int8_t, int32_t> Vector2DInt8;
	/// A 2D Vector of unsigned 8-bit values.
	typedef Vector<2, uint8_t, int32_t> Vector2DUint8;
	/// A 2D Vector of signed 16-bit values.
	typedef Vector<2, int16_t, int32_t> Vector2DInt16;
	/// A 2D Vector of unsigned 16-bit values.
	typedef Vector<2, uint16_t, int32_t> Vector2DUint16;
	/// A 2D Vector of signed 32-bit values.
	typedef Vector<2, int32_t, int32_t> Vector2DInt32;
	/// A 2D Vector of unsigned 32-bit values.
	typedef Vector<2, uint32_t, int32_t> Vector2DUint32;

	/// A 3D Vector of floats.
	typedef Vector<3, float, float> Vector3DFloat;
	/// A 3D Vector of doubles.
	typedef Vector<3, double, double> Vector3DDouble;
	/// A 3D Vector of signed 8-bit values.
	typedef Vector<3, int8_t, int32_t> Vector3DInt8;
	/// A 3D Vector of unsigned 8-bit values.
	typedef Vector<3, uint8_t, int32_t> Vector3DUint8;
	/// A 3D Vector of signed 16-bit values.
	typedef Vector<3, int16_t, int32_t> Vector3DInt16;
	/// A 3D Vector of unsigned 16-bit values.
	typedef Vector<3, uint16_t, int32_t> Vector3DUint16;
	/// A 3D Vector of signed 32-bit values.
	typedef Vector<3, int32_t, int32_t> Vector3DInt32;
	/// A 3D Vector of unsigned 32-bit values.
	typedef Vector<3, uint32_t, int32_t> Vector3DUint32;

	/// A 4D Vector of floats.
	typedef Vector<4, float, float> Vector4DFloat;
	/// A 4D Vector of doubles.
	typedef Vector<4, double, double> Vector4DDouble;
	/// A 4D Vector of signed 8-bit values.
	typedef Vector<4, int8_t, int32_t> Vector4DInt8;
	/// A 4D Vector of unsigned 8-bit values.
	typedef Vector<4, uint8_t, int32_t> Vector4DUint8;
	/// A 4D Vector of signed 16-bit values.
	typedef Vector<4, int16_t, int32_t> Vector4DInt16;
	/// A 4D Vector of unsigned 16-bit values.
	typedef Vector<4, uint16_t, int32_t> Vector4DUint16;
	/// A 4D Vector of signed 32-bit values.
	typedef Vector<4, int32_t, int32_t> Vector4DInt32;
	/// A 4D Vector of unsigned 32-bit values.
	typedef Vector<4, uint32_t, int32_t> Vector4DUint32;


}//namespace PolyVox

namespace std
{
	template <>
	struct hash<PolyVox::Vector3DInt32>
	{
		std::size_t operator()(const PolyVox::Vector3DInt32& vec) const
		{
			return ((vec.getX() & 0xFF)) | ((vec.getY() & 0xFF) << 8) | ((vec.getZ() & 0xFF) << 16);
		}
	};
}


#include "Vector.inl"

#endif

