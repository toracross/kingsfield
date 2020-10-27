// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "ISwitcherooModule.h"

enum class ESwitcherooInputDevice : uint8;
class FSwitcherooInputProcessor;

/**
 * Switcheroo module implementation.
 */
class FSwitcherooModule final : public ISwitcherooModule
{

public:

	DECLARE_DERIVED_EVENT(FSwitcherooModule, ISwitcherooModule::FSwitcherooInputDeviceChangedEvent, FSwitcherooInputDeviceChangedEvent);

private:

	/** Registered delegates to be called when the detected input device changes. */
	TArray<FSwitcherooInputDeviceChangedDelegate> Delegates;

	/** Input processor. */
	TSharedPtr<FSwitcherooInputProcessor> InputProcessor;

	/** Last detected device, if any. */
	TOptional<ESwitcherooInputDevice> DetectedDevice;

	/** Event fired when the detected input device changes. */
	FSwitcherooInputDeviceChangedEvent InputDeviceChangedEvent;

public:

	/** Called when the detected input device has changed. */
	void UpdateDetectedDevice(const ESwitcherooInputDevice NewDetectedDevice);

private:

	/** Performs cleanup when the module is shut down. */
	void Cleanup();

	// ISwitcherooModule interface
public:
	bool IsInputDetectionEnabled() const override;
	void EnableInputDetection() override;
	void DisableInputDetection() override;
	void SetInputDetectionEnabled(const bool bEnabled) override;
	ESwitcherooInputDevice GetDetectedInputDevice() const override;
	FSwitcherooInputDeviceChangedEvent& OnDetectedInputDeviceChanged() override;
	void AddOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate) override;
	void RemoveOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate) override;
#if WITH_EDITOR
	void CleanupAfterPIE() override;
#endif

	// IModuleInterface interface
public:
	void ShutdownModule() override;

};
