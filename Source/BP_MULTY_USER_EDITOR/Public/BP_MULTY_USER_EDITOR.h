// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <Kismet/Private/SGraphTitleBar.h>
#include <Engine/Public/GraphEditAction.h>
//#include <HPTcpSocketPlugin/Private/TcpServer/HPTcpServerManager.h>
#include <Developer/ToolMenus/Public/ToolMenuDelegates.h>
#include "BlueprintEditorContext.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditor.h"
#include "TCP_MUE_BP.h"


class FBlueprintEditor;
class FToolBarBuilder;
class FMenuBuilder;
class FMultiBoxBuilder;
class UTCP_MUE_BP;

class FBP_MULTY_USER_EDITORModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	
	void PluginButtonClicked();
	UFUNCTION()
	void ReciveDataServer(FString Data);

	//TArray<TSharedRef<SGraphPanel>> FindSGraphPanel();
	
private:
	TArray<TSharedRef<SGraphEditor>> FindSGraphPanel();
	TArray<TSharedRef<SStandaloneAssetEditorToolkitHost>> FindAssetEditorToolkitHost();
	TArray<TSharedRef<SGraphTitleBar>> FindGraphTitleBar();
	UFUNCTION()
	void PropertyChanged(const FPropertyChangedEvent& a, const FString& b);
	void GraphChanged(const FEdGraphEditAction& a);

	FOnGraphChanged::FDelegate DelegateGraphChanged;
	FOnPropertyChanged::FDelegate DelegatePropertyChanged;
	FOnGraphChanged OnGraphChanged;

	UAssetEditorSubsystem* AssetEditorSubsystem = nullptr;
	UAssetEditorSubsystem::FOnAssetOpenedInEditorEvent AssetOpenedInEditorEvent;

	FToolUIActionChoice D_ToolUIActionChoice;
	FNewToolMenuChoice D_NewToolMenuChoice;
	TAttribute<FText> LabelComboButton = TAttribute < FText>(FText::FromString("MUE_BP"));
	TAttribute<FText> ToolTipComboButton = TAttribute < FText>(FText::FromString("MUE_BP"));

	void onAssetOpenedInEditor(UObject* obj);
	TSharedRef<SWidget> MakeMUE_BPMenu();
	/*UHPTcpServerManager* HPTcpServerManager;*/
	TArray<TSharedRef<SWidget>> GetAllChildrenFromAll(TSharedRef<SWidget> widget);
public:
	UPROPERTY()
	UTCP_MUE_BP* TcpServerObject;
	UPROPERTY()
	UTCP_MUE_BP* TcpClientObject;
	void UIStartServer();
	
	void RegisterMenus();
	UFUNCTION()
	void ReciveDataClient(FString Data);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};