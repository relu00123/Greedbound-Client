// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/Character/GBClassType.h"
#include "GBLobbyClassSelectWidget.generated.h"

class UButton;
class UGBClassSelectData;
class UGBLobbyClassSelectPortraitTile;

UCLASS()
class GREEDBOUND_API UGBLobbyClassSelectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 위젯 생성/리빌드 후 호출되어도 안전
    UFUNCTION(BlueprintCallable, Category = "ClassSelect")
    void                    InitializeTiles();

    // 코드/블루프린트에서 선택 강제 변경
    UFUNCTION(BlueprintCallable, Category = "ClassSelect")
    void                    SelectClass(EGBCharacterClassType InClass);

    // 현재 선택 읽기
    UFUNCTION(BlueprintPure, Category = "ClassSelect")
    EGBCharacterClassType   GetSelectedClass() const { return SelectedClass; }

    UFUNCTION(BlueprintCallable, Category = "ClassSelect")
    void                    ClearTempSelection();

    // 외부 바인딩용 안전 API (BP/C++ 공용)
    UFUNCTION(BlueprintCallable, Category = "Event")
    void                    BindOnClassSelected(UObject* Target, FName FunctionName);

    UFUNCTION(BlueprintCallable, Category = "Event")
    void                    UnbindOnClassSelected(UObject* Target, FName FunctionName);

    UFUNCTION(BlueprintCallable, Category = "Event")
    void                    BindOnConfirmChangeRequested(UObject* Target, FName FunctionName);

    void                    SetClassSelectPending(bool bInPending);

    

protected:
    virtual void            NativeConstruct() override;
    virtual void            NativePreConstruct() override;

private:
    // 타일 클릭 -> 임시 선택 갱신
    UFUNCTION()
    void                    HandleTileClicked(EGBCharacterClassType ClassType);

    // 버튼 클릭 → 현재 선택으로 확정
    UFUNCTION()
    void                    HandleConfirmClicked();

    // 내부 헬퍼
    void                    BindTile(UGBLobbyClassSelectPortraitTile* Tile);
    void                    RefreshSelectionVisuals();

    UFUNCTION(BlueprintCallable, Category = "ClassSelect")
    // 현재 위젯 트리에 존재하는 모든 타일 수집
    void                    CollectTiles(TArray<UGBLobbyClassSelectPortraitTile*>& OutTiles) const;

private:
    // 부모/상위 UI로 “최종 선택 클래스”를 알려줄 이벤트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClassSelected, EGBCharacterClassType, SelectedClass);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmChangeRequested, EGBCharacterClassType, RequestedClass);


    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnClassSelected                            OnClassSelected;

    UPROPERTY()
    FOnConfirmChangeRequested                   OnConfirmChangeRequested;

    // 디자이너가 바인딩할 4개의 타일(이름은 WBP에서 가진 이름대로 맞춰 바인딩)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBLobbyClassSelectPortraitTile> KnightClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBLobbyClassSelectPortraitTile> ClericClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBLobbyClassSelectPortraitTile> ArcherClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBLobbyClassSelectPortraitTile> MageClass;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UButton>                         ConfirmButton;

    // (옵션) 모든 타일에 동일하게 적용할 DataAsset
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGBClassSelectData>              PortraitData;

    // 선택 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
    EGBCharacterClassType                       SelectedClass = EGBCharacterClassType::Knight;

    TOptional<EGBCharacterClassType>            TempSelectedClass;

    // 빠른 접근을 위한 맵(클래스 타입 -> 타일)
    TMap<EGBCharacterClassType, TWeakObjectPtr<UGBLobbyClassSelectPortraitTile>> TileByClass;
};
