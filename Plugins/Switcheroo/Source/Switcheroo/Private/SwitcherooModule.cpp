// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#include "SwitcherooModule.h"
#include "SwitcherooSettings.h"
#include "SwitcherooGlobals.h"
#include "SwitcherooTypes.h"

#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Stats/Stats.h"

DECLARE_CYCLE_STAT(TEXT("SwitcherooTick"), STAT_SwitcherooTick, STATGROUP_Switcheroo);

/** Input processor. */
class FSwitcherooInputProcessor final : public IInputProcessor
{

public:

	FSwitcherooInputProcessor(FSwitcherooModule* InOwner)
		: Owner(InOwner)
	{}

private:

	/** Information about an analog input. */
	struct FSwitcherooAnalogInput
	{
		FVector2D Value = FVector2D::ZeroVector;
		double Time = -1000.0;
	};

	FSwitcherooModule* Owner;
	double LastKeyboardMouseInputTime = -1000.0;
	double LastGamepadInputTime = -1000.0;
	FSwitcherooAnalogInput GamepadAnalogInputs[2];

private:

	static FORCEINLINE bool IsVirtualKey(const FKey& InKey)
	{
		// Gamepad
		if (InKey == EKeys::Gamepad_LeftStick_Right
			|| InKey == EKeys::Gamepad_LeftStick_Left
			|| InKey == EKeys::Gamepad_LeftStick_Up
			|| InKey == EKeys::Gamepad_LeftStick_Down
			|| InKey == EKeys::Gamepad_RightStick_Right
			|| InKey == EKeys::Gamepad_RightStick_Left
			|| InKey == EKeys::Gamepad_RightStick_Up
			|| InKey == EKeys::Gamepad_RightStick_Down)
		{
			return true;
		}

		// Motion controllers
		if (InKey == EKeys::Vive_Left_Trackpad_Up
			|| InKey == EKeys::Vive_Left_Trackpad_Down
			|| InKey == EKeys::Vive_Left_Trackpad_Left
			|| InKey == EKeys::Vive_Left_Trackpad_Right
			|| InKey == EKeys::Vive_Right_Trackpad_Up
			|| InKey == EKeys::Vive_Right_Trackpad_Down
			|| InKey == EKeys::Vive_Right_Trackpad_Left
			|| InKey == EKeys::Vive_Right_Trackpad_Right
			|| InKey == EKeys::MixedReality_Left_Thumbstick_Up
			|| InKey == EKeys::MixedReality_Left_Thumbstick_Down
			|| InKey == EKeys::MixedReality_Left_Thumbstick_Left
			|| InKey == EKeys::MixedReality_Left_Thumbstick_Right
			|| InKey == EKeys::MixedReality_Right_Thumbstick_Up
			|| InKey == EKeys::MixedReality_Right_Thumbstick_Down
			|| InKey == EKeys::MixedReality_Right_Thumbstick_Left
			|| InKey == EKeys::MixedReality_Right_Thumbstick_Right
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_Up
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_Down
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_Left
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_Right
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_Up
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_Down
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_Left
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_Right
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_Up
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_Down
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_Left
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_Right
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_Up
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_Down
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_Left
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_Right)
		{
			return true;
		}

		return false;
	}

	static FORCEINLINE bool IsMotionControllerLeftX(const FKey& InKey)
	{
		return InKey == EKeys::Daydream_Left_Trackpad_X
			|| InKey == EKeys::Vive_Left_Trackpad_X
			|| InKey == EKeys::MixedReality_Left_Thumbstick_X
			|| InKey == EKeys::OculusGo_Left_Trackpad_X
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_X
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_X;
	}

	static FORCEINLINE bool IsMotionControllerRightX(const FKey& InKey)
	{
		return InKey == EKeys::Daydream_Right_Trackpad_X
			|| InKey == EKeys::Vive_Right_Trackpad_X
			|| InKey == EKeys::MixedReality_Right_Thumbstick_X
			|| InKey == EKeys::OculusGo_Right_Trackpad_X
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_X
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_X;
	}

	static FORCEINLINE bool IsMotionControllerLeftY(const FKey& InKey)
	{
		return InKey == EKeys::Daydream_Left_Trackpad_Y
			|| InKey == EKeys::Vive_Left_Trackpad_Y
			|| InKey == EKeys::MixedReality_Left_Thumbstick_Y
			|| InKey == EKeys::OculusGo_Left_Trackpad_Y
			|| InKey == EKeys::OculusTouch_Left_Thumbstick_Y
			|| InKey == EKeys::ValveIndex_Left_Thumbstick_Y;
	}

