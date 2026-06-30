/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */
#ifndef DUSK_WINDOW_H
#define DUSK_WINDOW_H

#include <Window.h>

#include "GammaEngine.h"

class DuskView;

// Message constants
enum {
	kMsgToggle			= 'tggl',
	kMsgSetTemperature	= 'stmp',
	kMsgQuitApp			= 'quit'
};


class DuskWindow : public BWindow {
public:
						DuskWindow();
	virtual				~DuskWindow();

	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

	GammaEngine*		Engine() { return &fEngine; }

private:
	DuskView*			fView;
	GammaEngine			fEngine;
};

#endif // DUSK_WINDOW_H
