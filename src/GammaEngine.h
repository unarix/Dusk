/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef GAMMA_ENGINE_H
#define GAMMA_ENGINE_H

#include <SupportDefs.h>


// rango de temperaturas que tiene sentido manejar
static const int32 kMinTemperature = 1900;
static const int32 kMaxTemperature = 6500;
static const int32 kDefaultTemperature = 3500;


class GammaEngine {
public:
						GammaEngine();
						~GammaEngine();

	void				SetTemperature(int32 kelvin);
	int32				Temperature() const { return fTemperature; }

	void				Enable();
	void				Disable();
	bool				IsEnabled() const { return fEnabled; }

	void				Toggle();

private:
	void				_ApplyGamma();
	void				_ResetGamma();
	void				_TemperatureToRGB(int32 kelvin,
							float* outRed, float* outGreen, float* outBlue);

	int32				fTemperature;
	bool				fEnabled;
};

#endif // GAMMA_ENGINE_H
