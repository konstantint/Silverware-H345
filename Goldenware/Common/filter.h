// Number filtering utilities.
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#pragma once
#include <algorithm>
#include <ratio>

// Helper wrapper class around numerics, which provides
// low-pass filtered updates to the contained value.
//
// We can't pass float arguments to templates so we have
// to wrap them into a std::ratio class
template<typename T, typename Coef> class LPFValue {
private:
	static constexpr float coef = (float) Coef::num / (float) Coef::den;
	T _value;
public:
	inline LPFValue() :
			_value() {
	}
	inline LPFValue(T value) :
			_value(value) {
	}
	inline void update(T value) {
		_value = _value * coef + value * (1.0 - coef);
	}
	inline operator T() {
		return _value;
	}
	const T& value() {
		return _value;
	}
};

// Same as LPVValue, but specialized to fix-sized container types
template<typename Container, typename Coef> class LPFArray {
private:
	static constexpr float coef = (float) Coef::num / (float) Coef::den;
	Container _value;
public:
	inline LPFArray() :
			_value() {
	}
	inline LPFArray(Container value) :
			_value() {
		std::copy(value.begin(), value.end(), _value.begin());
	}
	inline void update(Container value) {
		for (int i = 0; i < value.size(); i++)
			_value[i] = _value[i] * coef + value[i] * (1.0 - coef);
	}
	inline operator Container() {
		return _value;
	}
	const Container& value() {
		return _value;
	}
};
