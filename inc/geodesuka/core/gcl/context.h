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
		vk_command_buffer create_command_buffer(device::qfeo aQueueFamilySupportOption, vk_command_buffer_level aLevel);

		// Creates a series of command buffers from selected family support option.
		command_list create_command_list(device::qfeo aQueueFamilySupportOption, vk_command_buffer_level aLevel, uint32_t aCommandBufferCount);

		// Destroys a single command buffer.
		bool destroy_command_buffer(device::qfeo aQueueFamilySupportOption, vk_command_buffer aCommandBuffer);

		// Destroys a command list.
		bool destroy_command_list(device::qfeo aQueueFamilySupportOption, command_list& aCommandList);

		vk_semaphore create_semaphore();
		vk_result create_semaphore(int aSemaphoreCount, vk_semaphore* aSemaphoreList);
		void destroy_semaphore(vk_semaphore& aSemaphore);
		void destroy_semaphore(int aSemaphoreCount, vk_semaphore* aSemaphoreList);

		vk_fence create_fence();
		vk_fence create_fence(vk_fence_create_flags aFenceCreateFlags);
		vk_result create_fence(int aFenceCount, vk_fence* aFenceList);
		vk_result create_fence(vk_fence_create_flags aFenceCreateFlags, int aFenceCount, vk_fence* aFenceList);

		void destroy_fence(vk_fence& aFence);
		void destroy_fence(int aFenceCount, vk_fence* aFenceList);

		// --------------- Execution --------------- //
		
		vk_result execute(device::qfeo aQFS, vk_command_buffer aCommandBuffer, vk_fence aFence);
		vk_result execute(device::qfeo aQFS, const command_list& aCommandList, vk_fence aFence);
		vk_result execute(device::qfeo aQFS, const std::vector<gcl::command_list>& aCommandBatch, vk_fence aFence);
		vk_result execute(device::qfeo aQFS, const std::vector<vk_submit_info>& aSubmissionList, vk_fence aFence);
		vk_result execute(const std::vector<vk_present_info_khr>& aPresentationList, vk_fence aFence);
		vk_result execute(device::qfeo aQFS, const command_batch& aCommandBatch, vk_fence aFence);





		vk_result wait(vk_fence aFence, vk_bool_32 aWaitOnAll);
		vk_result wait(uint32_t aFenceCount, vk_fence* aFenceList, vk_bool_32 aWaitOnAll);

		// Buffer memory requirements.
		vk_memory_requirements get_buffer_memory_requirements(vk_buffer aBufferHandle);

		// Image memory requirements.
		vk_memory_requirements get_image_memory_requirements(vk_image aImageHandle);

		engine* parent_engine();
		device* parent_device();

		vk_instance inst();
		device* parent();
		vk_device handle();

		std::mutex 			Mutex;
		std::atomic<bool> 	EnableProcessing;

		// -------------------- Engine Metadata -------------------- //
		
		// This data is used for engine execution.
		std::mutex 			ExecutionMutex;
		vk_fence 			ExecutionFence[3];
		command_batch 		BackBatch[3];
		command_batch 		WorkBatch[3];


	//private:

		struct queue_family {
			std::mutex*				Mutex;
			std::vector<float>		Priority;
			std::vector<vk_queue>	Handle;
			queue_family(vk_queue_family_properties aProperties);
			queue_family(const queue_family& aInput);
			queue_family(queue_family&& aInput) noexcept;
			~queue_family();
			vk_queue& operator[](size_t aIndex);
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
		std::vector<vk_device_queue_create_info>	QueueCreateInfo;
		vk_device_create_info 						CreateInfo{};
		vk_device 									Handle;
		command_pool*								CommandPool[4];

		int qfi_to_i(device::qfeo aQFEO);

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_CONTEXT_H
