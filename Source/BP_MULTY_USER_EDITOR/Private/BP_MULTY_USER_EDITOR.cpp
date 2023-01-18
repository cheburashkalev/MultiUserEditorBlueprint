// Copyright Epic Games, Inc. All Rights Reserved.

#include "BP_MULTY_USER_EDITOR.h"

#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include <Editor/UnrealEd/Private/Toolkits/SStandaloneAssetEditorToolkitHost.h>
#include "BP_MULTY_USER_EDITORStyle.h"
#include "EditorConfigSubsystem.h"
#include "TCP_MUE_BP.h"
#include "UObject/UObjectBaseUtility.h"
#include "Logging/MessageLog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Async/Async.h"
#include "MUE_Settings.h"
#include "Kismet/KismetStringLibrary.h"
#include "Types/SlateEnums.h"

static const FName BP_MULTI_USER_EDITORTabName("BP_MULTY_USER_EDITOR");

#define LOCTEXT_NAMESPACE "FBP_MULTY_USER_EDITORModule"
void FBP_MULTY_USER_EDITORModule::StartupModule()
{
	FBP_MULTY_USER_EDITORStyle::Initialize();
	FBP_MULTY_USER_EDITORStyle::ReloadTextures();
	
	AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

	AssetEditorSubsystem->OnAssetEditorOpened().AddRaw(this, &FBP_MULTY_USER_EDITORModule::onAssetOpenedInEditor);
	
	UE_LOG(LogTemp, Log, TEXT("PLUGIN_MUED_started"));
}

void FBP_MULTY_USER_EDITORModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);
	
	FBP_MULTY_USER_EDITORStyle::Shutdown();
}

void FBP_MULTY_USER_EDITORModule::PluginButtonClicked()
{
	
	
	/*
	FText Titles;
	TArray<TSharedRef<SGraphEditor>> graphs = FindSGraphPanel();
	TArray<TSharedRef<SStandaloneAssetEditorToolkitHost>> AssetEditorToolkitHost = FindAssetEditorToolkitHost();
	TArray<TSharedRef<SGraphTitleBar>> graphsTitleBar = FindGraphTitleBar();
	AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (graphs.IsValidIndex(0)) 
	{
		auto b = graphs[0].Get().GetCurrentGraph();
		DelegatePropertyChanged.BindRaw(this, &FBP_MULTY_USER_EDITORModule::PropertyChanged);
		DelegateGraphChanged.BindRaw(this, &FBP_MULTY_USER_EDITORModule::GraphChanged);
		b->AddPropertyChangedNotifier(DelegatePropertyChanged);
		b->AddOnGraphChangedHandler(DelegateGraphChanged);
		TArray<TSharedRef<SWidget>> TempWidgets = GetAllChildrenFromAll(graphsTitleBar[0]);
		/*for (int32 j = 0; j < TempWidgets.Num(); j++)
		{
			auto a = TempWidgets[j].Get().ToString();
			if (a.Contains("SGraphTitleBar.cpp")) {
				if (a.Contains("STextBlock")) {
					Titles = StaticCastSharedRef<STextBlock>((TempWidgets[j])).Get().GetText();
					break;
				}
			}
		}
		auto assets = AssetEditorSubsystem->GetAllEditedAssets();
		for (int32 j = 0; j < assets.Num(); j++)
		{
			IAssetEditorInstance* editor = AssetEditorSubsystem->FindEditorForAsset(assets[j],false);
			//Titles = FText::FromName(assets[j]->GetPathName());
			Titles = FText::FromString(assets[j]->GetName());
		}
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "\"{0}\"     \"{1}\""),
			FText::FromName(graphs[0].Get().GetCurrentGraph()->GetFName()),
			Titles
		);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
*/
}
TArray<TSharedRef<SGraphTitleBar>> FBP_MULTY_USER_EDITORModule::FindGraphTitleBar() 
{
	TArray < TSharedRef<SGraphTitleBar>> ResultGraphs;

	TArray<TSharedRef<SWindow>> VisibleWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(VisibleWindows);

	for (int32 i = 0; i < VisibleWindows.Num(); i++)
	{
		TArray<TSharedRef<SWidget>> TempWidgets = GetAllChildrenFromAll(VisibleWindows[i]);
		for (int32 j = 0; j < TempWidgets.Num(); j++)
		{
			auto a = TempWidgets[j].Get().ToString();
			if (a.Contains("BlueprintEditor.cpp")) {
				if (a.Contains("SGraphTitleBar")) {
					TSharedRef <SGraphTitleBar> PaintParentPtr = StaticCastSharedRef<SGraphTitleBar>((TempWidgets[j]));
					ResultGraphs.AddUnique(PaintParentPtr);
				}
			}
		}
	}

	return ResultGraphs;
}

