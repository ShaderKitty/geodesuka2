#include <geodesuka/engine.h>

/* --------------- Standard C Libraries --------------- */
#include <cstdlib>
#include <cstring>
#include <cassert>

/* --------------- Standard C++ Libraries --------------- */

#include <sstream>
#include <vector>
#include <chrono>

/* --------------- Third Party Libraries --------------- */

#include <portaudio.h>

#include <GLFW/glfw3.h>

namespace geodesuka {

	bool engine::ThirdPartyLibrariesInitialized = false;
	int engine::EngineInstanceCount = 0;

	using namespace core;

	using namespace math;
	using namespace util;
	using namespace logic;
	using namespace io;
	using namespace hid;
	using namespace gcl;
	using namespace graphics;
	using namespace object;
	using namespace stage;

	engine::engine(int aCmdArgCount, const char** aCmdArgList, int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList) {
		bool isVulkanReady				= false;
		bool isEngineResourcesReady 	= false;

		EngineInstanceCount				+= 1;
		this->Name 						= "Fred";
		this->Handle					= VK_NULL_HANDLE;

		// --------------- Third Party Library Initialization Process --------------- //
		// Initializes all third party libraries the geodesuka engine relies on. This
		// is done on a per-process basis, so it is treated globally.

		if (!ThirdPartyLibrariesInitialized) {
			ThirdPartyLibrariesInitialized = this->initialize_third_party_libraries();
			if (ThirdPartyLibrariesInitialized) {
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Initialization Success!");
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Initialization Failure!");
			}
		} else {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Already Initialized!");
		}

		// --------------- Vulkan Initialization Process --------------- //
		// Creates a Vulkan instance for Graphics & Computation operations
		// to be performed.

		// (Vulkan) Load required window extensions.
		if (ThirdPartyLibrariesInitialized) {
			isVulkanReady = this->initialize_vulkan_instance(aLayerCount, aLayerList, aExtensionCount, aExtensionList);
		}

		// --------------- Engine Resource Initialization Process --------------- //
		// Initialize all starting engine resources such as desktop stages, system_displays
		// and system_terminal.

		// APIs initialized, query for hardware options.
		if (ThirdPartyLibrariesInitialized && isVulkanReady) {
			isEngineResourcesReady = this->initialize_engine_resources();
			if (isEngineResourcesReady) {
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Engine Resources: Initialization Success!");
			} else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Engine Resources: Initialization Failure!");
			}
		}

		// Store main thread ID.
		this->MainThreadID = std::this_thread::get_id();
		
