#pragma once
#include "../Fitting/JFMIFitting.hpp"
#include "JFMAdditionalParameters.hpp"

namespace JFMService
{
	enum JFMModelID
	{
		None = 0,
		Model4P = 3,
		Model6P = 5
	};
	using namespace FittingService;
	class DataCalculator
	{
	public:
		DataCalculator() = default;
		void CalculateData(CalculatingData &input);
	};
}