void FBP_MULTY_USER_EDITORModule::PropertyChanged(const FPropertyChangedEvent& a, const FString& b)
{
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "\"{0}\"     \"{1}\""),
		FText::FromName(a.GetPropertyName()),
		FText::FromString(b)
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}
void FBP_MULTY_USER_EDITORModule::GraphChanged(const FEdGraphEditAction& a) 
{
	
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "\"{0}\"     \"{1}\""),
		a.Nodes.Array()[0]->GetNodeTitle(ENodeTitleType::MenuTitle)
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}
TArray<TSharedRef<SStandaloneAssetEditorToolkitHost>> FBP_MULTY_USER_EDITORModule::FindAssetEditorToolkitHost()
{
	TArray < TSharedRef<SStandaloneAssetEditorToolkitHost>> ResultGraphs;

	TArray<TSharedRef<SWindow>> VisibleWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(VisibleWindows);

	for (int32 i = 0; i < VisibleWindows.Num(); i++)
	{
		TArray<TSharedRef<SWidget>> TempWidgets = GetAllChildrenFromAll(VisibleWindows[i]);
		for (int32 j = 0; j < TempWidgets.Num(); j++)
		{
			auto a = TempWidgets[j].Get().ToString();
			if (a.Contains("AssetEditorToolkit.cpp")) {
				if (a.Contains("SStandaloneAssetEditorToolkitHost")) {
					TSharedRef <SStandaloneAssetEditorToolkitHost> PaintParentPtr = StaticCastSharedRef<SStandaloneAssetEditorToolkitHost>((TempWidgets[j]));
					ResultGraphs.AddUnique(PaintParentPtr);
				}
			}
		}
	}

	return ResultGraphs;
}
TArray<TSharedRef<SGraphEditor>> FBP_MULTY_USER_EDITORModule::FindSGraphPanel()
{
	TArray < TSharedRef<SGraphEditor>> ResultGraphs;

	TArray<TSharedRef<SWindow>> VisibleWindows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(VisibleWindows);

	for (int32 i = 0; i < VisibleWindows.Num(); i++)
	{
		TArray<TSharedRef<SWidget>> TempWidgets = GetAllChildrenFromAll(VisibleWindows[i]);
		for (int32 j = 0; j < TempWidgets.Num(); j++) 
		{
			auto a = TempWidgets[j].Get().ToString();
			if (a.Contains("BlueprintEditor.cpp")) {
				if (a.Contains("SGraphEditor")) {
					TSharedRef <SGraphEditor> PaintParentPtr = StaticCastSharedRef<SGraphEditor>((TempWidgets[j]));
					ResultGraphs.AddUnique(PaintParentPtr);
				}
			}
		}
	}
	
	return ResultGraphs;
}

TArray<TSharedRef<SWidget>> FBP_MULTY_USER_EDITORModule::GetAllChildrenFromAll(TSharedRef<SWidget> widget)
{
	TArray<TSharedRef<SWidget>> Childrens;
	
	FChildren* a = widget.Get().GetAllChildren();
	if (a != nullptr) 
	{
		if (a->Num() > 0)
		{
			for (int32 i = 0; i < a->Num(); i++) 
			{
				auto TempWidget = a->GetChildAt(i);
				Childrens.AddUnique(TempWidget);
				TArray<TSharedRef<SWidget>> TempWidgets = GetAllChildrenFromAll(TempWidget);
				Childrens.Append(TempWidgets);
			}
		}
	}
	return Childrens;
}

