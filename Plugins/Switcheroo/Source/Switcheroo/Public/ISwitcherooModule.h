// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "Modules/ModuleManager.h"

enum class ESwitcherooInputDevice : uint8;
class FSwitcherooInputDeviceChangedDelegate;

/**
 * Public interface to Switcheroo.
 */
class SWITCHEROO_API ISwitcherooModule : public IModuleInterface
{

public:

	/** Gets whether the module is loaded and ready. */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("Switcheroo");
	}

	/** Gets the module. */
	static inline ISwitcherooModule* Get()
	{
		return FModuleManager::GetModulePtr<ISwitcherooModule>("Switcheroo");
	}

	/** Gets the module. Checked version.  */
	static inline ISwitcherooModule& GetChecked()
	{
		ISwitcherooModule* Module = Get();
		checkf(Module != nullptr, TEXT("Switcheroo is not loaded."));
		return *Module;
	}

public:

	/** Gets whether detection of the input device is enabled or disabled. */
	virtual bool IsInputDetectionEnabled() const = 0;

	/** Enables detection of the input device. */
	virtual void EnableInputDetection() = 0;

	/** Disables detection of the input device. */
	virtual void DisableInputDetection() = 0;

	/** Enables or disables detection of the input device. */
	virtual void SetInputDetectionEnabled(const bool bEnabled) = 0;

	/**
	 * Gets the detected input device.
	 * @returns The active input device if one has been detected; otherwise, the default input device specified in project settings.
	 */
	virtual ESwitcherooInputDevice GetDetectedInputDevice() const = 0;

	/** Event fired when the detected input device changes. */
	DECLARE_EVENT_OneParam(ISwitcherooModule, FSwitcherooInputDeviceChangedEvent, ESwitcherooInputDevice /* ActiveInputDevice */);
	virtual FSwitcherooInputDeviceChangedEvent& OnDetectedInputDeviceChanged() = 0;

	/** Registers a dynamic delegate to be called when the detected input device changes. */
	virtual void AddOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate) = 0;

	/** Unregisters a dynamic delegate. */
	virtual void RemoveOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate) = 0;

#if WITH_EDITOR
public:

	/** Performs some cleanup after PIE has ended. */
	virtual void CleanupAfterPIE() = 0;

#endif

};
