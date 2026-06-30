/*
 * Dusk - Night Shift for Haiku
 * Released under MIT License.
 * Copyright 2026, Nahuel Tello, <ntello@upe.edu.ar>
 */

#include "DuskView.h"
#include "DuskWindow.h"
#include "GammaEngine.h"

#include <Box.h>
#include <Button.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <Slider.h>
#include <StringView.h>
#include <String.h>


DuskView::DuskView(BRect frame, GammaEngine* engine)
	:
	BView(frame, "DuskView", B_FOLLOW_ALL, B_WILL_DRAW),
	fEngine(engine),
	fToggleButton(NULL),
	fTempSlider(NULL),
	fStatusLabel(NULL),
	fTempValue(NULL),
	fDriverLabel(NULL)
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

	// Estado actual
	fStatusLabel = new BStringView("status", "Inactivo");

	// Valor de temperatura actual
	fTempValue = new BStringView("tempValue", "3500K");
	fTempValue->SetAlignment(B_ALIGN_RIGHT);

	// Slider de temperatura
	fTempSlider = new BSlider("tempSlider", "Temperatura de color",
		new BMessage(kMsgSetTemperature),
		kMinTemperature, kMaxTemperature, B_HORIZONTAL);
	fTempSlider->SetValue(fEngine->Temperature());
	fTempSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fTempSlider->SetHashMarkCount(10);
	fTempSlider->SetLimitLabels("Cálido", "Neutro");
	fTempSlider->SetTarget(Window());
	fTempSlider->SetModificationMessage(new BMessage(kMsgSetTemperature));

	// Botón toggle
	fToggleButton = new BButton("toggle", "Activar",
		new BMessage(kMsgToggle));
	fToggleButton->SetTarget(Window());

	// Grupo "Filtro" con BBox
	BBox* filterBox = new BBox("filterGroup");
	filterBox->SetLabel("Filtro");

	BLayoutBuilder::Group<>(filterBox, B_VERTICAL, B_USE_HALF_ITEM_SPACING)
		.SetInsets(B_USE_SMALL_INSETS)
		.AddGroup(B_HORIZONTAL)
			.Add(fStatusLabel)
			.AddGlue()
			.Add(fTempValue)
		.End()
		.Add(fTempSlider)
		.Add(fToggleButton)
	.End();

	// Info del driver
	BString driverStr;
	driverStr.SetToFormat("GPU: %s", fEngine->DriverName());
	fDriverLabel = new BStringView("driver", driverStr.String());

	// Layout principal
	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(filterBox)
		.AddGlue()
		.Add(fDriverLabel)
	.End();

	// Si el driver no soporta gamma
	if (fEngine->InitCheck() != B_OK) {
		fStatusLabel->SetText("No soportado");
		fStatusLabel->SetHighColor((rgb_color){180, 40, 40, 255});
		fToggleButton->SetEnabled(false);
		fTempSlider->SetEnabled(false);
	}
}


void
DuskView::UpdateState()
{
	if (fEngine->IsEnabled()) {
		fStatusLabel->SetText("Activo");
		fStatusLabel->SetHighColor((rgb_color){40, 140, 40, 255});
		fToggleButton->SetLabel("Desactivar");
	} else {
		fStatusLabel->SetText("Inactivo");
		fStatusLabel->SetHighUIColor(B_PANEL_TEXT_COLOR);
		fToggleButton->SetLabel("Activar");
	}

	BString tempStr;
	tempStr.SetToFormat("%ldK", (long)fEngine->Temperature());
	fTempValue->SetText(tempStr.String());

	fTempSlider->SetValue(fEngine->Temperature());
}
