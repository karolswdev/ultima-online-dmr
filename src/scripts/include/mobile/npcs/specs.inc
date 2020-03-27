//-----------------------------------------------------------------------------

include "../../../ai/include/specs/energyshield";
include "include/objects/location";
include "include/items";
include "include/items/temp_storage";
include "include/items/groups";

//-----------------------------------------------------------------------------

const NSPEC_THRESHOLD        := 1;
const NSPEC_AUTODISPEL       := 2;
const NSPEC_PROVOKETAMED     := 3;
const NSPEC_DISPELALL        := 4;
const NSPEC_HPDRAIN          := 5;
const NSPEC_PLAGUE           := 6;
const NSPEC_AUTOSPELL        := 7;
const NSPEC_ACIDPOOL         := 8;
const NSPEC_WHIRLWIND        := 9;
const NSPEC_CONCUSSION       := 10;
const NSPEC_AUTOSUMMON       := 11;
const NSPEC_FLAMEBREATH      := 12;
const NSPEC_AUTOHEALING      := 13;
const NSPEC_KAMIKAZEDMG      := 14;
const NSPEC_MINDRAGE         := 15;
const NSPEC_CONFLAGRATION    := 16;
const NSPEC_ENERGYSHIELD     := 17;
const NSPEC_SUMMONMINIONS    := 18;
const NSPEC_CORPSEEATER      := 19;
const NSPEC_CORPSESPITTER    := 20;
const NSPEC_ABYSSFORCE       := 21;
const NSPEC_CREATEITEM       := 22;
const NSPEC_STONEFALL        := 23;
const NSPEC_BLADEDANCE       := 24;
const NSPEC_REVEALTERROR     := 25;
const NSPEC_MINDCONTROL      := 26;
const NSPEC_REGENERATION     := 27;
const NSPEC_POLYMORPH        := 28;
const NSPEC_CURSE            := 29;
const NSPEC_SPIDERWEBPOOL    := 30;

//-----------------------------------------------------------------------------

const NSPECF_AUTODISPEL      := 0x00000001;
const NSPECF_PROVOKETAMED    := 0x00000002;
const NSPECF_DISPELALL       := 0x00000004;
const NSPECF_HPDRAIN         := 0x00000008;
const NSPECF_PLAGUE          := 0x00000010;
const NSPECF_AUTOSPELL       := 0x00000020;
const NSPECF_ACIDPOOL        := 0x00000040;
const NSPECF_WHIRLWIND       := 0x00000080;
const NSPECF_CONCUSSION      := 0x00000100;
const NSPECF_AUTOSUMMON      := 0x00000200;
const NSPECF_FLAMEBREATH     := 0x00000400;
const NSPECF_AUTOHEALING     := 0x00000800;
const NSPECF_KAMIKAZE        := 0x00001000;
const NSPECF_MINDRAGE        := 0x00002000;
const NSPECF_CONFLAGRATION   := 0x00004000;
const NSPECF_ENERGYSHIELD    := 0x00008000;
const NSPECF_SUMMONMINIONS   := 0x00010000;
const NSPECF_CORPSEEATER     := 0x00020000;
const NSPECF_CORPSESPITTER   := 0x00040000;
const NSPECF_ABYSSFORCE      := 0x00080000; 
const NSPECF_CREATEITEM      := 0x00100000;
const NSPECF_STONEFALL       := 0x00200000;
const NSPECF_BLADEDANCE      := 0x00400000;
const NSPECF_REVEALTERROR    := 0x00800000;
const NSPECF_MINDCONTROL     := 0x01000000;
const NSPECF_REGENERATION    := 0x02000000;
const NSPECF_POLYMORPH       := 0x04000000;
const NSPECF_CURSE           := 0x08000000;
const NSPECF_SPIDERWEBPOOL   := 0x10000000;

const NSPECF_DEBUG           := 0x20000000;

//-----------------------------------------------------------------------------

const NSPEC_GLOBAL_DELAY     := 240;

//-----------------------------------------------------------------------------

function SetNpcSpecPerforming(Npc, SpecId, Counter := 1, Type := 0)

  SetObjProperty(Npc, "#SpecPerf_"+SpecId, Counter);

  if(Type)
    SetObjProperty(Npc, "#SpecPerfType_"+SpecId, Type);
  endif

endfunction

//-----------------------------------------------------------------------------

function IsNpcSpecPerforming(Npc, SpecId, Type := 0)

  if(Type)
    if(GetObjProperty(Npc, "#SpecPerfType_"+SpecId) != Type)
      return 0;
    endif
  endif

  return CInt(GetObjProperty(Npc, "#SpecPerf_"+SpecId)) > 0;

endfunction

//-----------------------------------------------------------------------------

function EraseNpcSpecPerforming(Npc, SpecId)

  EraseObjProperty(Npc, "#SpecPerf_"+SpecId);
  EraseObjProperty(Npc, "#SpecPerfType_"+SpecId);

