#pragma once

#include "Character/GBCharacterStateTypes.h"

#define GBANIMSECTIONNAME_ATTACK01              FName("Attack01")
#define GBANIMSECTIONNAME_ATTACK02              FName("Attack02")
#define GBANIMSECTIONNAME_ATTACK03              FName("Attack03")
#define GBANIMSECTIONNAME_ATTACK04              FName("Attack04")

#define GBANIMSECTIONNAME_DEATH_FRONT           FName("Death_Front")
#define GBANIMSECTIONNAME_DEATH_LEFT            FName("Death_Left")
#define GBANIMSECTIONNAME_DEATH_RIGHT           FName("Death_Right")
#define GBANIMSECTIONNAME_DEATH_BACK            FName("Death_Back")

#define GBANIMSECTIONNAME_TURN_LEFT             FName("TurnLeft")
#define GBANIMSECTIONNAME_TURN_RIGHT            FName("TurnRight")

#define GBANIMSECTIONNAME_HITREACTION_NOREACT   FName("HitReaction_NoReact")
#define GBANIMSECTIONNAME_HITREACTION_FRONT     FName("HitReaction_F")
#define GBANIMSECTIONNAME_HITREACTION_LEFT      FName("HitReaction_L")
#define GBANIMSECTIONNAME_HITREACTION_RIGHT     FName("HitReaction_R")
#define GBANIMSECTIONNAME_HITREACTION_BACK      FName("HitReaction_B")

class FGBAnimSectionNameHelper
{
public:
    static const FName GetAttackSectionNameByComboIndex(const uint8 ComboIndex) 
    {
        switch (ComboIndex)
        {
        case 1 :
            return GBANIMSECTIONNAME_ATTACK01;
        case 2 :
            return GBANIMSECTIONNAME_ATTACK02;
        case 3:
            return GBANIMSECTIONNAME_ATTACK03;
        case 4:
            return GBANIMSECTIONNAME_ATTACK04;
        default:
        return FName();
            break;
        }
    }

    static const FName GetDeathSectionNameByDirection(const float Direction)
    {
        FName SectionName = NAME_None;
        if (Direction == static_cast<float>(EGBDirection::Forward))
        {
            SectionName = GBANIMSECTIONNAME_DEATH_FRONT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Left))
        {
            SectionName = GBANIMSECTIONNAME_DEATH_LEFT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Right))
        {
            SectionName = GBANIMSECTIONNAME_DEATH_RIGHT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Backward))
        {
            SectionName = GBANIMSECTIONNAME_DEATH_BACK;
        }
        return SectionName;
    }

    static const FName GetHitReactionNameByDirection(const float Direction)
    {
        FName SectionName = NAME_None;
        if (Direction == static_cast<float>(EGBDirection::NoReact))
        {
            SectionName = GBANIMSECTIONNAME_HITREACTION_NOREACT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Forward))
        {
            SectionName = GBANIMSECTIONNAME_HITREACTION_FRONT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Left))
        {
            SectionName = GBANIMSECTIONNAME_HITREACTION_LEFT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Right))
        {
            SectionName = GBANIMSECTIONNAME_HITREACTION_RIGHT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Backward))
        {
            SectionName = GBANIMSECTIONNAME_HITREACTION_BACK;
        }
        return SectionName;
    }

    static const FName GetTurnInPlaceSectionName(const float Direction)
    {
        FName SectionName = NAME_None;
        if (Direction == static_cast<float>(EGBDirection::Left))
        {
            SectionName = GBANIMSECTIONNAME_TURN_LEFT;
        }
        else if (Direction == static_cast<float>(EGBDirection::Right))
        {
            SectionName = GBANIMSECTIONNAME_TURN_RIGHT;
        }

        return SectionName;
    }
};

