/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef TRAY_VIEW_H
#define TRAY_VIEW_H

#include <View.h>

class GammaEngine;


// Replicante para el Deskbar tray
class TrayView : public BView {
public:
						TrayView(GammaEngine* engine);
						TrayView(BMessage* archive);
	virtual				~TrayView();

	// replicant stuff
	static TrayView*	Instantiate(BMessage* archive);
	virtual status_t	Archive(BMessage* archive, bool deep = true) const;

	virtual void		Draw(BRect updateRect);
	virtual void		MouseDown(BPoint where);
	virtual void		MessageReceived(BMessage* message);

	virtual void		AttachedToWindow();

private:
	GammaEngine*		fEngine;
	bool				fActive;
};

#endif // TRAY_VIEW_H
