#pragma once

#include "pch.hpp"

#include "Random.hpp"

namespace JFMService
{
	using namespace NumericStorm::Utils;
	class LogDist
	{
	public:
		static float value(float min, float max)
		{
			int pow_min = std::floor(std::log10(min));
			int pow_max = std::floor(std::log10(max));

			double c_min = min / std::pow(10, pow_min);
			double c_max = max / std::pow(10, pow_max);

			int p = Random::UInt(pow_min, pow_max);

			double c{0.0};

			if (p == pow_min)
			{
				c = Random::Float(c_min, 10.0);
			}
			else if (p == pow_max)
			{
				c = Random::Float(1.0, c_max);
			}
			else
				c = Random::Float(1.0, 10.0);

			return c * std::pow(10, p);
		}
	};
}