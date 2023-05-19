#pragma once
#ifndef GEODESUKA_CORE_APP_H
#define GEODESUKA_CORE_APP_H

#include <atomic>

#include "../config.h"

namespace geodesuka::core {

	struct version {
		int Major;
		int Minor;
		int Revision;
	};

	class app {
	public:

		friend class geodesuka::engine;

		std::atomic<bool> ExitApp;

		virtual ~app() = default;

	protected:

		engine* 				Engine;
		//engine::state& State;
		std::atomic<double> 	TimeStep;
		const char* 			Name;
		version 				Version;		

		app(engine* aEngine);

		virtual void run() = 0;

	private:

		void prerun();

	};

}

#endif // !GEODESUKA_CORE_APP_H

