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

#include "TestArray.h"

#include "PolyVox/Array.h"

#include <QtTest>

using namespace PolyVox;

void TestArray::testCArraySpeed()
{
	const int width = 64;
	const int height = 32;
	const int depth = 16;

	int cArray[width][height][depth];

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int z = 0; z < depth; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					cArray[x][y][z] = ct;
					expectedTotal += cArray[x][y][z];
					ct++;
				}
			}
		}
	}
}

void TestArray::testPolyVoxArraySpeed()
{
	const int width = 64;
	const int height = 32;
	const int depth = 16;

	Array<3, int> polyvoxArray(width, height, depth);

	QBENCHMARK
	{
		int ct = 1;
		int expectedTotal = 0;
		for (int z = 0; z < depth; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					polyvoxArray(x, y, z) = ct;
					expectedTotal += polyvoxArray(x, y, z);
					ct++;
				}
			}
		}
	}
}

void TestArray::testReadWrite()
{
	int width = 5;
	int height = 10;
	int depth = 20;

	Array<3, int> myArray(width, height, depth);

	int ct = 1;
	int expectedTotal = 0;
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				myArray(x, y, z) = ct;
				expectedTotal += myArray(x, y, z);
				ct++;
			}
		}
	}

	ct = 1;
	int total = 0;
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				QCOMPARE(myArray(x, y, z), ct);
				total += myArray(x, y, z);
				ct++;
			}
		}
	}

	QCOMPARE(total, expectedTotal);
}

QTEST_MAIN(TestArray)
