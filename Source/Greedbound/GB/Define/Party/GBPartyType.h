#pragma once

#include "CoreMinimal.h"
#include "GBPartyType.generated.h"


USTRUCT(BlueprintType)
struct FPartyInvite {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString InviterName;

    UPROPERTY(BlueprintReadOnly)
    FDateTime ReceivedAt = FDateTime::UtcNow();
};

namespace GBParty
{
    // 자신 포함 최대 인원
    static constexpr int32 MaxNumbers = 3;

    // 자신 제외 가능한 파티원 수
    static constexpr int32 MaxOtherMembers = MaxNumbers - 1;

    static constexpr int32 InvalidPartyId = -1;
}


UENUM(BlueprintType)
enum class EPartyRole : uint8
{
    NONE,  
    Solo,     // 파티에 가입되지 않은 상태
    Member,
    Host,
    END
};


/**
 * @brief 클라이언트에서 사용하는 파티 정보 구조체
 */
USTRUCT(BlueprintType)
struct FPartyInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 PartyId = GBParty::InvalidPartyId;

    UPROPERTY(BlueprintReadOnly)
    FString HostName;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> Members;

    bool IsFull() const { return Members.Num() >= GBParty::MaxNumbers; }
    bool IsValid() const { return PartyId != GBParty::InvalidPartyId; }

    void Reset()
    {
        PartyId = GBParty::InvalidPartyId;
        HostName.Reset();
        Members.Reset();
    }
    // Default constructor for proper initialization
    FPartyInfo() : PartyId(GBParty::InvalidPartyId) {}
};



/**
 * @brief 델리게이트 선언: 파티 전체 정보가 업데이트되었을 때 호출
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyInfoUpdated, const FPartyInfo& /*UpdatedPartyInfo*/);


// 내가 파티에 들어갔을때 호출  (사용하지 않고 있음)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyJoinedByMe, const FPartyInfo& /*PartyInfo*/);

// 내 파티에 대한 정보가 -> 파티장, 파티멤버, 파티없음로 상태가 바뀌었을때 호출
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMyPartyRoleChanged, EPartyRole /*NewPartyRole*/);

/**
 * @brief 델리게이트 선언: 파티에 멤버가 들어왔을 때 호출
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMemberJoined, const FString& /*NewMemberName*/);

/**
 * @brief 델리게이트 선언: 파티에서 스스로 나왔을시 호출
 */
DECLARE_MULTICAST_DELEGATE(FOnLeaveFromParty);

/**
 * @brief 델리게이트 선언: 파티에서 멤버가 나갔을 때 호출
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMemberLeft, const FString& /*MemberName*/);


/**
 * @brief 델리게이트 선언: 파티에서 강퇴되었을때 호출
 */
DECLARE_MULTICAST_DELEGATE(FOnKickedFromParty);

/**
 * @brief 델리게이트 선언: 파티에서 멤버가 강퇴되었을때 호출
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMemberKicked, const FString& /*KickedMemberName*/);

/**
 * @brief 델리게이트 선언: 파티 호스트가 변경되었을 때 호출
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHostTransferred, int32 /*PartyId*/, const FString& /*OldHostName*/, const FString& /*NewHostName*/);

/**
 * @brief 델리게이트 선언: "지금 표시할 초대"가 바뀔때 브로드캐스트 (팝업은 이 이벤트만 구독한다)
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActivePartyInviteChanged, const FPartyInvite&);
