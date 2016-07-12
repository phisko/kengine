#include <cmath>
#include "Vector.hpp"

template<typename T>
Vector2<T>::Vector2(const T& _x, const T& _y) :
    x(_x), y(_y) {}

template<typename T>
Vector2<T>::Vector2(const Vector2<T>& rhs) :
    x(rhs.x), y(rhs.y) {}

template<typename T>
Vector2<T>	&Vector2<T>::operator=(const Vector2<T>& rhs)
{
  if (this != &rhs)
    {
      this->x = rhs.x;
      this->y = rhs.y;
    }
  return *this;
}

template<typename T>
Vector2<T>	Vector2<T>::operator+(const Vector2<T>& rhs) const
{
  return (Vector2<T>(this->x + rhs.x, this->y + rhs.y));
}

template<typename T>
Vector2<T>	Vector2<T>::operator+(const T& rhs) const
{
  return (Vector2<T>(this->x + rhs, this->y + rhs));
}

template<typename T>
Vector2<T>	Vector2<T>::operator-(const Vector2<T>& rhs) const
{
  return (Vector2<T>(this->x - rhs.x, this->y - rhs.y));
}

template<typename T>
Vector2<T>	Vector2<T>::operator-(const T& rhs) const
{
  return (Vector2<T>(this->x - rhs, this->y - rhs));
}

template<typename T>
Vector2<T>	Vector2<T>::operator*(const T& rhs) const
{
  return (Vector2<T>(this->x * rhs, this->y * rhs));
}

template<typename T>
T		Vector2<T>::dot(const Vector2<T>& rhs) const
{
  return T(this->x * rhs.x + this->y * rhs.y);
}

template<typename T>
T		Vector2<T>::length() const
{
  return std::sqrt(this->x * this->x + this->y * this->y);
}

template<typename T>
Vector2<T>	&Vector2<T>::normalize()
{
  T length = this->length();
  this->x /= length;
  this->y /= length;
  return *this;
}

template<typename T>
std::ostream	&operator<<(std::ostream& os, const Vector2<T>& rhs)
{
  os << '(' << rhs.x << ", " << rhs.y << ')';
  return os;
}
