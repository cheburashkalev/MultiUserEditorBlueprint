// Fill out your copyright notice in the Description page of Project Settings.


#include "MUE_Settings.h"

#include "EditorConfigSubsystem.h"

UMUE_Settings::UMUE_Settings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMUE_Settings::SaveConfig()
{
	
	FString platFormName = FPlatformProperties::PlatformName();
	FString gameConfigPath = FPaths::ProjectConfigDir() + TEXT("DefaultEngine.ini");
	
	if (GConfig)
	{
		GConfig->SetString(TEXT("/Script/BP_MULTY_USER_EDITOR.MUE_Settings"),
			TEXT("ip_port"),
			*ip_port,
			gameConfigPath);
 
		GConfig->Flush(true, gameConfigPath);
		GEditor->GetEditorSubsystem<UEditorConfigSubsystem>()->TryUpdateDefaultConfigFile(gameConfigPath);
	}
}
UMUE_Settings* UMUE_Settings::LoadConfig()
{
	
	FString platFormName = FPlatformProperties::PlatformName();
	FString gameConfigPath = FPaths::ProjectConfigDir() + TEXT("DefaultEngine.ini");
	
	if (GConfig)
	{
		GConfig->GetString(TEXT("/Script/BP_MULTY_USER_EDITOR.MUE_Settings"),
			TEXT("ip_port"),
			ip_port,
			gameConfigPath);
 
		GConfig->Flush(true, gameConfigPath);
	}
	return this;
}
