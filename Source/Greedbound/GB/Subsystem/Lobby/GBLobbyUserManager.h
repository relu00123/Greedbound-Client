// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/User/UserInfo.h"
#include "GBLobbyUserManager.generated.h"

// 유저 추가/갱신
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUserUpdated, const FUserInfo&);
// 유저 제거
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUserRemoved, const FUserInfo&);
// 유저 추가
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUserAdded, const FUserInfo&);

// 유저 클래스 정보 업데이트 
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUserClassChanged, const FString&  /*UserName*/, EGBCharacterClassType /*ChangedClassType*/);

// 유저 게임 Phase 정보 업데이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUserGamePhaseChanged, const FString& /*UserName*/, EGBClientGamePhase /*ClientGamePhase*/);

// 내정보 전용
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMyUserInfoReady, const FUserInfo&);  // 최초 세팅 1회

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMyUserInfoChanged, const FUserInfo&);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeClassResult, bool /*bSuccess*/, EGBCharacterClassType /*OldClass*/, EGBCharacterClassType /*NewClass*/);

UCLASS()
class GREEDBOUND_API UGBLobbyUserManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // 전체 초기 목록 설정 (LobbyUserListResponse 수신 시 사용)
    void                                    InitUserList(const TArray<FUserInfo> &InList);

    // 개별 유저 추가 (중간 입장)
    void                                    AddUser(const FUserInfo& NewUser);

    // 유저 제거 (중간 퇴장)
    void                                    RemoveUser(const FString& UserName); 

    // 목록 초기화
    void                                    ClearUserList();

    // 전제 유저목록 조회 
    const TMap<FString, FUserInfo>&         GetUserMap() const; 

    // 특정 유저 조회 
    const FUserInfo*                        FindUser(const FString& UserName) const;



    // 내 Info 가져오기
    const FUserInfo*                        GetMyUserInfo() const;

    // 내 유저이름 가져오기
    const FString&                          GetMyUserName() const;

    void                                    SetMyUserInfo(const FUserInfo& MyInfo);

    // Class(직업) 변경함수
    void                                    SetClasss(const FString& UserName, EGBCharacterClassType ClassType);

    // GamePhase 변경함수
    void                                    SetGamePhase(const FString& UserName, EGBClientGamePhase GamePhase);

    void                                    RequestChangeClass(EGBCharacterClassType NewClass);

    // 소켓 응답을 받았을시
    UFUNCTION()
    void                                    HandleChangeMyUserInfoResult(bool bSuccess, const FUserInfo& NewInfo);
    UFUNCTION()
    void                                    HandleChangeClassResponse(bool bSuccess, EGBCharacterClassType OldClass, EGBCharacterClassType NewClass);
    UFUNCTION()
    void                                    HandleUpdateUserInfo(FUserInfo userInfo);

    // 델리게이트 등록
    FORCEINLINE FDelegateHandle             BindOnUserUpdated(const FOnUserUpdated::FDelegate& Delegate) { return OnUserUpdated.Add(Delegate); }
    FORCEINLINE void                        UnbindOnUserUpdated(FDelegateHandle Handle) { OnUserUpdated.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnUserRemoved(const FOnUserRemoved::FDelegate& Delegate) { return OnUserRemoved.Add(Delegate); }
    FORCEINLINE void                        UnbindOnUserRemoved(FDelegateHandle Handle) { OnUserRemoved.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnUserAdded(const FOnUserAdded::FDelegate& Delegate) { return OnUserAdded.Add(Delegate); }
    FORCEINLINE void                        UnbindOnUserAdded(FDelegateHandle Handle) { OnUserAdded.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnMyUserInfoReady(const FOnMyUserInfoReady::FDelegate& D) { return OnMyUserInfoReady.Add(D); }
    FORCEINLINE void                        UnbindOnMyUserInfoReady(FDelegateHandle Handle) { OnMyUserInfoReady.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnMyUserInfoChanged(const FOnMyUserInfoChanged::FDelegate& Delegate) { return OnMyUserInfoChanged.Add(Delegate); }
    FORCEINLINE void                        UnbindOnMyUserInfoChanged(FDelegateHandle Handle) { OnMyUserInfoChanged.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnChangeClassResult(const FOnChangeClassResult::FDelegate& Delegate) { return OnChangeClassResult.Add(Delegate); }
    FORCEINLINE void                        UnbindOnChangeClassResult(FDelegateHandle Handle) { OnChangeClassResult.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnUserClassChanged(const FOnUserClassChanged::FDelegate& Deleage) { return OnUserClassChanged.Add(Deleage); }
    FORCEINLINE void                        UnbindOnUserClassChanged(FDelegateHandle Handle) { OnUserClassChanged.Remove(Handle); }

    FORCEINLINE FDelegateHandle             BindOnUserGamePhaseChanged(const FOnUserGamePhaseChanged::FDelegate& Deleage) { return OnUserGamePhaseChanged.Add(Deleage); }
    FORCEINLINE void                        UnbindOnUserGamePhaseChanged(FDelegateHandle Handle) { OnUserGamePhaseChanged.Remove(Handle); }
    
private :
    UPROPERTY() 
    TMap<FString, FUserInfo>                CachedUserMap;

    /*UPROPERTY()
    FUserInfo                               MyUserInfo;*/

    FString                                 MyUserName; 

    FOnUserAdded                            OnUserAdded; 
    FOnUserUpdated                          OnUserUpdated;
    FOnUserRemoved                          OnUserRemoved;

    FOnMyUserInfoReady                      OnMyUserInfoReady;
    FOnMyUserInfoChanged                    OnMyUserInfoChanged;  // UserInfo가 변경되었을시에 대한 응답 

    FOnChangeClassResult                    OnChangeClassResult;  // 클래스 변경 요청에 대한 응답

    FOnUserClassChanged                     OnUserClassChanged;   // 유저의 클래스 정보가 변경되었을때 

    FOnUserGamePhaseChanged                 OnUserGamePhaseChanged; // 유저의 게임페이즈 정보가 변경되었을때. 

    bool                                    bChangeClassPending = false;

    bool                                    bMyUserReady = false; 
};
