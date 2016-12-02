#pragma once

#include <vector>

#include "cinder/Color.h"

#include "CoreMath.h"

template <typename T>
class Gradient {
public:
	Gradient() {}
	Gradient(std::vector<std::pair<float, T>> stops) : mStops(stops) {}
	Gradient(std::initializer_list<std::pair<float, T>> ilist) : mStops(ilist) {}

	void add(float stopVal, T colorVal) {
		mStops.push_back(std::make_pair(stopVal, colorVal));
	}

	T sample(float tval) {
		T retval = mStops.front().second;
		for (int idx = 1; idx < mStops.size(); idx++) {
			float lo = mStops[idx - 1].first;
			float hi = mStops[idx].first;
			if (lo <= tval && hi > tval) {
				float t = glm::smoothstep(lo, hi, tval);
				return mStops[idx - 1].second.lerp(t, mStops[idx].second);
			}
		}
		return mStops.back().second;
	}

	std::vector<std::pair<float, T>> mStops;
};