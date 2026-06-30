/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef DUSK_APP_H
#define DUSK_APP_H

#include <Application.h>

class DuskWindow;

class DuskApp : public BApplication {
public:
						DuskApp();
	virtual				~DuskApp();

	virtual void		ReadyToRun();
	virtual void		MessageReceived(BMessage* message);

private:
	DuskWindow*			fWindow;
};

#endif // DUSK_APP_H
