#pragma once
#ifndef GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H
#define GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H

#include <vector>
#include <thread>

#include "../math.h"

#include "../gcl/config.h"
#include "../gcl/device.h"
#include "../gcl/context.h"
#include "../gcl/image.h"
#include "../gcl/framebuffer.h"

#include "../object_t.h"
#include "window.h"
#include "system_display.h"

struct GLFWwindow;
typedef GLFWwindow glfw_window;

/// <summary>
/// system_window.h is an object that will represent Operating System (OS) managed windows, managed
/// by a window manager like Wayland, X11, win32, and so on. It will also be a render target that
/// can be used by other objects to create render commands for. Position and Size will accept
/// two units for modifying the state of a SystemWindow instance. It will take physical coordinates
/// in units of meters which the position and size will be in reference to the parent display it
/// is a child of.
/// </summary>
namespace geodesuka::core::object {

	class system_window : public window {
	public:

		friend class geodesuka::engine;

		enum colorspace {
			SRGB_NONLINEAR			= 0,
			DISPLAY_P3_NONLINEAR	= 1000104001,
			EXTENDED_SRGB_LINEAR	= 1000104002,
			DISPLAY_P3_LINEAR		= 1000104003,
			DCI_P3_NONLINEAR		= 1000104004,
			BT709_LINEAR			= 1000104005,
			BT709_NONLINEAR			= 1000104006,
			BT2020_LINEAR			= 1000104007,
			HDR10_ST2084			= 1000104008,
			DOLBYVISION				= 1000104009,
			HDR10_HLG				= 1000104010,
			ADOBERGB_LINEAR			= 1000104011,
			ADOBERGB_NONLINEAR		= 1000104012,
			PASS_THROUGH			= 1000104013,
			EXTENDED_SRGB_NONLINEAR = 1000104014,
			DISPLAY_NATIVE_AMD		= 1000213000,
		};

		enum composite {
			ALPHA_OPAQUE			= 0x00000001,
			ALPHA_PRE_MULTIPLIED	= 0x00000002,
			ALPHA_POST_MULTIPLIED	= 0x00000004,
			ALPHA_INHERIT			= 0x00000008,
		};

		enum present_mode {
			IMMEDIATE		= 0,
			MAILBOX			= 1,
			FIFO			= 2,
			FIFO_RELAXED	= 3
		};

		struct swapchain {

			struct prop {
				//int Flags;
				uint32_t	FrameCount;
				double		FrameRate;
				int			ColorSpace;
				int			Usage;
				int			CompositeAlpha;
				int			PresentMode;
				bool		Clipped;

				prop();
			};

		};

		struct property {
			setting				Setting;
			swapchain::prop		Swapchain;
			VkFormat			PixelFormat;
			const char*			Title;
			property();
		};


		// Required Extensions for the class
		static std::vector<const char*> RequiredInstanceExtension;
		static std::vector<const char*> RequiredContextExtension;
		static constexpr int ID = 2;

