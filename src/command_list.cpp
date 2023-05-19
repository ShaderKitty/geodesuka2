#include <geodesuka/core/gcl/command_list.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include <geodesuka/core/gcl/config.h>

namespace geodesuka::core::gcl {

	command_list::command_list() { }

	command_list::command_list(vk_command_buffer aCommandBuffer) {
		this->Handle.push_back(aCommandBuffer);
	}

	command_list::command_list(uint32_t aCommandBufferCount) {
		this->Handle.resize(aCommandBufferCount);
		for (size_t i = 0; i < this->Handle.size(); i++) {
			this->Handle[i] = VK_NULL_HANDLE;
		}
	}

	command_list::command_list(uint32_t aCommandBufferCount, vk_command_buffer* aCommandBufferList) {
		this->Handle.resize(aCommandBufferCount);
		memcpy(this->Handle.data(), aCommandBufferList, aCommandBufferCount * sizeof(vk_command_buffer));
	}

	vk_command_buffer command_list::operator[](uint32_t aIndex) const {
		return this->Handle[aIndex];
	}

	vk_command_buffer& command_list::operator[](uint32_t aIndex) {
		return this->Handle[aIndex];
	}

	// TODO: Fill out these three functions.
	command_list command_list::operator&(const command_list& aRhs) const {
		command_list L = this->purify();
		command_list R = aRhs.purify();

		size_t IntersectionCount = 0;
		for (size_t i = 0; i < L.count(); i++) {
			if (R.exists_in(L[i])) {
				IntersectionCount += 1;
			}
		}

		if (IntersectionCount == 0) return command_list();

		int j = 0;
		command_list Intersection(IntersectionCount);
		for (uint32_t i = 0; i < L.count(); i++) {
			if (R.exists_in(L[i])) {
				Intersection[j] = L[i];
				j += 1;
			}
		}

		return Intersection;
	}

	command_list command_list::operator|(const command_list& aRhs) const {
		command_list Union;
		if ((this->count() > 0) && (aRhs.count() > 0)) {
			command_list L = this->purify();
			command_list R = aRhs.purify();
			command_list Intersection = L & R;
			size_t LeftUniqueCount = L.count() - Intersection.count();
			size_t RightUniqueCount = R.count() - Intersection.count();
			size_t UnionCount = LeftUniqueCount + Intersection.count() + RightUniqueCount;

			Union.resize(UnionCount);

			int j = 0;
			// Copy over left hand side into array.
			for (size_t i = 0; i < this->count(); i++) {
				if (!Intersection.exists_in(this->Handle[i])) {
					Union[j] = this->Handle[i];
					j += 1;
				}
			}

			// Copy over intersecting elements.
			for (size_t i = 0; i < Intersection.count(); i++) {
				Union[j] = Intersection[j];
				j += 1;
			}

			// Copy over left hand side into array.
			for (size_t i = 0; i < aRhs.count(); i++) {
				if (!Intersection.exists_in(aRhs[i])) {
					Union[j] = aRhs[i];
					j += 1;
				}
			}

			return Union;
		}
		else if (this->count() > 0) {
			return *this;
		}
		else if (aRhs.count() > 0) {
			return aRhs;
		}
		else {
			return Union;
		}
	}

	command_list command_list::operator-(const command_list& aRhs) const {
		command_list Difference;
		if (this->count() > 0) {
			command_list Intersection = *this & aRhs;
			if (this->count() == Intersection.count()) {
				// All elements removed

			}
			else if ((this->count() > Intersection.count()) && (Intersection.count() > 0)) {
				// Different Sizes
				Difference.resize(this->count() - Intersection.count());

				int j = 0;
				for (size_t i = 0; i < this->count(); i++) {
					if (!Intersection.exists_in(this->Handle[i])) {
						Difference[j] = this->Handle[i];
						j += 1;
					}
				}
			}
			else {
				// No intersection
				Difference = *this;
			}
		}
		return Difference;
	}

	command_list command_list::operator&(vk_command_buffer aRhs) const {
		return (*this & command_list(aRhs));
	}

	command_list command_list::operator|(vk_command_buffer aRhs) const {
		return (*this | command_list(aRhs));
	}

	command_list command_list::operator-(vk_command_buffer aRhs) const {
		return (*this - command_list(aRhs));
	}

