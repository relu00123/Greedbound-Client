#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/Character/GBClassType.h"
#include "Define/User/UserInfo.h"
#include "Define/UI/GBLobbyUITypes.h"
#include "Define/GamePhase/GBClientGamePhase.h"
#include "GBLobbyPlayerEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UGBPlayerEntryClassVisualData;
class UGBRightClickMenuWidget;

UENUM(BlueprintType)
enum class EPartyEntryState : uint8
{
    Empty,
    EmptyPartySlot,
    Filled
};


UCLASS()
class GREEDBOUND_API UGBLobbyPlayerEntryWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // 리팩토링 중인부분. SetupUserInfo를 사용하고 SetCharacterInfo는 사용하지 않을듯? 
    // void SetCharacterInfo(EGBCharacterClassType classType, FString characterName);
    void                                        SetupUserInfo(const FUserInfo Info);
    void                                        SetupUserClass(EGBCharacterClassType ClassType);
    void                                        SetupUserGamePhase(EGBClientGamePhase GamePhase);
    void                                        SetPartyLeaderIcon(bool isLeader);

    void                                        SetLobbyPlayerListType(EGBLobbyPlayerListType PlayerListType) { LobbyPlayerListType = PlayerListType; }
    const FUserInfo&                            GetUserInfo() const { return CurrentUserInfo; }
    void                                        SetEmptyPartySlot();
    void                                        SetEmptySlot();
    FORCEINLINE bool                            IsEmptySlot() const { return bIsEmpty; }
    FORCEINLINE bool                            IsEmptyPartySlot() const { return bIsEmptyPartySlot; }
    FORCEINLINE bool                            IsEmpty() const { return bIsEmpty || bIsEmptyPartySlot;  }
    FORCEINLINE bool                            IsPartyHost() const { return bIsPartyHost; }
    //void                                        ChangeOnlineState(bool isOnline); (아직 쓰는 곳 없음)

    virtual FReply                              NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override; 

    UFUNCTION()
    void                                        HandleMenuAction(ERightClickMenuAction  Action);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>                      CharacterNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>                      ClassNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>                          ClassImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>                          PartyLeaderImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>                          ClassIconImage;


    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>                      EmptyPartySlot;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>                      StatusText; // 일단은 온라인만 표시하기에 건들이지 않았음.

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UGBPlayerEntryClassVisualData>   ClassVisualDataAsset;


    UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = "RightClick")
    TSubclassOf<UGBRightClickMenuWidget>        RightClickMenuClass;

    //UFUNCTION()
    //void                                        HandleMenuAction(ERightClickMenuAction Action);

private :
    bool                                        bIsEmptyPartySlot   = false; 
    bool                                        bIsEmpty            = true;
    bool                                        bIsPartyHost        = false;

    FUserInfo                                   CurrentUserInfo;

    EGBLobbyPlayerListType                      LobbyPlayerListType; 
};
