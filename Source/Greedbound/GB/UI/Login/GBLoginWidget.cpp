// Fill out your copyright notice in the Description page of Project Settings.

#include "GBLoginWidget.h"
#include "Components/Button.h"
#include "Subsystem/UI/GBPopupSubsystem.h"
#include "Components/EditableText.h"
#include "Subsystem/UI/GBPopupSubsystem.h"
#include "Subsystem/Network/Client/GBClientHttpManagerSubsystem.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/Lobby/GBLobbyInitSubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/Network/Client/GBClientSocketSubsystem.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"
#include "UI/Popup/Utils/GBPopupConfig.h"

void UGBLoginWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Button_Login)
    {
        Button_Login->OnClicked.AddDynamic(this, &UGBLoginWidget::OnLoginClicked);
    }

    if (Button_Register)
    {
        Button_Register->OnClicked.AddDynamic(this, &UGBLoginWidget::OnRegisterClicked);
    }

    // Debugging 용도 
    if (!Button_Login)
    {
        UE_LOG(LogTemp, Error, TEXT("Button_Login is nullptr!BindWidget failed!"));
    }
    if (!Button_Register)
    {
        UE_LOG(LogTemp, Error, TEXT("Button_Register is nullptr! BindWidget failed!"));
    }
}

void UGBLoginWidget::HandleLoginResult(bool bSuccess)
{
    // 일단 테스트 
    //GetGameInstance()->GetSubsystem<UGBPopupSubsystem>()->ShowPopup(
    //    TEXT("Clicked Login Button."),
    //    false,
    //    []() { UE_LOG(LogTemp, Log, TEXT("Confirm Clicked")); }
    //);

    FGBPopupConfig Config;
    Config.Message = TEXT("Clicked Login Button.");
    Config.bUseCancel = false;
    Config.bUseTimeout = false;
    Config.OnConfirm = []() {UE_LOG(LogTemp, Log, TEXT("Confirm Clicked!"))};
    GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>()->ShowPopup(Config);

    if (bSuccess)
    {
        // 로그인 성공, 맵 전환, 메세지 , UI.. ?  이걸 굳이 Widget이 결과를 받을 필요가 없을 것 같은데. 
         
    }
}

void UGBLoginWidget::OnLoginClicked()
{
    // HTTP 로그인 요청 전송 
    UE_LOG(LogTemp, Warning, TEXT("OnLoginClicked Function Called!"));

    if (UGBClientHttpManagerSubsystem* HttpSubsystem = GetGameInstance()->GetSubsystem<UGBClientHttpManagerSubsystem>())
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP Subsystem Exists"));

        FString InputID = ID->GetText().ToString();
        FString InputPWD = Password->GetText().ToString();


        HttpSubsystem->SendLoginRequest(InputID, InputPWD,
            FOnLoginResult::CreateLambda([this](bool bSuccess, const FString& Message, const FString& Token, const FString& NickName)
                {
                    if (bSuccess)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("로그인 성공: %s"), *Token);

                        //  세션저장 & 토큰 저장
                        if (UGBClientSessionSubsystem* Session = GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>())
                        {
                            Session->SetAuthToken(Token);
                            Session->SetNickName(NickName);

                            UE_LOG(LogTemp, Warning, TEXT("테스트중...."));

                            UGBClientSocketSubsystem* ClientSocketSubsystem = GetGameInstance()->GetSubsystem<UGBClientSocketSubsystem>();


                            //if (UGBLobbyInitSubsystem* LobbyInitSubSystem = GetGameInstance()->GetSubsystem<UGBLobbyInitSubsystem>())
                            //{
                            //    // 로비 진입전 초기화 대상 설정
                            //    LobbyInitSubSystem->SetTargetInitSteps(ELobbyInitStep::ALL);

                            //    // 웹 소켓으로 정보 요청
                            //    GBClientSocketRequestHelper::SendFriendListRequest();
                            //    GBClientSocketRequestHelper::SendLobbyUserListRequest();
                            //}
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("로그인 실패: %s"), *Message);

                        /*if (UGBPopupSubsystem* Popup = GetGameInstance()->GetSubsystem<UGBPopupSubsystem>())
                        {
                            Popup->ShowPopup(Message, false, []()
                                {
                                    UE_LOG(LogTemp, Log, TEXT("로그인 실패 확인 클릭"));
                                });
                        }*/

                        if (UGBPopupManagerSubsystem* Popup = GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>())
                        {
                            FGBPopupConfig Config;
                            Config.Message = Message;
                            Config.bUseCancel = false;
                            Config.OnConfirm = []() { UE_LOG(LogTemp, Log, TEXT("로그인 실패 확인 클릭"))};
                            Popup->ShowPopup(Config);
                        }
                    }
                }));
    }
}

