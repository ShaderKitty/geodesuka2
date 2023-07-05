#pragma once
#ifndef GEODESUKA_CORE_OBJECT_T_H
#define GEODESUKA_CORE_OBJECT_T_H

// ------------------------------ object_t.h ------------------------------ //
/*
* object_t will serve as a generic object that is extendable for the user
* to implement custom object derivative classes. cube.h is an example of this
* and more examples will be provided. The difference here is that the extended
* classes of object_t will have full control over how they will be rendered to
* the target window.h or camera.h object.
* 
* Since an object will have part of itself associated with memory objects
* of particular GPUs, it is important to recognize that objects exist within
* a common space in which they are rendered. On top of that, a rendering context
* will indicate which graphics capable device it exists on. Maybe figure out
* how to stream line 
*/

#include <vector>
#include <map>

#include <atomic>
#include <mutex>

#include "./math.h"

#include "gcl/command_list.h"
#include "gcl/device.h"
#include "gcl/context.h"

#include "gcl/pipeline.h"

#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/model.h"
#include "graphics/render_operation.h"

#include "hid/keyboard.h"
#include "hid/mouse.h"
#include "hid/joystick.h"

namespace geodesuka::core {
	namespace object {
		class window;
		class camera2d;
		class camera3d;
	}

	class stage_t;

	namespace stage {
		class canvas;
		class scene2d;
		class scene3d;
	}
}

namespace geodesuka::core {

	class object_t {
	public:

		friend class geodesuka::engine;
		friend class stage_t;
		friend class object::render_target;
		friend class object_list;

		class default_renderer : public graphics::render_operation {
		public:

			default_renderer(gcl::context* aContext, object::window* aWindow, object_t* aObject);
			default_renderer(gcl::context* aContext, object::camera2d* aCamera2D, object_t* aObject);
			default_renderer(gcl::context* aContext, object::camera3d* aCamera3D, object_t* aObject);

		private:

		};

		friend class default_renderer;

		enum state {
			CREATION,
			READY,
			DESTRUCTION
		};

		enum blending {
			OPAQUE,
			TRANSLUCENT,
			TRANSPARENT
		};

		virtual ~object_t();

		virtual void set_position(glm::vec3 aPosition);
		glm::vec3 get_position() const;

		virtual void set_name(const char* aName);
		const char* get_name() const;

		virtual void predraw(object::render_target* aRenderTarget);
		void operator-=(object::render_target* aRenderTarget);

		// Will provide a command list on how this object will be rendered to target.
		virtual gcl::command_list draw(object::render_target* aRenderTarget);

	protected:

		struct object_ubo {
			glm::mat4 Translation;
			glm::mat4 Orientation;
		};

		// Used for shared usage between Engine & App.
		std::mutex					Mutex;
		std::atomic<state>			State;
		engine*						Engine;
		gcl::context*				Context;
		stage_t*					Stage;
		util::string				Name;

		glm::vec3						InputVelocity;
		glm::vec3						InputForce;
		float						Mass;				// Kilogram			[kg]
		float						Time;				// Second 			[s]
		glm::vec3						Position;			// Meter			[m]
		glm::vec3						Momentum;			// Linear Momentum	[kg*m/s]
		glm::vec3						AngularMomentum;	// Angular Momentum [kg*m/s]
		glm::vec3						Force;				// Newton			[kg*m^2/s^2]
		glm::vec3						DirectionX;			// Right			[Normalized]
		glm::vec3						DirectionY;			// Up				[Normalized]
		glm::vec3						DirectionZ;			// Forward			[Normalized]

		blending					Blending;
		graphics::model*			Model;

		VkDescriptorPool			DescriptorPool;
		gcl::buffer					ObjectUBO;
		VkDescriptorSet			ObjectDS;

		std::map<object::render_target*, graphics::render_operation*> RenderOperation;

		object_t(gcl::context* aContext, stage_t* aStage, const char* aName);

		// If input stream is being forwarded to a particular instance of an object,
		// this method will be called depending on input recieved by the engine. This
		// method is effectively a forwarded callback stream for the particular instance
		// of the object.
		virtual void input(const hid::mouse& aMouse, const hid::keyboard& aKeyboard);

		// The implementation of this method will allow the object to internally time evolve
		// itself with a provided time step. It will be the responsibility of the engine
		// to insure that the time step remains stable.
		virtual VkSubmitInfo update(double aDeltaTime);

		// Will produce compute operation submissions.
		virtual VkSubmitInfo compute();

	};

}

#endif // !GEODESUKA_CORE_OBJECT_T_H
