/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "GammaEngine.h"

#include <AppServerLink.h>
#include <ServerProtocol.h>

#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>


GammaEngine::GammaEngine()
	:
	fTemperature(kDefaultTemperature),
	fEnabled(false),
	fInitStatus(B_NO_INIT),
	fDeviceFD(-1),
	fAccelerantImage(-1),
	fGetHook(NULL),
	fSetIndexedColors(NULL)
{
	fDriverName[0] = '\0';
	fInitStatus = _InitAccelerant();
}


GammaEngine::~GammaEngine()
{
	if (fEnabled)
		_ResetGamma();
	_UninitAccelerant();
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


status_t
GammaEngine::_InitAccelerant()
{
	// Pedimos al app_server la ruta del driver gráfico activo
	char driverPath[B_PATH_NAME_LENGTH];
	char accelerantPath[B_PATH_NAME_LENGTH];

	driverPath[0] = '\0';
	accelerantPath[0] = '\0';

	{
		BPrivate::AppServerLink link;
		link.StartMessage(AS_GET_DRIVER_PATH);
		link.Attach<int32>(0);  // screen ID = main

		status_t status;
		if (link.FlushWithReply(status) != B_OK || status != B_OK) {
			strlcpy(fDriverName, "(desconocido)", sizeof(fDriverName));
			return B_ERROR;
		}
		link.ReadString(driverPath, B_PATH_NAME_LENGTH);
	}

	// Extraer el nombre del driver del path
	// Path típico: /dev/graphics/vesa/0 o /dev/graphics/intel_extreme/0
	_ExtractDriverName(driverPath);

	{
		BPrivate::AppServerLink link;
		link.StartMessage(AS_GET_ACCELERANT_PATH);
		link.Attach<int32>(0);  // screen ID = main

		status_t status;
		if (link.FlushWithReply(status) != B_OK || status != B_OK)
			return B_ERROR;
		link.ReadString(accelerantPath, B_PATH_NAME_LENGTH);
	}

	// Abrir el device del driver gráfico
	fDeviceFD = open(driverPath, O_RDWR | O_CLOEXEC);
	if (fDeviceFD < 0)
		return B_ERROR;

	// Cargar el accelerant como add-on
	fAccelerantImage = load_add_on(accelerantPath);
	if (fAccelerantImage < 0) {
		close(fDeviceFD);
		fDeviceFD = -1;
		return B_ERROR;
	}

	// Obtener el entry point del accelerant
	status_t result = get_image_symbol(fAccelerantImage,
		B_ACCELERANT_ENTRY_POINT, B_SYMBOL_TYPE_TEXT, (void**)&fGetHook);
	if (result != B_OK) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return B_ERROR;
	}

	// Clonar el accelerant (otro proceso ya lo inicializó — el app_server)
	typedef status_t (*clone_accelerant_func)(void* data);
	clone_accelerant_func cloneFunc = (clone_accelerant_func)
		fGetHook(B_CLONE_ACCELERANT, NULL);
	if (cloneFunc == NULL) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return B_ERROR;
	}

	// Para clonar necesitamos el clone info del accelerant primario
	// Primero obtenemos el tamaño del clone info
	typedef ssize_t (*clone_info_size_func)(void);
	clone_info_size_func sizeFunc = (clone_info_size_func)
		fGetHook(B_ACCELERANT_CLONE_INFO_SIZE, NULL);
	if (sizeFunc == NULL) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return B_ERROR;
	}

	ssize_t cloneInfoSize = sizeFunc();
	if (cloneInfoSize <= 0) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return B_ERROR;
	}

	// Obtener la info de clonación
	typedef void (*get_clone_info_func)(void* data);
	get_clone_info_func getInfoFunc = (get_clone_info_func)
		fGetHook(B_GET_ACCELERANT_CLONE_INFO, NULL);
	if (getInfoFunc == NULL) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return B_ERROR;
	}

	char* cloneInfo = new char[cloneInfoSize];
	getInfoFunc(cloneInfo);

	// Clonar usando la info (normalmente es el path del device)
	result = cloneFunc(cloneInfo);
	delete[] cloneInfo;

	if (result != B_OK) {
		unload_add_on(fAccelerantImage);
		close(fDeviceFD);
		fAccelerantImage = -1;
		fDeviceFD = -1;
		return result;
	}

	// Obtener el hook de set_indexed_colors
	fSetIndexedColors = (set_indexed_colors)
		fGetHook(B_SET_INDEXED_COLORS, NULL);

	// Puede ser NULL si el driver no lo soporta.
	// Intel/Radeon lo implementan, pero nvidia-haiku (X547) todavía no.
	// Sin este hook no podemos tocar la LUT del hardware.
	if (fSetIndexedColors == NULL) {
		// Igualmente dejamos el clone activo por si en el futuro
		// se agrega soporte. Reportamos el error para que la UI avise.
		fprintf(stderr, "Dusk: driver no soporta B_SET_INDEXED_COLORS\n");
		return B_NOT_SUPPORTED;
	}

	return B_OK;
}


