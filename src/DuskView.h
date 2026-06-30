/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef DUSK_VIEW_H
#define DUSK_VIEW_H

#include <View.h>

class BButton;
class BSlider;
class BStringView;
class GammaEngine;


class DuskView : public BView {
public:
						DuskView(BRect frame, GammaEngine* engine);
	virtual				~DuskView();

	virtual void		AttachedToWindow();
	void				UpdateState();

private:
	GammaEngine*		fEngine;
	BButton*			fToggleButton;
	BSlider*			fTempSlider;
	BStringView*		fStatusLabel;
	BStringView*		fTempLabel;
	BStringView*		fDriverLabel;
};

#endif // DUSK_VIEW_H
