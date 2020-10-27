// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SwitcherooTypes.h"
#include "SwitcherooBlueprintLibrary.generated.h"

/**
 * Switcheroo Blueprint library.
 */
UCLASS(Meta = (ScriptName = "SwitcherooLibrary"))
class SWITCHEROO_API USwitcherooBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Gets whether the detection of the input device is enabled or disabled. */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Input|Switcheroo")
	static bool IsInputDetectionEnabled();

	/** Enables detection of the input device. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input|Switcheroo")
	static void EnableInputDetection();

	/** Disables detection of the input device. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input|Switcheroo")
	static void DisableInputDetection();

	/** Enables or disables detection of the input device. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input|Switcheroo")
	static void SetInputDetectionEnabled(const bool bEnabled);

	/**
	 * Gets the detected input device.
	 * @returns The active input device if one has been detected; otherwise, the default input device specified in project settings.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Input|Switcheroo")
	static ESwitcherooInputDevice GetDetectedInputDevice();

	/** Registers an event to be called when the detected input device changes. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input|Switcheroo")
	static void BindDetectedInputDeviceChanged(FSwitcherooInputDeviceChangedDelegate Event);

	/** Unregisters an event previously registered using BindDetectedInputDeviceChanged. */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input|Switcheroo")
	static void UnbindDetectedInputDeviceChanged(FSwitcherooInputDeviceChangedDelegate Event);

	//////////////////////////////////////////////////////////////////////////

	/** Gets whether the specified analog input is outside the current dead zone. */
	UFUNCTION(BlueprintPure, Category = "Input|Switcheroo|Utilities")
	static bool IsAnalogInputOutsideDeadZone(FVector2D Input);

	/** Applies the current dead zone to the specified analog input. */
	UFUNCTION(BlueprintPure, Category = "Input|Switcheroo|Utilities")
	static FVector2D ApplyDeadZoneToAnalogInput(FVector2D Input);

};
