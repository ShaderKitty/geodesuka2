#include <geodesuka/engine.h>
#include <geodesuka/core/object_t.h>

#include <assert.h>

#include <vector>

// Recognized Render Targets
#include <geodesuka/core/object/render_target.h>
#include <geodesuka/core/object/window.h>
#include <geodesuka/core/object/system_display.h>
#include <geodesuka/core/object/system_window.h>
#include <geodesuka/core/object/virtual_window.h>
#include <geodesuka/core/object/camera.h>
#include <geodesuka/core/object/camera2d.h>
#include <geodesuka/core/object/camera3d.h>
//#include <geodesuka/core/object/cameravr.h>

// ------------------------------ public methods ------------------------------- //
// ------------------------------ protected methods ---------------------------- //
// ------------------------------ private methods ------------------------------ //

namespace geodesuka::core {

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::window* aWindow, object_t* aObject) {
		this->Context = aContext;

		this->DrawCommand.resize(aWindow->Frame.size());

		float Vertex[] = {
			-1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		};

		for (size_t i = 0; i < aWindow->Frame.size(); i++) {

		}
		
	}

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::camera2d* aCamera2D, object_t* aObject) {

	}

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::camera3d* aCamera3D, object_t* aObject) {
		vk_result Result = VK_SUCCESS;

		this->Context 		= aContext;

		this->RenderTarget 	= aCamera3D;

		// The number of descriptor sets needed must be determined by the render target.
		std::vector<vk_descriptor_pool_size> DescriptorPoolSize = aCamera3D->descriptor_pool_sizes();
		vk_descriptor_pool_create_info DescriptorPoolCreateInfo{};
		DescriptorPoolCreateInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.pNext				= NULL;
		DescriptorPoolCreateInfo.flags				= 0;
		DescriptorPoolCreateInfo.maxSets			= aCamera3D->descriptor_set_count();
		DescriptorPoolCreateInfo.poolSizeCount		= DescriptorPoolSize.size();
		DescriptorPoolCreateInfo.pPoolSizes			= DescriptorPoolSize.data();
		
		// ---------- Create Descriptor Sets ---------- //

		this->DescriptorSet.resize(aCamera3D->Frame.size());
		for (size_t i = 0; i < aCamera3D->Frame.size(); i++) {
			vk_descriptor_set_allocate_info AllocateInfo{};
			AllocateInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			AllocateInfo.pNext					= NULL;
			AllocateInfo.descriptorPool			= this->DescriptorPool;
			AllocateInfo.descriptorSetCount		;
			AllocateInfo.pSetLayouts			;
			Result = vkAllocateDescriptorSets(aContext->handle(), &AllocateInfo, this->DescriptorSet[i].data());
		}

		// ---------- Command Buffers ---------- //

		// Creates command buffers for render operation.
		this->DrawCommand.resize(aCamera3D->Frame.size());
		for (size_t i = 0; i < aCamera3D->Frame.size(); i++) {
			this->DrawCommand[i] = aCamera3D->CommandPool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY, aObject->Model->command_buffer_count());
		}

		for (size_t i = 0; i < aCamera3D->Frame.size(); i++) {
			size_t j = 0;
			// Iterate through nodes with meshes references.
			for (int a = 0; a < aObject->Model->MeshNode.size(); a++) {
				// iterate through mesh instances.
				for (int b = 0; b < aObject->Model->MeshNode[a].MeshIndexCount; b++) {
					int MeshIndex = aObject->Model->MeshNode[a].MeshIndex[b];
					// Iterate through mesh faces
					for (int c = 0; c < aObject->Model->Mesh[MeshIndex]->Face.size(); c++) {
						vk_result Result = VK_SUCCESS;
						vk_command_buffer_begin_info CommandBufferBeginInfo{};
						vk_render_pass_begin_info RenderPassBeginInfo{};

						CommandBufferBeginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
						CommandBufferBeginInfo.pNext				= NULL;
						CommandBufferBeginInfo.flags				= 0;
						CommandBufferBeginInfo.pInheritanceInfo		= NULL;

						RenderPassBeginInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
						RenderPassBeginInfo.pNext					= NULL;
						RenderPassBeginInfo.renderPass				= aCamera3D->RenderPass;
						RenderPassBeginInfo.framebuffer				= aCamera3D->Frame[i].Buffer;
						RenderPassBeginInfo.renderArea				= aCamera3D->RenderArea;
						RenderPassBeginInfo.clearValueCount			= 0;
						RenderPassBeginInfo.pClearValues			= NULL;

						Result = vkBeginCommandBuffer(this->DrawCommand[i][j], &CommandBufferBeginInfo);
						vkCmdBeginRenderPass(this->DrawCommand[i][j], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
						vkCmdBindVertexBuffers(
							this->DrawCommand[i][j],
							0, 
							1,
							&aObject->Model->Mesh[MeshIndex]->VertexBuffer.handle(),
							NULL
						);
						vkCmdBindIndexBuffer(
							this->DrawCommand[i][j],
							aObject->Model->Mesh[MeshIndex]->IndexBuffer[c].handle(),
							0,
							aObject->Model->Mesh[MeshIndex]->Face[c].IndexType
						);
						vkCmdBindDescriptorSets(
							this->DrawCommand[i][j], 
							aCamera3D->Pipeline[0].BindPoint,
							aCamera3D->Pipeline[0].Layout,
							0, 
							aCamera3D->Pipeline[0].DescriptorSetLayoutCount,
							NULL, // <-- Descriptor Sets go here
							0, 
							NULL
						);
						vkCmdBindPipeline(
							this->DrawCommand[i][j], 
							aCamera3D->Pipeline[0].BindPoint,
							aCamera3D->Pipeline[0].Handle
						);
						vkCmdDrawIndexed(this->DrawCommand[i][j], aObject->Model->Mesh[MeshIndex]->Face[c].IndexCount, 1, 0, 0, 0);
						vkCmdEndRenderPass(this->DrawCommand[i][j]);
						Result = vkEndCommandBuffer(this->DrawCommand[i][j]);

						j += 1;
					}
				}
			}
		}

	}

	// ------------------------------ public methods ------------------------------- //

	object_t::~object_t() {
		if ((Stage != nullptr) && (Stage->State != DESTRUCTION)) {
			// Suspend all other shared threads.
			Engine->ThreadController.suspend_all();

			// Remove object from parent stage, and all shared stages.
			for (int i = 0; i < Engine->Stage.count(); i++) {
				Engine->Stage[i]->Object -= this;
			}

			// Resume all engine threads.
			Engine->ThreadController.resume_all();
		}
	}

	void object_t::set_position(float3 aPosition) {
		this->Mutex.lock();
		this->Position = aPosition;
		this->Mutex.unlock();
	}

	float3 object_t::get_position() const {
		return this->Position;
	}

	void object_t::set_name(const char* aName) {
		this->Mutex.lock();
		this->Name = aName;
		this->Mutex.unlock();
	}

	const char* object_t::get_name() const {
		return this->Name.ptr();
	}

	void object_t::predraw(object::render_target* aRenderTarget) {
		graphics::render_operation* NewRenderOperation = nullptr;
		switch (aRenderTarget->id()) {
		default:
			break;
		case object::system_window::ID:
		case object::virtual_window::ID:
		// Check if object is a render target, the swap render ops.
			NewRenderOperation = new default_renderer(
				Context,
				(object::window*)aRenderTarget,
				this
			);
			break;
		case object::camera2d::ID:
			NewRenderOperation = new default_renderer(
				Context,
				(object::camera2d*)aRenderTarget,
				this
			);
			break;
		case object::camera3d::ID:
			NewRenderOperation = new default_renderer(
				Context,
				(object::camera3d*)aRenderTarget,
				this
			);
			break;
		}

		assert(NewRenderOperation);
		this->RenderOperation.insert(std::make_pair(aRenderTarget, NewRenderOperation));
	}

	void object_t::operator-=(object::render_target* aRenderTarget) {
		if (this->RenderOperation.count(aRenderTarget) > 0) {
			delete this->RenderOperation[aRenderTarget];
			this->RenderOperation.erase(aRenderTarget);
		}
	}

	gcl::command_list object_t::draw(object::render_target* aRenderTarget) {
		gcl::command_list DrawBatch;

		// Check if rendering operation exists for render target.
		if (this->RenderOperation.count(aRenderTarget) == 0) {
			this->predraw(aRenderTarget);
		}

		// Get Draw Commands
		DrawBatch = (*this->RenderOperation[aRenderTarget])[aRenderTarget->FrameDrawIndex];
		return DrawBatch;
	}

	// ------------------------------ protected methods ---------------------------- //

	//object_t::object_t() {}

	object_t::object_t(gcl::context* aContext, stage_t* aStage, const char* aName) {

		// Internal API.
		State			= state::CREATION;
		if (aContext != nullptr){
			Engine			= aContext->parent_engine();
		}
		else {
			Engine 			= nullptr;
		}
		Context			= aContext;
		Stage			= aStage;
		Name			= aName;

		if (((Engine != nullptr) && (Stage != nullptr)) ? (Stage->State != DESTRUCTION) : false)  {
			// Suspend all other shared threads.
			Engine->ThreadController.suspend_all();
			
			// Add object to stage.
			Stage->Object |= this;

			// Resume all engine threads.
			Engine->ThreadController.resume_all();
		}

		InputVelocity	= float3(0.0, 0.0, 0.0);
		InputForce		= float3(0.0, 0.0, 0.0);

		Mass			= 1.0;
		Time			= logic::get_time();
		Position		= float3(0.0, 0.0, 0.0);
		Momentum		= float3(0.0, 0.0, 0.0);
		Force			= float3(0.0, 0.0, 0.0);
		DirectionX		= float3(1.0, 0.0, 0.0);
		DirectionY		= float3(0.0, 1.0, 0.0);
		DirectionZ		= float3(0.0, 0.0, 1.0);
	}

	void object_t::input(const hid::mouse& aMouse, const hid::keyboard& aKeyboard) {

	}

	vk_submit_info object_t::update(double aDeltaTime) {
		vk_submit_info TransferBatch{};
		TransferBatch.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
		TransferBatch.pNext = NULL;
		TransferBatch.waitSemaphoreCount = 0;
		TransferBatch.pWaitSemaphores = NULL;
		TransferBatch.pWaitDstStageMask = NULL;
		TransferBatch.commandBufferCount = 0;
		TransferBatch.pCommandBuffers = NULL;
		TransferBatch.signalSemaphoreCount = 0;
		TransferBatch.pSignalSemaphores = NULL;

		//tex:
		// Update equations for generic object type with applied forces.
		// $$ d\vec{p} = \big( \vec{F}_{\text{applied}} + \vec{F}_{user} \big) dt $$ 
		// $$ d \vec{x} = \big( \frac{\vec{p}}{m} + \vec{v}_{user} \big) dt $$

		this->Mutex.lock();

		// Generic Free body motion equations.
		Momentum += (Force + InputForce) * aDeltaTime;
		Position += ((Momentum / Mass) + InputVelocity) * aDeltaTime;

		this->Mutex.unlock();
		return TransferBatch;
	}

	vk_submit_info object_t::compute() {
		vk_submit_info ComputeBatch{};
		ComputeBatch.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		ComputeBatch.pNext = NULL;
		ComputeBatch.waitSemaphoreCount = 0;
		ComputeBatch.pWaitSemaphores = NULL;
		ComputeBatch.pWaitDstStageMask = NULL;
		ComputeBatch.commandBufferCount = 0;
		ComputeBatch.pCommandBuffers = NULL;
		ComputeBatch.signalSemaphoreCount = 0;
		ComputeBatch.pSignalSemaphores = NULL;
		return ComputeBatch;
	}

	//object_t::render_operation::render_operation() {
	//	this->RenderTarget = nullptr;
	//	this->RenderPass = VK_NULL_HANDLE;
	//	this->Framebuffer = NULL;
	//}

	//object_t::render_operation::render_operation(object::render_target* aRenderTarget, int aCommandCount) {
	//	this->RenderTarget = aRenderTarget;
	//	this->RenderPass = VK_NULL_HANDLE;
	//	this->Framebuffer = (vk_framebuffer*)malloc(aRenderTarget->FrameCount * sizeof(vk_framebuffer));
	//	assert(this->Framebuffer);
	//	this->DrawCommand = gcl::command_group(aRenderTarget->FrameCount, aCommandCount);
	//}

	//object_t::render_operation::~render_operation() {
	//	free(this->Framebuffer);
	//	this->Framebuffer = NULL;
	//}

	//gcl::command_list& object_t::render_operation::operator[](int aIndex) {
	//	return this->DrawCommand[aIndex];
	//}

}
