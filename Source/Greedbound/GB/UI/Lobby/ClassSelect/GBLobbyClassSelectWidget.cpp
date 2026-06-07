// Fill out your copyright notice in the Description page of Project Settings.


#include "GBLobbyClassSelectWidget.h"
#include "UI/Lobby/ClassSelect/GBLobbyClassSelectPortraitTile.h"
#include "Components/Button.h"
#include "Utils/GBDelegateUtils.h"

void UGBLobbyClassSelectWidget::InitializeTiles()
{
    TileByClass.Reset();

    TArray<UGBLobbyClassSelectPortraitTile*> Tiles;
    CollectTiles(Tiles);

    for (UGBLobbyClassSelectPortraitTile* T : Tiles)
    {
        if (!T) continue;

        // 공통 데이터 인젝션이 필요하면 여기에서
        // if (PortraitData) { T->PortraitData = PortraitData; }

        BindTile(T);

        const EGBCharacterClassType Type = T->GetClassType();
        TileByClass.FindOrAdd(Type) = T;
    }

    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.RemoveAll(this);
        ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::HandleConfirmClicked);
    }


}

void UGBLobbyClassSelectWidget::SelectClass(EGBCharacterClassType InClass)
{
    if (SelectedClass == InClass)
    {
        // 이미 동일하면 Temp만 초기화하고 비주얼만 갱신
        TempSelectedClass.Reset();
        RefreshSelectionVisuals();
        return;
    }

    SelectedClass = InClass;
    TempSelectedClass.Reset(); // 확정 후 임시 해제
    RefreshSelectionVisuals();

    // 상위에 알림
    OnClassSelected.Broadcast(SelectedClass);
}

void UGBLobbyClassSelectWidget::ClearTempSelection()
{
    TempSelectedClass.Reset();
    RefreshSelectionVisuals();
}

void UGBLobbyClassSelectWidget::BindOnClassSelected(UObject* Target, FName FunctionName)
{
    GB::DelegateUtils::BindScriptDelegateTo(Target, FunctionName, OnClassSelected);
}

void UGBLobbyClassSelectWidget::UnbindOnClassSelected(UObject* Target, FName FunctionName)
{
    GB::DelegateUtils::UnbindScriptDelegateFrom(Target, FunctionName, OnClassSelected);
}

void UGBLobbyClassSelectWidget::BindOnConfirmChangeRequested(UObject* Target, FName FunctionName)
{
    GB::DelegateUtils::BindScriptDelegateTo(Target, FunctionName, OnConfirmChangeRequested);
}

void UGBLobbyClassSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeTiles();
    RefreshSelectionVisuals();
}

void UGBLobbyClassSelectWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    // 에디터 미리보기에서도 DataAsset 미리 적용
    InitializeTiles();
}

void UGBLobbyClassSelectWidget::HandleTileClicked(EGBCharacterClassType ClassType)
{
    // 타일 클릭 → 임시 선택(노란색). 현재와 같으면 임시든 뭐든 하늘색이 우선 표시됨(타일 쪽 우선순위).
    TempSelectedClass = ClassType;
    RefreshSelectionVisuals();
}

void UGBLobbyClassSelectWidget::HandleConfirmClicked()
{
    // [선택] 버튼 → 임시 선택이 있으면 확정

    if (!TempSelectedClass.IsSet()) 
        return;

    const EGBCharacterClassType Req = TempSelectedClass.GetValue();


    UE_LOG(LogTemp, Warning, TEXT("TempClass = %s"), *UEnum::GetValueAsString(Req));
    UE_LOG(LogTemp, Warning, TEXT("SelectedClass = %s"), *UEnum::GetValueAsString(SelectedClass));


    // 같은 클래스면 전송 불필요
    if (Req == SelectedClass)
        return; 

    OnConfirmChangeRequested.Broadcast(Req);
    SetClassSelectPending(true);

    // SelectClass(TempSelectedClass.GetValue());
}

void UGBLobbyClassSelectWidget::BindTile(UGBLobbyClassSelectPortraitTile* Tile)
{
    if (!Tile) return;

    // 중복 바인딩 방지 위해 먼저 Unbind 호출 (타일에 우리가 만든 API 사용)
    Tile->UnbindOnTileClicked(this, GET_FUNCTION_NAME_CHECKED(UGBLobbyClassSelectWidget, HandleTileClicked));
    Tile->BindOnTileClicked(this, GET_FUNCTION_NAME_CHECKED(UGBLobbyClassSelectWidget, HandleTileClicked));
}

void UGBLobbyClassSelectWidget::RefreshSelectionVisuals()
{
    for (const TPair<EGBCharacterClassType, TWeakObjectPtr<UGBLobbyClassSelectPortraitTile>>& Pair : TileByClass)
    {
        if (UGBLobbyClassSelectPortraitTile* Tile = Pair.Value.Get())
        {
            UE_LOG(LogTemp, Warning, TEXT("ClassType = %s"), *UEnum::GetValueAsString(Tile->GetClassType()));
            const bool bIsCurrent = (Pair.Key == SelectedClass);
            const bool bIsTemp = (TempSelectedClass.IsSet() && Pair.Key == TempSelectedClass.GetValue());

            Tile->SetSelected(bIsCurrent);       // 하늘색
            Tile->SetTempSelected(bIsTemp);      // 노란색 (타일 내부에서 파랑이 우선)
        }
    }
}

void UGBLobbyClassSelectWidget::SetClassSelectPending(bool bInPending)
{
    if (ConfirmButton)
    {
        ConfirmButton->SetIsEnabled(!bInPending);
    }
}

void UGBLobbyClassSelectWidget::CollectTiles(TArray<UGBLobbyClassSelectPortraitTile*>& OutTiles) const
{
    OutTiles.Reset();

    OutTiles.Add(KnightClass);
    OutTiles.Add(ClericClass);
    OutTiles.Add(ArcherClass);
    OutTiles.Add(MageClass);
}
