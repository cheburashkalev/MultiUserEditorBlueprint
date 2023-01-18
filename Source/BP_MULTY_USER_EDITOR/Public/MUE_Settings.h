// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MUE_Settings.generated.h"

/**
 * 
*/
#define INI_SECTION TEXT("/Script/MUE.MUE_TCP_Server")

UCLASS(Config= Engine, DefaultConfig, meta = (DisplayName="MUE TCP Server"))
class BP_MULTY_USER_EDITOR_API UMUE_Settings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UMUE_Settings(const FObjectInitializer& ObjectInitializer);
void SaveConfig();
	UMUE_Settings* LoadConfig();
public:
	UPROPERTY(Config, EditAnywhere)
	FString ip_port;
	
};
