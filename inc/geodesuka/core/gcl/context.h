#pragma once
#ifndef GEODESUKA_CORE_GCL_CONTEXT_H
#define GEODESUKA_CORE_GCL_CONTEXT_H

#include <vector>
#include <atomic>
#include <mutex>

#include "../util/log.h"

#include "config.h"

#include "command_list.h"
#include "command_batch.h"
#include "device.h"

// context.h
// To run anything on a gpu, one must create a device execution 
// context for that device. A device context is neccessary for 
// the manipulation of all resources held on that device, under 
// the command of the host.
//

namespace geodesuka::core::gcl {

	class command_pool;

	class context {
	public:

		friend class geodesuka::core::state;
		friend class geodesuka::engine;

		context(engine* aEngine, device* aDevice, uint32_t aLayerCount, const char** aLayerList, uint32_t aExtensionCount, const char** aExtensionList);
		~context();

		// Used to aggregate engine logs.
		void operator<<(const util::log::message aNewLogMessage);

		// Acquires the qfi for a with a desired execution operations.
		int qfi(device::qfeo aQFEO);

		// Checks if QFEO is supported by device context.
		bool available(device::qfeo aQFEO);

		// --------------- Command Buffer Production --------------- //
		// Each enabled queue family has a built in command_pool, which
		// creates/destroys
		
		// Creates a single command buffer from the chosen family support option.
		VkCommandBuffer create_command_buffer(device::qfeo aQueueFamilySupportOption, VkCommandBufferLevel aLevel);

		// Creates a series of command buffers from selected family support option.
		command_list create_command_list(device::qfeo aQueueFamilySupportOption, VkCommandBufferLevel aLevel, uint32_t aCommandBufferCount);

		// Destroys a single command buffer.
		bool destroy_command_buffer(device::qfeo aQueueFamilySupportOption, VkCommandBuffer aCommandBuffer);

		// Destroys a command list.
		bool destroy_command_list(device::qfeo aQueueFamilySupportOption, command_list& aCommandList);

		VkSemaphore create_semaphore();
		VkResult create_semaphore(int aSemaphoreCount, VkSemaphore* aSemaphoreList);
		void destroy_semaphore(VkSemaphore& aSemaphore);
		void destroy_semaphore(int aSemaphoreCount, VkSemaphore* aSemaphoreList);

		VkFence create_fence();
		VkFence create_fence(VkFenceCreateFlags aFenceCreateFlags);
		VkResult create_fence(int aFenceCount, VkFence* aFenceList);
		VkResult create_fence(VkFenceCreateFlags aFenceCreateFlags, int aFenceCount, VkFence* aFenceList);

		void destroy_fence(VkFence& aFence);
		void destroy_fence(int aFenceCount, VkFence* aFenceList);

		// --------------- Execution --------------- //
		
		VkResult execute(device::qfeo aQFS, VkCommandBuffer aCommandBuffer, VkFence aFence);
		VkResult execute(device::qfeo aQFS, const command_list& aCommandList, VkFence aFence);
		VkResult execute(device::qfeo aQFS, const std::vector<gcl::command_list>& aCommandBatch, VkFence aFence);
		VkResult execute(device::qfeo aQFS, const std::vector<VkSubmitInfo>& aSubmissionList, VkFence aFence);
		VkResult execute(const std::vector<VkPresentInfoKHR>& aPresentationList, VkFence aFence);
		VkResult execute(device::qfeo aQFS, const command_batch& aCommandBatch, VkFence aFence);





		VkResult wait(VkFence aFence, VkBool32 aWaitOnAll);
		VkResult wait(uint32_t aFenceCount, VkFence* aFenceList, VkBool32 aWaitOnAll);

		// Buffer memory requirements.
		VkMemoryRequirements get_buffer_memory_requirements(VkBuffer aBufferHandle);

		// Image memory requirements.
		VkMemoryRequirements get_image_memory_requirements(VkImage aImageHandle);

		engine* parent_engine();
		device* parent_device();

		VkInstance inst();
		device* parent();
		VkDevice handle();

		std::mutex 			Mutex;
		std::atomic<bool> 	EnableProcessing;

		// -------------------- Engine Metadata -------------------- //
		
		// This data is used for engine execution.
		std::mutex 			ExecutionMutex;
		VkFence 			ExecutionFence[3];
		command_batch 		BackBatch[3];
		command_batch 		WorkBatch[3];


	//private:

		struct queue_family {
			std::mutex*				Mutex;
			std::vector<float>		Priority;
			std::vector<VkQueue>	Handle;
			queue_family(VkQueueFamilyProperties aProperties);
			queue_family(const queue_family& aInput);
			queue_family(queue_family&& aInput) noexcept;
			~queue_family();
			VkQueue& operator[](size_t aIndex);
			queue_family& operator=(const queue_family& aRhs);
			queue_family& operator=(queue_family&& aRhs) noexcept;
			size_t count() const;
		};

		// -------------------- Context Data -------------------- //

		// { T, C, G, GC, P ]
		engine* 									Engine;
		device* 									Device;
		int 										QFI[5];
		int 										UQFICount;
		int 										UQFI[5];
		std::vector<queue_family> 					Queue;
		std::vector<VkDeviceQueueCreateInfo>	QueueCreateInfo;
		VkDeviceCreateInfo 						CreateInfo{};
		VkDevice 									Handle;
		command_pool*								CommandPool[4];

		int qfi_to_i(device::qfeo aQFEO);

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_CONTEXT_H
