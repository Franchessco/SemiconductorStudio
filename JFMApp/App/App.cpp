#include "pch.hpp"
#include "App.hpp"

namespace JFMApp {
	App::App(const AppServiceBundle& services)
	{
		m_numerics = services.numerics;
		m_dataLoader = services.dataLoader;

		//setup callbacks

	}

	void App::init() {
		m_state.nConfig = Data::NumericsConfig{};


	}

	void App::draw() {

	}

	void App::update() {

	}

}