	command_list& command_list::operator&=(const command_list& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	command_list& command_list::operator|=(const command_list& aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	command_list& command_list::operator-=(const command_list& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	command_list& command_list::operator|=(vk_command_buffer aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	command_list& command_list::operator-=(vk_command_buffer aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	void command_list::resize(uint32_t aCount) {
		this->Handle.resize(aCount);
	}

	bool command_list::exists_in(vk_command_buffer aCommandBuffer) const {
		for (size_t i = 0; i < Handle.size(); i++) {
			if (Handle[i] == aCommandBuffer) return true;
		}
		return false;
	}

	size_t command_list::count() const {
		return this->Handle.size();
	}

	command_list command_list::prune() const {
		size_t NullCount = 0;

		for (size_t i = 0; i < Handle.size(); i++) {
			if (Handle[i] == VK_NULL_HANDLE) {
				NullCount += 1;
			}
		}

		if (NullCount == 0) return *this;

		size_t j = 0;
		command_list Pruned(Handle.size() - NullCount);
		for (uint32_t i = 0; i < Handle.size(); i++) {
			if (Handle[i] != VK_NULL_HANDLE) {
				Pruned[j] = Handle[i];
				j += 1;
			}
		}

		Pruned.WaitSemaphore			= this->WaitSemaphore;
		Pruned.WaitStage				= this->WaitStage;
		Pruned.SignalSemaphore			= this->SignalSemaphore;

		return Pruned;
	}

	command_list command_list::purify() const {
		command_list Pruned = this->prune();

		size_t ReducedCount = Pruned.Handle.size();
		for (size_t i = 0; i < Pruned.Handle.size(); i++) {
			for (size_t j = i + 1; j < Pruned.Handle.size(); j++) {
				if (Pruned[i] == Pruned[j]) {
					ReducedCount -= 1;
				}
			}
		}

		if (ReducedCount == Pruned.Handle.size()) return Pruned;

		size_t k = 0;
		command_list Purified(ReducedCount);
		for (size_t i = 0; i < Pruned.Handle.size(); i++) {
			if (!Purified.exists_in(Pruned[i])) {
				Purified[k] = Pruned[i];
				k += 1;
			}
		}

		Purified.WaitSemaphore			= this->WaitSemaphore;
		Purified.WaitStage				= this->WaitStage;
		Purified.SignalSemaphore		= this->SignalSemaphore;

		return Purified;
	}

	void command_list::depends_on(vk_semaphore aDependencyLink, command_list& aProducer, vk_pipeline_stage_flags aProducerStageFlag) {
		if (!aProducer.is_signalling(aDependencyLink)) {
			aProducer.SignalSemaphore.push_back(aDependencyLink);
		}
		if (!this->is_waiting_on(aDependencyLink)) {
			this->WaitSemaphore.push_back(aDependencyLink);
			this->WaitStage.push_back(aProducerStageFlag);
		}
	}

	void command_list::wait_on(vk_semaphore aWaitSemaphore, vk_pipeline_stage_flags aProducerStageFlag) {
		if (!this->is_waiting_on(aWaitSemaphore)) {
			this->WaitSemaphore.push_back(aWaitSemaphore);
			this->WaitStage.push_back(aProducerStageFlag);
		}
	}

	void command_list::signal_to(vk_semaphore aSignalSemaphore) {
		if (!this->is_signalling(aSignalSemaphore)) {
			this->SignalSemaphore.push_back(aSignalSemaphore);
		}
	}

	vk_submit_info command_list::build() const {
		vk_submit_info SubmissionBuild{};
		SubmissionBuild.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmissionBuild.pNext					= NULL;
		SubmissionBuild.waitSemaphoreCount		= this->WaitSemaphore.size();
		SubmissionBuild.pWaitSemaphores			= this->WaitSemaphore.data();
		SubmissionBuild.pWaitDstStageMask		= this->WaitStage.data();
		SubmissionBuild.commandBufferCount		= this->Handle.size();
		SubmissionBuild.pCommandBuffers			= this->Handle.data();
		SubmissionBuild.signalSemaphoreCount	= this->SignalSemaphore.size();
		SubmissionBuild.pSignalSemaphores		= this->SignalSemaphore.data();
		return SubmissionBuild;
	}

	bool command_list::is_waiting_on(vk_semaphore aSemaphore) {
		for (size_t i = 0; i < WaitSemaphore.size(); i++) {
			if (WaitSemaphore[i] == aSemaphore) return true;
		}
		return false;
	}

	bool command_list::is_signalling(vk_semaphore aSemaphore) {
		for (size_t i = 0; i < SignalSemaphore.size(); i++) {
			if (SignalSemaphore[i] == aSemaphore) return true;
		}
		return false;
	}

}
