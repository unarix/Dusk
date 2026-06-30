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
#include <Font.h>
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
	fTempLabel(NULL),
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

	// -- Título y estado --
	BStringView* titleLabel = new BStringView("title", "Dusk");
	BFont titleFont(be_bold_font);
	titleFont.SetSize(16.0f);
	titleLabel->SetFont(&titleFont);

	fStatusLabel = new BStringView("status", "Inactivo");
	fStatusLabel->SetFontSize(11.0f);

	// -- Indicador de temperatura actual --
	fTempLabel = new BStringView("tempLabel", "3500K");
	BFont tempFont(be_bold_font);
	tempFont.SetSize(13.0f);
	fTempLabel->SetFont(&tempFont);
	fTempLabel->SetAlignment(B_ALIGN_CENTER);

	// -- Slider --
	fTempSlider = new BSlider("tempSlider", NULL,
		new BMessage(kMsgSetTemperature),
		kMinTemperature, kMaxTemperature, B_HORIZONTAL);
	fTempSlider->SetValue(fEngine->Temperature());
	fTempSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fTempSlider->SetHashMarkCount(10);
	fTempSlider->SetLimitLabels("1900K", "6500K");
	fTempSlider->SetTarget(Window());
	fTempSlider->SetModificationMessage(new BMessage(kMsgSetTemperature));
	fTempSlider->SetBarColor((rgb_color){255, 140, 40, 255});

	// -- Botón toggle --
	fToggleButton = new BButton("toggle", "Activar",
		new BMessage(kMsgToggle));
	fToggleButton->SetTarget(Window());
	fToggleButton->MakeDefault(true);

	// -- Info del driver --
	BString driverStr;
	driverStr.SetToFormat("GPU: %s", fEngine->DriverName());
	fDriverLabel = new BStringView("driver", driverStr.String());
	fDriverLabel->SetFontSize(10.0f);
	fDriverLabel->SetHighUIColor(B_PANEL_TEXT_COLOR, 0.6f);

	// -- Layout --
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_WINDOW_INSETS)
		.AddGroup(B_HORIZONTAL)
			.Add(titleLabel)
			.AddGlue()
			.Add(fStatusLabel)
		.End()
		.AddStrut(B_USE_HALF_ITEM_SPACING)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddStrut(B_USE_ITEM_SPACING)
		.Add(fTempLabel)
		.AddStrut(B_USE_HALF_ITEM_SPACING)
		.Add(fTempSlider)
		.AddStrut(B_USE_ITEM_SPACING)
		.Add(fToggleButton)
		.AddGlue()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddStrut(B_USE_HALF_ITEM_SPACING)
		.Add(fDriverLabel)
	.End();

	// Si el driver no soporta gamma, deshabilitamos
	if (fEngine->InitCheck() != B_OK) {
		fStatusLabel->SetText("No soportado");
		fStatusLabel->SetHighColor(180, 40, 40);
		fToggleButton->SetEnabled(false);
		fTempSlider->SetEnabled(false);
	}
}


void
DuskView::UpdateState()
{
	if (fEngine->IsEnabled()) {
		BString status;
		status.SetToFormat("Activo");
		fStatusLabel->SetText(status.String());
		fStatusLabel->SetHighColor(40, 160, 60);
		fToggleButton->SetLabel("Desactivar");
	} else {
		fStatusLabel->SetText("Inactivo");
		fStatusLabel->SetHighUIColor(B_PANEL_TEXT_COLOR);
		fToggleButton->SetLabel("Activar");
	}

	BString tempStr;
	tempStr.SetToFormat("%ldK", (long)fEngine->Temperature());
	fTempLabel->SetText(tempStr.String());

	fTempSlider->SetValue(fEngine->Temperature());
}
