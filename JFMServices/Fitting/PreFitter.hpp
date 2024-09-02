#pragma once
#include "pch.hpp"
#include "JFMIFitting.hpp"

namespace JFMService
{
	class AbstractPreFit
	{
	public:
		std::pair<size_t, size_t> RangeData(const FittingService::PlotData& characteristic);
		virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput& input) = 0;

	private:
		size_t getLowerRange(const FittingService::PlotData& characteristic);
		size_t getUpperRange(const FittingService::PlotData& characteristic);
	};

	class FourParameterModelPreFit :public AbstractPreFit
	{
	public:
		FourParameterModelPreFit();

		virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput& input) override;
		std::pair<size_t, size_t> rangeData(const FittingService::PlotData& characteristic) { return this->RangeData(characteristic); };

	private:
		std::vector<std::pair<double, double>> m_AMultiplier;

	private:
		double adjustCoefficient(double dV);
	};

	class PreFitter
	{
		using PreFitterMap = std::unordered_map<FittingService::ModelID, std::shared_ptr<AbstractPreFit>>;

	public:
		PreFitter();
		std::pair<size_t, size_t> RangeData(const FittingService::PlotData& characteristic);
		FittingService::ParameterMap Estimate(const FittingService::EstimateInput& input);

	private:
		PreFitterMap preFitterMap{};
	};
}