	static FORCEINLINE bool IsMotionControllerRightY(const FKey& InKey)
	{
		return InKey == EKeys::Daydream_Right_Trackpad_Y
			|| InKey == EKeys::Vive_Right_Trackpad_Y
			|| InKey == EKeys::MixedReality_Right_Thumbstick_Y
			|| InKey == EKeys::OculusGo_Right_Trackpad_Y
			|| InKey == EKeys::OculusTouch_Right_Thumbstick_Y
			|| InKey == EKeys::ValveIndex_Right_Thumbstick_Y;
	}

	// IInputProcessor interface
public:
	void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		SCOPE_CYCLE_COUNTER(STAT_SwitcherooTick);
		check(Owner != nullptr);

		// Detect input method
		switch (Owner->GetDetectedInputDevice())
		{
			case ESwitcherooInputDevice::Gamepad:
			{
				// Check for keyboard input
				if (LastKeyboardMouseInputTime > LastGamepadInputTime
					&& LastKeyboardMouseInputTime > GamepadAnalogInputs[0].Time
					&& LastKeyboardMouseInputTime > GamepadAnalogInputs[1].Time)
				{
					Owner->UpdateDetectedDevice(ESwitcherooInputDevice::KeyboardMouse);
				}
				break;
			}

			case ESwitcherooInputDevice::KeyboardMouse:
			{
				// Check for gamepad input
				if (LastGamepadInputTime > LastKeyboardMouseInputTime)
				{
					Owner->UpdateDetectedDevice(ESwitcherooInputDevice::Gamepad);
				}
				else
				{
					// Check analog input
					bool bHasValidAnalogInput = false;

					const USwitcherooSettings* Settings = GetDefault<USwitcherooSettings>();
					switch (Settings->GetAnalogStick())
					{
						case ESwitcherooAnalogStick::Left:
							bHasValidAnalogInput =
								(GamepadAnalogInputs[0].Time > LastKeyboardMouseInputTime) && Settings->IsAnalogInputOutsideDeadZone(GamepadAnalogInputs[0].Value);
							break;

						case ESwitcherooAnalogStick::Right:
							bHasValidAnalogInput =
								(GamepadAnalogInputs[1].Time > LastKeyboardMouseInputTime) && Settings->IsAnalogInputOutsideDeadZone(GamepadAnalogInputs[1].Value);
							break;

						case ESwitcherooAnalogStick::Both:
							bHasValidAnalogInput =
								((GamepadAnalogInputs[0].Time > LastKeyboardMouseInputTime) && Settings->IsAnalogInputOutsideDeadZone(GamepadAnalogInputs[0].Value))
								|| ((GamepadAnalogInputs[1].Time > LastKeyboardMouseInputTime) && Settings->IsAnalogInputOutsideDeadZone(GamepadAnalogInputs[1].Value));
							break;
					}

					if (bHasValidAnalogInput)
					{
						Owner->UpdateDetectedDevice(ESwitcherooInputDevice::Gamepad);
					}
				}
				break;
			}
		}
	}

	bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& KeyEvent) override
	{
		if (KeyEvent.GetUserIndex() != SlateApp.GetUserIndexForKeyboard())
		{
			// Only interested in keyboard/mouse user
			return false;
		}

		const FKey Key = KeyEvent.GetKey();
		if (Key.IsGamepadKey())
		{
			// Not interested in virtual keys
			if (IsVirtualKey(Key))
			{
				return false;
			}

			LastGamepadInputTime = SlateApp.GetCurrentTime();
		}
		else
		{
			LastKeyboardMouseInputTime = SlateApp.GetCurrentTime();
		}

		return false;
	}

	bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& AnalogInputEvent) override
	{
		if (AnalogInputEvent.GetUserIndex() != SlateApp.GetUserIndexForKeyboard())
		{
			// Only interested in keyboard/mouse user
			return false;
		}

		const FKey AnalogKey = AnalogInputEvent.GetKey();
		if (AnalogKey == EKeys::Gamepad_LeftX || IsMotionControllerLeftX(AnalogKey))
		{
			GamepadAnalogInputs[0].Value.X = AnalogInputEvent.GetAnalogValue();
			GamepadAnalogInputs[0].Time = SlateApp.GetCurrentTime();
		}
		else if (AnalogKey == EKeys::Gamepad_LeftY || IsMotionControllerLeftY(AnalogKey))
		{
			GamepadAnalogInputs[0].Value.Y = -AnalogInputEvent.GetAnalogValue();
			GamepadAnalogInputs[0].Time = SlateApp.GetCurrentTime();
		}
		else if (AnalogKey == EKeys::Gamepad_RightX || IsMotionControllerRightX(AnalogKey))
		{
			GamepadAnalogInputs[1].Value.X = AnalogInputEvent.GetAnalogValue();
			GamepadAnalogInputs[1].Time = SlateApp.GetCurrentTime();
		}
		else if (AnalogKey == EKeys::Gamepad_RightY || IsMotionControllerRightY(AnalogKey))
		{
			GamepadAnalogInputs[1].Value.Y = -AnalogInputEvent.GetAnalogValue();
			GamepadAnalogInputs[1].Time = SlateApp.GetCurrentTime();
		}

		return false;
	}

	bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetUserIndex() != SlateApp.GetUserIndexForKeyboard())
		{
			// Only interested in keyboard/mouse user
			return false;
		}

		LastKeyboardMouseInputTime = SlateApp.GetCurrentTime();
		return false;
	}

	bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetUserIndex() != SlateApp.GetUserIndexForKeyboard())
		{
			// Only interested in keyboard/mouse user
			return false;
		}

		const float MoveThreshold = GetDefault<USwitcherooSettings>()->GetMouseMoveThreshold();
		if (MoveThreshold < SMALL_NUMBER
			|| MouseEvent.GetCursorDelta().SizeSquared() > FMath::Square(MoveThreshold))
		{
			LastKeyboardMouseInputTime = SlateApp.GetCurrentTime();
		}

		return false;
	}

	bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent, const FPointerEvent* GestureEvent) override
	{
		if (GestureEvent != nullptr)
		{
			// Only interested in mouse wheel events
			return false;
		}

		if (MouseEvent.GetUserIndex() != SlateApp.GetUserIndexForKeyboard())
		{
			// Only interested in keyboard/mouse user
			return false;
		}

		LastKeyboardMouseInputTime = SlateApp.GetCurrentTime();
		return false;
	}

};

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE(FSwitcherooModule, Switcheroo)