endfunction

//-----------------------------------------------------------------------------

function BreakNpcSpecial(Npc, SpecId, Dmg := 1, Type := 0)

  if(Type)
    if(GetObjProperty(Npc, "#SpecPerfType_"+SpecId) != Type)
      return 0;
    endif
  endif

  var Count := CInt(GetObjProperty(Npc, "#SpecPerf_"+SpecId));

  if(Count > Dmg)
    SetObjProperty(Npc, "#SpecPerf_"+SpecId, Count-Dmg);
    
    case(SpecId)
      NSPEC_ENERGYSHIELD:
        PerformEnergyShieldDowngrade(Npc);
        
      default:
        PlayPcEffect(Npc, FX_STATIC, 44, 24, 24, 3);
        PlaySoundEffect(Npc, SFX_SPELL_DISPEL);
    endcase
    
    return -1;

  elseif(Count)

    EraseObjProperty(Npc, "#SpecPerf_"+SpecId);

    case(SpecId)
      NSPEC_ENERGYSHIELD:
        DropEnergyShield(Npc);
    endcase

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcSpecialAttack(attacker, defender, Dmg, Type)

  if(Type == PHYSIC_DMG and defender.isa(POLCLASS_NPC))
    if(attacker.isa(POLCLASS_NPC))
      Dmg := CInt(Dmg/5);
    endif
    
    if(GetAttackSkill(attacker) == SKILLID_ARCHERY)
      var Res := BreakNpcSpecial(defender, NSPEC_ABYSSFORCE);

      if(Res > 0)
        SendSysMessage(attacker, "Przerwal"+ggm(attacker,3)+" sile otchlani!", FONT_NORMAL, COLOR_WHITE);
      elseif(Res < 0)
        SendSysMessage(attacker, "Prawie przerwal"+ggm(attacker,3)+" sile otchlani!", FONT_NORMAL, COLOR_ORANGE);
      endif
    endif
    
    var DefElem := GetNpcConfig(defender);

    CheckSpawnBaseItemOnHit(defender, DefElem);

    var MindControl := BreakNpcSpecial(defender, NSPEC_MINDCONTROL, Dmg);

    if(MindControl > 0)
      SendSysMessage(attacker, "Przerwal"+ggm(attacker,3)+" kontrole umyslu!", FONT_NORMAL, COLOR_WHITE);
    elseif(MindControl < 0)
      SendSysMessage(attacker, "Prawie przerwal"+ggm(attacker,3)+" kontrole umyslu!", FONT_NORMAL, COLOR_ORANGE);
    endif

  endif

endfunction

//-----------------------------------------------------------------------------

function CheckSpawnBaseItemOnHit(defender, DefElem)

  if(DefElem.SpawnBaseItem and HaveChance(50))
  
    if(CInt(GetObjProperty(defender, "#SpawnBaseItemDelay")) > ReadGameClock())
      return;
    endif

    var ItemProps := SplitWords(DefElem.SpawnBaseItem);

    if(ItemProps.size() != 5)
      LogError("[CheckNpcSpecialAttack]: Blednie zdefiniowany prop SpawnBaseItem u npc: "+defender.npctemplate);
      return 0;
    endif

    var Perc := GetHpPercent(defender);
    var Mod := CInt(ItemProps[4])*10;
    var Chance := Mod + Mod*(1.0 - Perc);

    if(Perc < 0.2)
      Chance *= 1.4;
    endif

    Chance := CInt(Chance);

    if(RandomInt(1000) > Chance)
      return;
    endif

    var ObjType := GetRandomItemFromGroup(ItemProps[1]);
    var Loc := FindStandableLocNear(defender, 3);
    var ItemElem := GetItemConfig(ObjType);

    if(Loc)
      var Item := PullTempItemAtLoc(Loc.x, Loc.y, Loc.z, ObjType, Loc.realm);

      if(Item)
        SetObjProperty(Item, "SpawnBaseGroup", ItemProps[2]);
        if(ItemElem.SpawnComing)
          SetObjProperty(Item, "SpawnComing", ItemElem.SpawnComing);
        endif

        if(ItemElem.SpawnText)
          SetObjProperty(Item, "SpawnText", ItemElem.SpawnText);
        endif
        PlaySoundEffect(defender, ItemProps[3]);
        if(!start_script(":specattack:spawnitem_control", Item))
          LogError("[CheckNpcSpecialAttack]: Nie udalo sie uruchomic skryptu: :specattack:spawnitem_control!");
        endif
        SetObjProperty(defender, "#SpawnBaseItemDelay", ReadGameClock() + CInt(ItemProps[5]));
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function StopSpawnBaseItemControl(Item)

  var Pid := GetProc(GetObjProperty(Item, "#SpawnBaseItemPid"));
  
  if(Pid)
    Pid.kill();
  endif

endfunction

//-----------------------------------------------------------------------------
