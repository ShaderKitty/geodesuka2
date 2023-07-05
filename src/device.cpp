#include <geodesuka/engine.h>
//#include <geodesuka/core/gcl/device.h>

#include <cstdlib>
#include <cstring>

#include <sstream>

#include <GLFW/glfw3.h>

namespace geodesuka::core::gcl {

	using namespace util;

	device::device(engine* aEngine, VkPhysicalDevice aPhysicalDevice) {

		this->ParentInstance = aEngine->handle();
		this->Handle = aPhysicalDevice;

		this->ExtensionCount = 0;
		this->Extension = NULL;

		this->QueueFamilyCount = 0;
		this->QueueFamilyProperty = NULL;
		this->QueueFamilyCapability = NULL;
		this->QueueFamilySupportCount = NULL;

		VkResult Result = VK_SUCCESS;

		// Gather all extensions.
		Result = vkEnumerateDeviceExtensionProperties(this->Handle, NULL, &this->ExtensionCount, NULL);

		// Gather all possible queues
		vkGetPhysicalDeviceQueueFamilyProperties(this->Handle, &this->QueueFamilyCount, NULL);

		// Failure condition, device cannot be used.
		if (this->QueueFamilyCount == 0) return;

		// Allocate memory is extensions exist.
		if (this->ExtensionCount > 0) {
			this->Extension = (VkExtensionProperties*)malloc(this->ExtensionCount * sizeof(VkExtensionProperties));
			if (this->Extension == NULL) return;
		}
		else {
			this->Extension = NULL;
		}

		// Allocate memory for queues.
		if (this->QueueFamilyCount > 0) {
			this->QueueFamilyProperty = (VkQueueFamilyProperties*)malloc(this->QueueFamilyCount * sizeof(VkQueueFamilyProperties));
			this->QueueFamilyCapability = (queue_family_capability*)malloc(this->QueueFamilyCount * sizeof(queue_family_capability));
			this->QueueFamilySupportCount = (uint32_t*)malloc(this->QueueFamilyCount * sizeof(uint32_t));
		}
		else {
			this->QueueFamilyProperty = NULL;
			this->QueueFamilyCapability = NULL;
			this->QueueFamilySupportCount = NULL;
		}

		// Create Temp Window
		// This section is neccessary to create dummy surface
		// to test for presentation support. This makes the assumption
		// that presentation support does not depend on the parameters
		// of the surface created.
		glfwWindowHint(GLFW_RESIZABLE,			GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED,			GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUSED,			GLFW_TRUE);
		glfwWindowHint(GLFW_AUTO_ICONIFY,		GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING,			GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED,			GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE,			GLFW_FALSE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR,	GLFW_FALSE);
		glfwWindowHint(GLFW_CENTER_CURSOR,		GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW,		GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API,			GLFW_NO_API);
		glfwWindowHint(GLFW_REFRESH_RATE,		GLFW_DONT_CARE);

		// Create OS window handle.
		GLFWwindow* lDummyWindow = glfwCreateWindow(640, 480, "Dummy Window", NULL, NULL);

		// Reset hints to insure no lingering parameters.
		glfwDefaultWindowHints();

		// Since this code only relies on an instance, maybe query in device class?
		VkSurfaceKHR lDummySurface = VK_NULL_HANDLE;
		Result = glfwCreateWindowSurface(aEngine->handle(), lDummyWindow, NULL, &lDummySurface);

		// Check needed objects are properly allocated, otherwise abort.
		if ((this->QueueFamilyProperty == NULL) || (this->QueueFamilyCapability == NULL) || (this->QueueFamilySupportCount == NULL) || (lDummyWindow == NULL) || (lDummySurface == VK_NULL_HANDLE)) {
			free(this->QueueFamilyProperty);
			this->QueueFamilyProperty = NULL;
			free(this->QueueFamilyCapability);
			this->QueueFamilyCapability = NULL;
			free(this->QueueFamilySupportCount);
			this->QueueFamilySupportCount = NULL;
			vkDestroySurfaceKHR(aEngine->handle(), lDummySurface, NULL);
			lDummySurface = VK_NULL_HANDLE;
			glfwDestroyWindow(lDummyWindow);
			lDummyWindow = NULL;
			return;
		}

		// Get extensions if there are any.
		if (this->Extension != NULL) {
			Result = vkEnumerateDeviceExtensionProperties(this->Handle, NULL, &this->ExtensionCount, this->Extension);
		}

		// Get Queue Family Information.
		vkGetPhysicalDeviceQueueFamilyProperties(this->Handle, &this->QueueFamilyCount, this->QueueFamilyProperty);

		for (uint32_t i = 0; i < this->QueueFamilyCount; i++) {
			this->QueueFamilySupportCount[i] = 0;

			this->QueueFamilyCapability[i].Support = 0;
			this->QueueFamilyCapability[i].Flags = this->QueueFamilyProperty[i].queueFlags;
			if (this->QueueFamilyProperty[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				this->QueueFamilyCapability[i].Support |= qfeo::TRANSFER;
				this->QueueFamilyCapability[i].isTransferSupported = true;
				this->QueueFamilySupportCount[i] += 1;
			}
			if (this->QueueFamilyProperty[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				this->QueueFamilyCapability[i].Support |= qfeo::COMPUTE;
				this->QueueFamilyCapability[i].isComputeSupported = true;
				this->QueueFamilySupportCount[i] += 1;
			}
			if (this->QueueFamilyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				this->QueueFamilyCapability[i].Support |= qfeo::GRAPHICS;
				this->QueueFamilyCapability[i].isGraphicsSupported = true;
				this->QueueFamilySupportCount[i] += 1;
			}

			VkBool32 PresentSupport;
			Result = vkGetPhysicalDeviceSurfaceSupportKHR(aPhysicalDevice, i, lDummySurface, &PresentSupport);
			if (PresentSupport == VK_TRUE) {
				this->QueueFamilyCapability[i].Support |= qfeo::PRESENT;
				this->QueueFamilyCapability[i].isPresentSupported = true;
				this->QueueFamilySupportCount[i] += 1;
			}
			else {
				this->QueueFamilyCapability[i].isPresentSupported = false;
			}

		}

		vkGetPhysicalDeviceProperties(this->Handle, &this->Properties);
		vkGetPhysicalDeviceFeatures(this->Handle, &this->Features);
		vkGetPhysicalDeviceMemoryProperties(this->Handle, &this->MemoryProperties);

		// Clear up Dummy stuff.
		vkDestroySurfaceKHR(aEngine->handle(), lDummySurface, NULL);
		lDummySurface = VK_NULL_HANDLE;
		glfwDestroyWindow(lDummyWindow);
		lDummyWindow = NULL;

		std::stringstream StringStream;
		StringStream << "\n\t" << "Queue Operations Supported: ";
		if (this->qfi(device::TRANSFER) != -1) {
			StringStream << "T";
		}
		if (this->qfi(device::COMPUTE) != -1) {
			StringStream << "C";
		}
		if (this->qfi(device::GRAPHICS) != -1) {
			StringStream << "G";
		}
		if (this->qfi(device::PRESENT) != -1) {
			StringStream << "P";
		}

		StringStream << "\n\t" << "API Version:\t" << Properties.apiVersion;
		StringStream << "\n\t" << "Driver Version:\t" << Properties.driverVersion;
		StringStream << "\n\t" << "Vendor ID:\t" << Properties.vendorID;
		StringStream << "\n\t" << "Device ID:\t" << Properties.deviceID;
		StringStream << "\n\t" << "Device Type:\t";
		switch (Properties.deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			StringStream << "Other";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			StringStream << "Integrated GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			StringStream << "Discrete GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			StringStream << "Virtual GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			StringStream << "CPU";
			break;
		default:
			StringStream << "Unknown";
			break;
		}
		StringStream << "\n\t" << "Device Name:\t" << Properties.deviceName;

		for (int i = 0; i < MemoryProperties.memoryHeapCount; i++) {
			StringStream << "\n\t" << "Heap[" << i << "] Size:\t" << MemoryProperties.memoryHeaps[i].size << " bytes";
			for (int j = 0; j < MemoryProperties.memoryTypeCount; j++) {
				if ((MemoryProperties.memoryTypes[j].heapIndex == i) && (MemoryProperties.memoryTypes[j].propertyFlags != 0)) {
					StringStream << "\n\t\t" << "Type[" << j << "]: | ";
					for (int k = 0; k < 9; k++) {
						switch (MemoryProperties.memoryTypes[j].propertyFlags & (1 << k)) {
						case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							StringStream << "Device Local";
							break;
						case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
							StringStream << "Host Visible";
							break;
						case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							StringStream << "Host Coherent";
							break;
						case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							StringStream << "Host Cached";
							break;
						case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							StringStream << "Lazily Allocated";
							break;
						case VK_MEMORY_PROPERTY_PROTECTED_BIT:
							StringStream << "Protected";
							break;
						case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
							StringStream << "Device Coherent (AMD)";
							break;
						case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
							StringStream << "Device Uncached (AMD)";
							break;
						case VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV:
							StringStream << "RDMA Capable (NV)";
							break;
						default:
							break;
						}
						if (MemoryProperties.memoryTypes[j].propertyFlags & (1 << k))
							StringStream << " | ";
					}
				}
			}
		}


		*aEngine << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "device", Properties.deviceName, StringStream.str().c_str());

	}

	device::~device() {
		free(this->QueueFamilyProperty);
		this->QueueFamilyProperty = NULL;
		free(this->QueueFamilyCapability);
		this->QueueFamilyCapability = NULL;
		free(this->QueueFamilySupportCount);
		this->QueueFamilySupportCount = NULL;
		free(this->Extension);
		this->Extension = NULL;
	}

	bool device::is_extension_list_supported(uint32_t aExtensionCount, const char** aExtensionList) const {
		// Checks if ExtensionList is a subset of existing extensions.
		bool isSupported = true;
		for (uint32_t i = 0; i < aExtensionCount; i++) {
			size_t L1 = strlen(aExtensionList[i]);
			bool isExtSupported = false;
			// Iterate through suppported extensions.
			for (uint32_t j = 0; j < this->ExtensionCount; j++) {
				size_t L2 = strlen(this->Extension[j].extensionName);
				if (L1 == L2) {
					// If support is found, the set isExtSupported.
					if (memcmp(aExtensionList[i], this->Extension[j].extensionName, L1 * sizeof(char)) == 0) {
						isExtSupported = true;
					}
				}
			}
			// Is equivalent to.
			// isSupported = (isExt1Supported) && (isExt2Supported) && ... (isExtNSupported);
			isSupported &= isExtSupported;
		}
		return isSupported;
	}

	VkPhysicalDeviceProperties device::get_properties() const {
		VkPhysicalDeviceProperties temp{};
		if (this->Handle != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceProperties(this->Handle, &temp);
		}
		return temp;
	}

	VkPhysicalDeviceFeatures device::get_features() const {
		VkPhysicalDeviceFeatures temp{};
		if (this->Handle != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceFeatures(this->Handle, &temp);
		}
		return temp;
	}

	VkPhysicalDeviceMemoryProperties device::get_memory_properties() const {
		VkPhysicalDeviceMemoryProperties temp;
		vkGetPhysicalDeviceMemoryProperties(this->Handle, &temp);
		return temp;
	}

	const VkExtensionProperties* device::get_extensions(uint32_t* aExtensionCount) const {
		*aExtensionCount = this->ExtensionCount;
		return this->Extension;
	}

	int device::get_memory_type_index(VkMemoryRequirements aMemoryRequirements, int aMemoryType) const {
		int MemoryTypeIndex = -1;

		//VkPhysicalDeviceMemoryProperties MemoryProperties = this->get_memory_properties();

		// Search for exact memory type index.
		for (uint32_t i = 0; i < this->MemoryProperties.memoryTypeCount; i++) {
			if (((aMemoryRequirements.memoryTypeBits & (1 << i)) >> i) && (this->MemoryProperties.memoryTypes[i].propertyFlags == aMemoryType)) {
				MemoryTypeIndex = i;
				break;
			}
		}


		// Search for approximate memory type index.
		if (MemoryTypeIndex == -1) {
			for (uint32_t i = 0; i < this->MemoryProperties.memoryTypeCount; i++) {
				if (((aMemoryRequirements.memoryTypeBits & (1 << i)) >> i) && ((this->MemoryProperties.memoryTypes[i].propertyFlags & aMemoryType) == aMemoryType)) {
					MemoryTypeIndex = i;
					break;
				}
			}
		}

		// return -1 if no types exist.
		return MemoryTypeIndex;
	}

	int device::get_memory_type(int aMemoryTypeIndex) {
		if (aMemoryTypeIndex < 0) return 0;
		return this->MemoryProperties.memoryTypes[aMemoryTypeIndex].propertyFlags;
	}

	const VkQueueFamilyProperties* device::get_queue_family_properties(uint32_t* aQueueFamilyCount) const {
		*aQueueFamilyCount = this->QueueFamilyCount;
		return this->QueueFamilyProperty;
	}

	const device::queue_family_capability* device::get_capability(uint32_t* aQueueFamilyCount) const {
		*aQueueFamilyCount = this->QueueFamilyCount;
		return this->QueueFamilyCapability;
	}

	VkSurfaceCapabilitiesKHR device::get_surface_capabilities(VkSurfaceKHR aSurface) {
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->Handle, aSurface, &SurfaceCapabilities);
		return SurfaceCapabilities;
	}

