#include <geodesuka/engine.h>

#include <geodesuka/core/app.h>

namespace geodesuka::core {

	app::app(engine* aEngine) {
		this->Engine = aEngine;
		this->TimeStep = 1.0;
		this->Name = "";
		this->Version = { 1, 0, 0 };
	}

	// This is used engine side to generate thread for Application.
	void app::prerun() {
		// App is now ready to be run.
		this->ExitApp.store(false);
		this->Engine->AppThreadID = std::this_thread::get_id();
		// Initializes game loop.
		this->run();
		// Forces all threads to finish.
		this->Engine->ThreadController.terminate_all();
	}

}
