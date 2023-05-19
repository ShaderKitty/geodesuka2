#include <geodesuka/engine.h>
#include <geodesuka/core/gcl/context.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
//#include <assert.h>

#include <vector>

#include <GLFW/glfw3.h>

namespace geodesuka::core::gcl {

	using namespace util;

	context::context(engine* aEngine, device* aDevice, uint32_t aLayerCount, const char** aLayerList, uint32_t aExtensionCount, const char** aExtensionList) {

		// List of operations.
		// Check for support of required extensions requested i
		// 1: Check for extensions.
		// 2: Queue Create Info.
		// 3: Create Logical Device.

		this->zero_out();

		if ((aEngine == nullptr) || (aDevice == nullptr)) return;

		this->Engine = aEngine;
		this->Device = aDevice;

		EnableProcessing.store(false);

		vk_result Result = VK_SUCCESS;

		// If -1, then the option is not supported by the device.
		this->QFI[0] = this->Device->qfi(device::qfeo::TRANSFER);
		this->QFI[1] = this->Device->qfi(device::qfeo::COMPUTE);
		this->QFI[2] = this->Device->qfi(device::qfeo::GRAPHICS);
		this->QFI[3] = this->Device->qfi(device::qfeo::GRAPHICS_AND_COMPUTE);
		this->QFI[4] = this->Device->qfi(device::qfeo::PRESENT);

		// Creates a list of unique QFIs.
		for (int i = 0; i < 5; i++) {
			// Operation Not Supported.
			if (this->QFI[i] == -1) continue;
			if (this->UQFICount == 0) {
				// Add first element.
				this->UQFI[this->UQFICount] = this->QFI[i];
				this->UQFICount += 1;
			}
			else {
				// Check if element is already present.
				bool AlreadyPresent = false;
				for (int j = 0; j < this->UQFICount; j++) {
					AlreadyPresent |= (this->QFI[i] == this->UQFI[j]);
				}
				if (!AlreadyPresent) {
					this->UQFI[this->UQFICount] = this->QFI[i];
					this->UQFICount += 1;
				}
			}
		}

		// With UQFI found, generate queues for selected indices.
		uint32_t QFPCount = 0;
		const vk_queue_family_properties* QFP = this->Device->get_queue_family_properties(&QFPCount);

		for (int i = 0; i < UQFICount; i++) {
			Queue.emplace_back(QFP[UQFI[i]]);
		}

		for (size_t i = 0; i < Queue.size(); i++) {
			vk_device_queue_create_info NewQueueCreateInfo{};
			NewQueueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			NewQueueCreateInfo.pNext				= NULL;
			NewQueueCreateInfo.flags				= 0;
			NewQueueCreateInfo.queueFamilyIndex		= UQFI[i];
			NewQueueCreateInfo.queueCount			= Queue[i].Priority.size();
			NewQueueCreateInfo.pQueuePriorities		= Queue[i].Priority.data();
			QueueCreateInfo.push_back(NewQueueCreateInfo);
		}

		// Load VkDevice Create Info.
		CreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		CreateInfo.pNext						= NULL;
		CreateInfo.flags						= 0;
		CreateInfo.queueCreateInfoCount			= QueueCreateInfo.size();
		CreateInfo.pQueueCreateInfos			= QueueCreateInfo.data();
		CreateInfo.enabledLayerCount			= aLayerCount;
		CreateInfo.ppEnabledLayerNames			= aLayerList;
		if (Device->is_extension_list_supported(aExtensionCount, aExtensionList)) {
			CreateInfo.enabledExtensionCount		= aExtensionCount;
			CreateInfo.ppEnabledExtensionNames		= aExtensionList;
		}
		else {
			CreateInfo.enabledExtensionCount		= 0;
			CreateInfo.ppEnabledExtensionNames		= NULL;
		}
		CreateInfo.pEnabledFeatures				= &Device->Features;

		// Create Vulkan Logical Device.
		Result = vkCreateDevice(Device->handle(), &this->CreateInfo, NULL, &this->Handle);
		if (Result == VK_SUCCESS) {
			*Engine << log::message(log::INFO, log::SUCCESS, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Successful!");
		}
		else {
			*Engine << log::message(Result, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Failed!");
		}

		// Get all create queues
		for (size_t i = 0; i < Queue.size(); i++) {
			for (size_t j = 0; j < Queue[i].count(); j++) {
				vkGetDeviceQueue(Handle, UQFI[i], j, &Queue[i][j]);
			}
		}

		this->CommandPool[0] = new command_pool(this, 0, device::qfeo::TRANSFER);
		this->CommandPool[1] = new command_pool(this, 0, device::qfeo::COMPUTE);
		this->CommandPool[2] = new command_pool(this, 0, device::qfeo::GRAPHICS);
		this->CommandPool[3] = new command_pool(this, 0, device::qfeo::GRAPHICS_AND_COMPUTE);

		vk_fence_create_info FenceCreateInfo{};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = NULL;
		FenceCreateInfo.flags = 0;

		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[0]);
		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[1]);
		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[2]);