void FSwitcherooModule::UpdateDetectedDevice(ESwitcherooInputDevice NewDetectedDevice)
{
	if (!DetectedDevice.IsSet() || NewDetectedDevice != DetectedDevice.GetValue())
	{
		DetectedDevice = NewDetectedDevice;
		InputDeviceChangedEvent.Broadcast(NewDetectedDevice);

		for (int32 i = Delegates.Num() - 1; i > INDEX_NONE; i--)
		{
			if (Delegates[i].IsBound())
			{
				Delegates[i].Execute(NewDetectedDevice);
			}
			else
			{
				Delegates.RemoveAtSwap(i, 1, false);
			}
		}
	}
}

void FSwitcherooModule::Cleanup()
{
	DisableInputDetection();
	Delegates.Empty();
	InputDeviceChangedEvent.Clear();
}

bool FSwitcherooModule::IsInputDetectionEnabled() const
{
	return InputProcessor.IsValid();
}

void FSwitcherooModule::EnableInputDetection()
{
	if (!InputProcessor.IsValid() && FSlateApplication::IsInitialized())
	{
		InputProcessor = MakeShared<FSwitcherooInputProcessor>(this);
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor);
	}
}

void FSwitcherooModule::DisableInputDetection()
{
	if (InputProcessor.IsValid())
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		}

		InputProcessor.Reset();
	}
}

void FSwitcherooModule::SetInputDetectionEnabled(const bool bEnabled)
{
	if (bEnabled)
	{
		EnableInputDetection();
	}
	else
	{
		DisableInputDetection();
	}
}

ESwitcherooInputDevice FSwitcherooModule::GetDetectedInputDevice() const
{
	return DetectedDevice.Get(GetDefault<USwitcherooSettings>()->GetDefaultInputDevice());
}

FSwitcherooModule::FSwitcherooInputDeviceChangedEvent& FSwitcherooModule::OnDetectedInputDeviceChanged()
{
	return InputDeviceChangedEvent;
}

void FSwitcherooModule::AddOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate)
{
	if (InDelegate.IsBound())
	{
		Delegates.AddUnique(InDelegate);
	}
}

void FSwitcherooModule::RemoveOnDetectedInputDeviceChangedDynamicDelegate(FSwitcherooInputDeviceChangedDelegate InDelegate)
{
	Delegates.RemoveSingle(InDelegate);
}

#if WITH_EDITOR
void FSwitcherooModule::CleanupAfterPIE()
{
	Cleanup();
}
#endif

void FSwitcherooModule::ShutdownModule()
{
	Cleanup();
}
