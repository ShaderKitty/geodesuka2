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
		glm::ivec2 PositionVSC;
		glm::ivec2 SizeVSC;

		glm::vec3 convert_to_physical_position(glm::ivec2 aPosition);
		glm::vec2 convert_to_physical_size(glm::ivec2 aSize);
		glm::ivec2 convert_to_vsc_position(glm::vec3 aPosition);
		glm::ivec2 convert_to_vsc_size(glm::vec2 aSize);

		glm::vec2 convert_to_physical(glm::ivec2 aVector);
		glm::ivec2 convert_to_screen(glm::vec2 aVector);

		// Converts GLFW screen coordinates to local display coordinates.
		glm::ivec2 convert_to_display_position(glm::ivec2 aPosition, glm::ivec2 aSize);
		glm::ivec2 convert_to_global_position(glm::ivec2 aPosition, glm::ivec2 aSize);

	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H
