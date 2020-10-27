// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/ObjectMacros.h"
#include "SwitcherooTypes.h"
#include "SwitcherooSettings.generated.h"

/**
 * Switcheroo runtime settings.
 */
UCLASS(Config = Engine, DefaultConfig)
class SWITCHEROO_API USwitcherooSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	USwitcherooSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/**
	 * Default input device to use when no input has been detected yet.
	 * Can be used to select an appropriate default on different platforms (e.g. keyboard on PCs, gamepad on consoles).
	 */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetDefaultInputDevice, Config, Category = "General")
	ESwitcherooInputDevice DefaultInputDevice = ESwitcherooInputDevice::KeyboardMouse;

	/**
	 * The minimum movement that the mouse should perform in order for it to be considered valid input.
	 * Resolution independent, defined in Slate units. Useful to filter out involuntary, minimal mouse movements.
	 * Set to zero to disable.
	 */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetMouseMoveThreshold, Config, Category = "Keyboard / Mouse", Meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MouseMoveThreshold = 1.0f;

	/** Determines which analog stick should be watched for input. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetAnalogStick, Config, Category = "Gamepad")
	ESwitcherooAnalogStick AnalogStick = ESwitcherooAnalogStick::Left;

	/**
	 * Determines which type of dead zone should be applied to the input provided by gamepad analog sticks.
	 * Useful to filter out involuntary, minimal stick movements which can happen i.e. with loose analog sticks.
	 */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetAnalogDeadZoneMode, Config, Category = "Gamepad", Meta = (DisplayName = "Dead Zone Mode"))
	ESwitcherooAnalogDeadZoneMode AnalogDeadZoneMode = ESwitcherooAnalogDeadZoneMode::Axial;

	/** Size of dead zone applied to the input provided by gamepad analog sticks. */
	UPROPERTY(EditAnywhere, BlueprintGetter = GetAnalogDeadZoneSize, Config, Category = "Gamepad", Meta = (DisplayName = "Dead Zone Size", ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.0f, UIMax = 1.0f))
	float AnalogDeadZoneSize = 0.2f;

public:

	/** Gets whether the specified analog input is outside the current dead zone. */
	bool IsAnalogInputOutsideDeadZone(const FVector2D& Input) const;

	/** Applies the current dead zone to the specified analog input. */
	FVector2D ApplyDeadZoneToAnalogInput(const FVector2D& Input) const;

public:

	/** Gets the Switcheroo runtime settings. */
	UFUNCTION(BlueprintPure, Category = "Input|Switcheroo")
	static USwitcherooSettings* GetSwitcherooSettings();

	/** Gets the default input device. */
	UFUNCTION(BlueprintGetter)
	ESwitcherooInputDevice GetDefaultInputDevice() const;

	/** Sets the default input device. */
	UFUNCTION(BlueprintCallable, Category = "Input|Switcheroo|Settings")
	void SetDefaultInputDevice(ESwitcherooInputDevice NewDefaultInputDevice);

	/** Gets the minimum movement that the mouse should perform in order for it to be considered valid input. */
	UFUNCTION(BlueprintGetter)
	float GetMouseMoveThreshold() const;

	/** Gets the minimum movement that the mouse should perform in order for it to be considered valid input. */
	UFUNCTION(BlueprintCallable, Category = "Input|Switcheroo|Settings")
	void SetMouseMoveThreshold(float NewMouseMoveThreshold);

	/** Gets which analog stick should be watched for input. */
	UFUNCTION(BlueprintGetter)
	ESwitcherooAnalogStick GetAnalogStick() const;

	/** Gets which analog stick should be watched for input. */
	UFUNCTION(BlueprintCallable, Category = "Input|Switcheroo|Settings")
	void SetAnalogStick(ESwitcherooAnalogStick NewAnalogStick);

	/** Gets which type of dead zone is applied to the input provided by gamepad analog sticks. */
	UFUNCTION(BlueprintGetter)
	ESwitcherooAnalogDeadZoneMode GetAnalogDeadZoneMode() const;

	/** Gets which type of dead zone is applied to the input provided by gamepad analog sticks. */
	UFUNCTION(BlueprintCallable, Category = "Input|Switcheroo|Settings")
	void SetAnalogDeadZoneMode(ESwitcherooAnalogDeadZoneMode NewAnalogDeadZoneMode);

	/** Gets the size of the dead zone applied to the input provided by gamepad analog sticks. */
	UFUNCTION(BlueprintGetter)
	float GetAnalogDeadZoneSize() const;

	/** Gets the size of the dead zone applied to the input provided by gamepad analog sticks. */
	UFUNCTION(BlueprintCallable, Category = "Input|Switcheroo|Settings")
	void SetAnalogDeadZoneSize(float NewAnalogDeadZoneSize);

	// UDeveloperSettings interface
public:
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif

};

// USwitcherooSettings inlines
FORCEINLINE USwitcherooSettings* USwitcherooSettings::GetSwitcherooSettings() { return GetMutableDefault<USwitcherooSettings>(); }
FORCEINLINE ESwitcherooInputDevice USwitcherooSettings::GetDefaultInputDevice() const { return DefaultInputDevice; }
FORCEINLINE float USwitcherooSettings::GetMouseMoveThreshold() const { return MouseMoveThreshold; }
FORCEINLINE ESwitcherooAnalogStick USwitcherooSettings::GetAnalogStick() const { return AnalogStick; }
FORCEINLINE ESwitcherooAnalogDeadZoneMode USwitcherooSettings::GetAnalogDeadZoneMode() const { return AnalogDeadZoneMode; }
FORCEINLINE float USwitcherooSettings::GetAnalogDeadZoneSize() const { return AnalogDeadZoneSize; }
