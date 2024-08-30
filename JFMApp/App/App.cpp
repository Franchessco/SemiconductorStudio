#include "pch.hpp"
#include "App.hpp"

namespace JFMApp {

	using namespace JFMService::DataManagementService;
	using namespace JFMService::FittingService;

	App::App(const AppServiceBundle& services)
	{
		m_numerics = services.numerics;
		m_dataLoader = services.dataLoader;


		init();
	}

	void App::init() {
		//if (!m_numerics || !m_dataLoader) return;

		//get the numerics config
		//m_state.nConfig = m_numerics->GetConfiguration();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		Data::NumericsConfig nConfig{};

		nConfig.models = std::unordered_map<ModelID, ModelName>{ {0, "6P IV"}, {1, "4P IV"} };
		nConfig.parameters = std::unordered_map<ParameterID, ParameterName>{ {0, "I0"}, {1, "A"}, {2, "Rs"}, {3, "Rsh"}, {4, "Alpha"}, {5, "Rsh2"} };
		nConfig.modelParameters = std::unordered_map<ModelID, std::vector<ParameterID>>{ {0, {0, 1, 2, 3, 4, 5}}, {1, {0, 1, 2, 3}} };
		nConfig.paramBounds = std::unordered_map<ParameterID, Bounds>{ {0, {0.0, 1.0}}, {1, {0.0, 1.0}}, {2, {0.0, 1.0}}, {3, {0.0, 1.0}}, {4, {0.0, 1.0}}, {5, {0.0, 1.0}} };

		m_state.nConfig = nConfig;

		Data::Characteristic ch{};
		ch.name = "Test";
		ch.T = 300;
		ch.modelID = 0;
		ch.savedModelID = 0;
		auto rg = std::ranges::views::iota(1) | std::ranges::views::transform([](double x) {return x * 0.1; }) | std::ranges::views::take(100);
		ch.V = std::ranges::to<std::vector<double>>(rg);
		ch.I = std::ranges::to<std::vector<double>>(rg | std::ranges::views::transform([](double x) {return std::exp(x); }));

		ch.fittedParameters = { {0, 0.1}, {1, 0.1}, {2, 0.1}, {3, 0.1}, {4, 0.1}, {5, 0.1} };
		ch.tunedParameters = ch.fittedParameters;
		ch.fixedParametersValues = ch.fittedParameters;
		ch.savedFixedParametersValues = ch.fittedParameters;
		ch.savedFixedParameterIDs = { {0, false}, {1, false}, {2, false}, {3, false}, {4, false}, {5, false} };
		ch.fixedParameterIDs = ch.savedFixedParameterIDs;
		ch.initialGuess = ch.fittedParameters;
		ch.savedInitialGuess = ch.fittedParameters;
		ch.bounds = nConfig.paramBounds;
		ch.savedBounds = nConfig.paramBounds;

		m_state.plotData.globalModelID = 0;
		m_state.plotData.savedGlobalModelID = 0;
		m_state.plotData.savedGlobalFixedParameterIDs = { {0, false}, {1, false}, {2, false}, {3, false}, {4, false}, {5, false} };
		m_state.plotData.globalFixedParameterIDs = m_state.plotData.savedGlobalFixedParameterIDs;

		m_state.browserData.m_characteristics.push_back(ch);

		Data::Characteristic::MCSimulation mcSim{};
		
		for (int i = 0; i < 30; i++) {
			Data::Characteristic::MCData data{};

			data.parameters[0] = i;
			data.parameters[1] = i;
			data.parameters[2] = i;
			data.parameters[3] = i;
			data.parameters[4] = i;
			data.parameters[5] = i;
			data.error = i;

			mcSim.data.push_back(data);
		}

		mcSim.fixConfig = { {0, 0.1}, {2, 1.0}, {4, 0.1} };

		mcSim.parent = &m_state.browserData.m_characteristics[0];
		m_state.browserData.m_characteristics[0].mcData.push_back(mcSim);

		m_state.plotData.mcTabs.push_back(1);
		

		// Disable the .ini file by setting IniFilename to nullptr
		io.IniFilename = nullptr;

		//setup all of the pointers
		//plot data
		m_state.plotData.characteristics = &m_state.browserData.m_characteristics;
		m_state.plotData.paramConfig = &m_state.nConfig;

		//browser data
		m_state.browserData.nConfig = &m_state.nConfig;

		//provide the callbacks
		setUpCallbacks();

		//init the root path
		m_state.browserData.rootPath = std::filesystem::current_path();
		m_state.browserData.currentPath = m_state.browserData.rootPath;

		//init the selection vector for the file browser
		m_state.browserData.m_selection.resize(std::distance(std::filesystem::directory_iterator(m_state.browserData.rootPath), std::filesystem::directory_iterator{}));

	}

