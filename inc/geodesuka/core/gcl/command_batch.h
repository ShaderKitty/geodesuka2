#pragma once
#ifndef GEODESUKA_CORE_GCL_COMMAND_BATCH_H
#define GEODESUKA_CORE_GCL_COMMAND_BATCH_H

#include <vector>

#include "config.h"

#include "command_list.h"

namespace geodesuka::core::gcl {

	/*
	* Simply a utility to manage batched queue submissions for execution.
	*/
	class command_batch {
	public:

		std::vector<vk_submit_info>			Submission;
		std::vector<vk_present_info_khr>	Presentation;

		friend class geodesuka::core::state;
		friend class geodesuka::engine;
		friend class context;

		command_batch();
		command_batch(vk_submit_info aSubmission);
		command_batch(uint32_t aSubmissionCount, const vk_submit_info* aSubmission);
		command_batch(const std::vector<command_list>& aSubmission);
		command_batch(vk_present_info_khr aPresentation);
		command_batch(uint32_t aPresentationCount, const vk_present_info_khr* aPresentation);

		command_batch& operator=(const std::vector<command_list>& aRhs);

		command_batch operator+(vk_submit_info aRhs);
		command_batch operator+(vk_present_info_khr aRhs);
		command_batch operator+(const std::vector<command_list>& aRhs);
		command_batch operator+(const command_batch& aRhs);

		// Will be used to aggregate render_target submissions.
		command_batch& operator+=(vk_submit_info aRhs);
		command_batch& operator+=(vk_present_info_khr aRhs);
		command_batch& operator+=(const std::vector<command_list>& aRhs);
		command_batch& operator+=(const command_batch& aRhs);

		// Removes all empty [Submit/Present]Infos.
		void purify();

		void clear();

	};

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_BATCH_H
