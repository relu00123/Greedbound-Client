// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/LevelStreaming.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Components/Player/GBInputComponent.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Character/Player/GBPlayerState.h"
#include "Define/GBTags.h"
#include "UI/InGame/GBInGameHUD.h"
#include "UI/InGame/GBInGameWaitingWidget.h"
#include "Game/Dungeon/GBDungeonGameMode.h"
#include "Game/GBGameState.h"

UGBAbilitySystemComponent* AGBPlayerController::GetAbilitySystemComponent()
{
    if (AbilitySystemComponent == nullptr)
    {
        TScriptInterface<IAbilitySystemInterface> AbilityInterface = GetPawn();
        if (AbilityInterface)
        {
            AbilitySystemComponent = Cast<UGBAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
        }
    }

    return AbilitySystemComponent;
}

void AGBPlayerController::Server_NotifyClientLoadedDungeonMap_Implementation()
{
    if (AGBDungeonGameMode* GameMode = GetWorld()->GetAuthGameMode<AGBDungeonGameMode>())
    {
        GameMode->HandleClientLoadedDungeonMap(this);
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("Dungeon GameMode Does Not Exists!"));
    }
}

void AGBPlayerController::HandleMatchStarted_Client()
{
    // 1. 대기용 HUD 제거
    HideWaitingHUD();

    // 2. 실제 인게임 HUD 켜기 
    ShowInGameHUD();
}

void AGBPlayerController::BeginPlay()
{
    bReplicates = true;
    Super::BeginPlay();

    UWorld* World = GetWorld();
    GB_NULL_CHECK(World);

    AGBGameState* GBGameState = World->GetGameState<AGBGameState>();
    GB_NULL_CHECK(GBGameState);

    if (IsLocalController() && GetNetMode() == NM_Client && GBGameState->GetSessionMode() == EGBSessionMode::DedicatedServer)
    {
        if (WaitingWidgetClass)
        {
            WaitingWidgetInstance = CreateWidget<UGBInGameWaitingWidget>(this, WaitingWidgetClass);

            if (WaitingWidgetInstance)
            {
                WaitingWidgetInstance->AddToViewport(1000);
            }
        }

        GetWorldTimerManager().SetTimer(TryNotifyReadyToPlayTickHandle, this, &ThisClass::TryNotifyReadyToPlay, 0.1f, true);
    }

    if (IsLocalController())
    {
        // Client
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
        SetShowMouseCursor(false);
        TryLogin();

        GB_BACKEND_LOG(Warning, TEXT("BeginPlay Called From Client"));
    }
    else
    {
        // Server 
    }
}

void AGBPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    //UE_LOG(LogTemp, Warning, TEXT("[Possess] %s 가 %s 를 Possess함"), *GetName(), *GetNameSafe(aPawn));
    GB_BACKEND_LOG(Warning, TEXT("OnPossess Function Called!"));

    if (IsLocalController())
    {
        GB_BACKEND_LOG(Warning, TEXT("[OnPossess] IsLocalController: %s"), *GetName());
        GB_BACKEND_LOG(Warning, TEXT("[OnPossess] Pawn: %s"), *GetNameSafe(GetPawn()));
    }

    AGBPlayerState* GBPS = GetPlayerState<AGBPlayerState>();
    if (GBPS)
    {
        UGBAbilitySystemComponent* GBASC = GBPS->GetAbilitySystemComponent<UGBAbilitySystemComponent>();
        if (GBASC)
        {
            GBASC->SetInputData(AbilityInputData);
        }
    }
}

void AGBPlayerController::OnRep_Pawn()
{
    Super::OnRep_Pawn();

    GB_BACKEND_LOG(Warning, TEXT("[Client] OnRep_Pawn() - %s possessed %s"), *GetName(), *GetNameSafe(GetPawn()));
}

void AGBPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    //UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState() 호출됨 - %s"), *GetName());

    //  여기서 입력 모드 설정 (UI에 의해 막히는 문제 방지)
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    SetShowMouseCursor(false);

    AGBPlayerState* GBPS = GetPlayerState<AGBPlayerState>();
    if (GBPS)
    {
        AbilitySystemComponent = Cast<UGBAbilitySystemComponent>(GBPS->GetAbilitySystemComponent());
        if (AbilitySystemComponent && GetLocalRole() == ROLE_AutonomousProxy)
        {
            AbilitySystemComponent->SetInputData(AbilityInputData);
            AbilitySystemComponent->RegisterGameplayTagEvent(GBTag::Block_All, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGBPlayerController::OnBlockTagChanged);
            AbilitySystemComponent->RegisterGameplayTagEvent(GBTag::Block_Input, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGBPlayerController::OnBlockTagChanged);
            AbilitySystemComponent->RegisterGameplayTagEvent(GBTag::Block_Move, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGBPlayerController::OnBlockTagChanged);

            BP_InitializePlayerUI();
        }
    }

    ConsoleCommand(TEXT("showdebug abilitysystem"));
}

void AGBPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UGBInputComponent* GBInput = Cast<UGBInputComponent>(InputComponent);
    GB_NULL_CHECK(GBInput);

    // Add Input Mapping Contexts (ThirdPerson Template - Unreal) 
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem)
    {
        GB_BACKEND_LOG(Warning, TEXT("EnhancedInputLocal Subsystem Found"));
        for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
        {
            GB_BACKEND_LOG(Warning, TEXT("%s"), *CurrentContext->GetFName().ToString());
            Subsystem->AddMappingContext(CurrentContext, 0);
        }
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("EnhancedInputLocal Subsystem Not Found"));
    }

    if (GBInput)
    {
        // 기본 Action Bind
        GB_BACKEND_LOG(Warning, TEXT("Trying to Bind Move And Look"));
        GBInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGBPlayerController::Move);
        GBInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGBPlayerController::Look);
        GBInput->SetTaggedInputActionDataAsset(AbilityInputData, this, &AGBPlayerController::AbilityTagTriggered, &AGBPlayerController::AbilityTagReleased, &AGBPlayerController::AbilityTagToggled);
    }
}

void AGBPlayerController::ShowDungeonWaitingHUD()
{
}

void AGBPlayerController::TryNotifyReadyToPlay()
{
    GB_NULL_CHECK(PlayerState);

    UWorld* World = GetWorld();
    if (World)
    {
        const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();
        for (ULevelStreaming* LevelStreaming : StreamingLevels)
        {
            if (LevelStreaming)
            {
                GB_CONDITION_CHECK(LevelStreaming->IsLevelLoaded() && LevelStreaming->IsLevelVisible());
            }
        }
    }

    GetWorldTimerManager().ClearTimer(TryNotifyReadyToPlayTickHandle);
    Server_NotifyClientLoadedDungeonMap();
}

void AGBPlayerController::AbilityTagTriggered(const FGameplayTag AssetTag)
{
    if (GetAbilitySystemComponent())
    {
        GetAbilitySystemComponent()->AbilityTagTriggered(AssetTag);
    }
}

void AGBPlayerController::AbilityTagReleased(const FGameplayTag AssetTag)
{
    if (GetAbilitySystemComponent())
    {
        GetAbilitySystemComponent()->AbilityTagReleased(AssetTag);
    }
}

void AGBPlayerController::AbilityTagToggled(const FGameplayTag AssetTag)
{
    if (GetAbilitySystemComponent())
    {
        GetAbilitySystemComponent()->AbilityTagToggled(AssetTag);
    }
}

void AGBPlayerController::OnBlockTagChanged(const FGameplayTag Tag, const int32 Count)
{
    FGameplayTagContainer InputBlockTags(GBTag::Block_All);
    InputBlockTags.AddTag(GBTag::Block_Input);
    InputBlockTags.AddTag(GBTag::Block_Move);

    if (GetAbilitySystemComponent())
    {
        bMoveBlock = GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(InputBlockTags);
    }
}

void AGBPlayerController::Move(const FInputActionValue& InputActionValue)
{
    if (bMoveBlock)
    {
        return;
    }

    const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}

void AGBPlayerController::Look(const FInputActionValue& InputActionValue)
{
    const FVector2D& LookAxisVector = InputActionValue.Get<FVector2D>();

    AddYawInput(LookAxisVector.X);
    AddPitchInput(LookAxisVector.Y);
}

void AGBPlayerController::TryLogin()
{
    UGBClientSessionSubsystem* Session = GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>();

    if (!Session)
    {
        GB_BACKEND_LOG(Error, TEXT("ClientSubsystem not found"));
        return;
    }

    const FString& Token = Session->GetAuthToken();
    const FString& NickName = Session->GetNickname();

    ServerLogin(Token, NickName); // 서버에 로그인 요청
}

void AGBPlayerController::HideWaitingHUD()
{
    // 로컬 컨트롤러만 UI를 가지고 있으니까 안전하게 가드해주는 거 추천
    if (!IsLocalController())
    {
        return;
    }

    if (WaitingWidgetInstance)
    {
        // 실제 화면에서 제거
        WaitingWidgetInstance->RemoveFromParent();

        // GC가 이걸 해제할 수 있도록 참조 끊어주기
        WaitingWidgetInstance = nullptr;

        GB_LOG(TEXT("HideWaitingHUD: Waiting widget removed from viewport"));
    }
    else
    {
        GB_LOG(TEXT("HideWaitingHUD: No waiting widget to remove"));
    }
}

void AGBPlayerController::ShowInGameHUD()
{
    AGBPlayerState* DungeonPlayerState = GetPlayerState<AGBPlayerState>();
    if (DungeonPlayerState)
    {
        EGBCharacterClassType SelectedClassType = DungeonPlayerState->GetSelectedClass();

        TSubclassOf<AHUD> HudClass = DungeonPlayerState->GetAssignedInGameHUDClass();

        ClientSetHUD(HudClass);
    }
}

void AGBPlayerController::ServerLogin_Implementation(const FString& Token, const FString& CharacterId)
{
    AGBPlayerState* PS = GetPlayerState<AGBPlayerState>();
    GB_NULL_CHECK(PS);    

    PS->SetAuthToken(Token);
    PS->SetCharacterId(CharacterId);

    GB_BACKEND_LOG(Warning, TEXT("ServerLogin success. Token: %s, CharacterId: %s"), *Token, *CharacterId);
}
