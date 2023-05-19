#include <geodesuka/core/gcl/context_list.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

namespace geodesuka::core::gcl {

	context_list::context_list() {}

	context_list::context_list(context* aContext) {
		this->Handle.push_back(aContext);
	}

	context_list::context_list(int aContextCount, context** aContextList) {
		this->Handle.resize(aContextCount);
		memcpy(this->Handle.data(), aContextList, aContextCount * sizeof(context*));
	}

	context_list::~context_list() {
		this->clear();
	}

	context*& context_list::operator[](int aIndex) {
		return this->Handle[aIndex];
	}

	context* context_list::operator[](int aIndex) const {
		return this->Handle[aIndex];
	}

	context_list context_list::operator&(const context_list& aRhs) const {
		context_list Intersection;
		context_list Left = this->purify();
		context_list Right = aRhs.purify();

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

	context_list context_list::operator|(const context_list& aRhs) const {
		context_list Union;
		context_list Left = this->purify();
		context_list Right = aRhs.purify();
		context_list Intersection = Left & Right;

		size_t LeftUniqueCount = Left.count() - Intersection.count();
		size_t RightUniqueCount = Right.count() - Intersection.count();
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

	context_list context_list::operator-(const context_list& aRhs) const {
		context_list Sublist;
		context_list Left = this->purify();
		context_list Right = aRhs.purify();
		context_list Intersection = *this & aRhs;

		if (Intersection.count() == 0) return *this;

		if (Intersection.count() == Left.count()) return Sublist;

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

	context_list context_list::operator&(context* aRhs) const {
		return (*this & context_list(aRhs));
	}

	context_list context_list::operator|(context* aRhs) const {
		return (*this | context_list(aRhs));
	}

	context_list context_list::operator-(context* aRhs) const {
		return (*this - context_list(aRhs));
	}

	context_list& context_list::operator&=(const context_list& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	context_list& context_list::operator|=(const context_list& aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	context_list& context_list::operator-=(const context_list& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	context_list& context_list::operator&=(context* aRhs) {
		return *this &= context_list(aRhs);
	}

	context_list& context_list::operator|=(context* aRhs) {
		return *this |= context_list(aRhs);
	}

	context_list& context_list::operator-=(context* aRhs) {
		return *this -= context_list(aRhs);
	}

	context_list context_list::purify() const {
		context_list Purified;

		size_t NullCount = 0;
		size_t RepeatCount = 0;
		size_t NewCount = 0;
		for (size_t i = 0; i < this->count(); i++) {
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
		for (size_t i = 0; i < this->count(); i++) {
			if (this->Handle[i] == nullptr) continue;
			bool AlreadyExists = false;
			for (size_t j = 0; j < NewCount; j++) {
				AlreadyExists |= (this->Handle[i] == Purified[j]);
			}
			if (AlreadyExists) continue;
			Purified[NewCount] = this->Handle[i];
			NewCount += 1;
		}

		return Purified;
	}

	bool context_list::exists(context* aContext) const {
		for (size_t i = 0; i < this->Handle.size(); i++) {
			if (this->Handle[i] == aContext) return true;
		}
		return false;
	}

	void context_list::resize(size_t aNewCount) {
		this->Handle.resize(aNewCount);
	}

	size_t context_list::count() const {
		return this->Handle.size();
	}

	void context_list::clear() {
		this->Handle.clear();
	}

}