	void App::draw() {
		//if (!m_numerics || !m_dataLoader) return;
		std::scoped_lock lk{ m_charMutex };
		//get the mainviewport dockspace

		ImGuiID mainDockID = Views::Widgets::mDS;

		//dock prograpatically the main window
		ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_Once);
		ImGuiWindowFlags JFMWFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;


		ImGui::Begin("JFM", nullptr, JFMWFlags);

		//ViewMenu is the menu of the main window

		Views::Widgets::ViewMenu(m_state.uiState);

		//Next plotting area
		if (m_state.uiState.m_showPlottingArea) {
			ImGuiChildFlags child_flags = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY;
			ImGui::BeginChild("Plot Area", ImVec2{ ImGui::GetContentRegionAvail().x * 0.6f, ImGui::GetContentRegionAvail().y }, child_flags);
			Views::Widgets::PlottingArea(m_state.plotData);
			ImGui::EndChild();
			ImGui::SameLine();
		}
		ImGui::BeginGroup();
		//next content browser
		if (m_state.uiState.m_showBrowserArea) {
			Views::Widgets::BrowserArea(m_state.browserData);
		}

		//	Characteristics inspector - by default
		if (m_state.uiState.m_showCharacteristicInspector) {
			Views::Widgets::CharacteristicInspector(m_state.plotData);
		}
		ImGui::EndGroup();
		ImGui::End();


