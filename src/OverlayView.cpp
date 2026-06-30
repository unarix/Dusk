/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "OverlayView.h"


OverlayView::OverlayView(BRect frame, uint8 red, uint8 green, uint8 blue,
	uint8 alpha)
	:
	BView(frame, "OverlayView", B_FOLLOW_ALL,
		B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fRed(red),
	fGreen(green),
	fBlue(blue),
	fAlpha(alpha)
{
	// El view no acepta eventos de mouse/teclado
	SetEventMask(0);
}


OverlayView::~OverlayView()
{
}


void
OverlayView::Draw(BRect updateRect)
{
	rgb_color color = {fRed, fGreen, fBlue, fAlpha};

	SetHighColor(color);
	SetDrawingMode(B_OP_ALPHA);
	SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
	FillRect(updateRect);
}


void
OverlayView::SetColor(uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	fRed = red;
	fGreen = green;
	fBlue = blue;
	fAlpha = alpha;
	Invalidate();
}
