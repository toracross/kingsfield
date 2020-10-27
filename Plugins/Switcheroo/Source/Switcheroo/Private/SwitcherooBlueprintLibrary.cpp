// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#include "SwitcherooBlueprintLibrary.h"
#include "ISwitcherooModule.h"
#include "SwitcherooSettings.h"

#define LOCTEXT_NAMESPACE "SwitcherooBlueprintLibrary"

bool USwitcherooBlueprintLibrary::IsInputDetectionEnabled()
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		return Switcheroo->IsInputDetectionEnabled();
	}
	return false;
}

void USwitcherooBlueprintLibrary::EnableInputDetection()
{
	SetInputDetectionEnabled(true);
}

void USwitcherooBlueprintLibrary::DisableInputDetection()
{
	SetInputDetectionEnabled(false);
}

void USwitcherooBlueprintLibrary::SetInputDetectionEnabled(bool bEnabled)
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		Switcheroo->SetInputDetectionEnabled(bEnabled);
	}
}

ESwitcherooInputDevice USwitcherooBlueprintLibrary::GetDetectedInputDevice()
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		return Switcheroo->GetDetectedInputDevice();
	}
	return GetDefault<USwitcherooSettings>()->GetDefaultInputDevice();
}

void USwitcherooBlueprintLibrary::BindDetectedInputDeviceChanged(FSwitcherooInputDeviceChangedDelegate Event)
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		Switcheroo->AddOnDetectedInputDeviceChangedDynamicDelegate(Event);
	}
}

void USwitcherooBlueprintLibrary::UnbindDetectedInputDeviceChanged(FSwitcherooInputDeviceChangedDelegate Event)
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		Switcheroo->RemoveOnDetectedInputDeviceChangedDynamicDelegate(Event);
	}
}

bool USwitcherooBlueprintLibrary::IsAnalogInputOutsideDeadZone(FVector2D Input)
{
	const USwitcherooSettings* Settings = GetDefault<USwitcherooSettings>();
	return Settings->IsAnalogInputOutsideDeadZone(Input);
}

FVector2D USwitcherooBlueprintLibrary::ApplyDeadZoneToAnalogInput(FVector2D Input)
{
	const USwitcherooSettings* Settings = GetDefault<USwitcherooSettings>();
	return Settings->ApplyDeadZoneToAnalogInput(Input);
}

#undef LOCTEXT_NAMESPACE
