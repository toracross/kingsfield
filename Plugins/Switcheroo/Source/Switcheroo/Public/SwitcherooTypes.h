// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SwitcherooTypes.generated.h"

/** Supported analog dead zones. */
UENUM(BlueprintType)
enum class ESwitcherooAnalogDeadZoneMode : uint8
{
	/** No dead zone. */
	Disabled = 0,

	/** Simple 4 ways dead zone. */
	Axial = 1,

	/** Radial dead zone. */
	Radial = 2,
};

/** Supported analog sticks. */
UENUM(BlueprintType)
enum class ESwitcherooAnalogStick : uint8
{
	/** Special value indicating both left and right sticks. */
	Both = 0,

	/** Left stick. */
	Left = 1,

	/** Right stick. */
	Right = 2,
};

/** Supported input device. */
UENUM(BlueprintType)
enum class ESwitcherooInputDevice : uint8
{
	/** Keyboard/mouse combo. */
	KeyboardMouse = 0 UMETA(DisplayName = "Keyboard / Mouse"),

	/** Gamepad. */
	Gamepad = 1,
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FSwitcherooInputDeviceChangedDelegate, ESwitcherooInputDevice, ActiveInputDevice);
