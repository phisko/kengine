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

template<typename T>
class Vector2
{
public:
  Vector2(const T & _x = T(0), const T & _y = T(0));
  Vector2(const Vector2<T> &);
  ~Vector2() {}

public:
  Vector2<T>	&operator=(const Vector2<T> &);
  Vector2<T>	operator+(const Vector2<T> &) const;
  Vector2<T>	operator+(const T &) const;
  Vector2<T>	operator-(const Vector2<T> &) const;
  Vector2<T>	operator-(const T &) const;
  Vector2<T>	operator*(const T &) const;

public:
  T		dot(const Vector2<T>&) const;
  T		length() const;
  Vector2<T>	&normalize();

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
std::ostream	&operator<<(std::ostream&, const Vector2<T>&);

#endif // !VECTOR_HPP_
