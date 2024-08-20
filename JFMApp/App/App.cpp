#include "pch.hpp"
#include "App.hpp"

namespace JFMApp {
	App::App(const AppServiceBundle& services)
	{
		m_numerics = services.numerics;
		m_dataLoader = services.dataLoader;

		//setup callbacks

	}

	void App::draw() {

	}

	void App::update() {

	}

}
