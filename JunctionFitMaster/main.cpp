#include "AppRunner.hpp"

int main()
{
	JFMApp::AppRunner runner{};

	JFMApp::AppServiceBundle bundle{ nullptr, nullptr };
	runner.addApp(bundle);

	runner.run();

	return 0;
}