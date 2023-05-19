#include <geodesuka/core/gcl/command_batch.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <geodesuka/core/gcl/config.h>

namespace geodesuka::core::gcl {

	command_batch::command_batch() {}

	command_batch::command_batch(vk_submit_info aSubmission) {
		this->Submission.push_back(aSubmission);
	}

	command_batch::command_batch(uint32_t aSubmissionCount, const vk_submit_info* aSubmission) {
		this->Submission.resize(aSubmissionCount);
		memcpy(this->Submission.data(), aSubmission, aSubmissionCount * sizeof(vk_submit_info));
	}

	command_batch::command_batch(const std::vector<command_list>& aSubmission) {
		this->Submission.resize(aSubmission.size());
		for (size_t i = 0; i < aSubmission.size(); i++) {
			this->Submission[i] = aSubmission[i].build();
		}
	}

	command_batch::command_batch(vk_present_info_khr aPresentation) {
		this->Presentation.push_back(aPresentation);
	}

	command_batch::command_batch(uint32_t aPresentationCount, const vk_present_info_khr* aPresentation) {
		this->Presentation.resize(aPresentationCount);
		memcpy(this->Presentation.data(), aPresentation, aPresentationCount * sizeof(vk_present_info_khr));
	}

	command_batch& command_batch::operator=(const std::vector<command_list>& aRhs) {
		return (*this = command_batch(aRhs));
	}

	command_batch command_batch::operator+(vk_submit_info aRhs) {
		return (*this + command_batch(aRhs));
	}

	command_batch command_batch::operator+(vk_present_info_khr aRhs) {
		return (*this + command_batch(aRhs));
	}

	command_batch command_batch::operator+(const std::vector<command_list>& aRhs) {
		return (*this + command_batch(aRhs));
	}

	command_batch command_batch::operator+(const command_batch& aRhs) {
		gcl::command_batch CommandBatch;
		CommandBatch.Submission.resize(this->Submission.size() + aRhs.Submission.size());
		CommandBatch.Presentation.resize(this->Presentation.size() + aRhs.Presentation.size());

		if (this->Submission.size() > 0) {
			memcpy(&CommandBatch.Submission[0], this->Submission.data(), this->Submission.size() * sizeof(vk_submit_info));
		}

		if (aRhs.Submission.size() > 0) {
			memcpy(&CommandBatch.Submission[this->Submission.size()], aRhs.Submission.data(), aRhs.Submission.size() * sizeof(vk_submit_info));
		}

		if (this->Presentation.size() > 0) {
			memcpy(&CommandBatch.Presentation[0], this->Presentation.data(), this->Presentation.size() * sizeof(vk_present_info_khr));
		}

		if (aRhs.Presentation.size() > 0) {
			memcpy(&CommandBatch.Presentation[this->Presentation.size()], aRhs.Presentation.data(), aRhs.Presentation.size() * sizeof(vk_present_info_khr));
		}

		return CommandBatch;
	}

	command_batch& command_batch::operator+=(vk_submit_info aRhs) {
		return (*this += command_batch(aRhs));
	}

	command_batch& command_batch::operator+=(vk_present_info_khr aRhs) {
		return (*this += command_batch(aRhs));
	}

	command_batch& command_batch::operator+=(const std::vector<command_list>& aRhs) {
		return (*this += command_batch(aRhs));
	}

	command_batch& command_batch::operator+=(const command_batch& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	void command_batch::purify() {
		size_t NewSubmissionCount = this->Submission.size();
		for (size_t i = 0; i < this->Submission.size(); i++) {
			if (this->Submission[i].pCommandBuffers == NULL) {
				NewSubmissionCount -= 1;
			}
		}

		size_t NewPresentationCount = this->Presentation.size();
		for (size_t i = 0; i < this->Presentation.size(); i++) {
			if (this->Presentation[i].pImageIndices == NULL) {
				NewPresentationCount -= 1;
			}
		}

		std::vector<vk_submit_info> NewSubmission(NewSubmissionCount);
		std::vector<vk_present_info_khr> NewPresentation(NewPresentationCount);
		size_t j = 0;
		for (size_t i = 0; i < this->Submission.size(); i++) {
			if (this->Submission[i].pCommandBuffers != NULL) {
				NewSubmission[j] = this->Submission[i];
				j += 1;
			}
		}

		j = 0;
		for (size_t i = 0; i < this->Presentation.size(); i++) {
			if (this->Presentation[i].pImageIndices != NULL) {
				NewPresentation[j] = this->Presentation[i];
				j += 1;
			}
		}

		this->Submission = NewSubmission;
		this->Presentation = NewPresentation;
	}

	void command_batch::clear() {
		Submission.clear();
		Presentation.clear();
	}

}