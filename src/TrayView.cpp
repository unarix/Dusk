/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "TrayView.h"
#include "DuskWindow.h"
#include "GammaEngine.h"

#include <Bitmap.h>
#include <Deskbar.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Window.h>


static const float kTrayIconSize = 16.0f;


TrayView::TrayView(GammaEngine* engine)
	:
	BView(BRect(0, 0, kTrayIconSize - 1, kTrayIconSize - 1),
		"Dusk", B_FOLLOW_NONE, B_WILL_DRAW),
	fEngine(engine),
	fActive(false)
{
}


TrayView::TrayView(BMessage* archive)
	:
	BView(archive),
	fEngine(NULL),
	fActive(false)
{
}


TrayView::~TrayView()
{
}


TrayView*
TrayView::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, "TrayView"))
		return NULL;
	return new TrayView(archive);
}


status_t
TrayView::Archive(BMessage* archive, bool deep) const
{
	status_t status = BView::Archive(archive, deep);
	if (status == B_OK)
		status = archive->AddString("class", "TrayView");
	if (status == B_OK)
		status = archive->AddString("add_on", "application/x-vnd.Dusk");
	return status;
}


void
TrayView::AttachedToWindow()
{
	BView::AttachedToWindow();
	if (Parent() != NULL)
		SetViewColor(Parent()->ViewColor());
}


void
TrayView::Draw(BRect updateRect)
{
	(void)updateRect;

	BRect bounds = Bounds();

	// circulito simple: naranja si está activo, gris si no
	if (fActive)
		SetHighColor(255, 160, 40);
	else
		SetHighColor(140, 140, 140);

	FillEllipse(bounds.InsetByCopy(2, 2));

	// borde
	SetHighColor(60, 60, 60);
	StrokeEllipse(bounds.InsetByCopy(2, 2));
}


void
TrayView::MouseDown(BPoint where)
{
	(void)where;

	BPoint cursor;
	uint32 buttons;
	GetMouse(&cursor, &buttons);

	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
		// toggle directo
		if (fEngine != NULL) {
			fEngine->Toggle();
			fActive = fEngine->IsEnabled();
			Invalidate();
		}
	} else if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		// menú contextual
		BPopUpMenu* menu = new BPopUpMenu("DuskMenu", false, false);

		BMenuItem* toggleItem = new BMenuItem(
			fActive ? "Desactivar" : "Activar",
			new BMessage(kMsgToggle));
		menu->AddItem(toggleItem);

		menu->AddSeparatorItem();

		BMenuItem* quitItem = new BMenuItem("Salir",
			new BMessage(kMsgQuitApp));
		menu->AddItem(quitItem);

		menu->SetTargetForItems(Window());
		ConvertToScreen(&cursor);
		menu->Go(cursor, true, true, true);
	}
}


void
TrayView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgToggle:
			if (fEngine != NULL) {
				fEngine->Toggle();
				fActive = fEngine->IsEnabled();
				Invalidate();
			}
			break;
		default:
			BView::MessageReceived(message);
			break;
	}
}
