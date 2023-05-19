#pragma once
#ifndef GEODESUKA_CORE_STAGE_LIST_H
#define GEODESUKA_CORE_STAGE_LIST_H

#include "stage_t.h"

namespace geodesuka::core {

	class stage_list {
	public:

		stage_list();
		stage_list(stage_t* aStage);
		stage_list(int aStageCount, stage_t** aStageList);
		stage_list(const stage_list& aInput);
		stage_list(stage_list&& aInput) noexcept;
		~stage_list();

		stage_t*& operator[](int aIndex);
		stage_t* operator[](int aIndex) const;

		stage_list& operator=(const stage_list& aRhs);
		stage_list& operator=(stage_list&& aRhs) noexcept;

		stage_list operator&(const stage_list& aRhs) const;
		stage_list operator|(const stage_list& aRhs) const;
		stage_list operator-(const stage_list& aRhs) const;

		stage_list operator&(stage_t* aRhs) const;
		stage_list operator|(stage_t* aRhs) const;
		stage_list operator-(stage_t* aRhs) const;

		stage_list& operator&=(const stage_list& aRhs);
		stage_list& operator|=(const stage_list& aRhs);
		stage_list& operator-=(const stage_list& aRhs);

		stage_list& operator&=(stage_t* aRhs);
		stage_list& operator|=(stage_t* aRhs);
		stage_list& operator-=(stage_t* aRhs);

		stage_list purify() const;

		bool exists(stage_t* aStage) const;

		bool resize(int aNewSize);

		void clear();

		int count() const;

	private:

		int Count;
		stage_t** Handle;

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_STAGE_LIST_H
