/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "DuskWindow.h"
#include "DuskView.h"

#include <Application.h>


DuskWindow::DuskWindow()
	:
	BWindow(BRect(100, 100, 380, 310), "Dusk",
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fView(NULL)
{
	fView = new DuskView(Bounds(), &fEngine);
	AddChild(fView);

	CenterOnScreen();
}


DuskWindow::~DuskWindow()
{
}


void
DuskWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgToggle:
			fEngine.Toggle();
			fView->UpdateState();
			break;

		case kMsgSetTemperature:
		{
			// BSlider manda "be:value" con el valor actual
			int32 temp;
			if (message->FindInt32("be:value", &temp) == B_OK) {
				fEngine.SetTemperature(temp);
				fView->UpdateState();
			}
			break;
		}

		case kMsgQuitApp:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


bool
DuskWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
