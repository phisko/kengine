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

namespace PolyVox
{
	//-------------------------- Constructors, etc ---------------------------------
	/**
	 * Creates a Vector object but does not initialise it.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(void)
	{
	}

	/**
	 * Creates a Vector object and initialises all components with the given value.
	 * \param tFillValue The value to write to every component.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(StorageType tFillValue)
	{
		std::fill(m_tElements, m_tElements + Size, tFillValue);
	}

	/**
	 * Creates a Vector object and initialises it with given values.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(StorageType x, StorageType y)
	{
		static_assert(Size == 2, "This constructor should only be used for vectors with two elements.");

		m_tElements[0] = x;
		m_tElements[1] = y;
	}

	/**
	 * Creates a Vector3D object and initialises it with given values.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 * \param z the Z component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(StorageType x, StorageType y, StorageType z)
	{
		static_assert(Size == 3, "This constructor should only be used for vectors with three elements.");

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;

	}

	/**
	 * Creates a Vector3D object and initialises it with given values.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 * \param z The Z component to set.
	 * \param w The W component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(StorageType x, StorageType y, StorageType z, StorageType w)
	{
		static_assert(Size == 4, "This constructor should only be used for vectors with four elements.");

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

	/**
	 * Copy constructor builds object based on object passed as parameter.
	 * \param vector A reference to the Vector to be copied.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(const Vector<Size, StorageType, OperationType>& vector)
	{
		std::memcpy(m_tElements, vector.m_tElements, sizeof(StorageType)* Size);
	}

	/**
	 * This copy constructor allows casting between vectors with different data types.
	 * It makes it possible to use code such as:
	 *
	 * Vector3DDouble v3dDouble(1.0,2.0,3.0);
	 * Vector3DFloat v3dFloat = static_cast<Vector3DFloat>(v3dDouble); //Casting
	 *
	 * \param vector A reference to the Vector to be copied.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	template <typename CastType>
	Vector<Size, StorageType, OperationType>::Vector(const Vector<Size, CastType>& vector)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] = static_cast<StorageType>(vector.getElement(ct));
		}
	}

	/**
	 * Destroys the Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::~Vector(void)
	{
		// We put the static asserts in the destructor because there is one one of these,
		// where as there are multiple constructors.

		// Force a vector to have a length greater than one. There is no need for a
		// vector with one element, and supporting this would cause confusion over the
		// behaviour of the constructor taking a single value, as this fills all elements
		// to that value rather than just the first one.
		static_assert(Size > 1, "Vector must have a length greater than one.");
	}

	/**
	 * Assignment operator copies each element of first Vector to the second.
	 * \param rhs Vector to assign to.
	 * \return A reference to the result to allow chaining.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator=(const Vector<Size, StorageType, OperationType>& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}
		std::memcpy(m_tElements, rhs.m_tElements, sizeof(StorageType)* Size);
		return *this;
	}

	/**
	 * Checks whether two Vectors are equal.
	 * \param rhs The Vector to compare to.
	 * \return true if the Vectors match.
	 * \see operator!=
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline bool Vector<Size, StorageType, OperationType>::operator==(const Vector<Size, StorageType, OperationType> &rhs) const
	{
		bool equal = true;
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			if (m_tElements[ct] != rhs.m_tElements[ct])
			{
				equal = false;
				break;
			}
		}
		return equal;
	}

	/**
	 * Checks whether two Vectors are not equal.
	 * \param rhs The Vector to compare to.
	 * \return true if the Vectors do not match.
	 * \see operator==
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline bool Vector<Size, StorageType, OperationType>::operator!=(const Vector<Size, StorageType, OperationType> &rhs) const
	{
		return !(*this == rhs); //Just call equality operator and invert the result.
	}

	/**
	 * Addition operator adds corresponding elements of the two Vectors.
	 * \param rhs The Vector to add
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator+=(const Vector<Size, StorageType, OperationType>& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] += rhs.m_tElements[ct];
		}
		return *this;
	}

	/**
	 * Subtraction operator subtracts corresponding elements of one Vector from the other.
	 * \param rhs The Vector to subtract
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator-=(const Vector<Size, StorageType, OperationType>& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] -= rhs.m_tElements[ct];
		}
		return *this;
	}

	/**
	 * Multiplication operator multiplies corresponding elements of the two Vectors.
	 * \param rhs The Vector to multiply by
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator*=(const Vector<Size, StorageType, OperationType>& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs.m_tElements[ct];
		}
		return *this;
	}

	/**
	 * Division operator divides corresponding elements of one Vector by the other.
	 * \param rhs The Vector to divide by
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator/=(const Vector<Size, StorageType, OperationType>& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs.m_tElements[ct];
		}
		return *this;
	}

	/**
	 * Multiplication operator multiplies each element of the Vector by a number.
	 * \param rhs The number the Vector is multiplied by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator*=(const StorageType& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs;
		}
		return *this;
	}

	/**
	 * Division operator divides each element of the Vector by a number.
	 * \param rhs The number the Vector is divided by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator/=(const StorageType& rhs)
	{
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs;
		}
		return *this;
	}

	/**
	 * Addition operator adds corresponding elements of the two Vectors.
	 * \param lhs The Vector to add to.
	 * \param rhs The Vector to add.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator+(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result += rhs;
		return result;
	}

	/**
	 * Subtraction operator subtracts corresponding elements of one Vector from the other.
	 * \param lhs The Vector to subtract from.
	 * \param rhs The Vector to subtract.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator-(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result -= rhs;
		return result;
	}

	/**
	 * Multiplication operator mulitplies corresponding elements of the two Vectors.
	 * \param lhs The Vector to multiply.
	 * \param rhs The Vector to multiply by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator*(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result *= rhs;
		return result;
	}

	/**
	 * Division operator divides corresponding elements of one Vector by the other.
	 * \param lhs The Vector to divide.
	 * \param rhs The Vector to divide by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator/(const Vector<Size, StorageType, OperationType>& lhs, const Vector<Size, StorageType, OperationType>& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result /= rhs;
		return result;
	}

	/**
	 * Multiplication operator multiplies each element of the Vector by a number.
	 * \param lhs The Vector to multiply.
	 * \param rhs The number the Vector is multiplied by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator*(const Vector<Size, StorageType, OperationType>& lhs, const StorageType& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result *= rhs;
		return result;
	}

	/**
	 * Division operator divides each element of the Vector by a number.
	 * \param lhs The Vector to divide.
	 * \param rhs The number the Vector is divided by.
	 * \return The resulting Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType> operator/(const Vector<Size, StorageType, OperationType>& lhs, const StorageType& rhs)
	{
		Vector<Size, StorageType, OperationType> result = lhs;
		result /= rhs;
		return result;
	}

	/**
	 * Enables the Vector to be used intuitively with output streams such as cout.
	 * \param os The output stream to write to.
	 * \param vector The Vector to write to the stream.
	 * \return A reference to the output stream to allow chaining.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	std::ostream& operator<<(std::ostream& os, const Vector<Size, StorageType, OperationType>& vector)
	{
		os << "(";
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			os << vector.getElement(ct);
			if (ct < (Size - 1))
			{
				os << ",";
			}
		}
		os << ")";
		return os;
	}

	/**
	 * Returns the element at the given position.
	 * \param index The index of the element to return.
	 * \return The element.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getElement(uint32_t index) const
	{
		if (index >= Size)
		{
			POLYVOX_THROW(std::out_of_range, "Attempted to access invalid vector element.");
		}

		return m_tElements[index];
	}

	/**
	 * \return A const reference to the X component of a 1, 2, 3, or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getX(void) const
	{
		return m_tElements[0]; // This is fine, a Vector always contains at least two elements.
	}

	/**
	 * \return A const reference to the Y component of a 2, 3, or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getY(void) const
	{
		return m_tElements[1]; // This is fine, a Vector always contains at least two elements.
	}

	/**
	 * \return A const reference to the Z component of a 3 or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getZ(void) const
	{
		static_assert(Size >= 3, "You can only get the 'z' component from a vector with at least three elements.");

		return m_tElements[2];
	}

	/**
	 * \return A const reference to the W component of a 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getW(void) const
	{
		static_assert(Size >= 4, "You can only get the 'w' component from a vector with at least four elements.");

		return m_tElements[3];
	}

	/**
	 * \param index The index of the element to set.
	 * \param tValue The new value for the element.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setElement(uint32_t index, StorageType tValue)
	{
		if (index >= Size)
		{
			POLYVOX_THROW(std::out_of_range, "Attempted to access invalid vector element.");
		}

		m_tElements[index] = tValue;
	}

	/**
	 * Sets several elements of a vector at once.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setElements(StorageType x, StorageType y)
	{
		// This is fine, a Vector always contains at least two elements.
		m_tElements[0] = x;
		m_tElements[1] = y;
	}

	/**
	 * Sets several elements of a vector at once.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 * \param z The Z component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setElements(StorageType x, StorageType y, StorageType z)
	{
		static_assert(Size >= 3, "You can only use this version of setElements() on a vector with at least three elements.");

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
	}

	/**
	 * Sets several elements of a vector at once.
	 * \param x The X component to set.
	 * \param y The Y component to set.
	 * \param z The Z component to set.
	 * \param w The W component to set.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setElements(StorageType x, StorageType y, StorageType z, StorageType w)
	{
		static_assert(Size >= 4, "You can only use this version of setElements() on a vector with at least four elements.");

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

	/**
	 * \param tX The new value for the X component of a 1, 2, 3, or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setX(StorageType tX)
	{
		m_tElements[0] = tX; // This is fine, a Vector always contains at least two elements.
	}

	/**
	 * \param tY The new value for the Y component of a 2, 3, or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setY(StorageType tY)
	{
		m_tElements[1] = tY; // This is fine, a Vector always contains at least two elements.
	}

	/**
	 * \param tZ The new value for the Z component of a 3 or 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setZ(StorageType tZ)
	{
		static_assert(Size >= 3, "You can only set the 'w' component from a vector with at least three elements.");

		m_tElements[2] = tZ;
	}

	/**
	 * \param tW The new value for the W component of a 4 dimensional Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setW(StorageType tW)
	{
		static_assert(Size >= 4, "You can only set the 'w' component from a vector with at least four elements.");

		m_tElements[3] = tW;
	}

	/**
	 * \note This function always returns a single precision floating point value, even when the StorageType is a double precision floating point value or an integer.
	 * \return The length of the Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline float Vector<Size, StorageType, OperationType>::length(void) const
	{
		return sqrt(static_cast<float>(lengthSquared()));
	}

	/**
	 * \return The squared length of the Vector.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline OperationType Vector<Size, StorageType, OperationType>::lengthSquared(void) const
	{
		OperationType tLengthSquared = static_cast<OperationType>(0);
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			tLengthSquared += static_cast<OperationType>(m_tElements[ct]) * static_cast<OperationType>(m_tElements[ct]);
		}
		return tLengthSquared;
	}

	/**
	 * This function is commutative, such that a.angleTo(b) == b.angleTo(a). The angle
	 * returned is in radians and varies between 0 and 3.14(pi). It is always positive.
	 *
	 * \note This function always returns a single precision floating point value, even when the StorageType is a double precision floating point value or an integer.
	 *
	 * \param vector The Vector to find the angle to.
	 * \return The angle between them in radians.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline float Vector<Size, StorageType, OperationType>::angleTo(const Vector<Size, StorageType, OperationType>& vector) const
	{
		return acos(static_cast<float>(dot(vector)) / (vector.length() * this->length()));
	}

	/**
	 * This function is used to calculate the cross product of two Vectors.
	 * The cross product is the Vector which is perpendicular to the two
	 * given Vectors. It is worth remembering that, unlike the dot product,
	 * it is not commutative. E.g a.b != b.a. The cross product obeys the
	 * right-hand rule such that if the two vectors are given by the index
	 * finger and middle finger respectively then the cross product is given
	 * by the thumb.
	 * \param vector The vector to cross with this
	 * \return The value of the cross product.
	 * \see dot()
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType> Vector<Size, StorageType, OperationType>::cross(const Vector<Size, StorageType, OperationType>& vector) const
	{
		StorageType i = vector.getZ() * this->getY() - vector.getY() * this->getZ();
		StorageType j = vector.getX() * this->getZ() - vector.getZ() * this->getX();
		StorageType k = vector.getY() * this->getX() - vector.getX() * this->getY();
		return Vector<Size, StorageType, OperationType>(i, j, k);
	}

	/**
	 * Calculates the dot product of the Vector and the parameter.
	 * This function is commutative, such that a.dot(b) == b.dot(a).
	 * \param rhs The Vector to find the dot product with.
	 * \return The value of the dot product.
	 * \see cross()
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline OperationType Vector<Size, StorageType, OperationType>::dot(const Vector<Size, StorageType, OperationType>& rhs) const
	{
		OperationType dotProduct = static_cast<OperationType>(0);
		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			dotProduct += static_cast<OperationType>(m_tElements[ct]) * static_cast<OperationType>(rhs.m_tElements[ct]);
		}
		return dotProduct;
	}

	/**
	 * Divides the i, j, and k components by the length to give a Vector of length 1.0. If the vector is
	 * very short (or zero) then a divide by zero may cause elements to take on invalid values. You may
	 * want to check for this before normalising.
	 *
	 * \note You should not attempt to normalise a vector whose StorageType is an integer.
	 */
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::normalise(void)
	{
		float fLength = this->length();

		// We could wait until the NAN occurs before throwing, but then we'd have to add some roll-back code.
		// This seems like a lot of overhead for a common operation which should rarely go wrong.
		if (fLength <= 0.0001)
		{
			POLYVOX_THROW(invalid_operation, "Cannot normalise a vector with a length of zero");
		}

		for (uint32_t ct = 0; ct < Size; ++ct)
		{
			// Standard float rules apply for divide-by-zero
			m_tElements[ct] /= fLength;

			//This shouldn't happen as we had the length check earlier. So it's probably a bug if it does happen.
			POLYVOX_ASSERT(m_tElements[ct] == m_tElements[ct], "Obtained NAN during vector normalisation. Perhaps the input vector was too short?");
		}
	}
}//namespace PolyVox
