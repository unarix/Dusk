/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "GammaEngine.h"
#include "OverlayWindow.h"

#include <Screen.h>
#include <algorithm>
#include <cmath>


GammaEngine::GammaEngine()
	:
	fTemperature(kDefaultTemperature),
	fEnabled(false),
	fOverlay(NULL)
{
}


GammaEngine::~GammaEngine()
{
	if (fOverlay != NULL) {
		fOverlay->Lock();
		fOverlay->Quit();
		fOverlay = NULL;
	}
}


void
GammaEngine::SetTemperature(int32 kelvin)
{
	fTemperature = std::max(kMinTemperature, std::min(kMaxTemperature, kelvin));
	if (fEnabled)
		_UpdateOverlay();
}


void
GammaEngine::Enable()
{
	if (!fEnabled) {
		fEnabled = true;
		_UpdateOverlay();
	}
}


void
GammaEngine::Disable()
{
	if (fEnabled) {
		fEnabled = false;
		if (fOverlay != NULL) {
			fOverlay->Lock();
			fOverlay->Quit();
			fOverlay = NULL;
		}
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
GammaEngine::_UpdateOverlay()
{
	float red, green, blue;
	_TemperatureToRGB(fTemperature, &red, &green, &blue);

	// El overlay tiene el color inverso: cuanto más baja la temperatura,
	// más rojo/naranja necesitamos. El tinte es lo que "falta" respecto
	// a la identidad (6500K). Solo filtramos azul y un poco de verde.
	// Usamos un naranja cálido con alpha proporcional a la intensidad.
	uint8 tintR = 255;
	uint8 tintG = (uint8)(255 * (1.0f - (1.0f - green) * 0.6f));
	uint8 tintB = 0;

	// alpha va de 0 (6500K, sin efecto) a ~100 (1900K, máximo filtro)
	float intensity = 1.0f - (blue + green) / 2.0f;
	uint8 alpha = (uint8)(intensity * 110.0f);

	if (fOverlay == NULL) {
		BScreen screen(B_MAIN_SCREEN_ID);
		BRect frame = screen.Frame();
		fOverlay = new OverlayWindow(frame, tintR, tintG, tintB, alpha);
		fOverlay->Show();
	} else {
		fOverlay->SetColor(tintR, tintG, tintB, alpha);
	}
}


// Mapeo de temperatura de color a multiplicadores RGB.
// Basado en la aproximación de Tanner Helland para curvas de
// radiación de cuerpo negro. Kelvin entra, sale un factor 0-1 por canal.
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