void
GammaEngine::_UninitAccelerant()
{
	if (fAccelerantImage >= 0) {
		typedef void (*uninit_func)(void);
		uninit_func uninit = (uninit_func)
			fGetHook(B_UNINIT_ACCELERANT, NULL);
		if (uninit != NULL)
			uninit();
		unload_add_on(fAccelerantImage);
		fAccelerantImage = -1;
	}

	if (fDeviceFD >= 0) {
		close(fDeviceFD);
		fDeviceFD = -1;
	}
}


void
GammaEngine::_ApplyGamma()
{
	if (fSetIndexedColors == NULL)
		return;

	float redMul, greenMul, blueMul;
	_TemperatureToRGB(fTemperature, &redMul, &greenMul, &blueMul);

	// Armamos 256 entradas de paleta.
	// En los drivers Intel/Radeon esto escribe a los registros
	// PALETTE del CRTC, que actúan como LUT de gamma para todos
	// los modos de color (incluyendo RGB32).
	uint8 colors[256 * 3];
	for (int i = 0; i < 256; i++) {
		colors[i * 3 + 0] = (uint8)(i * redMul);
		colors[i * 3 + 1] = (uint8)(i * greenMul);
		colors[i * 3 + 2] = (uint8)(i * blueMul);
	}

	_SetPalette(colors, 0, 256);
}


void
GammaEngine::_ResetGamma()
{
	if (fSetIndexedColors == NULL)
		return;

	// Rampa lineal — identidad
	uint8 colors[256 * 3];
	for (int i = 0; i < 256; i++) {
		colors[i * 3 + 0] = (uint8)i;
		colors[i * 3 + 1] = (uint8)i;
		colors[i * 3 + 2] = (uint8)i;
	}

	_SetPalette(colors, 0, 256);
}


void
GammaEngine::_SetPalette(const uint8* colors, uint8 first, uint32 count)
{
	if (fSetIndexedColors != NULL)
		fSetIndexedColors(count, first, colors, 0);
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


void
GammaEngine::_ExtractDriverName(const char* path)
{
	// Path típico: /dev/graphics/vesa/0
	// Queremos extraer "vesa" (el penúltimo componente)
	if (path == NULL || path[0] == '\0') {
		strlcpy(fDriverName, "(desconocido)", sizeof(fDriverName));
		return;
	}

	// Copiar para poder manipular
	char tmp[B_PATH_NAME_LENGTH];
	strlcpy(tmp, path, sizeof(tmp));

	// Sacar el último componente (ej: "0")
	char* lastSlash = strrchr(tmp, '/');
	if (lastSlash != NULL) {
		*lastSlash = '\0';
		// Ahora tmp es algo como /dev/graphics/vesa
		// Extraer el último componente de eso
		char* drvName = strrchr(tmp, '/');
		if (drvName != NULL)
			drvName++;
		else
			drvName = tmp;
		strlcpy(fDriverName, drvName, sizeof(fDriverName));
	} else {
		strlcpy(fDriverName, path, sizeof(fDriverName));
	}
}