void UGBLoginWidget::OnRegisterClicked()
{
    // HTTP 회원가입 요청 전송 
    UE_LOG(LogTemp, Warning, TEXT("OnRgisterClicked Function Called!"));

    if (!ID || !Password)
    {
        UE_LOG(LogTemp, Error, TEXT("ID 또는 Password 위젯이 null입니다."));
        return;
    }

    FString InputID = ID->GetText().ToString();
    FString InputPWD = Password->GetText().ToString();
     
    if (UGBClientHttpManagerSubsystem* HttpSubsystem = GetGameInstance()->GetSubsystem<UGBClientHttpManagerSubsystem>())
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP Subsystem Exists"));
        HttpSubsystem->RegisterUser(InputID, InputPWD, [this](bool bSuccess, const FString& Response)
            {

                if (UGBPopupManagerSubsystem* Popup = GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>())
                {
                    if (bSuccess)
                    {
                        FGBPopupConfig config;
                        config.Message = FString::Printf(TEXT("계정을 생성하였습니다: %s"), *Response);
                        config.bUseCancel = false;
                        config.OnConfirm = []() { UE_LOG(LogTemp, Warning, TEXT("확인 버튼 누름"))};

                        Popup->ShowPopup(config);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("이미 존재하는 계정입니다. : %s"), *Response);


                        FGBPopupConfig config;
                        config.Message = FString::Printf(TEXT("이미 존재하는 계정입니다"));
                        config.bUseCancel = false;
                        config.OnConfirm = []() { UE_LOG(LogTemp, Warning, TEXT("Confirm 버튼 누름")); };

                        Popup->ShowPopup(config);
                    }
                }

                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Popup생성 실패!"));
                }

                //if (UGBPopupSubsystem* Popup = GetGameInstance()->GetSubsystem<UGBPopupSubsystem>())
                //{
                //    if (bSuccess)
                //    {
                //        Popup->ShowPopup(
                //            FString::Printf(TEXT("계정을 생성하였습니다: %s"), *Response),
                //            false,  // Cancel 버튼 사용 여부 (false면 확인 버튼만 나옴)
                //            []() { UE_LOG(LogTemp, Warning, TEXT("확인 버튼 누름")); }  // ConfirmFn
                //        );
                //    }
                //    else
                //    {
                //        UE_LOG(LogTemp, Error, TEXT("이미 존재하는 계정입니다. : %s"), *Response);

                //        /*Popup->ShowPopup(
                //            FString::Printf(TEXT("이미 존재하는 계정입니다: %s"), *Response),
                //            false,
                //            []() { UE_LOG(LogTemp, Warning, TEXT("에러 확인")); }
                //        );*/

                //        Popup->ShowPopup(
                //            FString::Printf(TEXT("이미 존재하는 계정입니다")),
                //            false,
                //            []() { UE_LOG(LogTemp, Warning, TEXT("Confirm 버튼 누름")); }
                //        );
                //    }
                //}
                //else
                //{
                //    UE_LOG(LogTemp, Warning, TEXT("Popup생성 실패!"));
                //}
            });
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP Subsystem Does not Exists"));
    }
}
