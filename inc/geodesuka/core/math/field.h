#pragma once
#ifndef GEODESUKA_CORE_MATH_FIELD_H
#define GEODESUKA_CORE_MATH_FIELD_H

// ------------------------------ field.h ------------------------------ //
/*
* field.h is a class which exists over a selected space with
*/



namespace geodesuka::core::math {

	// T1 = 1, 2, 3, 4
	template <typename T1, typename T2, typename T3>
	class field {
	public:

		T1 n;
		T2 r1, r2;
		T3* d;

	private:

	};


}

#endif // !GEODESUKA_CORE_MATH_FIELD_H
