#pragma once
#ifndef GEODESUKA_ENGINE_H
#define GEODESUKA_ENGINE_H

/*

// ------------------------- TODO: ------------------------- \\

// ------------------------- Notes ------------------------- \\
As of right now geodesuka is the primary namespace. Engine
is the factory manager of all objects, including memory. Can
be used to 

\\ ------------------------- Notes ------------------------- //
*/

/* --------------- System Macros (Tweak) --------------- */

/*
Disabled Warnings:
C4244
C6386
C26451

Include: $(ProjectDir)..\glslang

*/

/* --------------- Standard C Libraries --------------- */

/* --------------- Standard C++ Libraries --------------- */
#include <vector>
#include <chrono>

#include <thread>
#include <mutex>
#include <atomic>

// ----- Core Libraries of the Geodesuka Engine ----- //
#include "config.h"
#include "core/math.h"
#include "core/util.h"
#include "core/logic.h"
#include "core/io.h"
#include "core/hid.h"
#include "core/gcl.h"
#include "core/graphics.h"
#include "core/sound.h"
#include "core/object.h"
#include "core/stage.h"
#include "core/app.h"

namespace geodesuka {

	class engine {
	public:

		// So Engine internals can be accessed.
		friend class core::gcl::context;
		friend class core::object_t;
		friend class core::object::system_window;
		friend class core::stage_t;
		friend class core::app;

		enum state {
			FAILURE = -1,
			CREATION,						// Engine instance is being constructed.
			READY,							// Engine instance is active, and ready to be used.
			RUNNING,						// Threads are launched and running backend.
			DESTRUCTION						// Engine is currently in destruction phase.
		};

		core::util::string 								Name;
		core::util::log									Log;
		core::util::string								WorkingDirectory;

		core::object::system_terminal*					SystemTerminal;
		core::gcl::device*								PrimaryDevice;
		core::object::system_display*					PrimaryDisplay;

		std::vector<core::gcl::device*> 				Device;
		std::vector<core::object::system_display*> 		Display;
		std::vector<core::object::system_window*> 		Window;
		std::vector<core::stage::desktop*> 				Desktop;

		std::atomic<state>								State;
		core::io::file_manager							File;
		core::gcl::context_list							Context;
		core::stage_list								Stage;

		core::logic::thread_controller					ThreadController;
		std::thread::id									MainThreadID;
		std::thread::id									RenderThreadID;
		std::thread::id									AudioThreadID;
		std::thread::id									SystemTerminalThreadID;
		std::thread::id									AppThreadID;

		engine(int aCmdArgCount, const char** aCmdArgList, int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList);
		~engine();

		// Returns true if input is gathered. Returns false if empty line.
		void operator>>(core::util::string& aRhs);

		// Prints output to terminal.
		void operator<<(core::util::string& aRhs);

		// Same as above, but good for string literals.
		void operator<<(const char* aRhs);

		// Used to aggregate engine logs.
		void operator<<(const core::util::log::message aNewLogMessage);

		// TODO: Reference Pools?
		//const char* get_process_directory();
		core::io::file* open(const char* aFilePath);
		void close(core::io::file* aFile);

		vk_instance handle();
		core::version get_version();
		int get_date();

 		bool is_ready();
		int run(core::app* aApp);
		int return_value();

	private:

		const core::version								Version = { 
			GEODESUKA_ENGINE_VERSION_MAJOR, 
			GEODESUKA_ENGINE_VERSION_MINOR, 
			GEODESUKA_ENGINE_VERSION_PATCH 
		};
		const int										Date = GEODESUKA_ENGINE_VERSION_BIRTH_DATE;
		std::vector<const char*>						Layer;
		std::vector<const char*>						Extension;
		// vk::ApplicationInfo								AppInfo;
		// vk::InstanceCreateInfo							CreateInfo;
		// vk::Instance									Handle;
		vk_application_info								AppInfo{};
		vk_instance_create_info							CreateInfo{};
		vk_instance										Handle;

		static bool 									ThirdPartyLibrariesInitialized;
		static int 										EngineInstanceCount;

		// Initialize and terminate third party libraries.
		bool initialize_third_party_libraries();
		void terminate_third_party_libraries();

		// Initializes vulkan instance for engine.
		bool initialize_vulkan_instance(int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList);
		void destroy_vulkan_instance();

		// Initialize all engine resources.
		bool initialize_engine_resources();
		void destroy_engine_resources();

		// ------------------------------ Back end runtime ------------------------------ //

		std::thread										RenderThread;
		std::thread										AudioThread;
		std::thread										SystemTerminalThread;
		std::thread										AppThread;

		void update(core::app* aApp);		// Updates all unique objects, and then stages.
		void render();						// Thread honors frame rates of respective targets.
		void audio();						// Thread Handles audio streams.
		void terminal();					// Thread handles terminal input to the engine.

		void update_host_resources(double aDeltaTime);
		vk_result update_device_resources();
		void gather_render_operations();
		vk_result submit_render_operations();

	};

}

#endif // !GEODESUKA_ENGINE_H
