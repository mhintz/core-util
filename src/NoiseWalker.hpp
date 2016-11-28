#pragma once

#include "cinder/Rand.h"
#include "cinder/Perlin.h"

class NoiseWalker;
typedef std::unique_ptr<NoiseWalker> NoiseWalkerBox;
typedef std::shared_ptr<NoiseWalker> NoiseWalkerRef;

static const float noiseInitialOffset = 100.0f;
static const float noiseStepSizeDefault = 0.1f;
static const ci::Perlin noiseSampler = ci::Perlin();

class NoiseWalker {
public:
	NoiseWalker() : mPosition(randVec2() * noiseInitialOffset), mNormalStep(randVec2()) {
		mAdjustedStep = noiseStepSizeDefault * mNormalStep;
	}

	NoiseWalker(float stepSize) : mPosition(randVec2() * noiseInitialOffset), mNormalStep(randVec2()) {
		mAdjustedStep = stepSize * mNormalStep;
	}

	static NoiseWalkerRef create(float stepSize) { return NoiseWalkerRef(new NoiseWalker(stepSize)); }

	float step() { mPosition += mAdjustedStep; return noiseSampler.noise(mPosition); }
	float sampleAt(float dist) { return noiseSampler.noise(mPosition + dist * mNormalStep); }

private:
	vec2 mPosition;
	vec2 mNormalStep;
	vec2 mAdjustedStep;
};