		//displaying MC as a separate window
		if (m_state.uiState.m_showMonteCarloInspector) {
			if (ImGui::Begin("MC Inspector"), nullptr, ImGuiWindowFlags_NoDocking) {
				Views::Widgets::MonteCarloInspector(m_state.plotData);
			}
			ImGui::End();
		}
		ImGui::ShowDemoWindow();

	}

	void App::update() {
		//if (!m_numerics && !m_dataLoader) return;
		std::scoped_lock lk{ m_charMutex };


		//update the characteristic list and active characteristic
		if (m_state.plotData.active && !(m_state.plotData.active->checked)) {
			m_state.plotData.active = nullptr;
			for (auto& ch : m_state.browserData.m_characteristics) {
				if (ch.checked) {
					m_state.plotData.active = &ch;
					break;
				}
			}
		}

		//update the parameters according to the model of the active characteristic for MC choice
		//?

	}

	void App::setUpCallbacks() {

		//browser callbacks

		{
			m_state.browserData.m_loadCallback = [&]() {

				std::vector<std::filesystem::path> paths{};

				for (const auto& [index, selected] : std::views::enumerate(std::filesystem::directory_iterator(m_state.browserData.currentPath))) {
					if (m_state.browserData.m_selection[index] && !selected.is_directory()) {
						paths.push_back(selected.path());
					}
				}

				m_dataLoader->load(paths, [&](std::vector<LoaderOutput> characteristics) {


					for (auto& c : characteristics) {
						if (!c.success) continue;
						if (c.data) {
							Data::Characteristic temp{ *c.data };
							const auto& p = std::find_if(paths.begin(), paths.end(), [&](const std::filesystem::path& path) {
								return path.string().contains(temp.name);
								});

							if (p != paths.end()) {
								temp.path = *p;
							}

							temp.m_tuneCallback = [&]() {
								//assuming the tuned parameters are copied into fitted
								CalculatingData cData = temp.getCalculatingData();

								m_numerics->CalculateData(cData);

								temp.fitError = m_numerics->CalculateError(cData.characteristic.currentData, temp.getEstimateInput().characteristic.currentData);
								};

							//do preFit

							//range the data

							temp.dataRange = m_numerics->RangeData({ temp.V, temp.I });

							//estimate

							auto eParams = m_numerics->Estimate(temp.getEstimateInput());

							//fit

							m_numerics->Fit(temp.getFittingInput(), [&](ParameterMap&& output) {
								std::scoped_lock lk{ m_charMutex };

								CalculatingData cData = temp.getCalculatingData();
								temp.fittedParameters = output;

								m_numerics->CalculateData(cData);

								double fitError = m_numerics->CalculateError(cData.characteristic.currentData, temp.getEstimateInput().characteristic.currentData);
								temp.submitFitting(output, fitError);

								m_state.browserData.m_characteristics.push_back(temp);
								});

						}
					}

					});


				};

			m_state.browserData.m_loadAllCallback = [&]() {

				std::vector<std::filesystem::path> paths{};

				for (const auto& [index, selected] : std::views::enumerate(std::filesystem::directory_iterator(m_state.browserData.currentPath))) {
					if (!selected.is_directory()) {
						paths.push_back(selected.path());
					}
				}

				m_dataLoader->load(paths, [&](std::vector<LoaderOutput> characteristics) {


					for (auto& c : characteristics) {
						if (!c.success) continue;
						if (c.data) {
							Data::Characteristic temp{ *c.data };

							//do preFit

							//range the data

							temp.dataRange = m_numerics->RangeData({ temp.V, temp.I });

							//estimate

							auto eParams = m_numerics->Estimate(temp.getEstimateInput());

							//fit

							m_numerics->Fit(temp.getFittingInput(), [&](ParameterMap&& output) {
								std::scoped_lock lk{ m_charMutex };

								CalculatingData cData = temp.getCalculatingData();
								temp.fittedParameters = output;

								m_numerics->CalculateData(cData);

								double fitError = m_numerics->CalculateError(cData.characteristic.currentData, temp.getEstimateInput().characteristic.currentData);
								temp.submitFitting(output, fitError);

								m_state.browserData.m_characteristics.push_back(temp);
								});

						}
					}

					});


				};

			m_state.browserData.m_updateColorsCallback = [&]() {

				auto& startColor = m_state.browserData.startColor;
				auto& endColor = m_state.browserData.endColor;

				size_t cn = m_state.browserData.m_characteristics.size();

				for (const auto& [i, c] : std::views::enumerate(m_state.browserData.m_characteristics)) {

					ImVec4 col{};
					col.w = 1.0f;

					float t = static_cast<float>(i) / static_cast<float>(cn);

					for (size_t i = 0; i < 3; i++) {
						auto& r = *(reinterpret_cast<float*>(&col) + i);
						auto& s = *(reinterpret_cast<float*>(&startColor) + i);
						auto& e = *(reinterpret_cast<float*>(&endColor) + i);

						r = s * (1.0f - t) + e * t;
					}
					col.w = c.color.w;
					c.color = col;
				}

				};

			m_state.browserData.m_invertSelectionCallback = [&]() {
				auto& active = m_state.plotData.active;
				for (auto& ch : m_state.browserData.m_characteristics) {
					if (active == &ch) active = nullptr;
					if (!ch.checked && !active) active = &ch;
					ch.checked = !ch.checked;
				}

				};

			m_state.browserData.m_selectAllCallback = [&]() {
				for (auto& ch : m_state.browserData.m_characteristics) {
					ch.checked = true;
				}

				};

			m_state.browserData.m_unselectAllCallback = [&]() {
				for (auto& ch : m_state.browserData.m_characteristics) {
					ch.checked = false;
				}
				m_state.plotData.active = nullptr;
				};

			m_state.browserData.m_removeSelectedCallback = [&]() {
				//remove characteristics that are checked

				std::erase_if(m_state.browserData.m_characteristics, [&](Data::Characteristic& ch) {
					return ch.checked;
					});

				m_state.plotData.active = nullptr;

				};

			m_state.browserData.m_removeUnselectedCallback = [&]() {
				std::erase_if(m_state.browserData.m_characteristics, [](Data::Characteristic& ch) {
					return !ch.checked;
					});
				};
		}

		//plot area callbacks

		{
			m_state.plotData.m_estimateCallback = [&]() {
				if (!m_state.plotData.active) return;

				auto& active = *m_state.plotData.active;

				auto eParams = m_numerics->Estimate(active.getEstimateInput());

				active.fittedParameters = eParams;

				};

			m_state.plotData.m_fitCallback = [&]() {
				if (!m_state.plotData.active) return;

				auto& active = *m_state.plotData.active;

				m_numerics->Fit(active.getFittingInput(), [&](ParameterMap&& output) {
					if (!&active) return;

					CalculatingData cData = active.getCalculatingData();
					active.fittedParameters = output;

					m_numerics->CalculateData(cData);

					double fitError = m_numerics->CalculateError(cData.characteristic.currentData, active.getEstimateInput().characteristic.currentData);
					active.submitFitting(output, fitError);

					});


				};

			m_state.plotData.m_tuneCallback = [&]() {
				if (!m_state.plotData.active) return;

				auto& active = *m_state.plotData.active;

				auto tData = active.getTuningData();

				m_numerics->CalculateData(tData);

				active.tuneError = m_numerics->CalculateError(tData.characteristic.currentData, active.getEstimateInput().characteristic.currentData);
				};

			//Monte Carlo callbacks

			m_state.plotData.m_saveMCConfCallback = [&]() {

				for (auto& ch : m_state.browserData.m_characteristics) {
					MCOutput out{ ch.getMCConfig(), {} };
					LoaderOutput lOut{};
					lOut.mcData = std::make_unique<MCOutput>(std::move(out));

					m_dataLoader->save(ch.path, lOut, [&](LoaderOutput out) {

						});
				}

				};

			m_state.plotData.m_performMCCallback = [&]() {
				if (!m_state.plotData.active) return;

				auto& active = *m_state.plotData.active;

				auto mcData = active.getMCConfig();

				m_numerics->Simulate(mcData, [&](MCOutput&& output) {
					if (!&active) return;
					std::scoped_lock lk{ *active.mcMutex };

					active.submitMC(output);
					});
				};

			m_state.plotData.m_performMCOnAllCallback = [&]() {

				for (auto& ch : m_state.browserData.m_characteristics) {
					if (!ch.checked) continue;
					auto fInput = ch.getFittingInput();

					fInput.modelID = m_state.plotData.savedGlobalModelID;
					fInput.name = ch.name;

					ParameterMap fv{};
					for (auto& [id, val] : m_state.plotData.savedGlobalFixedParameterIDs) {
						if (val) fv[id] = ch.fittedParameters[id];
					}
					fInput.fixConfig = fv;

					fInput.useBounds = false;

					MCInput mcI{};
					mcI.startingData = fInput;
					mcI.relPath = "./MC";
					mcI.trueParameters = ch.fittedParameters;
					mcI.iterations = m_state.plotData.savedGlobalMCConfig.n;
					mcI.noise = m_state.plotData.savedGlobalMCConfig.sigma;

					m_numerics->Simulate(mcI, [&](MCOutput&& output) {
						std::scoped_lock lk{ *ch.mcMutex };
						ch.submitMC(output);
						});
				}

				};

		}
	}
};