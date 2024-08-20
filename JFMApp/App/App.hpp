#pragma once

#include "pch.hpp"
#include "ViewData.hpp"
#include "INumerics.hpp"
#include "IDataLoader.hpp"

namespace JFMApp {

	struct AppServiceBundle {
		std::shared_ptr<JFMServices::INumerics> numerics{};
		std::shared_ptr<JFMServices::IDataLoader> dataLoader{};
	};

	class App
	{
	public:
		App() = delete;
		App(const AppServiceBundle& services);
		~App() = default;

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		App(App&&) = delete;
		App& operator=(App&&) = delete;

		void update();
		void draw();

	private:
		Data::ViewData m_state{};
		std::shared_ptr<JFMServices::INumerics> m_numerics{};
		std::shared_ptr<JFMServices::IDataLoader> m_dataLoader{};
	};
}



