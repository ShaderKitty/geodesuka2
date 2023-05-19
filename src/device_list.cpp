#include <geodesuka/core/gcl/device_list.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace geodesuka::core::gcl {

	device_list::device_list() {
		this->zero_out();
	}

	device_list::device_list(device* aDevice) {
		this->zero_out();
		assert(!this->resize(1));
		this->Handle[0] = aDevice;
	}

	device_list::device_list(int aDeviceCount, device** aDeviceList) {
		this->zero_out();
		assert(!this->resize(aDeviceCount));
		memcpy(this->Handle, aDeviceList, aDeviceCount * sizeof(device*));
	}

	device_list::device_list(const device_list& aInput) {
		this->zero_out();
		assert(!this->resize(aInput.Count));
		memcpy(this->Handle, aInput.Handle, aInput.Count * sizeof(device*));
	}

	device_list::device_list(device_list&& aInput) noexcept {
		this->Count = aInput.Count;
		this->Handle = aInput.Handle;
		aInput.zero_out();
	}

	device_list::~device_list() {
		this->clear();
	}

	device*& device_list::operator[](int aIndex){
		return this->Handle[aIndex];
	}

	device* device_list::operator[](int aIndex) const {
		return this->Handle[aIndex];
	}

	device_list& device_list::operator=(const device_list& aRhs) {
		if (this == &aRhs) return *this;
		assert(!this->resize(aRhs.Count));
		memcpy(this->Handle, aRhs.Handle, aRhs.Count * sizeof(device*));
		return *this;
	}

	device_list& device_list::operator=(device_list&& aRhs) noexcept {
		this->clear();
		this->Count = aRhs.Count;
		this->Handle = aRhs.Handle;
		aRhs.zero_out();
		return *this;
	}

	device_list device_list::operator&(const device_list& aRhs) const {
		device_list Intersection;
		device_list Left = this->purify();
		device_list Right = aRhs.purify();

		int k = 0;
		for (int i = 0; i < Left.Count; i++) {
			if (Right.exists(Left[i])) {
				k += 1;
			}
		}

		if (k == 0) return Intersection;
		assert(!Intersection.resize(k));

		k = 0;
		for (int i = 0; i < Left.Count; i++) {
			if (Right.exists(Left[i])) {
				Intersection[k] = Left[i];
				k += 1;
			}
		}

		return Intersection;
	}

	device_list device_list::operator|(const device_list& aRhs) const {
		device_list Union;
		device_list Left = this->purify();
		device_list Right = aRhs.purify();
		device_list Intersection = Left & Right;

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

	device_list device_list::operator-(const device_list& aRhs) const {
		device_list Sublist;
		device_list Left = this->purify();
		device_list Right = aRhs.purify();
		device_list Intersection = *this & aRhs;

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

	device_list device_list::operator&(device* aRhs) const {
		return (*this & device_list(aRhs));
	}

	device_list device_list::operator|(device* aRhs) const {
		return (*this | device_list(aRhs));
	}

	device_list device_list::operator-(device* aRhs) const {
		return (*this - device_list(aRhs));
	}

	device_list& device_list::operator&=(const device_list& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	device_list& device_list::operator|=(const device_list& aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	device_list& device_list::operator-=(const device_list& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	device_list& device_list::operator&=(device* aRhs) {
		return *this &= device_list(aRhs);
	}

	device_list& device_list::operator|=(device* aRhs) {
		return *this |= device_list(aRhs);
	}

	device_list& device_list::operator-=(device* aRhs) {
		return *this -= device_list(aRhs);
	}

	device_list device_list::purify() const {
		device_list Purified;

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

	bool device_list::exists(device* aDevice) const {
		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i] == aDevice) return true;
		}
		return false;
	}

	bool device_list::resize(int aNewCount) {
		if (this->Count == aNewCount) return false;
		if (aNewCount > 0) {
			size_t TypeSize = sizeof(device*);
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
			this->Handle = (device**)nptr;
			return false;
		}
		else {
			this->clear();
			return false;
		}
	}

	void device_list::clear() {
		free(this->Handle);
		this->zero_out();
	}

	int device_list::count() const {
		return this->Count;
	}

	device* device_list::get_device_type(vk_physical_device_type aType) const {
		// Search for device that is discrete gpu.
		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i]->get_properties().deviceType == aType) {
				return this->Handle[i];
			}
		}
		return nullptr;
	}

	void device_list::zero_out() {
		this->Count = 0;
		this->Handle = NULL;
	}

}