	std::vector<VkSurfaceFormatKHR> device::get_surface_format(VkSurfaceKHR aSurface) {
		VkResult Result = VK_SUCCESS;
		uint32_t VectorCount = 0;
		Result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->Handle, aSurface, &VectorCount, NULL);
		std::vector<VkSurfaceFormatKHR> SurfaceFormat(VectorCount);
		Result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->Handle, aSurface, &VectorCount, SurfaceFormat.data());
		return SurfaceFormat;
	}

	std::vector<VkPresentModeKHR> device::get_surface_present_mode(VkSurfaceKHR aSurface) {
		VkResult Result = VK_SUCCESS;
		uint32_t VectorCount = 0;
		Result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->Handle, aSurface, &VectorCount, NULL);
		std::vector<VkPresentModeKHR> PresentMode(VectorCount);
		Result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->Handle, aSurface, &VectorCount, PresentMode.data());
		return PresentMode;
	}

	//VkImageFormatProperties device::image_format_properties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) {
	//	vkGetPhysicalDeviceImageFormatProperties()
	//	return VkImageFormatProperties();
	//}

	bool device::available(unsigned int aQFS) const {
		for (uint32_t i = 0; i < this->QueueFamilyCount; i++) {
			if ((this->QueueFamilyCapability[i].Support & aQFS) == aQFS) {
				return true;
			}
		}
		return false;
	}

	int device::qfi(unsigned int aQFS) const {
		int temp = -1;
		uint32_t MinimumSupportCount = 10;
		for (uint32_t i = 0; i < this->QueueFamilyCount; i++) {
			if ((MinimumSupportCount > this->QueueFamilySupportCount[i]) && ((this->QueueFamilyCapability[i].Support & aQFS) == aQFS)) {
				MinimumSupportCount = this->QueueFamilySupportCount[i];
				temp = (int)i;
			}
		}
		return temp;
	}

	VkInstance device::inst() {
		return this->ParentInstance;
	}

	VkPhysicalDevice device::handle() {
		return this->Handle;
	}

}