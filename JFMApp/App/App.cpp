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
		//get the numerics config
		//setup all of the pointers
		//provide the callbacks
		//init the selection vector for the file browser
		//init the root path
		//


	}

	void App::draw() {
		//get the mainviewport dockspace
		//dock prograpatically all of the sections
		//ViewMenu is the menu of the main window
		//Next plotting area
		//next content browser
		//	Characteristics table
		//	File browser
		//	Characteristics inspector - by default

		//displaying MC as a separate window
	}

	void App::update() {
		//update the characteristic list and active characteristic
		//update the parameters according to the model of the active characteristic for MC choice
		//
	}

}
