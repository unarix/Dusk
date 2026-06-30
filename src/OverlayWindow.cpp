/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "OverlayWindow.h"
#include "OverlayView.h"


OverlayWindow::OverlayWindow(BRect frame, uint8 red, uint8 green,
	uint8 blue, uint8 alpha)
	:
	BWindow(frame, "DuskOverlay",
		B_NO_BORDER_WINDOW_LOOK,
		B_FLOATING_ALL_WINDOW_FEEL,
		B_NOT_MOVABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE
			| B_NOT_MINIMIZABLE | B_AVOID_FOCUS
			| B_NOT_ANCHORED_ON_ACTIVATE),
	fView(NULL)
{
	// Hacemos que la ventana no acepte clicks — los deja pasar
	SetFlags(Flags() | B_WILL_ACCEPT_FIRST_CLICK);

	fView = new OverlayView(frame.OffsetToCopy(0, 0), red, green, blue, alpha);
	AddChild(fView);
}


OverlayWindow::~OverlayWindow()
{
}


void
OverlayWindow::SetColor(uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	if (Lock()) {
		fView->SetColor(red, green, blue, alpha);
		Unlock();
	}
}