		system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, glm::vec3 aPosition, glm::vec2 aSize);
		system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, glm::ivec2 aPosition, glm::ivec2 aResolution);
		~system_window();

		// -------------------- object_t methods -------------------- //

		virtual void set_position(glm::vec3 aPosition) override;

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		virtual void set_size(glm::vec2 aSize) override;
		virtual void set_resolution(glm::uvec2 aResolution) override;

		// -------------------- native methods ---------------------- //

		void set_position(glm::ivec2 aPosition);
		void set_size(glm::ivec2 aSize);
		void set_option(setting::id, bool aValue);

	protected:

		// -------------------- object_t methods -------------------- //

		virtual VkSubmitInfo update(double aDeltaTime) override;

		//virtual VkSubmitInfo compute() override;
		
		// -------------------- render_target methods --------------- //

		virtual gcl::command_batch render(stage_t* aStage) override;

		virtual VkResult next_frame() override;
		//virtual std::vector<gcl::command_list> draw(const std::vector<object_t*>& aObject) override;
		virtual VkPresentInfoKHR present_frame() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //


	private:

		glm::ivec2									PositionVSC;
		glm::ivec2									SizeVSC;
		system_display*							Display;
		glfw_window*							Window;
		VkSurfaceKHR							Surface;
		VkPipelineStageFlags					PipelineStageFlags;
		VkResult								PresentationResult;
		VkSemaphore							RenderWait;
		VkSemaphore							RenderComplete;
		VkSwapchainCreateInfoKHR			CreateInfo;
		VkSwapchainKHR						Swapchain;
		VkPresentInfoKHR						PresentInfo;
		std::vector<gcl::command_list>			PreRenderOperations;
		std::vector<gcl::command_list>			PostRenderOperations;

		// ------------------------------ Utility (Internal, Do Not Use) ------------------------------ //

		// Render Target Side.
		VkResult create_system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, glm::ivec2 aPosition, glm::ivec2 aResolution);
		
		// ---------- system_window resources ---------- //

		int create_glfw_window(engine* aEngine, window::setting aSetting, int aWidth, int aHeight, const char* aTitle, system_display* aDisplay);
		VkResult create_vulkan_surface();
		VkResult create_semaphores();
		VkResult create_swapchain(property aProperty);
		VkResult create_images();
		VkResult create_cscbs();

		// ---------- system_window renderer ---------- //

		VkResult create_render_pass();
		VkResult create_pipelines();

		VkResult recreate_swapchain(int aFrameSizeX, int aFrameSizeY);

		void clear();
		void zero_out();

		// ------------------------- Backend ------------------------- //

		struct glfwargs {
			window::setting Property;
			int Width;
			int Height;
			const char* Title;
			glfw_monitor* Monitor;
			glfw_window* Window;
		};

		static bool initialize();
		static void terminate();
		static void poll_events();

		// Signals to update thread to create window handle
		// Needed backend for system window creation
		static std::thread::id MainThreadID;
		static std::atomic<bool> SignalCreate;
		static std::atomic<bool> WindowCreated;
		static glfwargs WindowTempData;
		static glfw_window* ReturnWindow;
		static std::atomic<glfw_window*> DestroyWindow;

		// This is necessary for main thread to create window handles.
		static glfw_window* create_window_handle(core::object::window::setting aProperty, int aWidth, int aHeight, const char* aTitle, glfw_monitor* aMonitor, glfw_window* aWindow); 

		static void destroy_window_handle(glfw_window* aWindow);
		// This function is by the engine update thread to create and destroy handles.
		static void process_window_handle_call();

		// ------------------------------ Callbacks (Internal, Do Not Use) ------------------------------ //

		// Window Callbacks
		static void position_callback(glfw_window* ContextHandle, int PosX, int PosY);
		static void size_callback(glfw_window* ContextHandle, int SizeX, int SizeY);
		static void close_callback(glfw_window* ContextHandle);
		static void refresh_callback(glfw_window* ContextHandle);
		static void focus_callback(glfw_window* ContextHandle, int Focused);
		static void iconify_callback(glfw_window* ContextHandle, int Iconified);
		static void maximize_callback(glfw_window* ContextHandle, int Maximized);
		static void content_scale_callback(glfw_window* ContextHandle, float XScale, float YScale);

		// Framebuffer Callbacks
		static void framebuffer_size_callback(glfw_window* ContextHandle, int FrameSizeX, int FrameSizeY);

		// Cursor Callback
		static void mouse_button_callback(glfw_window* ContextHandle, int Button, int Action, int Mods);
		static void cursor_position_callback(glfw_window* ContextHandle, double PosX, double PosY);
		static void cursor_enter_callback(glfw_window* ContextHandle, int Entered);
		static void scroll_callback(glfw_window* ContextHandle, double OffsetX, double OffsetY);

		// Keyboard Callback
		static void key_callback(glfw_window* ContextHandle, int Key, int Scancode, int Action, int Mods);
		static void character_callback(glfw_window* ContextHandle, unsigned int Codepoint);

		// File drop
		static void file_drop_callback(glfw_window* ContextHandle, int PathCount, const char** Path);


	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H
