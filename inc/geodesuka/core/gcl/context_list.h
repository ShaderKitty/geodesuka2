#pragma once
#ifndef GEODESUKA_CORE_GCL_CONTEXT_LIST_H
#define GEODESUKA_CORE_GCL_CONTEXT_LIST_H

#include <vector>

#include "context.h"

namespace geodesuka::core::gcl {

	class context_list {
	public:

		std::vector<context*> Handle;

		context_list();
		context_list(context* aContext);
		context_list(int aContextCount, context** aContextList);
		~context_list();

		context*& operator[](int aIndex);
		context* operator[](int aIndex) const;

		context_list operator&(const context_list& aRhs) const;
		context_list operator|(const context_list& aRhs) const;
		context_list operator-(const context_list& aRhs) const;

		context_list operator&(context* aRhs) const;
		context_list operator|(context* aRhs) const;
		context_list operator-(context* aRhs) const;

		context_list& operator&=(const context_list& aRhs);
		context_list& operator|=(const context_list& aRhs);
		context_list& operator-=(const context_list& aRhs);

		context_list& operator&=(context* aRhs);
		context_list& operator|=(context* aRhs);
		context_list& operator-=(context* aRhs);

		context_list purify() const;

		bool exists(context* aContext) const;

		void resize(size_t aNewSize);

		void clear();

		size_t count() const;

	};

}

#endif // !GEODESUKA_CORE_GCL_CONTEXT_LIST_H
