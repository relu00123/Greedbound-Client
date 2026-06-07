// Fill out your copyright notice in the Description page of Project Settings.

#include "GBClientHttpManagerSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GB/Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "GB/Subsystem/Network/Client/GBClientSocketSubsystem.h"

void UGBClientHttpManagerSubsystem::SendLoginRequest(const FString& ID, const FString& PWD, FOnLoginResult OnResult)
{
    // Http 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(ServerTravelURL);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // 본문 작성
    FString Body = FString::Printf(TEXT("{\"username\":\"%s\", \"password\":\"%s\"}"), *ID, *PWD);
    Request->SetContentAsString(Body);

    // 응답 콜백 바인딩
    Request->OnProcessRequestComplete().BindLambda(
        [this, OnResult](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bWasSuccessful)
        {
            OnLoginResponse(Req, Resp, bWasSuccessful, OnResult);
        });

    // 요청 전송
    Request->ProcessRequest();
}

void UGBClientHttpManagerSubsystem::SendLogoutRequest(const FString& Token)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("http://localhost:3000/logout");
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    const FString RequestBody = FString::Printf(TEXT("{\"token\":\"%s\"}"), *Token);
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindLambda([WeakThis = TWeakObjectPtr<UGBClientHttpManagerSubsystem>(this)](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
        {
            if (!WeakThis.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("UGBClientHttpManagerSubsystem is no longer valid."));
                return;
            }

            if (bSuccess && Resp.IsValid() && Resp->GetResponseCode() == 200)
            {
                UE_LOG(LogTemp, Log, TEXT("로그아웃 성공"));

                if (IsEngineExitRequested())
                {
                    UE_LOG(LogTemp, Warning, TEXT("엔진 종료 중이므로 세션 정리 생략"));
                    return;
                }

                UGameInstance* GameInstance = WeakThis->GetGameInstance();
                if (IsValid(GameInstance))
                {
                    if (UGBClientSessionSubsystem* Session = GameInstance->GetSubsystem<UGBClientSessionSubsystem>())
                    {
                        Session->ClearSession();
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("로그아웃 실패"));
            }
        });

    Request->ProcessRequest();
}

void UGBClientHttpManagerSubsystem::RegisterUser(const FString& ID, const FString& PWD, TFunction<void(bool, const FString&)> Callback)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL("http://localhost:3000/register");
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");

    FString Body = FString::Printf(TEXT("{\"username\":\"%s\", \"password\":\"%s\"}"), *ID, *PWD);
    Request->SetContentAsString(Body);

    Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
        {
            if (bSuccess && Resp.IsValid())
            {
                const FString ResponseStr = Resp->GetContentAsString();

                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

                if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
                {
                    bool bServerSuccess = JsonObject->GetBoolField(TEXT("success"));
                    FString Message = JsonObject->GetStringField(TEXT("message"));

                    // 서버의 Success 필드 기준으로 Callback을 호출
                    Callback(bServerSuccess, Message);
                }
                else
                {
                    // JSON 파싱 실패
                    Callback(false, TEXT("응답 파싱 실패"));
                }
            }
            else
            {
                FString ErrorMsg = Resp.IsValid() ? Resp->GetContentAsString() : TEXT("서버 응답없음");
                Callback(false, ErrorMsg);
            }
        });

    Request->ProcessRequest();
}

void UGBClientHttpManagerSubsystem::SendStartMatch(const FString& Token, int32 MapNumericId, TFunction<void(bool, const FString&)> Callback, bool bAllowOthers)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL("http://localhost:3000/match/start");
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *Token));

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetNumberField(TEXT("GameMapNumericId"), MapNumericId);
    Json->SetStringField(TEXT("joinPolicy"), bAllowOthers ? TEXT("Open") : TEXT("Closed"));

    FString Body;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Json, Writer);
    Request->SetContentAsString(Body);

    auto ExtractMessage = [](FHttpResponsePtr Resp) -> FString
        {
            if (!Resp.IsValid()) return TEXT("응답이 없습니다.");

            const FString Content = Resp->GetContentAsString();

            TSharedPtr<FJsonObject> Obj;
            const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
            if (FJsonSerializer::Deserialize(Reader, Obj) && Obj.IsValid())
            {
                FString Msg;
                // 서버가 { success, message } 형태라면 여기서 잡힘
                Obj->TryGetStringField(TEXT("message"), Msg);

                FString TicketId;
                Obj->TryGetStringField(TEXT("ticketId"), TicketId);

                if (Msg.IsEmpty())
                {
                    // 서버가 ACK만 보낼 수 있으므로 기본 문구
                    Msg = TEXT("queued");
                }

                if (!TicketId.IsEmpty())
                {
                    // 디버깅/로그 편의를 위해 ticketId를 덧붙임
                    Msg += FString::Printf(TEXT(" (ticketId: %s)"), *TicketId);
                }
                return Msg;
            }
            return Content.IsEmpty() ? TEXT("빈 응답") : Content; // JSON이 아니면 원문 반환
        };

    Request->OnProcessRequestComplete().BindLambda(
        [Callback, ExtractMessage](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bOK)
        {
            if (!Resp.IsValid())
            {
                Callback(false, TEXT("네트워크 오류: 응답이 유효하지 않습니다."));
                return;
            }

            const int32 Code = Resp->GetResponseCode();
            const FString Msg = ExtractMessage(Resp);

            // 서버는 202 Accepted(또는 200 OK)로 얇은 ACK를 반환
            if (bOK && (Code == 200 || Code == 202))
            {
                Callback(true, Msg);
            }
            else
            {
                // 400/401/403/500 등 실패
                Callback(false, FString::Printf(TEXT("[%d] %s"), Code, *Msg));
            }
        });

    Request->ProcessRequest();
}

void UGBClientHttpManagerSubsystem::OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnLoginResult OnResult)
{
    bool bLoginSuccess = false;
    FString Message;
    FString Token;
    FString NickName;

    if (bWasSuccessful && Response.IsValid())
    {
        const int32 StatusCode = Response->GetResponseCode();
        const FString Content = Response->GetContentAsString();

        UE_LOG(LogTemp, Log, TEXT("응답 코드: %d"), StatusCode);
        UE_LOG(LogTemp, Log, TEXT("응답 본문: %s"), *Content);

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            if (StatusCode == 200)
            {
                bLoginSuccess = true;
                Message = JsonObject->GetStringField(TEXT("message"));
                Token = JsonObject->GetStringField(TEXT("token"));
                NickName = JsonObject->GetStringField(TEXT("nickname"));

                // 웹 소켓 연결 
                if (UGBClientSocketSubsystem* Socket = GetGameInstance()->GetSubsystem<UGBClientSocketSubsystem>())
                {
                    Socket->ConnectSocket(Token, NickName);

                    UE_LOG(LogTemp, Warning, TEXT("Web Socket Connected!"));
                }
            }
            else
            {
                bLoginSuccess = false;
                Message = JsonObject->GetStringField(TEXT("message"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("JSON 파싱 실패"));
            Message = TEXT("서버 응답 파싱 실패");
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("로그인 요청 실패 또는 응답 없음"));
        Message = TEXT("서버에 연결할 수 없습니다.");
    }

    OnResult.ExecuteIfBound(bLoginSuccess, Message, Token, NickName);
}

 
