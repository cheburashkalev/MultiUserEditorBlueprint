// Copyright Epic Games, Inc. All Rights Reserved.

#include "BP_MULTY_USER_EDITORStyle.h"
#include "BP_MULTY_USER_EDITOR.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FBP_MULTY_USER_EDITORStyle::StyleInstance = nullptr;

void FBP_MULTY_USER_EDITORStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FBP_MULTY_USER_EDITORStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FBP_MULTY_USER_EDITORStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("BP_MULTY_USER_EDITORStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FBP_MULTY_USER_EDITORStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("BP_MULTY_USER_EDITORStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("BP_MULTY_USER_EDITOR")->GetBaseDir() / TEXT("Resources"));

	Style->Set("BP_MULTY_USER_EDITOR.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FBP_MULTY_USER_EDITORStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FBP_MULTY_USER_EDITORStyle::Get()
{
	return *StyleInstance;
}
