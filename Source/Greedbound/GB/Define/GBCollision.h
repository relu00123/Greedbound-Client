
#pragma once

#include "CoreMinimal.h"

#define GBCOLLISIONCHANNEL_MONSTER          ECC_GameTraceChannel2
#define GBCOLLISIONCHANNEL_PLAYER           ECC_GameTraceChannel3
#define GBCOLLISIONCHANNEL_MONSTERMESHTRACE ECC_GameTraceChannel4
#define GBCOLLISIONCHANNEL_PLAYERMESHTRACE  ECC_GameTraceChannel5
#define GBCOLLISIONCHANNEL_PLAYERAOE        ECC_GameTraceChannel7
#define GBCOLLISIONCHANNEL_MONSTERAOE       ECC_GameTraceChannel8
#define GBCOLLISIONCHANNEL_ITEM             ECC_GameTraceChannel6
#define GBCOLLISIONCHANNEL_PLAYERINTERACT   ECC_GameTraceChannel9

#define GBCOLLISION_PROFILE_RAGDOLL         FName("Ragdoll")
#define GBCOLLISION_PROFILE_BLOCKALL        FName("BlockAll")
#define GBCOLLISION_PROFILE_NOCOLLISION     FName("NoCollision")
#define GBCOLLISION_PROFILE_ITEM            FName("Item")
#define GBCOLLISION_PROFILE_PLAYERAOE       FName("PlayerAOE")
#define GBCOLLISION_PROFILE_PLAYERMESH      FName("PlayerMesh")
#define GBCOLLISION_PROFILE_PLAYERCAPSULE   FName("PlayerCapsule")
#define GBCOLLISION_PROFILE_MONSTERAOE      FName("MonsterAOE")
#define GBCOLLISION_PROFILE_MONSTERMESH     FName("MonsterMesh")
#define GBCOLLISION_PROFILE_MONSTERCAPSULE  FName("MonsterCapsule")
