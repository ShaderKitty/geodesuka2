#include <geodesuka/core/stage_list.h>

#include <string.h>
#include <assert.h>

namespace geodesuka::core {

	stage_list::stage_list() {
		this->Count = 0;
		this->Handle = NULL;
	}

	stage_list::stage_list(stage_t* aStage) {
		this->zero_out();
		assert(!this->resize(1));
		this->Handle[0] = aStage;
	}

	stage_list::stage_list(int aStageCount, stage_t** aStageList) {
		this->zero_out();
		assert(!this->resize(aStageCount));
		memcpy(this->Handle, aStageList, aStageCount * sizeof(stage_t*));
	}

	stage_list::stage_list(const stage_list& aInput) {
		this->zero_out();
		assert(!this->resize(aInput.Count));
		memcpy(this->Handle, aInput.Handle, aInput.Count * sizeof(stage_t*));
	}

	stage_list::stage_list(stage_list&& aInput) noexcept {
		this->Count = aInput.Count;
		this->Handle = aInput.Handle;
		aInput.zero_out();
	}

	stage_list::~stage_list() {
		this->clear();
	}

	stage_t* stage_list::operator[](int aIndex) const {
		return this->Handle[aIndex];
	}

	stage_t*& stage_list::operator[](int aIndex) {
		return this->Handle[aIndex];
	}

	stage_list& stage_list::operator=(const stage_list& aRhs) {
		if (this == &aRhs) return *this;
		assert(!this->resize(aRhs.Count));
		memcpy(this->Handle, aRhs.Handle, aRhs.Count * sizeof(stage_t*));
		return *this;
	}

	stage_list& stage_list::operator=(stage_list&& aRhs) noexcept {
		this->clear();
		this->Count = aRhs.Count;
		this->Handle = aRhs.Handle;
		aRhs.zero_out();
		return *this;
	}

	stage_list stage_list::operator&(const stage_list& aRhs) const {
		stage_list Intersection;
		stage_list Left = this->purify();
		stage_list Right = aRhs.purify();
		int i = 0, j = 0, k = 0;
		for (i = 0; i < Left.Count; i++) {
			for (j = 0; j < Right.Count; j++) {
				if (Left[i] == Right[j]) {
					k += 1;
				}
			}
		}
		if (k == 0) return Intersection;
		assert(!Intersection.resize(k));

		i = 0;
		j = 0;
		k = 0;
		for (i = 0; i < Left.Count; i++) {
			if (Right.exists(Left[i])) {
				Intersection[k] = Left[i];
				k += 1;
			}
		}
		return Intersection;
	}

	stage_list stage_list::operator|(const stage_list& aRhs) const {
		stage_list Union;
		stage_list Left = this->purify();
		stage_list Right = aRhs.purify();
		stage_list Intersection = Left & Right;

		int LeftUniqueCount = Left.Count - Intersection.Count;
		int RightUniqueCount = Right.Count - Intersection.Count;
		assert(!Union.resize(LeftUniqueCount + Intersection.Count + RightUniqueCount));

		int k = 0;
		for (int i = 0; i < Left.Count; i++) {
			if (!Intersection.exists(Left[i])) {
				Union[k] = Left[i];
				k += 1;
			}
		}

		for (int i = 0; i < Intersection.Count; i++) {
			Union[k] = Intersection[i];
			k += 1;
		}

		for (int i = 0; i < Right.Count; i++) {
			if (!Intersection.exists(Right[i])) {
				Union[k] = Right[i];
				k += 1;
			}
		}

		return Union;
	}

	stage_list stage_list::operator-(const stage_list& aRhs) const {
		stage_list Sublist;
		stage_list Left = this->purify();
		stage_list Right = aRhs.purify();
		stage_list Intersection = *this & aRhs;

		if (Intersection.Count == 0) return *this;

		if (Intersection.Count == Left.Count) return Sublist;

		assert(!Sublist.resize(Left.Count - Intersection.Count));

		int k = 0;
		for (int i = 0; i < Left.Count; i++) {
			if (!Intersection.exists(Left[i])) {
				Sublist[k] = Left[i];
				k += 1;
			}
		}

		return Sublist;
	}

	stage_list stage_list::operator&(stage_t* aRhs) const {
		return (*this & stage_list(aRhs));
	}

	stage_list stage_list::operator|(stage_t* aRhs) const {
		return (*this | stage_list(aRhs));
	}

	stage_list stage_list::operator-(stage_t* aRhs) const {
		return (*this - stage_list(aRhs));
	}

	stage_list& stage_list::operator&=(const stage_list& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	stage_list& stage_list::operator|=(const stage_list& aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	stage_list& stage_list::operator-=(const stage_list& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	stage_list& stage_list::operator&=(stage_t* aRhs) {
		return *this &= stage_list(aRhs);
	}

	stage_list& stage_list::operator|=(stage_t* aRhs) {
		return *this |= stage_list(aRhs);
	}

	stage_list& stage_list::operator-=(stage_t* aRhs) {
		return *this -= stage_list(aRhs);
	}

	stage_list stage_list::purify() const {
		stage_list Purified;

		int NullCount = 0;
		int RepeatCount = 0;
		int NewCount = 0;
		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i] == nullptr) {
				NullCount += 1;
				continue;
			}
			for (int j = i + 1; j < this->Count; j++) {
				if (this->Handle[i] == this->Handle[j]) {
					RepeatCount += 1;
				}
			}
		}

		NewCount = this->Count - NullCount - RepeatCount;
		if (NewCount == this->Count) return *this;
		assert(!Purified.resize(NewCount));

		NewCount = 0;
		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i] == nullptr) continue;
			bool AlreadyExists = false;
			for (int j = 0; j < NewCount; j++) {
				AlreadyExists |= (this->Handle[i] == Purified[j]);
			}
			if (AlreadyExists) continue;
			Purified[NewCount] = this->Handle[i];
			NewCount += 1;
		}

		return Purified;
	}

	bool stage_list::exists(stage_t* aStage) const {
		for (int i = 0; i < this->Count; i++){
			if (this->Handle[i] == aStage) {
				return true;
			}
		}
		return false;
	}

	bool stage_list::resize(int aNewCount) {
		if (this->Count == aNewCount) return false;
		if (aNewCount > 0) {
			size_t TypeSize = sizeof(stage_t*);
			void* nptr = NULL;
			if (this->Handle == NULL) {
				nptr = malloc(aNewCount * TypeSize);
			}
			else {
				nptr = realloc(this->Handle, aNewCount * TypeSize);
			}
			if (nptr == NULL) return true;
			if (aNewCount > this->Count) {
				uintptr_t Address = (uintptr_t)nptr + this->Count * TypeSize;
				memset((void*)Address, 0x00, (aNewCount - this->Count) * TypeSize);
			}
			this->Count = aNewCount;
			this->Handle = (stage_t**)nptr;
		}
		else {
			this->clear();
		}
		return false;
	}

	int stage_list::count() const {
		return this->Count;
	}

	void stage_list::clear() {
		free(this->Handle);
		this->zero_out();
	}

	void stage_list::zero_out() {
		this->Count = 0;
		this->Handle = NULL;
	}

}
