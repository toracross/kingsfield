// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#include "SwitcherooSettings.h"

#define LOCTEXT_NAMESPACE "SwitcherooSettings"

USwitcherooSettings::USwitcherooSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("Switcheroo");
}

void USwitcherooSettings::SetDefaultInputDevice(ESwitcherooInputDevice NewDefaultInputDevice)
{
	DefaultInputDevice = NewDefaultInputDevice;
}

bool USwitcherooSettings::IsAnalogInputOutsideDeadZone(const FVector2D& Input) const
{
	switch (AnalogDeadZoneMode)
	{
		case ESwitcherooAnalogDeadZoneMode::Axial:
			return (FMath::Abs(Input.X) > AnalogDeadZoneSize) || (FMath::Abs(Input.Y) > AnalogDeadZoneSize);

		case ESwitcherooAnalogDeadZoneMode::Radial:
			return Input.Size() > AnalogDeadZoneSize;

		case ESwitcherooAnalogDeadZoneMode::Disabled:
		default:
			return true;
	}
}

FVector2D USwitcherooSettings::ApplyDeadZoneToAnalogInput(const FVector2D& Input) const
{
	FVector2D Output = Input;
	switch (AnalogDeadZoneMode)
	{
		case ESwitcherooAnalogDeadZoneMode::Axial:
		{
			if (FMath::Abs(Input.X) < AnalogDeadZoneSize)
			{
				Output.X = 0.0f;
			}

			if (FMath::Abs(Input.Y) < AnalogDeadZoneSize)
			{
				Output.Y = 0.0f;
			}
			break;
		}

		case ESwitcherooAnalogDeadZoneMode::Radial:
		{
			const float InputSize = Input.Size();
			if (InputSize < AnalogDeadZoneSize)
			{
				Output.Set(0.0f, 0.0f);
			}
			else
			{
				Output.Normalize();
				Output *= (InputSize - AnalogDeadZoneSize) / (1.0f - AnalogDeadZoneSize);
			}
			break;
		}

		case ESwitcherooAnalogDeadZoneMode::Disabled:
		default:
			// Nothing to do
			break;
	}

	return Output;
}

void USwitcherooSettings::SetMouseMoveThreshold(float NewMouseMoveThreshold)
{
	MouseMoveThreshold = FMath::Max(NewMouseMoveThreshold, 0.0f);
}

void USwitcherooSettings::SetAnalogStick(ESwitcherooAnalogStick NewAnalogStick)
{
	AnalogStick = NewAnalogStick;
}

void USwitcherooSettings::SetAnalogDeadZoneMode(ESwitcherooAnalogDeadZoneMode NewAnalogDeadZoneMode)
{
	AnalogDeadZoneMode = NewAnalogDeadZoneMode;
}

void USwitcherooSettings::SetAnalogDeadZoneSize(float NewAnalogDeadZoneSize)
{
	AnalogDeadZoneSize = FMath::Clamp(NewAnalogDeadZoneSize, 0.0f, 1.0f);
}

#if WITH_EDITOR
FText USwitcherooSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Switcheroo");
}

FText USwitcherooSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Switcheroo runtime settings.");
}
#endif

#undef LOCTEXT_NAMESPACE