TSharedRef<SWidget> FBP_MULTY_USER_EDITORModule::MakeMUE_BPMenu()
{			
	//make menu for button 
	FMenuBuilder MenuBuilder(true, NULL);
	/*MenuBuilder.AddMenuEntry(LOCTEXT("MUE_BP_enabled", "MUE_BP is fork"),
		FText(), FSlateIcon(), FUIAction());
	MenuBuilder.AddEditableText(FText::FromString("MUE_BP"), FText(),FSlateIcon(), TAttribute< FText >(), FOnTextCommitted(), FOnTextChanged(), false);
	// Create a Submenu inside of the Section*/
	MenuBuilder.BeginSection("ServerSettings", TAttribute(FText::FromString("Server/Settings")));
	FNewMenuDelegate DelegateNewServerMenu;
	
	
	DelegateNewServerMenu.BindLambda([&](class FMenuBuilder& SubMenuBuilder) 
    {
        
		//GEditor->GetEditorSubsystem<UEditorConfigSubsystem>()->LoadConfigObject(TcpServerSetting->GetClass(), TcpServerSetting);
		SubMenuBuilder.AddEditableText(FText::FromString("ip:port"),
			FText(),
			FSlateIcon(),
			TAttribute< FText >(FText::FromString(NewObject<UMUE_Settings>()->LoadConfig()->ip_port)),
			FOnTextCommitted::CreateLambda([&](const FText text,ETextCommit::Type EnumCommit)
			{
				UMUE_Settings* TcpServerSetting = NewObject<UMUE_Settings>();
				TcpServerSetting->ip_port = text.ToString();
				TcpServerSetting->SaveConfig();
			}),
			FOnTextChanged(),
			IsValid(TcpServerObject));
		
		SubMenuBuilder.AddMenuEntry(IsValid(TcpServerObject)? LOCTEXT("DisableServer", "DisableServer"):LOCTEXT("EnableServer", "EnableServer"),
		FText(), FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateLambda([&]()
		{
			if(IsValid (TcpServerObject))
			{
				TcpServerObject->ConditionalBeginDestroy();
				TcpServerObject = nullptr;
			}
			else
			{
				TcpServerObject = NewObject<UTCP_MUE_BP>();
				
				FString S_ip;
				FString S_Port;
				NewObject<UMUE_Settings>()->LoadConfig()->ip_port.Split(":",&S_ip,&S_Port);
				FString ip = S_ip;
				int32 port = UKismetStringLibrary::Conv_StringToInt(*S_Port);
				TcpServerObject->CreateServer(ip,port);
				UE_LOG(LogTemp, Log, TEXT("UIStartServer"));
			}
		}),
			FCanExecuteAction(),
				FGetActionCheckState::CreateLambda([&](){return IsValid (TcpServerObject) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked ;})
				),
		 FName(),EUserInterfaceActionType::RadioButton);
    });

    MenuBuilder.AddSubMenu(FText::FromString("ServerSettings"),FText::FromString("Settings TCP Server your editor"),DelegateNewServerMenu);
	MenuBuilder.EndSection();
	
	MenuBuilder.BeginSection("ClientSettings", TAttribute(FText::FromString("Client/Settings")));
	FNewMenuDelegate DelegateNewClientMenu;
	DelegateNewClientMenu.BindLambda([](class FMenuBuilder& SubMenuBuilder) 
	{

		   SubMenuBuilder.AddEditableText(FText::FromString("ip:port"), FText(), FSlateIcon(), TAttribute< FText >(), FOnTextCommitted(), FOnTextChanged(), false);
		   SubMenuBuilder.AddMenuEntry(LOCTEXT("TryConnect", "TryConnect"),
		 FText(), FSlateIcon(), FUIAction());
	});

	MenuBuilder.AddSubMenu(FText::FromString("ClientSettings"),FText::FromString("Settings TCP Server your editor"),DelegateNewClientMenu);
	MenuBuilder.EndSection();
	//end make menu for button

	return MenuBuilder.MakeWidget();
	
}
void FBP_MULTY_USER_EDITORModule::onAssetOpenedInEditor(UObject* obj)
{
	AssetEditorSubsystem->FindEditorForAsset(obj, false);
	FString nameclass = obj->GetClass()->GetName();
if(nameclass != "Blueprint")
{
	return;
}
	FName ToolAppName = *(TEXT("AssetEditor.") + obj->GetClass()->GetFName().ToString() + TEXT("Editor") + TEXT(".ToolBar"));
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(ToolAppName);
	FName name = Menu->GetFName();
	FToolMenuSection& Section = Menu->FindOrAddSection("MUE_BP");


	Section.AddDynamicEntry("MUE_BP", FNewToolMenuSectionDelegate::CreateLambda([&](FToolMenuSection& InSection)
		{
	        const UBlueprintEditorToolMenuContext* Context = InSection.FindContext<UBlueprintEditorToolMenuContext>();
	        if (Context && Context->BlueprintEditor.IsValid() && Context->GetBlueprintObj())
	        {
	        	TSharedPtr<class FBlueprintEditorToolbar> BlueprintEditorToolbar = Context->BlueprintEditor.Pin()->GetToolbarBuilder();
	        	if (BlueprintEditorToolbar.IsValid())
	        	{
	        		InSection.InsertPosition = FToolMenuInsert();
	        		FToolMenuEntry DiffEntry = FToolMenuEntry::InitComboButton(
	        			"MUE_BP",
	        			FUIAction(), 
	        			FOnGetContent::CreateRaw(this,&FBP_MULTY_USER_EDITORModule::MakeMUE_BPMenu),
	        			LOCTEXT("MUE_BP", "MUE_BP"),
	        			LOCTEXT("BP_MULTY_USER_EDITORToolTip", "Multy user editing for blueprints"),
						FSlateIcon("BP_MULTY_USER_EDITORStyle", "BP_MULTY_USER_EDITOR.PluginAction")
	        		);
	        		DiffEntry.StyleNameOverride = "CalloutToolbar";
	        		InSection.AddEntry(DiffEntry);
	        	}
	        }
		}));
}

void FBP_MULTY_USER_EDITORModule::UIStartServer()
{

}

void FBP_MULTY_USER_EDITORModule::RegisterMenus()
{
	
	/*
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBP_MULTY_USER_EDITORCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBP_MULTY_USER_EDITORCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}*/
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBP_MULTY_USER_EDITORModule, BP_MULTY_USER_EDITOR)