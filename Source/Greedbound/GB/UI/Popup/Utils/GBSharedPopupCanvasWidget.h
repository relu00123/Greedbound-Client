// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBSharedPopupCanvasWidget.generated.h"

class UCanvasPanel;

// 팝업들을 붙일 수 있는 공용 캔버스 위젯 
UCLASS()
class GREEDBOUND_API UGBSharedPopupCanvasWidget : public UUserWidget
{
    GENERATED_BODY()
    
protected:
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel*               RootCanvas;

public :
    FORCEINLINE UCanvasPanel*   GetRootCanvs() const { return RootCanvas; }
};
