#pragma once

#include "pch.hpp"
#include "App.hpp"
#include "window.hpp"

namespace JFMApp
{
	class AppRunner
	{
	public:
		AppRunner();
		~AppRunner() = default;

		AppRunner(const AppRunner&) = delete;
		AppRunner& operator=(const AppRunner&) = delete;

		AppRunner(AppRunner&&) = delete;
		AppRunner& operator=(AppRunner&&) = delete;

		void run();

		void addApp(const AppServiceBundle& services);

	private:
		std::vector<std::unique_ptr<App>> m_apps{};

	};
}