		// Suspend all other shared threads.
		if (this->Engine->State == engine::state::RUNNING) {
			this->Engine->ThreadController.suspend_all();
		}

		this->Engine->Context |= this;

		// Resume all engine threads.
		if (this->Engine->State == engine::state::RUNNING) {
			this->Engine->ThreadController.resume_all();
		}

		this->EnableProcessing.store(true);

		// -------------------------------------------------- //

		/*
		this->Queue = (queue**)malloc(this->UQFICount * sizeof(queue*));
		if (this->Queue == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Context Creation", log::GEODESUKA, "context", "", "Memory Allocation Failure: this->Queue");
		}

		this->QueuePriority = (float**)malloc(this->UQFICount * sizeof(float*));
		if (this->QueuePriority == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Context Creation", log::GEODESUKA, "context", "", "Memory Allocation Failure: this->QueuePriority");
		}

		this->QueueCreateInfo = (vk_device_queue_create_info*)malloc(this->UQFICount * sizeof(vk_device_queue_create_info));
		if (this->QueueCreateInfo == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Context Creation", log::GEODESUKA, "context", "", "Memory Allocation Failure: this->QueuePriority");
		}

		for (int i = 0; i < this->UQFICount; i++) {
			// Get total number of queues per UQFI.
			this->UQFIQueueCount[i] = QFP[this->UQFI[i]].queueCount;

			// Create Container Class.
			this->Queue[i] = new queue[this->UQFIQueueCount[i]];
			if (this->Queue[i] == NULL) {
				throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Context Creation", log::GEODESUKA, "context", "", "Memory Allocation Failure: this->Queue[i]");
			}

			// allocate memory for individual queue priorities.
			this->QueuePriority[i] = (float*)malloc(this->UQFIQueueCount[i] * sizeof(float));
			if (this->QueuePriority[i] == NULL) {
				throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Context Creation", log::GEODESUKA, "context", "", "Memory Allocation Failure: this->QueuePriority[i]");
			}

			// Set queue priorities.
			for (int j = 0; j < this->UQFIQueueCount[i]; j++) {
				this->QueuePriority[i][j] = 1.0f;// / ((float)(QFP[this->UQFI[i]].queueCount));
			}

			// Fill out Queue Creation Info.
			this->QueueCreateInfo[i].sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			this->QueueCreateInfo[i].pNext				= NULL;
			this->QueueCreateInfo[i].flags				= 0;
			this->QueueCreateInfo[i].queueFamilyIndex	= this->UQFI[i];
			this->QueueCreateInfo[i].queueCount			= this->UQFIQueueCount[i];
			this->QueueCreateInfo[i].pQueuePriorities	= this->QueuePriority[i];
		}

		// Load VkDevice Create Info.
		this->CreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		this->CreateInfo.pNext						= NULL;
		this->CreateInfo.flags						= 0;
		this->CreateInfo.queueCreateInfoCount		= this->UQFICount;
		this->CreateInfo.pQueueCreateInfos			= this->QueueCreateInfo;
		this->CreateInfo.enabledLayerCount			= aLayerCount;
		this->CreateInfo.ppEnabledLayerNames		= aLayerList;
		if (this->Device->is_extension_list_supported(aExtensionCount, aExtensionList)) {
			this->CreateInfo.enabledExtensionCount			= aExtensionCount;
			this->CreateInfo.ppEnabledExtensionNames		= aExtensionList;
		}
		else {
			this->CreateInfo.enabledExtensionCount			= 0;
			this->CreateInfo.ppEnabledExtensionNames		= NULL;
		}
		this->CreateInfo.pEnabledFeatures			= &this->Device->Features;

		// Create Vulkan Logical Device.
		Result = vkCreateDevice(this->Device->handle(), &this->CreateInfo, NULL, &this->Handle);
		if (Result == VK_SUCCESS) {
			*Engine << log::message(log::INFO, log::SUCCESS, "Context Creation", log::GEODESUKA, "context", "", "Device Context Creation Successful!");
		} else {
			*Engine << log::message(Result, "Context Creation", log::GEODESUKA, "context", "", "Device Context Creation Failed!");
		}

		// Get Queues created queues.
		for (int i = 0; i < this->UQFICount; i++) {
			for (int j = 0; j < this->UQFIQueueCount[i]; j++) {
				vkGetDeviceQueue(this->Handle, this->UQFI[i], j, &this->Queue[i][j].Handle);
			}
		}

		this->CommandPool[0] = new command_pool(this, 0, device::qfeo::TRANSFER);
		this->CommandPool[1] = new command_pool(this, 0, device::qfeo::COMPUTE);
		this->CommandPool[2] = new command_pool(this, 0, device::qfeo::GRAPHICS);
		this->CommandPool[3] = new command_pool(this, 0, device::qfeo::GRAPHICS_AND_COMPUTE);

		vk_fence_create_info FenceCreateInfo{};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = NULL;
		FenceCreateInfo.flags = 0;

		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[0]);
		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[1]);
		Result = vkCreateFence(Handle, &FenceCreateInfo, NULL, &ExecutionFence[2]);

		// Suspend all other shared threads.
		if (this->Engine->State == engine::state::RUNNING) {
			this->Engine->ThreadController.suspend_all();
		}

		this->Engine->Context |= this;

		// Resume all engine threads.
		if (this->Engine->State == engine::state::RUNNING) {
			this->Engine->ThreadController.resume_all();
		}

		this->EnableProcessing.store(true);
		*/
	}

	context::~context() {
		// lock so context can be safely removed from engine instance.
		// If engine is in destruction state, do not attempt to remove from engine.
		this->EnableProcessing.store(false);

		if (this->Engine->State != engine::state::DESTRUCTION) {
			// Suspend all other shared threads.
			if (this->Engine->State == engine::state::RUNNING) {
				this->Engine->ThreadController.suspend_all();
			}

			this->Engine->Context -= this;

			// Resume all engine threads.
			if (this->Engine->State == engine::state::RUNNING) {
				this->Engine->ThreadController.resume_all();
			}
		}

		vkDestroyFence(Handle, ExecutionFence[0], NULL);
		vkDestroyFence(Handle, ExecutionFence[1], NULL);
		vkDestroyFence(Handle, ExecutionFence[2], NULL);
		delete this->CommandPool[0];
		delete this->CommandPool[1];
		delete this->CommandPool[2];
		delete this->CommandPool[3];
		vkDestroyDevice(this->Handle, NULL);

		this->zero_out();
	}

	void context::operator<<(const log::message aNewLogMessage) {
		*Engine << aNewLogMessage;
	}

	int context::qfi(device::qfeo aQFEO) {
		switch (aQFEO) {
		case device::qfeo::TRANSFER: 
			return this->QFI[0];
		case device::qfeo::COMPUTE: 
			return this->QFI[1];
		case device::qfeo::GRAPHICS: 
			return this->QFI[2];
		case device::qfeo::GRAPHICS_AND_COMPUTE: 
			return this->QFI[3];
		case device::qfeo::PRESENT: 
			return this->QFI[4];
		default: 
			return -1;
		}
	}

	bool context::available(device::qfeo aQFEO) {
		return (this->qfi(aQFEO) != -1);
	}

	vk_command_buffer context::create_command_buffer(device::qfeo aQueueFamilySupportOption, vk_command_buffer_level aLevel) {
		switch (aQueueFamilySupportOption) {
		case geodesuka::core::gcl::device::TRANSFER:
			return this->CommandPool[0]->allocate(aLevel);
		case geodesuka::core::gcl::device::COMPUTE:
			return this->CommandPool[1]->allocate(aLevel);
		case geodesuka::core::gcl::device::GRAPHICS:
			return this->CommandPool[2]->allocate(aLevel);
		case geodesuka::core::gcl::device::GRAPHICS_AND_COMPUTE:
			return this->CommandPool[3]->allocate(aLevel);
		default:
			return VK_NULL_HANDLE;
		}
	}

	command_list context::create_command_list(device::qfeo aQueueFamilySupportOption, vk_command_buffer_level aLevel, uint32_t aCommandBufferCount) {
		command_list List;
		switch (aQueueFamilySupportOption) {
		case geodesuka::core::gcl::device::TRANSFER:
			List = this->CommandPool[0]->allocate(aLevel, aCommandBufferCount);
			break;
		case geodesuka::core::gcl::device::COMPUTE:
			List = this->CommandPool[1]->allocate(aLevel, aCommandBufferCount);
			break;
		case geodesuka::core::gcl::device::GRAPHICS:
			List = this->CommandPool[2]->allocate(aLevel, aCommandBufferCount);
			break;
		case geodesuka::core::gcl::device::GRAPHICS_AND_COMPUTE:
			List = this->CommandPool[3]->allocate(aLevel, aCommandBufferCount);
			break;
		default:
			break;
		}
		return List;
	}

	bool context::destroy_command_buffer(device::qfeo aQueueFamilySupportOption, vk_command_buffer aCommandBuffer) {
		switch (aQueueFamilySupportOption) {
		case geodesuka::core::gcl::device::TRANSFER:
			this->CommandPool[0]->release(aCommandBuffer);
			break;
		case geodesuka::core::gcl::device::COMPUTE:
			this->CommandPool[1]->release(aCommandBuffer);
			break;
		case geodesuka::core::gcl::device::GRAPHICS:
			this->CommandPool[2]->release(aCommandBuffer);
			break;
		case geodesuka::core::gcl::device::GRAPHICS_AND_COMPUTE:
			this->CommandPool[3]->release(aCommandBuffer);
			break;
		default:
			break;
		}
		return false;
	}

	bool context::destroy_command_list(device::qfeo aQueueFamilySupportOption, command_list& aCommandList) {
		switch (aQueueFamilySupportOption) {
		case geodesuka::core::gcl::device::TRANSFER:
			this->CommandPool[0]->release(aCommandList);
			break;
		case geodesuka::core::gcl::device::COMPUTE:
			this->CommandPool[1]->release(aCommandList);
			break;
		case geodesuka::core::gcl::device::GRAPHICS:
			this->CommandPool[2]->release(aCommandList);
			break;
		case geodesuka::core::gcl::device::GRAPHICS_AND_COMPUTE:
			this->CommandPool[3]->release(aCommandList);
			break;
		default:
			break;
		}
		return false;
	}

	vk_semaphore context::create_semaphore() {
		vk_result Result = VK_SUCCESS;
		vk_semaphore Semaphore = VK_NULL_HANDLE;
		Result = this->create_semaphore(1, &Semaphore);
		return Semaphore;
	}

	vk_result context::create_semaphore(int aSemaphoreCount, vk_semaphore* aSemaphoreList) {
		vk_result Result = VK_SUCCESS;
		vk_semaphore_create_info CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0;
		for (int i = 0; i < aSemaphoreCount; i++){
			Result = vkCreateSemaphore(this->Handle, &CreateInfo, NULL, &aSemaphoreList[i]);
		}
		return Result;
	}

	void context::destroy_semaphore(vk_semaphore& aSemaphore) {
		this->destroy_semaphore(1, &aSemaphore);
	}

	void context::destroy_semaphore(int aSemaphoreCount, vk_semaphore* aSemaphoreList) {
		for (int i = 0; i < aSemaphoreCount; i++){
			if (aSemaphoreList[i] != VK_NULL_HANDLE){
				vkDestroySemaphore(this->Handle, aSemaphoreList[i], NULL);
				aSemaphoreList[i] = VK_NULL_HANDLE;
			}
		}
	}

	vk_fence context::create_fence() {
		vk_result Result = VK_SUCCESS;
		vk_fence Fence = VK_NULL_HANDLE;
		Result = this->create_fence((vk_fence_create_flags)0, 1, &Fence);
		return Fence;
	}

	vk_fence context::create_fence(vk_fence_create_flags aFenceCreateFlags) {
		vk_result Result = VK_SUCCESS;
		vk_fence Fence = VK_NULL_HANDLE;
		Result = this->create_fence(aFenceCreateFlags, 1, &Fence);
		return Fence;
	}

	vk_result context::create_fence(int aFenceCount, vk_fence* aFenceList) {
		return this->create_fence((vk_fence_create_flags)0, aFenceCount, aFenceList);
	}

	vk_result context::create_fence(vk_fence_create_flags aFenceCreateFlags, int aFenceCount, vk_fence* aFenceList) {
		vk_result Result = VK_SUCCESS;
		vk_fence_create_info CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = aFenceCreateFlags;
		for (int i = 0; i < aFenceCount; i++){
			vkCreateFence(this->Handle, &CreateInfo, NULL, &aFenceList[i]);
		}
		return Result;
	}

	void context::destroy_fence(vk_fence& aFence) {
		this->destroy_fence(1, &aFence);
	}

	void context::destroy_fence(int aFenceCount, vk_fence* aFenceList) {
		for (int i = 0; i < aFenceCount; i++){
			if (aFenceList[i] != VK_NULL_HANDLE){
				vkDestroyFence(this->Handle, aFenceList[i], NULL);
				aFenceList[i] = VK_NULL_HANDLE;
			}
		}
	}

	vk_result context::execute(device::qfeo aQFEO, vk_command_buffer aCommandBuffer, vk_fence aFence) {
		vk_submit_info SubmitInfo{};
		SubmitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.pNext				= NULL;
		SubmitInfo.waitSemaphoreCount	= 0;
		SubmitInfo.pWaitSemaphores		= NULL;
		SubmitInfo.pWaitDstStageMask	= NULL;
		SubmitInfo.commandBufferCount	= 1;
		SubmitInfo.pCommandBuffers		= &aCommandBuffer;
		SubmitInfo.signalSemaphoreCount	= 0;
		SubmitInfo.pSignalSemaphores	= NULL;
		std::vector<vk_submit_info> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute(aQFEO, Submission, aFence);
	}

	vk_result context::execute(device::qfeo aQFEO, const command_list& aCommandList, vk_fence aFence) {
		vk_submit_info SubmitInfo = aCommandList.build();
		std::vector<vk_submit_info> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute(aQFEO, Submission, aFence);
	}

	vk_result context::execute(device::qfeo aQFS, const std::vector<gcl::command_list>& aCommandBatch, vk_fence aFence) {
		std::vector<vk_submit_info> SubmissionList(aCommandBatch.size());
		for (size_t i = 0; i < aCommandBatch.size(); i++) {
			SubmissionList[i] = aCommandBatch[i].build();
		}
		return this->execute(aQFS, SubmissionList, aFence);
	}

	vk_result context::execute(device::qfeo aQFEO, const std::vector<vk_submit_info>& aSubmissionList, vk_fence aFence) {
		vk_result Result = VK_SUCCESS;
		gcl::command_batch CommandBatch((uint32_t)aSubmissionList.size(), aSubmissionList.data());
		return this->execute(aQFEO, CommandBatch, aFence);
	}

	vk_result context::execute(const std::vector<vk_present_info_khr>& aPresentationList, vk_fence aFence) {
		vk_result Result = VK_SUCCESS;
		gcl::command_batch CommandBatch((uint32_t)aPresentationList.size(), aPresentationList.data());
		return this->execute(device::qfeo::PRESENT, CommandBatch, aFence);
	}

	vk_result context::execute(device::qfeo aQFEO, const command_batch& aCommandBatch, vk_fence aFence) {
		vk_result Result = VK_SUCCESS;

		int i = this->qfi_to_i(aQFEO);
		int j = 0;

		// Queue Operation Not supported.
		if (i == -1) return VK_ERROR_FEATURE_NOT_PRESENT;

		while (true) {
			if (Queue[i].Mutex[j].try_lock()) {
				switch (aQFEO) {
				case device::TRANSFER:
				case device::COMPUTE:
				case device::GRAPHICS:
				case device::GRAPHICS_AND_COMPUTE:
					Result = vkQueueSubmit(Queue[i][j], aCommandBatch.Submission.size(), aCommandBatch.Submission.data(), aFence);
					break;
				case device::PRESENT:
					for (size_t k = 0; k < aCommandBatch.Presentation.size(); k++) {
						Result = vkQueuePresentKHR(Queue[i][j], &aCommandBatch.Presentation[k]);
					}
					break;
				default:
					Result = VK_ERROR_FEATURE_NOT_PRESENT;
					break;
				}
				Queue[i].Mutex[j].unlock();
				break;
			}
			j = ((j == (Queue[i].count() - 1)) ? 0 : (j + 1));
		}
		return Result;
	}


	vk_result context::wait(vk_fence aFence, vk_bool_32 aWaitOnAll) {
		return this->wait(1, &aFence, aWaitOnAll);
	}

	vk_result context::wait(uint32_t aFenceCount, vk_fence* aFenceList, vk_bool_32 aWaitOnAll) {
		return vkWaitForFences(this->Handle, aFenceCount, aFenceList, aWaitOnAll, UINT64_MAX);
	}

	vk_memory_requirements context::get_buffer_memory_requirements(vk_buffer aBufferHandle) {
		vk_memory_requirements MemoryRequirements;
		vkGetBufferMemoryRequirements(this->Handle, aBufferHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	vk_memory_requirements context::get_image_memory_requirements(vk_image aImageHandle) {
		vk_memory_requirements MemoryRequirements;
		vkGetImageMemoryRequirements(this->Handle, aImageHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	engine* context::parent_engine() {
		return this->Engine;
	}

	device* context::parent_device() {
		return this->Device;
	}

	VkInstance context::inst() {
		return this->Engine->handle();
	}

	device* context::parent() {
		return this->Device;
	}

	VkDevice context::handle() {
		return this->Handle;
	}

	context::queue_family::queue_family(vk_queue_family_properties aProperties) {
		Mutex = new std::mutex[aProperties.queueCount];
		for (uint32_t i = 0; i < aProperties.queueCount; i++) {
			this->Priority.push_back(1.0f);
			this->Handle.push_back(VK_NULL_HANDLE);
		}
	}

	context::queue_family::queue_family(const queue_family& aInput) {
		*this = aInput;
	}

	context::queue_family::queue_family(queue_family&& aInput) noexcept {
		this->Mutex		= aInput.Mutex;
		aInput.Mutex	= nullptr;
		this->Priority	= aInput.Priority;
		this->Handle	= aInput.Handle;
	}

	context::queue_family::~queue_family() {
		delete[] this->Mutex;
		this->Mutex = nullptr;
	}

	vk_queue& context::queue_family::operator[](size_t aIndex) {
		return this->Handle[aIndex];
	}

	context::queue_family& context::queue_family::operator=(const queue_family& aRhs) {
		if (this == &aRhs) return *this;
		delete[] this->Mutex;
		this->Mutex = new std::mutex[aRhs.Handle.size()];
		this->Priority = aRhs.Priority;
		this->Handle = aRhs.Handle;
		return *this;
	}

	context::queue_family& context::queue_family::operator=(queue_family&& aRhs) noexcept {
		this->Mutex		= aRhs.Mutex;
		aRhs.Mutex		= nullptr;
		this->Priority	= aRhs.Priority;
		this->Handle	= aRhs.Handle;
		return *this;
	}

	size_t context::queue_family::count() const {
		return this->Handle.size();
	}

	int context::qfi_to_i(device::qfeo aQFEO) {
		for (int i = 0; i < UQFICount; i++) {
			if (this->qfi(aQFEO) == UQFI[i]) {
				return i;
			}
		}
		return -1;
	}

	void context::zero_out() {
		this->EnableProcessing.store(false);
		this->ExecutionFence[0] = VK_NULL_HANDLE;
		this->ExecutionFence[1] = VK_NULL_HANDLE;
		this->ExecutionFence[2] = VK_NULL_HANDLE;
		this->Engine = nullptr;
		this->Device = nullptr;
		this->UQFICount = 0;
		for (int i = 0; i < 5; i++) {
			this->QFI[i] = -1;
			this->UQFI[i] = -1;
			//this->UQFIQueueCount[i] = 0;
		}
		//this->QueuePriority = NULL;
		//this->QueueCreateInfo = NULL;
		this->Handle = VK_NULL_HANDLE;
		for (int i = 0; i < 4; i++) {
			this->CommandPool[i] = nullptr;
		}
	}

}
