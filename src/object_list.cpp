#include <geodesuka/core/object_list.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

//#include <geodesuka/core/stage_t.h>

namespace geodesuka::core {

	object_list::object_list() {}

	object_list::object_list(int aObjectCount) {
		this->Handle.resize(aObjectCount);
	}

	object_list::object_list(object_t* aObject) {
		this->Handle.push_back(aObject);
	}

	object_list::object_list(int aObjectCount, object_t** aObjectList) {
		this->Handle.resize(aObjectCount);
		memcpy(this->Handle.data(), aObjectList, aObjectCount * sizeof(object_t*));
	}

	object_list::~object_list() {
		this->clear();
	}

	object_t* object_list::operator[](int aIndex) const {
		return this->Handle[aIndex];
	}

	object_t*& object_list::operator[](int aIndex) {
		return this->Handle[aIndex];
	}

	// Gathers the intersection between two sets, and nothing else.
	object_list object_list::operator&(const object_list& aRhs) const {
		object_list Intersection;
		object_list Left = this->purify();
		object_list Right = aRhs.purify();

		int k = 0;
		for (int i = 0; i < Left.count(); i++) {
			if (Right.exists(Left[i])) {
				k += 1;
			}
		}

		if (k == 0) return Intersection;
		Intersection.resize(k);

		k = 0;
		for (int i = 0; i < Left.count(); i++) {
			if (Right.exists(Left[i])) {
				Intersection[k] = Left[i];
				k += 1;
			}
		}

		return Intersection;
	}

	object_list object_list::operator|(const object_list& aRhs) const {
		object_list Union;
		object_list Left = this->purify();
		object_list Right = aRhs.purify();
		object_list Intersection = Left & Right;

		int LeftUniqueCount = Left.count() - Intersection.count();
		int RightUniqueCount = Right.count() - Intersection.count();
		Union.resize(LeftUniqueCount + Intersection.count() + RightUniqueCount);

		int k = 0;
		for (int i = 0; i < Left.count(); i++) {
			if (!Intersection.exists(Left[i])) {
				Union[k] = Left[i];
				k += 1;
			}
		}

		for (int i = 0; i < Intersection.count(); i++) {
			Union[k] = Intersection[i];
			k += 1;
		}

		for (int i = 0; i < Right.count(); i++) {
			if (!Intersection.exists(Right[i])) {
				Union[k] = Right[i];
				k += 1;
			}
		}

		return Union;
	}

	object_list object_list::operator-(const object_list& aRhs) const {
		object_list Sublist;
		object_list Left = this->purify();
		object_list Right = aRhs.purify();
		object_list Intersection = *this & aRhs;

		if (Intersection.count() == 0) return *this;

		if (Left.count() == Intersection.count()) return Sublist;

		Sublist.resize(Left.count() - Intersection.count());

		int k = 0;
		for (int i = 0; i < Left.count(); i++) {
			if (!Intersection.exists(Left[i])) {
				Sublist[k] = Left[i];
				k += 1;
			}
		}

		return Sublist;
	}

	object_list object_list::operator&(object_t* aRhs) const {
		return (*this & object_list(aRhs));
	}

	object_list object_list::operator|(object_t* aRhs) const {
		return (*this | object_list(aRhs));
	}

	object_list object_list::operator-(object_t* aRhs) const {
		return (*this - object_list(aRhs));
	}

	object_list& object_list::operator&=(const object_list& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	object_list& object_list::operator|=(const object_list& aRhs) {
		object_list R = aRhs.purify();
		*this = *this | R;
		return *this;
	}

	object_list& object_list::operator-=(const object_list& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	object_list& object_list::operator&=(object_t* aRhs) {
		return *this &= object_list(aRhs);
	}

	object_list& object_list::operator|=(object_t* aRhs) {
		return *this |= object_list(aRhs);
	}

	object_list& object_list::operator-=(object_t* aRhs) {
		return *this -= object_list(aRhs);
	}

	object_list object_list::purify() const {
		object_list Purified;

		int NullCount = 0;
		int RepeatCount = 0;
		int NewCount = 0;
		for (int i = 0; i < this->count(); i++) {
			if (this->Handle[i] == nullptr) {
				NullCount += 1;
				continue;
			}
			for (int j = i + 1; j < this->count(); j++) {
				if (this->Handle[i] == this->Handle[j]) {
					RepeatCount += 1;
				}
			}
		}

		NewCount = this->count() - NullCount - RepeatCount;
		if (NewCount == this->count()) return *this;
		Purified.resize(NewCount);

		NewCount = 0;
		for (int i = 0; i < this->count(); i++) {
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

	bool object_list::exists(object_t* aObject) const {
		for (int i = 0; i < this->count(); i++) {
			if (this->Handle[i] == aObject) {
				return true;
			}
		}
		return false;
	}

	void object_list::resize(size_t aNewSize) {
		this->Handle.resize(aNewSize);
	}

	int object_list::count() const {
		return this->Handle.size();
	}

	void object_list::clear() {
		this->Handle.clear();
	}

	// Add access to parent stage.
	object_list object_list::gather_by(stage_t* aStage) const {
		object_list List;

		size_t StageCount = 0;
		for (size_t i = 0; i < this->Handle.size(); i++){
			if (this->Handle[i]->Stage == aStage) {
				StageCount += 1;
			}
		}

		if (StageCount == 0) return List;
		List.resize(StageCount);

		size_t j = 0;
		for (size_t i = 0; i < this->Handle.size(); i++) {
			if (this->Handle[i]->Stage == aStage) {
				List[j] = this->Handle[i];
				j += 1;
			}
		}

		return List;
	}

	object_t* object_list::find_by_name(const char* aName) const {
		for (int i = 0; i < count(); i++) {
			if (Handle[i]->Name == aName) {
				return Handle[i];
			}
		}
		return nullptr;
	}

}