		// Is ready if startup condition = success.
		this->State = ThirdPartyLibrariesInitialized && isVulkanReady && isEngineResourcesReady ? state::READY : state::FAILURE;
		if (this->State == state::READY) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Geodesuka Engine: Startup Successful!");
		} else {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Geodesuka Engine: Startup Failure!");
		}
	}

	engine::~engine() {

		EngineInstanceCount -= 1;

		this->State = state::DESTRUCTION;

		this->destroy_engine_resources();

		this->destroy_vulkan_instance();

		if (EngineInstanceCount == 0) {
			this->terminate_third_party_libraries();
		}

	}

	void engine::operator>>(core::util::string& aRhs) {
		*this->SystemTerminal >> aRhs;
	}

	void engine::operator<<(core::util::string& aRhs) {
		*this->SystemTerminal << aRhs;
	}

	void engine::operator<<(const char* aRhs) {
		*this->SystemTerminal << aRhs;
	}

	void engine::operator<<(const core::util::log::message aNewLogMessage) {
		// Add new log to engine log.
		this->Log << aNewLogMessage;
	}

	file* engine::open(const char* aFilePath) {
		return this->File.open(aFilePath);
	}

	void engine::close(file* aFile) {

	}

	vk_instance engine::handle() {
		return Handle;
	}

	core::version engine::get_version() {
		return Version;
	}

	int engine::get_date() {
		return Date;
	}

	bool engine::is_ready() {
		return (this->State == state::READY);
	}

	int engine::run(app* aApp) {

		this->Log << log::message("engine", log::INFO, log::SUCCESS, "Update Thread");

		this->State = state::RUNNING;
		RenderThread = std::thread(&engine::render, this);
		//AudioThread = std::thread(&engine::audio, this);
		//SystemTerminalThread = std::thread(&engine::terminal, this);
		AppThread = std::thread(&app::prerun, aApp);

		this->update(aApp);

		AppThread.join();
		//SystemTerminalThread.join();
		//AudioThread.join();
		RenderThread.join();
		this->State = state::READY;

		return 0;
	}

	int engine::return_value() {
		*this->SystemTerminal << this->Log;
		return this->Log[this->Log.count() - 1].Code;
	}

	bool engine::initialize_third_party_libraries() {
		PaError PortAudioErrorCode 		= paNoError;
		bool isGLSLANGReady				= false;
		bool isFreeTypeReady			= false;
		bool isFreeImageReady			= false;
		bool isAssimpReady				= false;
		bool isPortAudioReady			= false;
		bool isGLFWReady				= false;

		// glslang - SPIRV compiler
		isGLSLANGReady = shader::initialize();
		if (isGLSLANGReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GLSLANG, "engine", this->Name.Handle, "glslang (SPIRV Compiler Tools): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GLSLANG, "engine", this->Name.Handle, "glslang (SPIRV Compiler Tools): Initialization Failure!");
		}

		// freetype - Font Loading Library
		isFreeTypeReady = font::initialize();
		if (isFreeTypeReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::FREETYPE, "engine", this->Name.Handle, "FreeType (Font Loading Library): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::FREETYPE, "engine", this->Name.Handle, "FreeType (Font Loading Library): Initialization Failure!");
		}

		// freeimage - Image Loading Library.
		isFreeImageReady = image::initialize();
		if (isFreeImageReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::FREEIMAGE, "engine", this->Name.Handle, "FreeImage (Image Loading Library): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::FREEIMAGE, "engine", this->Name.Handle, "FreeImage (Image Loading Library): Initialization Failure!");
		}

		// Initialize Model Loading Library.
		isAssimpReady = model::initialize();
		if (isAssimpReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::ASSIMP, "engine", this->Name.Handle, "Assimp (Model Loading Library): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::ASSIMP, "engine", this->Name.Handle, "Assimp (Model Loading Library): Initialization Failure!");
		}

		PortAudioErrorCode = Pa_Initialize();
		isPortAudioReady = (PortAudioErrorCode == paNoError);
		if (isPortAudioReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::PORTAUDIO, "engine", this->Name.Handle, "PortAudio (System Audio Interface): Initialization Success!");
		} else {
			this->Log << log::message(PortAudioErrorCode, "Engine Startup", log::PORTAUDIO, "engine", this->Name.Handle, "PortAudio (System Audio Interface): Initialization Failure!");
		}

		// glfw - operating system window handling.
		isGLFWReady = system_window::initialize();
		if (isGLFWReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GLFW, "engine", this->Name.Handle, "GLFW (System Window Interface): Initialization Success!");
		} else {
			const char* ReturnError;
			int ReturnErrorCode = glfwGetError(&ReturnError);
			this->Log << log::message(ReturnErrorCode, "Engine Startup", log::GLFW, "engine", this->Name.Handle, "GLFW (System Window Interface): Initialization Failure!");
			this->Log << log::message(ReturnErrorCode, "Engine Startup", log::GLFW, "engine", this->Name.Handle, ReturnError);
		}

		return (isGLSLANGReady && isFreeTypeReady && isFreeImageReady && isAssimpReady && isPortAudioReady && isGLFWReady);
	}

	void engine::terminate_third_party_libraries() {
		system_window::terminate();

		Pa_Terminate();

		model::terminate();

		image::terminate();

		font::terminate();

		shader::terminate();
	}

	bool engine::initialize_vulkan_instance(int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList) {

		// Adds WSI instance extensions to list.
		for (size_t i = 0; i < system_window::RequiredInstanceExtension.size(); i++) {
			Extension.push_back(system_window::RequiredInstanceExtension[i]);
		}

		// Adds proposed layers to list.
		for (int i = 0; i < aLayerCount; i++) {
			Layer.push_back(aLayerList[i]);
		}

		// Adds proposed extensions.
		for (int i = 0; i < aExtensionCount; i++) {
			Extension.push_back(aExtensionList[i]);
		}

		//"VK_KHR_display";
		// Does not work on Windows OS right now, only Linux
		//this->Extension.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);		

		AppInfo.sType						= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext						= NULL;
		AppInfo.pApplicationName			= "";
		AppInfo.applicationVersion			= VK_MAKE_VERSION(1, 0, 0);
		AppInfo.pEngineName					= "Geodesuka Engine";
		AppInfo.engineVersion				= VK_MAKE_VERSION(Version.Major, Version.Minor, Version.Revision);
		AppInfo.apiVersion					= VK_MAKE_VERSION(1, 2, 0);

		CreateInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateInfo.pNext					= NULL;
		CreateInfo.flags					= 0;
		CreateInfo.pApplicationInfo			= &AppInfo;
		CreateInfo.enabledLayerCount		= (uint32_t)Layer.size();
		CreateInfo.ppEnabledLayerNames		= Layer.data();
		CreateInfo.enabledExtensionCount	= (uint32_t)Extension.size();
		CreateInfo.ppEnabledExtensionNames	= Extension.data();

		vk_result Result = vkCreateInstance(&CreateInfo, NULL, &Handle);
		if (Result == VK_SUCCESS) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::VULKAN, "engine", this->Name.Handle, "Vulkan (Graphics & Computation API): Initialization Success!");
			return true;
		} else {
			this->Log << log::message(Result, "Engine Startup", log::VULKAN, "engine", this->Name.Handle, "Vulkan (Graphics & Computation API): Initialization Failure!");
			return false;
		}
	}

	void engine::destroy_vulkan_instance() {
		if (this->Handle != VK_NULL_HANDLE) {
			vkDestroyInstance(Handle, NULL);
			this->Handle = VK_NULL_HANDLE;
		}
	}

	bool engine::initialize_engine_resources() {
		// Checklist:
		// DisplayCount > 0
		// DeviceCount > 0
		// At least one device supports TCGP

		bool isSystemDisplayAvailable		= false;
		bool isGCDeviceAvailable			= false;

		this->PrimaryDevice = nullptr;
		this->PrimaryDisplay = nullptr;

		// Create System Terminal for logging output and getting user commands. 
		this->SystemTerminal = new system_terminal(this, "SystemTerminal");
		if (this->SystemTerminal != nullptr) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "System Terminal: Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "System Terminal: Initialization Failure!");
			return false;
		}

		// Use GLFW API to query for all physical displays.
		{
			int PhysicalMonitorCount = 0;
			glfw_monitor** PhysicalMonitor = NULL;
			PhysicalMonitor = glfwGetMonitors(&PhysicalMonitorCount);
			if (PhysicalMonitorCount > 0) {
				std::stringstream MainStream;
				MainStream << "System Display Count: " << PhysicalMonitorCount;
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "system_display", "", MainStream.str().c_str());
				this->Display.resize(PhysicalMonitorCount);
				for (int i = 0; i < PhysicalMonitorCount; i++) {
					// Create New Display
					object::system_display* NewDisplay = new system_display(this, PhysicalMonitor[i], "");
					if (NewDisplay == nullptr) {
						this->Log << log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "SystemDisplay: Insuffient Memory for Display Instance!");
						return false;
					}

					// Add to list.
					this->Display[i] = NewDisplay;
					if (PhysicalMonitor[i] == glfwGetPrimaryMonitor()) {
						this->PrimaryDisplay = NewDisplay;
					}
				}
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "No System Displays have been detected!");
				return false;
			}
		}

		// Use Vulkan API to query for all Graphics & Computation capable devices.
		{
			uint32_t PhysicalDeviceCount = 0;
			vk_result Result = vkEnumeratePhysicalDevices(this->Handle, &PhysicalDeviceCount, NULL);
			if (PhysicalDeviceCount > 0) {
				std::stringstream MainStream;
				std::vector<vk_physical_device> PhysicalDevice(PhysicalDeviceCount);

				MainStream << "Physical Device Count: " << PhysicalDeviceCount;
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "device", "", MainStream.str().c_str());
				Result = vkEnumeratePhysicalDevices(this->Handle, &PhysicalDeviceCount, PhysicalDevice.data());
				this->Device.resize(PhysicalDeviceCount);
				for (uint32_t i = 0; i < PhysicalDeviceCount; i++) {
					this->Device[i] = new device(this, PhysicalDevice[i]);
					if (this->Device[i] == nullptr) {
						this->Log << log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Insuffient Memory for Physical Device Instance!");
						return false;
					}
				}
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "No Graphics & Computation Devices Detected!");
				return false;
			}
		}

		// Determine Primary Device.
		for (int i = 0; i < this->Device.size(); i++) {
			if (this->Device[i]->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				this->PrimaryDevice = this->Device[i];
				break;
			}
		}

		// Construct Desktop Stages.
		this->Desktop.resize(this->Display.size());
		for (int i = 0; i < this->Desktop.size(); i++) {
			this->Desktop[i] = new desktop(this, (object::system_display*)Display[i]);
			this->Display[i]->Stage = this->Desktop[i];
		}

		return ((Display.size() > 0) && (Device.size() > 0));
	}

	void engine::destroy_engine_resources() {
		for (int i = 0; i < Stage.count(); i++) {
			delete Stage[i];
			Stage[i] = nullptr;
		}

		for (int i = 0; i < Context.count(); i++) {
			delete Context[i];
			Context[i] = nullptr;
		}

		//for (int i = 0; i < File.count(); i++) {
		//	delete File[i];
		//	File[i] = nullptr;
		//}

		Stage.clear();
		Context.clear();
		//File.clear();

		for (int i = 0; i < Device.size(); i++) {
			delete Device[i];
			Device[i] = nullptr;
		}

		delete SystemTerminal;
	}

	// --------------- Engine Main Thread --------------- //
	// The main thread is used to spawn backend threads along
	// with the app thread.
	// --------------- Engine Main Thread --------------- //
	void engine::update(core::app* aApp) {
		vk_result Result = VK_SUCCESS;

		this->MainThreadID = std::this_thread::get_id();
		while (ThreadController.cycle(aApp->TimeStep.load())) {

			// Process system_window Creation/Destruction calls.
			system_window::process_window_handle_call();

			// Poll Input Events.
			system_window::poll_events();

			// --------------- Host work is done here --------------- //

			this->update_host_resources(aApp->TimeStep.load());

			// --------------- Per Device Context work is done here --------------- //

			Result = this->update_device_resources();

		}

	}

	// --------------- Render Thread --------------- //
	// The job of the render thread is to honor and schedule draw
	// calls of respective render targets stored in memory.
	// --------------- Render Thread --------------- //
	void engine::render() {
		vk_result Result = VK_SUCCESS;

		this->Log << log::message("engine", log::INFO, log::SUCCESS, "Render Thread");

		this->RenderThreadID = std::this_thread::get_id();
		while (ThreadController.cycle(0.0)) {
			//*this->State.SystemTerminal << "RenderThread\n";

			this->gather_render_operations();

			Result = this->submit_render_operations();

		}

	}

	void engine::audio() {
		this->Log << log::message("engine", log::INFO, log::SUCCESS, "Audio Thread");

		// Does nothing currently.
		this->AudioThreadID = std::this_thread::get_id();
		while (ThreadController.cycle(0.25)) {
			//*this->State.SystemTerminal << "AudioThread\n";

		}

	}

	// --------------- System Terminal Thread --------------- //
	// Will be used for runtime debugging of engine using terminal.
	// --------------- System Terminal Thread --------------- //
	void engine::terminal() {
		this->Log << log::message("engine", log::INFO, log::SUCCESS, "Terminal Thread");

		this->SystemTerminalThreadID = std::this_thread::get_id();
		while (ThreadController.cycle(0.0)) {
			util::string Command;

			*this->SystemTerminal >> Command;

			if (Command == "exit") {
				this->ThreadController.terminate_all();
			}
		}

	}

	void engine::update_host_resources(double aDeltaTime) {
		// Update all objects independent of stage, and acquire all transfer operations.
		for (int i = 0; i < Context.count(); i++) {
			if (!Context[i]->EnableProcessing.load()) continue;
			for (int j = 0; j < Stage.count(); j++) {
				if ((!this->Stage[j]->State == stage_t::state::READY) || (this->Stage[j]->Context != Context[i])) continue;

				// Get all command batches for each unique object per stage.
				Context[i]->BackBatch[0] += Stage[j]->update_owned_objects(aDeltaTime);
				Context[i]->BackBatch[1] += Stage[j]->compute_owned_objects();

				// Gather all command batches stage wide.
				Context[i]->BackBatch[0] += Stage[i]->update(aDeltaTime);
				Context[i]->BackBatch[1] += Stage[i]->compute();
			}
		}
	}

	vk_result engine::update_device_resources() {
		vk_result Result = VK_SUCCESS;
		for (int i = 0; i < Context.count(); i++) {
			// Go to next context if not ready.
			if (!Context[i]->EnableProcessing.load()) continue;

			// Lock Context for execution.
			Context[i]->ExecutionMutex.lock();

			// Iterate through all workbatches and search for inflight operations.
			for (int j = 0; j < 3; j++) {
				if (Context[i]->WorkBatch[j].Submission.size() > 0) {
					vkWaitForFences(Context[i]->Handle, 1, &Context[i]->ExecutionFence[j], VK_TRUE, UINT64_MAX);
					vkResetFences(Context[i]->Handle, 1, &Context[i]->ExecutionFence[j]);
					Context[i]->WorkBatch[j].clear();
				}
			}

			// Check if either transfer back batch, or compute back batch have accumulated submission.
			if ((Context[i]->BackBatch[0].Submission.size() > 0) || (Context[i]->BackBatch[1].Submission.size() > 0)) {
				// Loads back batch, ready for execution.
				Context[i]->WorkBatch[0] = Context[i]->BackBatch[0];
				Context[i]->WorkBatch[1] = Context[i]->BackBatch[1];

				// Clears back batch for another run through.
				Context[i]->BackBatch[0].clear();
				Context[i]->BackBatch[1].clear();

				// Purify set from all empyt [Submit/Present]Infos
				Context[i]->WorkBatch[0].purify();
				Context[i]->WorkBatch[1].purify();

				// Submit Current Transfer Workload.
				if (Context[i]->WorkBatch[0].Submission.size() > 0) {
					Context[i]->execute(device::TRANSFER, Context[i]->WorkBatch[0], Context[i]->ExecutionFence[0]);
				}

				// Submit Current Compute Workload.
				if (Context[i]->WorkBatch[1].Submission.size() > 0) {
					Context[i]->execute(device::COMPUTE, Context[i]->WorkBatch[1], Context[i]->ExecutionFence[1]);
				}
			}

			// Release context from execution lock.
			Context[i]->ExecutionMutex.unlock();
		}

		return Result;
	}

	void engine::gather_render_operations() {
		// Aggregate all render operations from each stage to each context.
		for (int i = 0; i < Context.count(); i++) {
			// Go to next context if not ready.
			if (!Context[i]->EnableProcessing.load()) continue;
			for (int j = 1; j < Stage.count(); j++) {
				// Go to next stage if stage is not ready.
				if ((!this->Stage[j]->State == stage_t::state::READY) && (Stage[j]->Context == Context[i])) {
					Context.Handle[i]->BackBatch[2] += Stage[j]->render();
				}
			}
		}
	}

	vk_result engine::submit_render_operations() {
		// Per Context/GPU works is submitted in this section.
		vk_result Result = VK_SUCCESS;
		for (int i = 0; i < Context.count(); i++) {
			// Go to next context if not ready.
			if (!Context[i]->EnableProcessing.load()) continue;

			// If no operations, continue and check next context.
			if ((Context[i]->BackBatch[2].Submission.size() == 0) && (Context[i]->BackBatch[2].Presentation.size() == 0)) continue;

			// Lock Context for execution.
			Context[i]->ExecutionMutex.lock();

			// Iterate through all workbatches and search for inflight operations.
			for (int j = 0; j < 3; j++) {
				if (Context[i]->WorkBatch[j].Submission.size() > 0) {
					vkWaitForFences(Context[i]->Handle, 1, &Context[i]->ExecutionFence[j], VK_TRUE, UINT64_MAX);
					vkResetFences(Context[i]->Handle, 1, &Context[i]->ExecutionFence[j]);
					Context[i]->WorkBatch[j].clear();
				}
			}

			// Check for Graphics & Compute operations accumulated.
			if ((Context[i]->BackBatch[2].Submission.size() > 0) || (Context[i]->BackBatch[2].Presentation.size() > 0)) {
				// Loads back batch, ready for execution.
				Context[i]->WorkBatch[2] = Context[i]->BackBatch[2];

				// Clears back batch for another run through.
				Context[i]->BackBatch[2].clear();

				// Purify set from all empyt [Submit/Present]Infos
				Context[i]->WorkBatch[2].purify();

				// Submit Current Graphics & Compute Workloads.
				if (Context[i]->WorkBatch[2].Submission.size() > 0) {
					Result = Context[i]->execute(device::GRAPHICS_AND_COMPUTE, Context[i]->WorkBatch[2], Context[i]->ExecutionFence[2]);
					//Result = Context[i]->execute(device::GRAPHICS_AND_COMPUTE, Context[i]->WorkBatch[2].Submission, Context[i]->ExecutionFence[2]);
				}

				// Submit All Presentation Commands. (Note: this should not be very often unless lots of system_windows)
				if (Context[i]->WorkBatch[2].Presentation.size() > 0) {
					Result = Context[i]->execute(device::PRESENT, Context[i]->WorkBatch[2], Context[i]->ExecutionFence[2]);
					//Result = Context[i]->execute(Context[i]->WorkBatch[2].Presentation, Context[i]->ExecutionFence[2]);
				}
			}
			// Release context from executin lock.
			Context[i]->ExecutionMutex.unlock();
		}
		return Result;
	}

}
