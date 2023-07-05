#pragma once
#ifndef GEODESUKA_CORE_GCL_COMMAND_LIST_H
#define GEODESUKA_CORE_GCL_COMMAND_LIST_H

#include <vector>

#include "config.h"

namespace geodesuka::core::gcl {

	// ----- command_list ----- //
	// command_list is just simply a container class with the ultimate
	// goal of creating VkSubmitInfo structures. The nature of 
	// VkSubmitInfo is that its members are unmodifiable. Therefore
	// this class has been created because it contains a set of methods
	// for modifying the members of VkSubmitInfo before it is built.
	// This includes semaphore linking between lists. Since command_list
	// is simply just a conatainer class, the user must bring his/her
	// own vk_command_buffers and vk_semaphores. command_list will
	// not create/destroy these resources. In fact, in the geodesuka engine
	// it is encouraged that all resources be owned/managed/possessed by
	// the parent object_t of the resource.
	// 
	class command_list {
	public:

		friend class command_pool;

		command_list();
		command_list(VkCommandBuffer aCommandBuffer);
		command_list(uint32_t aCommandBufferCount);
		command_list(uint32_t aCommandBufferCount, VkCommandBuffer* aCommandBufferList);

		// ---------- Command List Manipulation ---------- //
		// These operations do not copy over depency semaphores.
		// Dependency semaphores must be explicitly set for a
		// command_list object.
		//
		
		// []: Access operators.

		VkCommandBuffer operator[](uint32_t aIndex) const;
		VkCommandBuffer& operator[](uint32_t aIndex);

		// &: Intersection of two lists.
		// |: Union of two lists.
		// -: Removes the elements on the left from the right.

		command_list operator&(const command_list& aRhs) const;
		command_list operator|(const command_list& aRhs) const;
		command_list operator-(const command_list& aRhs) const;

		command_list operator&(VkCommandBuffer aRhs) const;
		command_list operator|(VkCommandBuffer aRhs) const;
		command_list operator-(VkCommandBuffer aRhs) const;

		command_list& operator&=(const command_list& aRhs);
		command_list& operator|=(const command_list& aRhs);
		command_list& operator-=(const command_list& aRhs);

		command_list& operator|=(VkCommandBuffer aRhs);
		command_list& operator-=(VkCommandBuffer aRhs);

		// Resizes size of command list.
		void resize(uint32_t aCount);

		// Checks if command buffer exists in list.
		bool exists_in(VkCommandBuffer aCommandBuffer) const;

		size_t count() const;

		// Removes VK_NULL_HANDLE elements.
		command_list prune() const;

		// Removes all redundant elements.
		command_list purify() const;

		// ---------- Dependency/Synchronization ---------- //
		// This section is rather short, because setting up 
		// dependencies between command_lists should be easy.
		//

		// Create a dependency bewteen command lists. Must provide a valid semaphore.
		// A.depends_on(S, B, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
		void depends_on(VkSemaphore aDependencyLink, command_list& aProducer, VkPipelineStageFlags aProducerStageFlag);

		void wait_on(VkSemaphore aWaitSemaphore, VkPipelineStageFlags aProducerStageFlag);

		void signal_to(VkSemaphore aSignalSemaphore);

		// A VkSubmitInfo object is created to reference the contents
		// within command_list.
		VkSubmitInfo build() const;

	private:

		std::vector<VkSemaphore>				WaitSemaphore;
		std::vector<VkPipelineStageFlags>	WaitStage;
		std::vector<VkCommandBuffer>			Handle;
		std::vector<VkSemaphore>				SignalSemaphore;

		bool is_waiting_on(VkSemaphore aSemaphore);
		bool is_signalling(VkSemaphore aSemaphore);

	};

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_LIST_H
