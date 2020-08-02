// 3D point with standard vector space operations.
// Why isn't this provided by STL, really?
//
// License: MIT
// Author: konstantint

#pragma once
#include <array>

template<typename T> struct Point3d: public std::array<T, 3> {
	typedef std::array<T, 3> base_t;

	inline Point3d() :
			base_t { 0, 0, 0 } {
	}
	inline Point3d(T x, T y, T z) :
			base_t { x, y, z } {
	}

	// We use point3d to represent x/y/z or roll/pitch/yaw orientations
	inline const T& x() const {
		return (*this)[0];
	}
	inline const T& y() const {
		return (*this)[1];
	}
	inline const T& z() const {
		return (*this)[2];
	}
	inline T& x() {
		return (*this)[0];
	}
	inline T& y() {
		return (*this)[1];
	}
	inline T& z() {
		return (*this)[2];
	}
	inline const T& roll() const {
		return x();
	}
	inline const T& pitch() const {
		return y();
	}
	inline const T& yaw() const {
		return z();
	}

	template<typename T2> inline Point3d<T2> cast() const {
		return Point3d<T2>((T2) x(), (T2) y(), (T2) z());
	}

	inline Point3d<T> operator/(const T& value) const {
		return Point3d(x() / value, y() / value, z() / value);
	}

	inline Point3d<T> operator*(const T& value) const {
		return Point3d(x() * value, y() * value, z() * value);
	}

	inline Point3d<T> operator-(const Point3d<T>& value) const {
		return Point3d(x() - value.x(), y() - value.y(), z() - value.z());
	}

	inline Point3d<T> operator+(const Point3d<T>& value) const {
		return Point3d(x() + value.x(), y() + value.y(), z() + value.z());
	}
};
