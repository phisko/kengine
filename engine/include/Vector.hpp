//
// File: [Vector.hpp]
//
// Author: flff.
// Contact: <Samy.kettani@gmail.com> (github.com/Flff)
//
// Created on 2016-07-12 16:06
//

#ifndef	VECTOR_HPP_
# define VECTOR_HPP_

# include <ostream>
# include <cmath>

template<typename T>
class Vector2
{
public:
  Vector2(const T & _x = T(0), const T & _y = T(0)) : x(_x), y(_y) {}
  Vector2(const Vector2<T>& rhs) : x(rhs.x), y(rhs.y) {}
  ~Vector2() {}

public:
  Vector2<T>	&operator=(const Vector2<T> &rhs)
    {
      if (this != &rhs)
	{
	  this->x = rhs.x;
	  this->y = rhs.y;
	}
      return *this;
    }

  Vector2<T>	operator+(const Vector2<T> &rhs) const
    {
      return (Vector2<T>(this->x + rhs.x, this->y + rhs.y));
    }

  Vector2<T>	operator+(const T& rhs) const
    {
      return (Vector2<T>(this->x + rhs, this->y + rhs));
    }

  Vector2<T>	operator-(const Vector2<T>& rhs) const
    {
      return (Vector2<T>(this->x - rhs.x, this->y - rhs.y));
    }

  Vector2<T>	operator-(const T& rhs) const
    {
      return (Vector2<T>(this->x - rhs, this->y - rhs));
    }

  Vector2<T>	operator*(const T& rhs) const
    {
      return (Vector2<T>(this->x * rhs, this->y * rhs));
    }

public:
  T		dot(const Vector2<T>& rhs) const
    {
      return T(this->x * rhs.x + this->y * rhs.y);
    }

  T		length() const
    {
      return std::sqrt(this->x * this->x + this->y * this->y);
    }

  Vector2<T>	&normalize()
    {
      T length = this->length();
      this->x /= length;
      this->y /= length;
      return *this;
    }

public:
  union
    {
      T	x;
      T	w;
    };
  union
    {
      T	y;
      T	h;
    };
};

  template<typename T>
std::ostream	&operator<<(std::ostream& os, const Vector2<T>& rhs)
{
  os << '(' << rhs.x << ", " << rhs.y << ')';
  return os;
}

#endif // !VECTOR_HPP_
