/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <Window.h>


class OverlayView;

// Ventana fullscreen transparente que cubre toda la pantalla.
// No acepta foco, no acepta input, queda flotando arriba de todo.
// El truco es usar B_FLOATING_ALL_WINDOW_FEEL para que quede encima
// de todas las ventanas, y B_AVOID_FOCUS + flags de input para que
// no interfiera con el uso normal.
class OverlayWindow : public BWindow {
public:
						OverlayWindow(BRect frame, uint8 red, uint8 green,
							uint8 blue, uint8 alpha);
	virtual				~OverlayWindow();

	void				SetColor(uint8 red, uint8 green, uint8 blue,
							uint8 alpha);

private:
	OverlayView*		fView;
};

#endif // OVERLAY_WINDOW_H
