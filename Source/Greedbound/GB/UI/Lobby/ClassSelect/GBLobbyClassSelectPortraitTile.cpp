// Fill out your copyright notice in the Description page of Project Settings.


#include "GBLobbyClassSelectPortraitTile.h"
#include "GB/UI/Lobby/Data/GBClassSelectData.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "GB/Utils/GBDelegateUtils.h"

void UGBLobbyClassSelectPortraitTile::BindOnTileClicked(UObject* Target, FName FunctionName)
{
    GB::DelegateUtils::BindScriptDelegateTo(Target, FunctionName, OnTileClicked);
}

void UGBLobbyClassSelectPortraitTile::UnbindOnTileClicked(UObject* Target, FName FunctionName)
{
    GB::DelegateUtils::UnbindScriptDelegateFrom(Target, FunctionName, OnTileClicked);
}

void UGBLobbyClassSelectPortraitTile::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;
    ApplyVisual();
}

void UGBLobbyClassSelectPortraitTile::SetTempSelected(bool bInTempSelected)
{
    bTempSelected = bInTempSelected;  
    ApplyVisual();
}

void UGBLobbyClassSelectPortraitTile::NativeConstruct()
{
    Super::NativeConstruct();

    if (TileButton)
    {
        TileButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
    }

    ApplyVisual();
}

void UGBLobbyClassSelectPortraitTile::NativePreConstruct()
{
    Super::NativePreConstruct();
    ApplyVisual();
}

void UGBLobbyClassSelectPortraitTile::NativeDestruct()
{
    OnTileClicked.Clear();
    //OnTileHoverChanged.Clear();

    Super::NativeDestruct();
}

void UGBLobbyClassSelectPortraitTile::HandleClicked()
{
    OnTileClicked.Broadcast(ClassType); 
}

void UGBLobbyClassSelectPortraitTile::ApplyVisual()
{
    if (!FrameBorder || !PortraitData) return;

    // 초상화 
    if (PortraitImage)
    {
        FGBClassPortraitEntry Entry;
        if (PortraitData->GetPortraitEntry(ClassType, Entry) && Entry.Portrait)
        {
            PortraitImage->SetBrushFromTexture(Entry.Portrait, true);
        }
    }

    // 테두리 색
    FLinearColor Normal, Selected, TempSelected;
    PortraitData->GetFrameColors(ClassType, Normal, Selected, TempSelected);

    // 보더 색 적용 
    if (bSelected)
    {
        FrameBorder->SetBrushColor(Selected);   // 하늘색
    }
    else if (bTempSelected)
    {
        FrameBorder->SetBrushColor(TempSelected);  // 노란색
    }
    else
    {
        FrameBorder->SetBrushColor(Normal);
    }
}

 

