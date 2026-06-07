// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPrimaryLayoutWidget.h"
#include "Input/CommonUIInputTypes.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "UI/GBUIFunctionLibrary.h"
#include "UI/InGame/PlayerInfo/GBPlayerHUDWidget.h"
#include "UI/InGame/Options/GBOptionsScreenWidget.h"
#include "UI/InGame/Inventory/GBInventoryScreenWidget.h"
#include "UI/InGame/AsyncAction/GBAsyncAction_PushSoftWidget.h"
#include "AbilitySystem/Attribute/GBHealthAttributeSet.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"
#include "AbilitySystem/GameplayAbility/Player/GBGA_PlayerSkill.h"
#include "Subsystem/UI/GBUISubsystem.h"

UCommonActivatableWidgetContainerBase* UGBPrimaryLayoutWidget::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
    GB_CONDITION_CHECK_WITH_RETURN(RegisteredWidgetStackMap.Contains(InTag), nullptr);
    return RegisteredWidgetStackMap.FindRef(InTag);
}

void UGBPrimaryLayoutWidget::SetHUDVisible(bool bVisibie)
{
    if (HUDWidget)
    {
        HUDWidget->SetVisibility(bVisibie ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UGBPrimaryLayoutWidget::SetHUDWidget(UAbilitySystemComponent* InASC)
{
    GB_NULL_CHECK(InASC);
    GB_NULL_CHECK(HUDWidgetClass);
    
    HUDWidget = CreateWidget<UGBPlayerHUDWidget>(GetWorld(), HUDWidgetClass);
    HUDWidget->InitializeAttributeInfo(InASC);
    
    TArray<TPair<FGameplayTag, UTexture2D*>> SkillIconInfoList;
    TArray<FGameplayAbilitySpec>& Abilities = InASC->GetActivatableAbilities();
    uint8 Skillndex = 1;
    
    for (const FGameplayAbilitySpec& AbilitySpec : Abilities)
    {
        UGBGA_PlayerSkill* SkillAbility = Cast<UGBGA_PlayerSkill>(AbilitySpec.Ability);
        if (SkillAbility)
        {
            UTexture2D* IconTexture = SkillAbility->GetSkillIcon();
            if (IconTexture)
            {
                const FGameplayTag& SkillTag = GetSkillTagByIndex(Skillndex);
                SkillIconInfoList.Add({ SkillTag, IconTexture });
                ++Skillndex;
            }
        }
    }
    
    HUDWidget->SetSkillIconTexture(SkillIconInfoList);
    
    InASC->GetGameplayAttributeValueChangeDelegate(UGBHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnHealthAttributeChanged);
    InASC->GetGameplayAttributeValueChangeDelegate(UGBHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnHealthAttributeChanged);
    InASC->GetGameplayAttributeValueChangeDelegate(UGBSpeedAttributeSet::GetCurrnetStaminaAttribute()).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnSpeedAttributeChanged);
    InASC->GetGameplayAttributeValueChangeDelegate(UGBSpeedAttributeSet::GetMaxStaminaAttribute()).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnSpeedAttributeChanged);
    
    const FGameplayTagContainer& BuffTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Buff);
    for (const FGameplayTag BuffTag : BuffTags)
    {
        InASC->RegisterGameplayTagEvent(BuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnBuffStateChanged);
    }
    
    const FGameplayTagContainer& DebuffTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Debuff);
    const FGameplayTagContainer& CooldownTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Cooldown);
    
    for (const FGameplayTag DebuffTag : DebuffTags)
    {
        InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnBuffStateChanged);
    }
    
    for (const FGameplayTag CooldownTag : CooldownTags)
    {
        InASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnSkillCooldownChanged);
    }
    
    InASC->RegisterGameplayTagEvent(GBTag::State_Combat_Equipped, EGameplayTagEventType::NewOrRemoved).AddUObject(HUDWidget, &UGBPlayerHUDWidget::OnSkillInfoChanged);
    
    HUDWidget->AddToViewport();
}

void UGBPrimaryLayoutWidget::RegisterWidgetStack(UPARAM(meta = (Categories = "UI.WidgetStack")) FGameplayTag InTag, UWidget* InWidgetStack)
{
    // 에디터타임이 아닐 때만 등록
    if (IsDesignTime())
    {
        return;
    }

    if (RegisteredWidgetStackMap.Contains(InTag) == false)
    {
        UCommonActivatableWidgetContainerBase* ActivatableWidgetContainer = Cast<UCommonActivatableWidgetContainerBase>(InWidgetStack);
        GB_NULL_CHECK(ActivatableWidgetContainer);

        RegisteredWidgetStackMap.Add(InTag, ActivatableWidgetContainer);
    }
}

TOptional<FUIInputConfig> UGBPrimaryLayoutWidget::GetDesiredInputConfig() const
{
    UGBUISubsystem::Get(this)->SetHUDVisible(true);

    return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CapturePermanently, EMouseLockMode::LockOnCapture, true);
}

void UGBPrimaryLayoutWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (InventoryAction.IsNull() == false)
    {
        RegisterUIActionBinding(FBindUIActionArgs(
            InventoryAction,
            false,
            FSimpleDelegate::CreateUObject(this, &UGBPrimaryLayoutWidget::OnInventoryActionTriggered)
        ));
    }
}

void UGBPrimaryLayoutWidget::NativeOnActivated()
{
    Super::NativeOnActivated();
}

bool UGBPrimaryLayoutWidget::NativeOnHandleBackAction()
{
    UWidget* GameMenuActivateWidget = RegisteredWidgetStackMap[GBTag::UI_WidgetStack_GameMenu]->GetActiveWidget();
    if (Cast<UGBOptionsScreenWidget>(GameMenuActivateWidget))
    {
        return true;
    }

    UGBAsyncAction_PushSoftWidget* PushAction = UGBAsyncAction_PushSoftWidget::PushSoftWidget(this, GetOwningPlayerController(),
        UGBUIFunctionLibrary::GetSoftWidgetClassByTag(GBTag::UI_Widget_MainMenuScreen), GBTag::UI_WidgetStack_GameMenu);
    if (PushAction)
    {
        PushAction->Activate();
    }

    // BackAction 들어왔다고 Deactivate 되면 안됨 Super 호출 X
    return true;
}

void UGBPrimaryLayoutWidget::OnInventoryActionTriggered()
{
    UCommonActivatableWidget* ActivatedWidget = RegisteredWidgetStackMap[GBTag::UI_WidgetStack_GameMenu]->GetActiveWidget();
    if (ActivatedWidget && Cast<UGBInventoryScreenWidget>(ActivatedWidget))
    {
        ActivatedWidget->DeactivateWidget();
        return;
    }

    UGBAsyncAction_PushSoftWidget* PushAction = UGBAsyncAction_PushSoftWidget::PushSoftWidget(this, GetOwningPlayerController(),
        UGBUIFunctionLibrary::GetSoftWidgetClassByTag(GBTag::UI_Widget_Inventory), GBTag::UI_WidgetStack_GameMenu);
    if (PushAction)
    {
        PushAction->Activate();
    }
}

FGameplayTag UGBPrimaryLayoutWidget::GetSkillTagByIndex(uint8 Index) const
{
    switch (Index)
    {
    case 1:
        return GBTag::Trigger_Common_Skill01;
    case 2:
        return GBTag::Trigger_Common_Skill02;
    case 3:
        return GBTag::Trigger_Common_Skill03;
    case 4:
        return GBTag::Trigger_Common_Skill04;
    default:
        GB_LOG(TEXT("Invalid Index : %i"), Index)
            break;
    }

    return FGameplayTag::EmptyTag;
}
