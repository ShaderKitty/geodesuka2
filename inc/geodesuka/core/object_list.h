#pragma once
#ifndef GEODESUKA_CORE_OBJECT_LIST_H
#define GEODESUKA_CORE_OBJECT_LIST_H

#include <vector>

#include "object_t.h"

namespace geodesuka::core {

	class object_list {
	public:

		std::vector<object_t*> Handle;

		object_list();
		object_list(int aObjectCount);
		object_list(object_t* aObject);
		object_list(int aObjectCount, object_t** aObjectList);
		~object_list();

		object_t*& operator[](int aIndex);
		object_t* operator[](int aIndex) const;

		object_list operator&(const object_list& aRhs) const;
		object_list operator|(const object_list& aRhs) const;
		object_list operator-(const object_list& aRhs) const;

		object_list operator&(object_t* aRhs) const;
		object_list operator|(object_t* aRhs) const;
		object_list operator-(object_t* aRhs) const;

		object_list& operator&=(const object_list& aRhs);
		object_list& operator|=(const object_list& aRhs);
		object_list& operator-=(const object_list& aRhs);

		object_list& operator&=(object_t* aRhs);
		object_list& operator|=(object_t* aRhs);
		object_list& operator-=(object_t* aRhs);

		object_list purify() const;
		bool exists(object_t* aObject) const;
		void resize(size_t aNewSize);
		void clear();
		int count() const;

		// Creates a list of objects owned by that stage.
		object_list gather_by(stage_t* aStage) const;

		// Finds an object by a name in the object set.
		object_t* find_by_name(const char* aName) const;

	};

}

#endif // !GEODESUKA_CORE_OBJECT_LIST_H
