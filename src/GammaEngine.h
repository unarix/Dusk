/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef GAMMA_ENGINE_H
#define GAMMA_ENGINE_H

#include <Accelerant.h>
#include <SupportDefs.h>
#include <image.h>


// rango de temperaturas que tiene sentido manejar
static const int32 kMinTemperature = 1900;
static const int32 kMaxTemperature = 6500;
static const int32 kDefaultTemperature = 3500;


class GammaEngine {
public:
						GammaEngine();
						~GammaEngine();

	status_t			InitCheck() const { return fInitStatus; }
	const char*			DriverName() const { return fDriverName; }

	void				SetTemperature(int32 kelvin);
	int32				Temperature() const { return fTemperature; }

	void				Enable();
	void				Disable();
	bool				IsEnabled() const { return fEnabled; }

	void				Toggle();

private:
	status_t			_InitAccelerant();
	void				_UninitAccelerant();

	void				_ApplyGamma();
	void				_ResetGamma();
	void				_SetPalette(const uint8* colors, uint8 first,
							uint32 count);

	void				_TemperatureToRGB(int32 kelvin,
							float* outRed, float* outGreen, float* outBlue);
	void				_ExtractDriverName(const char* path);

	int32				fTemperature;
	bool				fEnabled;
	status_t			fInitStatus;

	int					fDeviceFD;
	image_id			fAccelerantImage;
	GetAccelerantHook	fGetHook;
	set_indexed_colors	fSetIndexedColors;
	char				fDriverName[64];
};

#endif // GAMMA_ENGINE_H
