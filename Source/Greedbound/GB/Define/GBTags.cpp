#pragma once

#include "GBTags.h"

namespace GBTag
{
    UE_DEFINE_GAMEPLAY_TAG(Block_All,							"Block.All");
    UE_DEFINE_GAMEPLAY_TAG(Block_Input,							"Block.Input");
    UE_DEFINE_GAMEPLAY_TAG(Block_Move,							"Block.Move");

    UE_DEFINE_GAMEPLAY_TAG(State_Combat,        			    "State.Combat");
    UE_DEFINE_GAMEPLAY_TAG(State_Combat_Equipped,			    "State.Combat.Equipped");
    UE_DEFINE_GAMEPLAY_TAG(State_Combat_Attack,			        "State.Combat.Attack");
    UE_DEFINE_GAMEPLAY_TAG(State_Combat_Dead,                   "State.Combat.Dead");
    UE_DEFINE_GAMEPLAY_TAG(State_Movement_Sprint,				"State.Movement.Sprint");
    UE_DEFINE_GAMEPLAY_TAG(State_Movement_Crouch,				"State.Movement.Crouch");

    // Buff
    UE_DEFINE_GAMEPLAY_TAG(State_Buff,							"State.Buff");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_Invincible,				"State.Buff.Invincible");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_SuperArmor,				"State.Buff.SuperArmor");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_Heal,						"State.Buff.Heal");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_Speed,					"State.Buff.Speed");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_Defense,					"State.Buff.Defense");
    UE_DEFINE_GAMEPLAY_TAG(State_Buff_Attack,					"State.Buff.Attack");

    // Debuff
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff,						"State.Debuff");
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Fire,					"State.Debuff.Fire");
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Poison,					"State.Debuff.Poison");
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Stun,					"State.Debuff.Stun");
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Knockback,				"State.Debuff.Knockback");
    UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Frozen,					"State.Debuff.Frozen");

    UE_DEFINE_GAMEPLAY_TAG(State_Select_Skill_Cleric_Heal,		"State.Select.Skill.Cleric.Heal");

    UE_DEFINE_GAMEPLAY_TAG(State_Cooldown,						"State.Cooldown");
    UE_DEFINE_GAMEPLAY_TAG(State_Cooldown_Skill01,				"State.Cooldown.Skill01");
    UE_DEFINE_GAMEPLAY_TAG(State_Cooldown_Skill02,				"State.Cooldown.Skill02");
    UE_DEFINE_GAMEPLAY_TAG(State_Cooldown_Skill03,				"State.Cooldown.Skill03");
    UE_DEFINE_GAMEPLAY_TAG(State_Cooldown_Skill04,				"State.Cooldown.Skill04");

    UE_DEFINE_GAMEPLAY_TAG(Attribute_Health,					"Attribute.Health");
    UE_DEFINE_GAMEPLAY_TAG(Attribute_Speed,						"Attribute.Speed");
    UE_DEFINE_GAMEPLAY_TAG(Attribute_Combat,					"Attribute.Combat");

    UE_DEFINE_GAMEPLAY_TAG(Data_Damage_Impact,					"Data.Damage.Impact");
    UE_DEFINE_GAMEPLAY_TAG(Data_Damage_Dot,						"Data.Damage.Dot");
    UE_DEFINE_GAMEPLAY_TAG(Data_Direction_Hit,					"Data.Direction.Hit");
    UE_DEFINE_GAMEPLAY_TAG(Data_Attribute_Attack,				"Data.Attribute.Attack");

    UE_DEFINE_GAMEPLAY_TAG(Duration_Cost_Stamina,				"Duration.Cost.Stamina");
    UE_DEFINE_GAMEPLAY_TAG(Duration_Regen_Stamina,				"Duration.Regen.Stamina");

    UE_DEFINE_GAMEPLAY_TAG(Event_Common_StaminaEmpty,			"Event.Common.StaminaEmpty");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_Attack,					"Event.Common.Attack");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_TrailAttackBegin,		"Event.Common.TrailAttackBegin");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_TrailAttackEnd,			"Event.Common.TrailAttackEnd");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_ComboCheck,				"Event.Common.ComboCheck");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_Ragdoll,                "Event.Common.Ragdoll");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_Crouch,                 "Event.Common.Crouch");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_Jump,                   "Event.Common.Jump");
    UE_DEFINE_GAMEPLAY_TAG(Event_Common_PlayMontage,            "Event.Common.PlayMontage");

    UE_DEFINE_GAMEPLAY_TAG(Trigger,								"Trigger");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common,						"Trigger.Common");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Skill01,				"Trigger.Common.Skill01");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Skill02,				"Trigger.Common.Skill02");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Skill03,				"Trigger.Common.Skill03");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Skill04,				"Trigger.Common.Skill04");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Sprint,				"Trigger.Common.Sprint");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Dead,					"Trigger.Common.Dead");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_HitReact,				"Trigger.Common.HitReact");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_RegenStamina,			"Trigger.Common.RegenStamina");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Attack,         		"Trigger.Common.Attack");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Primary,				"Trigger.Common.Primary");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Secondary,			"Trigger.Common.Secondary");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Crouch,				"Trigger.Common.Crouch");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Common_Jump,					"Trigger.Common.Jump");

    UE_DEFINE_GAMEPLAY_TAG(Trigger_Knight,						"Trigger.Knight");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Knight_CastBuff,				"Trigger.Knight.CastBuff");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Knight_Dash,					"Trigger.Knight.Dash");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Knight_LeapAttack,			"Trigger.Knight.LeapAttack");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Cleric,						"Trigger.Cleric");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Cleric_HealSelect,			"Trigger.Cleric.HealSelect");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Archer,						"Trigger.Archer");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Mage,						"Trigger.Mage");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Mage_IceBarrior,				"Trigger.Mage.IceBarrior");
    UE_DEFINE_GAMEPLAY_TAG(Trigger_Mage_IceSlash,				"Trigger.Mage.IceSlash");

    UE_DEFINE_GAMEPLAY_TAG(Character_Player,              		"Character.Player");
    UE_DEFINE_GAMEPLAY_TAG(Character_Player_Knight,				"Character.Player.Knight");
    UE_DEFINE_GAMEPLAY_TAG(Character_Player_Cleric,				"Character.Player.Cleric");
    UE_DEFINE_GAMEPLAY_TAG(Character_Player_Archer,				"Character.Player.Archer");
    UE_DEFINE_GAMEPLAY_TAG(Character_Player_Mage,				"Character.Player.Mage");

    UE_DEFINE_GAMEPLAY_TAG(Character_Monster_Common,			"Character.Monster.Common");
    UE_DEFINE_GAMEPLAY_TAG(Character_Monster_Sandbag,			"Character.Monster.Sandbag");

    UE_DEFINE_GAMEPLAY_TAG(AssetType,							"AssetType");
    UE_DEFINE_GAMEPLAY_TAG(AssetType_CharacterData,				"AssetType.CharacterData");

    UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_Modal,				"UI.WidgetStack.Modal");
    UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_GameMenu,				"UI.WidgetStack.GameMenu");
    UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_GameHud,				"UI.WidgetStack.GameHud");
    UE_DEFINE_GAMEPLAY_TAG(UI_WidgetStack_Frontend,				"UI.WidgetStack.Frontend");

    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_PressAnyKeyScreen,			"UI.Widget.PressAnyKeyScreen");
    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_MainMenuScreen,			"UI.Widget.MainMenuScreen");
    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_OptionsScreen,				"UI.Widget.OptionsScreen");

    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_ConfigScreen,				"UI.Widget.ConfigScreen");
    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_KeyRemapScreen,			"UI.Widget.KeyRemapScreen");
    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_ConfirmScreen,       		"UI.Widget.ConfirmScreen");

    UE_DEFINE_GAMEPLAY_TAG(UI_Widget_Inventory,                 "UI.Widget.Inventory");

    UE_DEFINE_GAMEPLAY_TAG(UI_Image_TestImage,                  "UI.Image.TestImage");
}
