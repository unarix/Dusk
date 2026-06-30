/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "DuskApp.h"
#include "DuskWindow.h"


static const char* kAppSignature = "application/x-vnd.Dusk";


DuskApp::DuskApp()
	:
	BApplication(kAppSignature),
	fWindow(NULL)
{
}


DuskApp::~DuskApp()
{
}


void
DuskApp::ReadyToRun()
{
	fWindow = new DuskWindow();
	fWindow->Show();
}


void
DuskApp::MessageReceived(BMessage* message)
{
	switch (message->what) {
		default:
			BApplication::MessageReceived(message);
			break;
	}
}
