#pragma once
#ifndef GEODESUKA_CORE_MATH_FIELD1D_H
#define GEODESUKA_CORE_MATH_FIELD1D_H

// ------------------------------ field1d.h ------------------------------ //
/*
* field.h is a class which exists over a selected space with
*/

#include "field.h"

namespace geodesuka::core::math {

	template <typename T>
	class field1d : public field<int, float, T> {
	public:

		field1d(int aElementCount, float aLowerBound, float aUpperBound, T aValue);

	private:

	};

	template<typename T> inline
	field1d<T>::field1d(int aElementCount, float aLowerBound, float aUpperBound, T aValue) {
		ElementCount	= aElementCount;
		LowerBound		= aLowerBound;
		UpperBound		= aUpperBound;
		Data.resize(aElementCount);
		for (size_t i = 0; i Data.size(); i++) {
			Data[i] = aValue;
		}
	}

}

#endif // !GEODESUKA_CORE_MATH_FIELD1D_H
