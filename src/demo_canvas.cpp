#include <geodesuka/builtin/stage/demo_canvas.h>

namespace geodesuka::builtin::stage {

	using namespace core;
	using namespace gcl;
	using namespace object;
	using namespace stage;

	//using namespace core;
	using namespace core::gcl;
	using namespace core::object;
	using namespace core::stage;

	demo_canvas::demo_canvas(core::gcl::context* aContext) : canvas(aContext) {
		// Get Primary Display
		system_display* PrimaryDisplay = Engine->PrimaryDisplay;

		// Setup window properties.
		system_window::property Property;
		Property.Swapchain.FrameCount			= 3;
		Property.Swapchain.FrameRate			= 60.0;
		Property.Swapchain.ColorSpace			= system_window::colorspace::SRGB_NONLINEAR;
		Property.Swapchain.Usage				= image::usage::COLOR_ATTACHMENT;
		Property.Swapchain.CompositeAlpha		= system_window::composite::ALPHA_OPAQUE;
		Property.Swapchain.PresentMode			= system_window::present_mode::FIFO;
		Property.Swapchain.Clipped				= true;
		Property.PixelFormat					= VK_FORMAT_B8G8R8A8_SRGB;
		Property.Title							= "Unit Test";

		//this->RenderTarget	|= new system_window(Engine, Context, PrimaryDisplay, "Unit Test", Property, glm::ivec2(0, 0), glm::ivec2(640, 480));
		//this->Object		|= new triangle(Engine, Context, this, "Simple Triangle");

	}

	demo_canvas::~demo_canvas() {

	}

}
