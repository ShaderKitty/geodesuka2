#pragma once
#ifndef GEODESUKA_CORE_GCL_DEVICE_LIST_H
#define GEODESUKA_CORE_GCL_DEVICE_LIST_H

#include "device.h"

namespace geodesuka::core::gcl {

	class device_list {
	public:

		device_list();
		device_list(device* aDevice);
		device_list(int aDeviceCount, device** aDeviceList);
		device_list(const device_list& aInput);
		device_list(device_list&& aInput) noexcept;
		~device_list();

		device*& operator[](int aIndex);
		device* operator[](int aIndex) const;

		device_list& operator=(const device_list& aRhs);
		device_list& operator=(device_list&& aRhs) noexcept;

		device_list operator&(const device_list& aRhs) const;
		device_list operator|(const device_list& aRhs) const;
		device_list operator-(const device_list& aRhs) const;

		device_list operator&(device* aRhs) const;
		device_list operator|(device* aRhs) const;
		device_list operator-(device* aRhs) const;

		device_list& operator&=(const device_list& aRhs);
		device_list& operator|=(const device_list& aRhs);
		device_list& operator-=(const device_list& aRhs);

		device_list& operator&=(device* aRhs);
		device_list& operator|=(device* aRhs);
		device_list& operator-=(device* aRhs);

		device_list purify() const;

		bool exists(device* aDevice) const;

		bool resize(int aNewSize);

		void clear();

		int count() const;

		// Search for support options.
		
		device* get_device_type(VkPhysicalDeviceType aType) const;

	private:

		int Count;
		device** Handle;

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_DEVICE_LIST_H
