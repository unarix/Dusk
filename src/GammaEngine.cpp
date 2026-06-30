/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "GammaEngine.h"

#include <Screen.h>
#include <algorithm>
#include <cmath>


GammaEngine::GammaEngine()
	:
	fTemperature(kDefaultTemperature),
	fEnabled(false)
{
}


GammaEngine::~GammaEngine()
{
	if (fEnabled)
		_ResetGamma();
}


void
GammaEngine::SetTemperature(int32 kelvin)
{
	fTemperature = std::max(kMinTemperature, std::min(kMaxTemperature, kelvin));
	if (fEnabled)
		_ApplyGamma();
}


void
GammaEngine::Enable()
{
	if (!fEnabled) {
		fEnabled = true;
		_ApplyGamma();
	}
}


void
GammaEngine::Disable()
{
	if (fEnabled) {
		fEnabled = false;
		_ResetGamma();
	}
}


void
GammaEngine::Toggle()
{
	if (fEnabled)
		Disable();
	else
		Enable();
}


void
GammaEngine::_ApplyGamma()
{
	float redMul, greenMul, blueMul;
	_TemperatureToRGB(fTemperature, &redMul, &greenMul, &blueMul);

	uint16 red[256], green[256], blue[256];

	for (int i = 0; i < 256; i++) {
		red[i]   = (uint16)(i * redMul) << 8;
		green[i] = (uint16)(i * greenMul) << 8;
		blue[i]  = (uint16)(i * blueMul) << 8;
	}

	BScreen screen(B_MAIN_SCREEN_ID);
	if (screen.IsValid())
		screen.SetGammaRamp(&red, &green, &blue);
}


void
GammaEngine::_ResetGamma()
{
	uint16 red[256], green[256], blue[256];

	for (int i = 0; i < 256; i++) {
		red[i]   = (uint16)i << 8;
		green[i] = (uint16)i << 8;
		blue[i]  = (uint16)i << 8;
	}

	BScreen screen(B_MAIN_SCREEN_ID);
	if (screen.IsValid())
		screen.SetGammaRamp(&red, &green, &blue);
}


// Mapeo de temperatura de color a multiplicadores RGB.
// Ref: https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
void
GammaEngine::_TemperatureToRGB(int32 kelvin, float* outRed, float* outGreen,
	float* outBlue)
{
	float temp = (float)kelvin / 100.0f;

	// Red
	if (temp <= 66.0f) {
		*outRed = 1.0f;
	} else {
		float r = 329.698727446f * powf(temp - 60.0f, -0.1332047592f);
		*outRed = std::max(0.0f, std::min(1.0f, r / 255.0f));
	}

	// Green
	if (temp <= 66.0f) {
		float g = 99.4708025861f * logf(temp) - 161.1195681661f;
		*outGreen = std::max(0.0f, std::min(1.0f, g / 255.0f));
	} else {
		float g = 288.1221695283f * powf(temp - 60.0f, -0.0755148492f);
		*outGreen = std::max(0.0f, std::min(1.0f, g / 255.0f));
	}

	// Blue
	if (temp >= 66.0f) {
		*outBlue = 1.0f;
	} else if (temp <= 19.0f) {
		*outBlue = 0.0f;
	} else {
		float b = 138.5177312231f * logf(temp - 10.0f) - 305.0447927307f;
		*outBlue = std::max(0.0f, std::min(1.0f, b / 255.0f));
	}
}
