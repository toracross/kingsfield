// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Switcheroo Editor module.
 */
class FSwitcherooEditorModule final : public IModuleInterface
{

public:

	FSwitcherooEditorModule() {}

private:

	FDelegateHandle DelegateHandle_EndPIE;

	void OnEndPIE(const bool bIsSimulating);

	// IModuleInterface interface
public:
	void StartupModule() override;
	void ShutdownModule() override;

};
