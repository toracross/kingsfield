// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#include "SwitcherooEditorModule.h"
#include "ISwitcherooModule.h"
#include "Editor.h"

IMPLEMENT_MODULE(FSwitcherooEditorModule, SwitcherooEditor)

void FSwitcherooEditorModule::OnEndPIE(const bool bIsSimulating)
{
	if (ISwitcherooModule* Switcheroo = ISwitcherooModule::Get())
	{
		Switcheroo->CleanupAfterPIE();
	}
}

void FSwitcherooEditorModule::StartupModule()
{
	if (!IsRunningCommandlet())
	{
		DelegateHandle_EndPIE = FEditorDelegates::EndPIE.AddRaw(this, &FSwitcherooEditorModule::OnEndPIE);
	}
}

void FSwitcherooEditorModule::ShutdownModule()
{
	if (DelegateHandle_EndPIE.IsValid())
	{
		FEditorDelegates::EndPIE.Remove(DelegateHandle_EndPIE);
		DelegateHandle_EndPIE.Reset();
	}
}
