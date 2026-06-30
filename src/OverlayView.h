/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef OVERLAY_VIEW_H
#define OVERLAY_VIEW_H

#include <View.h>


// Vista que simplemente pinta un color sólido con alpha.
// El blending se encarga de mezclar con lo que hay debajo.
class OverlayView : public BView {
public:
						OverlayView(BRect frame, uint8 red, uint8 green,
							uint8 blue, uint8 alpha);
	virtual				~OverlayView();

	virtual void		Draw(BRect updateRect);
	void				SetColor(uint8 red, uint8 green, uint8 blue,
							uint8 alpha);

private:
	uint8				fRed;
	uint8				fGreen;
	uint8				fBlue;
	uint8				fAlpha;
};

#endif // OVERLAY_VIEW_H
