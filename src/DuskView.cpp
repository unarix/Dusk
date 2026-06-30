/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "DuskView.h"
#include "DuskWindow.h"
#include "GammaEngine.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Slider.h>
#include <StringView.h>
#include <String.h>


DuskView::DuskView(BRect frame, GammaEngine* engine)
	:
	BView(frame, "DuskView", B_FOLLOW_ALL, B_WILL_DRAW),
	fEngine(engine),
	fToggleButton(NULL),
	fTempSlider(NULL),
	fStatusLabel(NULL)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
}


DuskView::~DuskView()
{
}


void
DuskView::AttachedToWindow()
{
	BView::AttachedToWindow();

	fStatusLabel = new BStringView("status", "Off");
	fStatusLabel->SetFontSize(12.0f);

	fToggleButton = new BButton("toggle", "Activar",
		new BMessage(kMsgToggle));
	fToggleButton->SetTarget(Window());

	// slider de temperatura: 1900K a 6500K
	fTempSlider = new BSlider("tempSlider", "Temperatura",
		new BMessage(kMsgSetTemperature),
		kMinTemperature, kMaxTemperature, B_HORIZONTAL);
	fTempSlider->SetValue(fEngine->Temperature());
	fTempSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fTempSlider->SetHashMarkCount(10);
	fTempSlider->SetLimitLabels("Cálido", "Neutro");
	fTempSlider->SetTarget(Window());
	fTempSlider->SetModificationMessage(new BMessage(kMsgSetTemperature));

	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(fStatusLabel)
		.Add(fTempSlider)
		.Add(fToggleButton)
	.End();

	// Mostrar qué driver está en uso
	BString driverInfo;
	driverInfo.SetToFormat("Driver: %s", fEngine->DriverName());
	BStringView* driverLabel = new BStringView("driver", driverInfo.String());
	driverLabel->SetFontSize(10.0f);
	driverLabel->SetHighColor(100, 100, 100);
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(driverLabel)
	.End();

	// Si el driver no soporta gamma, deshabilitamos los controles
	if (fEngine->InitCheck() != B_OK) {
		fStatusLabel->SetText("Driver no soportado");
		fStatusLabel->SetHighColor(180, 40, 40);
		fToggleButton->SetEnabled(false);
		fTempSlider->SetEnabled(false);
	}
}


void
DuskView::UpdateState()
{
	if (fEngine->IsEnabled()) {
		BString label;
		label.SetToFormat("On — %ldK", (long)fEngine->Temperature());
		fStatusLabel->SetText(label.String());
		fToggleButton->SetLabel("Desactivar");
	} else {
		fStatusLabel->SetText("Off");
		fToggleButton->SetLabel("Activar");
	}

	fTempSlider->SetValue(fEngine->Temperature());
}
