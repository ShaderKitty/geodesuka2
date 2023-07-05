#include <geodesuka/engine.h>
#include <geodesuka/core/stage_t.h>

namespace geodesuka::core {

	// Base destructor.
	stage_t::~stage_t() {
		// Generate a list of uniquely owned objects by stage.
		object_list OwnedObject = Object.gather_by(this);

		State = DESTRUCTION;

		if ((Engine != nullptr) ? (Engine->State != engine::state::DESTRUCTION) : false) {
			// Suspend all other shared threads.
			Engine->ThreadController.suspend_all();

			// I: Remove Stage from Engine.State
			Engine->Stage -= this;

			// II: Iterate through all engine stages to remove owned objects
			// from other stages.
			for (int i = 0; i < Engine->Stage.count(); i++) {
				Engine->Stage[i]->Object -= OwnedObject;
			}

			// Resume all engine threads.
			Engine->ThreadController.resume_all();
		}

		// Deleted owned objects
		for (int i = 0; i < OwnedObject.count(); i++) {
			delete OwnedObject[i];
			OwnedObject[i] = nullptr;
		}
		OwnedObject.clear();
		Context = nullptr;
		Engine = nullptr;
	}

	// Base constructor.
	stage_t::stage_t(engine* aEngine, gcl::context* aContext) {

		State					= state::CREATION;
		Engine					= aEngine;
		Context					= aContext;

		if ((Engine != nullptr) ? (Engine->State != engine::state::DESTRUCTION) : false) {
			// Suspend all other shared threads.
			Engine->ThreadController.suspend_all();

			// Add stage to the engine.
			Engine->Stage |= this;

			// Resume all engine threads.
			Engine->ThreadController.resume_all();
		}
	}

	void stage_t::generate_render_operations() {
		for (int i = 0; i < this->Object.count(); i++) {
			//this->Object[i]->generate_render_operations(this);
		}
	}

	gcl::command_batch stage_t::update_owned_objects(double aDeltaTime) {
		gcl::command_batch TransferBatch;
		for (int i = 0; i < this->Object.count(); i++) {
			if ((this->Object[i]->State == object_t::state::READY) && (this->Object[i]->Stage == this)) {
				TransferBatch += this->Object[i]->update(aDeltaTime);
			}
		}
		return TransferBatch;
	}

	VkSubmitInfo stage_t::update(double aDeltaTime) {
		VkSubmitInfo TransferBatch{};
		TransferBatch.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		TransferBatch.pNext					= NULL;
		TransferBatch.waitSemaphoreCount	= 0;
		TransferBatch.pWaitSemaphores		= NULL;
		TransferBatch.pWaitDstStageMask		= NULL;
		TransferBatch.commandBufferCount	= 0;
		TransferBatch.pCommandBuffers		= NULL;
		TransferBatch.signalSemaphoreCount	= 0;
		TransferBatch.pSignalSemaphores		= NULL;
		return TransferBatch;
	}

	gcl::command_batch stage_t::compute_owned_objects() {
		gcl::command_batch ComputeBatch;
		for (int i = 0; i < this->Object.count(); i++) {
			if ((this->Object[i]->State == object_t::state::READY) && (this->Object[i]->Stage == this)) {
				ComputeBatch += this->Object[i]->compute();
			}
		}
		return ComputeBatch;
	}

	VkSubmitInfo stage_t::compute() {
		VkSubmitInfo ComputeBatch{};
		ComputeBatch.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		ComputeBatch.pNext					= NULL;
		ComputeBatch.waitSemaphoreCount		= 0;
		ComputeBatch.pWaitSemaphores		= NULL;
		ComputeBatch.pWaitDstStageMask		= NULL;
		ComputeBatch.commandBufferCount		= 0;
		ComputeBatch.pCommandBuffers		= NULL;
		ComputeBatch.signalSemaphoreCount	= 0;
		ComputeBatch.pSignalSemaphores		= NULL;
		return ComputeBatch;
	}

	gcl::command_batch stage_t::render() {
		gcl::command_batch RenderBatch;
		//this->Mutex.lock();
		//for (int i = 0; i < this->RenderTarget.count(); i++) {
		//	// Type casts render target, for readability.
		//	object::render_target* RenderTarget = (object::render_target*)this->RenderTarget[i];
		//
		//	// Will be called if render_target is ready to be drawn.
		//	if ((RenderTarget->State == object_t::state::READY) && (RenderTarget->render_signal()))
		//		RenderBatch += RenderTarget->render(this);
		//}
		//this->Mutex.unlock();
		return RenderBatch;
	}

}

