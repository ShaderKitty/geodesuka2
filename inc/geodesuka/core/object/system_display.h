#pragma once
#ifndef GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H
#define GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H

#include "../object_t.h"
#include "window.h"

struct GLFWmonitor;
struct GLFWvidmode;

typedef GLFWmonitor glfw_monitor;
typedef GLFWvidmode glfw_vidmode;

namespace geodesuka::core::object {

	class system_display : public window {
	public:

		friend class geodesuka::engine;
		friend class system_window;

		static constexpr int ID = 1;

		// TODO: Move this constructor to private.
		system_display(engine *aEngine, glfw_monitor* aMonitor, const char* aName);
		~system_display();

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	private:

		glfw_monitor* Handle;
		//VkDisplayKHR Handle;

		int VideoModeCount;
		glfw_vidmode* VideoMode;
		const glfw_vidmode* CurrentVideoMode;

		// Used internally to interact with OS size and positioning.
		int2 PositionVSC;
		int2 SizeVSC;

		float3 convert_to_physical_position(int2 aPosition);
		float2 convert_to_physical_size(int2 aSize);
		int2 convert_to_vsc_position(float3 aPosition);
		int2 convert_to_vsc_size(float2 aSize);

		float2 convert_to_physical(int2 aVector);
		int2 convert_to_screen(float2 aVector);

		// Converts GLFW screen coordinates to local display coordinates.
		int2 convert_to_display_position(int2 aPosition, int2 aSize);
		int2 convert_to_global_position(int2 aPosition, int2 aSize);

	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H
