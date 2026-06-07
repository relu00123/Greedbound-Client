// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/User/UserInfo.h"
#include "Define/UI/GBLobbyUITypes.h"
#include "GBLobbyPlayerGrid.generated.h"

class UGBLobbyPlayerEntryWidget;
class UUniformGridPanel;

UCLASS()
class GREEDBOUND_API UGBLobbyPlayerGrid : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // 외부에서 유저 목록을 넘겨받아 초기 Entry들을 생성
    void                                                    Init(const TArray<FUserInfo>& UserList, EGBLobbyPlayerListType PlayerListType);

    // 유저 추가 
    void                                                    OnUserAdded(const FUserInfo& Info);

    // 유저 정보 변경에 대한 반영 
    void                                                    OnUserUpdated(const FUserInfo& Info); 

    // 유저 제거 반영
    void                                                    OnUserRemoved(const FUserInfo& Info);
    void                                                    OnUserRemoved(const FString& UserName);

    // 유저 클레스 정보 변경
    void                                                    HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType);

    // 유저 게임 페이즈 변경
    void                                                    HandleUserGamePhaseChanged(const FString& UserName, EGBClientGamePhase ChangedGamePhase);

    FORCEINLINE EGBLobbyPlayerListType                      GetPlayerGridType() { return PlayerGridType;  }

protected  :
    virtual void                                            NativeDestruct() override; 

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel>                           PlayerListPanel; // ScrollBox, VerticalBox 등

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGBLobbyPlayerEntryWidget>                  PlayerEntryClass;

    UPROPERTY()
    TMap<FString, TObjectPtr<UGBLobbyPlayerEntryWidget>>    EntryMap;

    FDelegateHandle                                         OnUserClassChangedHandle; 

    FDelegateHandle                                         OnUserGamePhaseChangedHandle; 

private:
    static constexpr int32                                  NumColumns  = 3;
    static constexpr int32                                  NumRows     = 6;
    static constexpr int32                                  TotalSlots  = NumColumns * NumRows;

    // 전체 슬롯 위젯들
    TMap<int32, UGBLobbyPlayerEntryWidget*>                 SlotMap;

    // 닉네임 -> 슬롯 인덱스
    TMap<FString, int32>                                    UserSlotMap;

    // 자신이 무슨 타입의 그리드인지
    EGBLobbyPlayerListType                                  PlayerGridType;

private:
    void                                                    ReorderEntries();

    // 아래부부은 안쓰이는듯? 

    /** 내부용 Entry 생성 함수 */
    //void CreateAndAddEntry(const FUserInfo& Info);

    //void RebuildGridLayout();
};
