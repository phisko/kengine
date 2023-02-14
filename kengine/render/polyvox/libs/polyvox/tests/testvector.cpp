/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 Matthew Williams and David Williams
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

#include "testvector.h"

#include "PolyVox/Vector.h"

#include <QtTest>

using namespace PolyVox;

Vector3DFloat incrementVector(Vector3DFloat input)
{
	Vector3DFloat result = input += 1.0f;
	return result;
}

void TestVector::testLength()
{
	Vector3DInt8 vec(3, 4, 5);
	QCOMPARE(vec.lengthSquared(), int32_t(3 * 3 + 4 * 4 + 5 * 5)); // QCOMPARE is strict on types. For an int8 vector, the OperationType is int32_t.
}

void TestVector::testDotProduct()
{
	Vector3DInt8 vecxy(3, 4, 0);
	Vector3DInt8 vecz(0, 0, 1);

	QCOMPARE(vecxy.dot(vecz), int32_t(0)); // QCOMPARE is strict on types. For an int8 vector, the OperationType is int32_t .
}

void TestVector::testEquality()
{
	Vector3DInt8 vecxy(3, 4, 0);
	Vector3DInt8 vecz(0, 0, 1);

	QCOMPARE(vecxy != vecz, true);
}

void TestVector::testPerformance()
{
	Vector3DFloat vec(1.0, 1.0, 1.0);

	QBENCHMARK
	{
		for (uint32_t ct = 0; ct < 10000000; ct++)
		{
			vec = incrementVector(vec);
		}
	}

		// Use the result so the calls don't get optimized away.
	QCOMPARE(vec.lengthSquared() > 0.0f, true);
}

QTEST_MAIN(TestVector)
