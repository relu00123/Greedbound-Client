#pragma once

#include "NativeGameplayTags.h"

namespace GBTag
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Block_All);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Block_Input);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Block_Move);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Equipped);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Attack);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Dead);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Sprint);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Crouch);

    // Buff
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Invincible);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_SuperArmor);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Heal);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Speed);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Defense);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Attack);

    // Debuff
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Fire);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Poison);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Stun);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Knockback);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Frozen);

    // Select
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Select_Skill_Cleric_Heal);


    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Cooldown);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Cooldown_Skill01);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Cooldown_Skill02);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Cooldown_Skill03);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Cooldown_Skill04);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Speed);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Combat);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage_Impact);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage_Dot);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Direction_Hit);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attribute_Attack);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Duration_Cost_Stamina);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Duration_Regen_Stamina);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_StaminaEmpty);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_Attack);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_TrailAttackBegin);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_TrailAttackEnd);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_ComboCheck);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_Ragdoll);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_Crouch);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_Jump);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Common_PlayMontage);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Skill01);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Skill02);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Skill03);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Skill04);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Sprint);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Dead);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_HitReact);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_RegenStamina);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Primary);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Common_Secondary);

    // Class Skill Ability AssetTag
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Knight);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Knight_CastBuff);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Knight_Dash);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Knight_LeapAttack);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Cleric);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Cleric_HealSelect);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Archer);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Mage);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Mage_IceBarrior);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Trigger_Mage_IceSlash);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player_Knight);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player_Cleric);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player_Archer);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player_Mage);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Monster_Common);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Monster_Sandbag);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetType);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetType_CharacterData);

    // UI
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_Modal);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_GameMenu);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_GameHud);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_WidgetStack_Frontend);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_PressAnyKeyScreen);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_MainMenuScreen);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_OptionsScreen);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_ConfigScreen);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_KeyRemapScreen);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_ConfirmScreen);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Widget_Inventory);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Image_TestImage);
}
