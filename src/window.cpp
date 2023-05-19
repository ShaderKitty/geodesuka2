#include <geodesuka/engine.h>
#include <geodesuka/core/object/window.h>

// Standard C Library.
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>

#include <geodesuka/core/math.h>

//#include "../hid/mouse.h"
//#include "../hid/keyboard.h"
//#include "../hid/joystick.h"

#include <geodesuka/core/object_t.h>

namespace geodesuka::core::object {

	window::setting::setting() {
		Resizable		= GLFW_TRUE;
		Decorated		= GLFW_TRUE;
		UserFocused		= GLFW_TRUE;
		AutoMinimize	= GLFW_TRUE;
		Floating		= GLFW_FALSE;
		Maximized		= GLFW_FALSE;
		Minimized		= GLFW_FALSE;
		Visible			= GLFW_TRUE;
		ScaleToMonitor	= GLFW_FALSE;
		CenterCursor	= GLFW_TRUE;
		FocusOnShow		= GLFW_TRUE;
		Fullscreen		= GLFW_FALSE;

		Hovered			= GLFW_TRUE;
	}

	window::~window() {

	}

	void window::set_title(util::string aTitle) {

	}

	void window::set_size(float2 aSize) {
		this->Size = aSize;
	}

	float2 window::get_size() {
		return this->Size;
	}

	void window::set_resolution(uint2 aResolution) {
		this->FrameResolution = uint3(aResolution.x, aResolution.y, 1u);
	}

	uint2 window::get_resolution() {
		return uint2(this->FrameResolution.x, this->FrameResolution.y);
	}

	bool window::should_close() {
		return false;
	}

	void window::set_input_target(object_t* aObject) {
		this->Mutex.lock();
		this->InputTarget = aObject;
		this->Mutex.unlock();
	}

	window::window(gcl::context* aContext, stage_t* aStage, const char* aName, uint3 aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount) :
		render_target(aContext, aStage, aName, aFrameResolution, aFrameRate, aFrameCount, aAttachmentCount)
	{
		vk_result Result = VK_SUCCESS;
		this->Title = aName;
		this->Size = float2(0.0f, 0.0f);
		this->Setting = {};
		this->InputTarget = nullptr;
	}

}