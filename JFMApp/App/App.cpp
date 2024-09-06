#include "pch.hpp"
#include "App.hpp"

std::vector<std::pair<std::vector<double>, std::vector<double>> >globalNoisyI{};
std::vector<std::vector<double>> globalErrors{};

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
		m_state.nConfig = m_numerics->GetConfiguration();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		//Data::NumericsConfig nConfig{};

		//nConfig.models = std::unordered_map<ModelID, ModelName>{ {0, "6P IV"}, {1, "4P IV"} };
		//nConfig.parameters = std::unordered_map<ParameterID, ParameterName>{ {0, "I0"}, {1, "A"}, {2, "Rs"}, {3, "Rsh"}, {4, "Alpha"}, {5, "Rsh2"} };
		//nConfig.modelParameters = std::unordered_map<ModelID, std::vector<ParameterID>>{ {0, {0, 1, 2, 3, 4, 5}}, {1, {0, 1, 2, 3}} };
		//nConfig.paramBounds = std::unordered_map<ParameterID, Bounds>{ {0, {0.0, 1.0}}, {1, {0.0, 1.0}}, {2, {0.0, 1.0}}, {3, {0.0, 1.0}}, {4, {0.0, 1.0}}, {5, {0.0, 1.0}} };

		//m_state.nConfig = nConfig;

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
		ch.bounds = m_state.nConfig.paramBounds;
		ch.savedBounds = m_state.nConfig.paramBounds;

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

		ImGuiID jfmID = ImGui::GetID("JFM");
		ImGui::Begin("JFM", nullptr, JFMWFlags);

		//ViewMenu is the menu of the main window

		Views::Widgets::ViewMenu(m_state.uiState);

		ImGui::DockSpace(jfmID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);


		ImGui::End();
		static bool docked = false;
		if (!docked) {
			docked = true;
			ImGui::DockBuilderRemoveNode(jfmID);
			ImGui::DockBuilderAddNode(jfmID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(jfmID, ImGui::GetMainViewport()->Size);

			ImGuiID dock_main_id = jfmID;
			ImGuiID right, left;
			ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.6f, &left, &right);

			ImGuiID rtop, rbottom;
			ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.5f, &rtop, &rbottom);
			ImGuiID ltop, lbottom;
			ImGui::DockBuilderSplitNode(left, ImGuiDir_Up, 0.8f, &ltop, &lbottom);

			ImGuiID cbTop, cbBottom;
			ImGui::DockBuilderSplitNode(rtop, ImGuiDir_Up, 0.5f, &cbTop, &cbBottom);

			ImGui::DockBuilderDockWindow("Plot Area", ltop);
			ImGui::DockBuilderDockWindow("Characteristic Settings", lbottom);
			ImGui::DockBuilderDockWindow("Characteristic List", cbTop);
			ImGui::DockBuilderDockWindow("File Explorer", cbBottom);
			ImGui::DockBuilderDockWindow("Characteristic Inspector", rbottom);


			ImGui::DockBuilderFinish(jfmID);

			std::string id = "MC Tab Dock" + std::to_string(1);
			m_state.plotData.tabsIDs.push_back(ImGui::GetID(id.c_str()));
		}


		//Next plotting area
		if (m_state.uiState.m_showPlottingArea) {

			Views::Widgets::PlottingArea(m_state.plotData);

		}
		//next content browser
		if (m_state.uiState.m_showBrowserArea) {
			Views::Widgets::BrowserArea(m_state.browserData);

		}

		//	Characteristics inspector - by default
		if (m_state.uiState.m_showCharacteristicInspector) {
			ImGui::Begin("Characteristic Inspector");
			Views::Widgets::CharacteristicInspector(m_state.plotData);
			ImGui::End();
		}

		ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_Once);
		if (ImGui::Begin("RT MC"))
		{
			static int curr_c = 0;
			ImVec2 s = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			if (ImPlot::BeginPlot("RT MC", s))
			{
				ImPlot::SetupAxes("V", "I", Data::PlotData::plotSettings.xFlags, Data::PlotData::plotSettings.yFlags);

				ImPlot::SetupAxisScale(ImAxis_Y1, Data::Characteristic::TFL, Data::Characteristic::TFNL);

				if (m_state.browserData.m_characteristics.size() > 1 && globalNoisyI.size())
				{
					ImPlot::PlotLine("I", m_state.browserData.m_characteristics[1].V.data() + m_state.browserData.m_characteristics[1].dataRange.first, globalNoisyI[curr_c].first.data(), globalNoisyI[curr_c].first.size());
					ImPlot::PlotLine("I1", m_state.browserData.m_characteristics[1].V.data() + m_state.browserData.m_characteristics[1].dataRange.first, globalNoisyI[curr_c].second.data(), globalNoisyI[curr_c].second.size());
				}
			}
			ImPlot::EndPlot();
			
			ImGui::SliderInt("Char", &curr_c, 0, globalNoisyI.size() - 1);
		}
		ImGui::End();

		ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_Once);
		if (ImGui::Begin("RT Error"))
		{
			static int curr_c = 0;
			ImVec2 s = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			if (ImPlot::BeginPlot("RT Error", s))
			{
				ImPlot::SetupAxes("N", "Err", Data::PlotData::plotSettings.xFlags, Data::PlotData::plotSettings.yFlags);

				//ImPlot::SetupAxisScale(ImAxis_Y1, Data::Characteristic::TFL, Data::Characteristic::TFNL);
				
				if (globalErrors.size())
					ImPlot::PlotLine("E", globalErrors[curr_c].data(), globalErrors[curr_c].size());
			}
			ImPlot::EndPlot();

			ImGui::SliderInt("Char", &curr_c, 0, globalErrors.size() - 1);
		}
		ImGui::End();


		//displaying MC as a separate window
		if (m_state.uiState.m_showMonteCarloInspector) {
			ImGui::SetNextWindowDockID(mainDockID, ImGuiCond_Once);
			if (ImGui::Begin("MC Inspector"), nullptr, ImGuiWindowFlags_NoDocking) {
				Views::Widgets::MonteCarloInspector(m_state.plotData);
			}
			ImGui::End();
		}
		//ImGui::ShowDemoWindow();

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

				if (paths.empty()) return;

				m_dataLoader->Load(paths, [&](std::vector<LoaderOutput> characteristics) {


					for (auto& c : characteristics) {
						if (!c.success) continue;
						//loading a characteristic
						if (c.data) {


							Data::Characteristic temp{ *c.data };
							temp.nConfig = m_state.nConfig;
							temp.checked = true;
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
							temp.savedInitialGuess = eParams;
							temp.savedUseInitial = true;
							//fit
							temp.savedUseBounds = true;
							for (const auto& [k, v] : eParams)
							{
								if (k != 0)
								{
									temp.savedBounds[k].first = 1.0 * std::pow(10.0, std::floor(std::log10(v)) - 1);
									temp.savedBounds[k].second = 9.0 * std::pow(10.0, std::floor(std::log10(v)) + 1);
								}
								else
								{
									temp.savedBounds[k].first = 1;
									temp.savedBounds[k].second = 5;
								}
							}


							m_numerics->Fit(temp.getFittingInput(), [&](ParameterMap&& output) {


								CalculatingData cData = temp.getCalculatingData();
								cData.parameters = output;

								m_numerics->CalculateData(cData);

								double fitError = m_numerics->CalculateError(cData.characteristic.currentData, temp.getEstimateInput().characteristic.currentData);
								temp.submitFitting(output, fitError);
								//std::scoped_lock lk{ m_charMutex };
								temp.savedUseInitial = false;
								temp.savedUseBounds = false;

								temp.bounds = temp.savedBounds;

								m_state.browserData.m_characteristics.push_back(temp);
								m_state.plotData.active = &m_state.browserData.m_characteristics.back();
								});

						}
					}

					for (auto& c : characteristics) {
						if (!c.success) continue;
						//loading a monte carlo
						// check if the characteristic is already loaded
						// if not, load the characteristic
						//	   
						// if yes, check if montecalro data is already loaded(distinguish by the fitting config)
						// if not, load the montecarlo data
						//     --put montecarlo data into th echaracteristic
						// if yes, do nothing
						if (c.mcData) {
							auto cc = std::find_if(m_state.browserData.m_characteristics.begin(), m_state.browserData.m_characteristics.end(), [&](Data::Characteristic& ch) {
								return ch.path == c.mcData->inputData.startingData.name;
								});

							Data::Characteristic* ch{ nullptr };

							auto loadMC = [&]() {
								auto& mc = *c.mcData;
								auto& cha = *ch;


								auto mcs = std::find_if(cha.mcData.begin(), cha.mcData.end(), [&](Data::Characteristic::MCSimulation& m) {
									return m.fixConfig == mc.inputData.startingData.fixConfig;
									});


								if (mcs == cha.mcData.end()) {
									cha.submitMC(mc);
								}
								};


							if (cc == m_state.browserData.m_characteristics.end()) {
								{
									m_dataLoader->Load(c.mcData->inputData.relPath, [&](LoaderOutput cdata) {
										if (cdata.success && cdata.data) {
											Data::Characteristic temp{ *cdata.data };
											temp.nConfig = m_state.nConfig;
											temp.path = c.mcData->inputData.relPath;


											temp.fittedParameters = c.mcData->inputData.trueParameters;
											temp.fixedParametersValues = c.mcData->inputData.startingData.fixConfig;
											temp.savedFixedParametersValues = temp.fixedParametersValues;
											temp.initialGuess = c.mcData->inputData.startingData.initialValues;
											temp.savedInitialGuess = temp.initialGuess;
											temp.modelID = c.mcData->inputData.startingData.initialData.modelID;
											temp.savedModelID = temp.modelID;
											temp.tunedParameters = temp.fittedParameters;
											temp.isFitted = true;
											temp.tunedParameters = temp.fittedParameters;
											temp.dataRange = m_numerics->RangeData({ temp.V, temp.I });

											auto cd = temp.getCalculatingData();
											m_numerics->CalculateData(cd);
											temp.fitError = m_numerics->CalculateError(temp.getEstimateInput().characteristic.currentData, cd.characteristic.currentData);


											std::scoped_lock lk{ m_charMutex };
											m_state.browserData.m_characteristics.push_back(temp);
											ch = &m_state.browserData.m_characteristics.back();
											loadMC();
										}
										});
								}
							}
							else {
								ch = &(*cc);
								loadMC();
							}

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

				m_dataLoader->Load(paths, [&](std::vector<LoaderOutput> characteristics) {


					for (auto& c : characteristics) {
						if (!c.success) continue;
						if (c.data) {
							Data::Characteristic temp{ *c.data };
							temp.nConfig = m_state.nConfig;
							temp.checked = true;
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
							temp.savedInitialGuess = eParams;
							temp.savedUseInitial = true;
							//fit
							temp.savedUseBounds = true;
							for (const auto& [k, v] : eParams)
							{
								if (k != 0)
								{
									temp.savedBounds[k].first = 1.0 * std::pow(10.0, std::floor(std::log10(v)) - 1);
									temp.savedBounds[k].second = 9.0 * std::pow(10.0, std::floor(std::log10(v)) + 1);
								}
								else
								{
									temp.savedBounds[k].first = 1;
									temp.savedBounds[k].second = 5;
								}
							}

							m_numerics->Fit(temp.getFittingInput(), [&](ParameterMap&& output) {
								//std::scoped_lock lk{ m_charMutex };


								temp.fittedParameters = output;
								CalculatingData cData = temp.getCalculatingData();


								m_numerics->CalculateData(cData);

								double fitError = m_numerics->CalculateError(cData.characteristic.currentData, temp.getEstimateInput().characteristic.currentData);
								temp.submitFitting(output, fitError);

								temp.savedUseInitial = false;
								temp.savedUseBounds = false;

								temp.bounds = temp.savedBounds;

								m_state.browserData.m_characteristics.push_back(temp);
								m_state.plotData.active = &m_state.browserData.m_characteristics.back();
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

					active.fittedParameters = output;
					CalculatingData cData = active.getCalculatingData();


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
					if (!ch.checked) continue;
					MCOutput out{ ch.getMCConfig(), {} };
					LoaderOutput lOut{};
					lOut.mcData = std::make_unique<MCOutput>(std::move(out));

					m_dataLoader->Save(ch.path.parent_path() / "MC" / lOut.mcData->inputData.relPath, lOut, [&](LoaderOutput out) {

						});
				}

				};

			m_state.plotData.m_performMCCallback = [&]() {
				if (!m_state.plotData.active) return;

				auto& active = *m_state.plotData.active;

				auto mcData = active.getMCConfig();

				m_numerics->Simulate(mcData, [&](MCOutput&& output) {
					if (!&active) return;
					//std::scoped_lock lk{ *active.mcMutex };
					m_state.plotData.activeMC = nullptr;
					active.submitMC(output);


					});
				};

			m_state.plotData.m_performMCOnAllCallback = [&]() {

				for (auto& ch : m_state.browserData.m_characteristics) {
					if (!ch.checked) continue;
					auto fInput = ch.getFittingInput();

					fInput.initialData.modelID = m_state.plotData.savedGlobalModelID;
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
						//std::scoped_lock lk{ *ch.mcMutex };
						ch.submitMC(output);
						});
				}

				};

		}
	}
};