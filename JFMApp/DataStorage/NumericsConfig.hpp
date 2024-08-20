#pragma once

#include "pch.hpp"

namespace JFMApp::Data
{
	using ModelID = unsigned int;
	using ModelName = std::string;
	using ParameterID = unsigned int;
	using ParameterName = std::string;
	using Models = std::unordered_map<ModelID, ModelName>;
	using Parameters = std::unordered_map<ParameterID, ParameterName>;
	using ModelParameters = std::unordered_map<ModelID, std::vector<ParameterID>>;
	//theoretical bounds for parameters
	using Bounds = std::pair<double, double>;
	using ParamBounds = std::unordered_map<ParameterID, Bounds>;

	struct NumericsConfig
	{
		//model list
		// ID -- Name
		//--parameter list
		// ID -- Name
		static Models models;
		static Parameters parameters;
		static ModelParameters modelParameters;
		static ParamBounds paramBounds;
	};
}


