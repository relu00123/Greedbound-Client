// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GBListEntryWidget_Base.generated.h"

class UCommonTextBlock;
class UGBListDataObject_Base;
enum class EGBOptionsListDataModifyReason : uint8;

/*
 * 옵션창에서 세부 옵션 ex) Option -> Video -> 'Resolution'
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBListEntryWidget_Base : public UCommonUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On ListEntry Widget Hovered"))
    void                                BP_OnListEntryWidgetHovered(bool bWasHovered, bool IsEntryWidgetStillSelected);
    void                                NativeOnListEntryWidgetHovered(bool bWasHovered);

protected:
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
    UWidget*                            BP_GetWidgetToFocusForGamepad() const;

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Toggle Entry Widget Highlight State"))
    void                                BP_OnToggleEntryWidgetHighlightState(bool bShouldHighlight) const;

    virtual void                        NativeOnListItemObjectSet(UObject* ListItemObject) override final;
    virtual void                        NativeOnItemSelectionChanged(bool bIsSelected) override final;
    virtual void                        NativeOnEntryReleased() override final;
    virtual FReply                      NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override final;
    virtual void                        OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject);

    // 파생클래스의 경우 데이터 수정 시 동작 별도 구현 필요 Super:: 호출x
    virtual void                        OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason) {};
    virtual void                        OnOwningDependencyDataObjectModified(UGBListDataObject_Base* InDependencyDataObject, EGBOptionsListDataModifyReason ModifyReason);

    // 소유한 위젯의 편집가능 상태 수정
    virtual void                        OnToggledEditableState(bool bIsEditable);

    void                                SelectThisEntryWidget();

private:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UCommonTextBlock>        CTB_SettingDisplayName;

    UPROPERTY(Transient)
    TObjectPtr<UGBListDataObject_Base>  CachedOwningDataObject;
};
