//-----------------------------------------------------------------------------

include "include/items";
include "include/effects";
include "include/bits";
include "include/finding";
include "include/objtypes/misc";
include "include/clock";

//-----------------------------------------------------------------------------

function DoTempMod(byref ModParms)

  if(who.dead)
    return 0;
  endif

  ModParms.+source  := who;
  ModParms.+serial  := WhoSer;

  var type              := ModParms.modtype;
  var amt               := ModParms.amt;
  var duration          := CInt(ModParms.duration);

  if(!who or !duration)
    return;
  endif

  if(!CanMagicMod(who, type))
    return;
  endif

  case(type)
    MOD_STR:
    MOD_DEX:
    MOD_INT:
    MOD_AR:
    MOD_GRAPHIC:
    MOD_COLOR:
    MOD_PARA:
    MOD_RA:
    MOD_LIGHT:
    MOD_POISON_PROT:
    MOD_LIFE:
    MOD_MANA:
    MOD_STAMINA:
    MOD_FIRERESIST:
    MOD_COLDRESIST:
    MOD_LIGHTRESIST:
    MOD_HP_REGEN:
    MOD_MANA_REGEN:
    MOD_STAM_REGEN:
    MOD_CTH:
    MOD_PENETRATE:
    MOD_UNDEAD_SLAY:
    MOD_DAEMON_SLAY:
    MOD_ORDER_SLAY:
    MOD_DEFENSE:
    MOD_TREE_SUPPORT:
    MOD_MONSTER_AR:
    MOD_SILVER_AR:
    MOD_FIRE_BLOOD:
    MOD_ONHIT_AR:
    MOD_LIFE_STOLEN:
    MOD_MANA_STOLEN:
    MOD_MANA_DMG:
    MOD_MULTI_SHOT:
    MOD_EXPLO_DMG:
    MOD_LOWER_RC:
    MOD_FASTER_CAST:
    MOD_MANA_COST:
    MOD_SPELL_DMG:
    MOD_POISON_DMG:
    MOD_FIRE_DMG:
    MOD_COLD_DMG:
    MOD_LIGHT_DMG:
    MOD_STATUS:
    MOD_RETORT:
    MOD_CRIT_HIT:
    MOD_STAM_DMG:
    MOD_DISARM:
    MOD_STUPOR:
    MOD_CHARM_ANIMAL:
    MOD_RED_SPELL:
    MOD_DISMOUNT_RES:
    MOD_2H_DMG:
    MOD_CASTSIGHT:    
    MOD_DMG_REDUCE: 
    MOD_BLOCK_CAST:
    MOD_BLOCK_FIGHT:
    MOD_BLOCK_SELFHEAL:
                        Amt := CInt(Amt);
                        if(!Amt)
                          return;
                        endif

    MOD_TPREFIX:
    MOD_TSUFFIX:
    MOD_TGUILD:
    MOD_TRACE:          if(!Amt and Amt != "")
                          return;
                        endif

    MOD_INCO:
                        if(!Amt)
                          return;
                        endif


    MOD_HAIRS:
    MOD_MOUNT:          if(!amt or amt.size() != 2 or !CInt(amt[1]))
                          return;
                        endif

                        amt[1] := CInt(amt[1]);
                        amt[2] := CInt(amt[2]);

    MOD_UNTOUCHABLE:
    MOD_MAGIC_SHIELD:   if(!amt or amt.size() != 2)
                          return;
                        endif

                        amt[1] := CInt(amt[1]);

    MOD_SLOWDOWN:       Amt := CInt(Amt);
                        if(!Amt)
                          return;
                        endif
                        
                        if(who.isa(POLCLASS_NPC))
                          if(amt > who.run_speed - 10)
                            amt := who.run_speed - 10;
                          endif
                        endif

    MOD_HGHOSTS:
    MOD_PARA:           break;
    MOD_RUN_SPEED:      Amt := CInt(Amt);
    
    default:            return;
  endcase

  var CurMod := GetObjProperty(who, PROP_TEMP_MOD + Type);

  case(CanMod(who, type))
    0:         if(ModParms.Flags & MODF_FORCE_MODIFY)
                 case(type)
                   MOD_HAIRS:
                   MOD_MOUNT:
                   MOD_HGHOSTS:
                   MOD_LIGHT:
                   MOD_PARA:      return;
                 endcase

                 if(CurMod) // wcale nie musi byc wlaczony aktualny mod, moze byc zablokowany z innych przyczyn
                  
                   if(IsDependantMod(who, Type)) // jesli jest blokniety przez dependa i ma juz moda, to nie mozna podwoic.
                     return;
                   endif

                   if(GetObjProperty(who, PROP_TEMP_PREVMOD + Type))
                     return;
                   endif
                 endif

               else
                 return;
               endif

    1:         SetBitOff(ModParms.Flags, MODF_FORCE_MODIFY);

    default:   return;
  endcase

  SetScriptController(who);
  NoLongerBusy(who);

  var PrevMod := CurMod;
  var PrevAmt := 0;

  if(PrevMod)
    SetObjProperty(who, PROP_TEMP_PREVMOD + Type, PrevMod);
    PrevAmt := CInt(PrevMod[1]);
  endif

  CurMod := array(Amt, Duration, ReadGameClock(), 0);

  SetObjProperty(who, PROP_TEMP_MOD + Type, CurMod);

  case(type)
    MOD_STR:            SetStrengthMod(who, GetStrengthMod(who) + amt);

    MOD_DEX:            SetDexterityMod(who, GetDexterityMod(who) + amt);

    MOD_INT:            SetIntelligenceMod(who, GetIntelligenceMod(who) + amt);

    MOD_AR:             who.ar_mod := who.ar_mod + Amt;

    MOD_PARA:           if(amt == 2)
                          who.frozen := 1;
                        else
                          who.SetParalyzed(1);
                        endif

                        if(Duration > 0) // moze byc infinite... chociaz z para to nie bardzo, ale dla safety
                          CagNpcDamaged(who, Duration*2); // na sztywno, aby nie bylo biernego na paralizu
                          SetObjProperty(who,"#NoDismount",ReadGameClock() + Duration + 8);
                        endif

    MOD_INCO:           if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.name);
                        endif
                        who.name := amt;
                        SetObjProperty(who, "#TempAge", 24 + RandomInt(25));

    MOD_RUN_SPEED:      if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          if(who.isa(POLCLASS_NPC))
                            SetModOldAmount(who, Type, who.run_speed);
                            if(amt == RUNSPEED_FAST)
                              who.run_speed := CInt(who.run_speed * 1.30);
                            elseif(amt == RUNSPEED_SLOW)
                              who.run_speed := CInt(who.run_speed * 0.70);
                            endif
                          else
                            SetModOldAmount(who, Type, amt);
                            SendMountSpeedPacket(who, amt);
                          endif                        
                        elseif(IsPlayer(who))
                          SendMountSpeedPacket(who, amt);
                        endif

    MOD_SLOWDOWN:       if(who.isa(POLCLASS_NPC))
                          if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                            SetModOldAmount(who, Type, who.run_speed);
                          endif
                          who.run_speed := who.run_speed - amt;
                        endif

    MOD_GRAPHIC:        if(GetEquipmentByLayer(who,LAYER_MOUNT))
                          if(who.isa(POLCLASS_NPC))
                            if(!MoveToCont(GetEquipmentByLayer(who,LAYER_MOUNT), who.backpack))
                              return;
                            endif
                          else
                            SendSysMessage(who,"Na wierzchowcu ciezko Ci bedzie zmienic ksztalt!");
                            return;
                          endif
                        endif

                        if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.graphic);
                        endif
                        who.graphic := amt;

    MOD_MOUNT:          var TempMount := CreateItemAtTempLoc(UOBJ_MOUNT);
                        if(TempMount)
                          SetFlag(TempMount, ITEMF_NO_LOOT);
                          TempMount.graphic := Amt[1];
                          TempMount.color   := Amt[2];
                          EquipItem(who, TempMount);
                          SetModOldAmount(who, Type, TempMount.serial);
                        endif

    MOD_COLOR:          if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.color);
                        endif
                        who.color := amt;

    MOD_HGHOSTS:        GrantPrivilege(who,"hearghosts");
                        who.enable("hearghosts");
                        SetModOldAmount(who, Type, 1);

    MOD_TPREFIX:        if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.title_prefix);
                        endif
                        who.title_prefix := amt;

    MOD_TSUFFIX:        if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.title_suffix);
                        endif
                        who.title_suffix := amt;

    MOD_TGUILD:         if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.title_guild);
                        endif
                        who.title_guild := amt;

    MOD_TRACE:          if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          SetModOldAmount(who, Type, who.title_race);
                        endif
                        who.title_race := amt;

    MOD_LIGHT:          if(duration == MODDUR_INFINITE)
                          who.setlightlevel( amt, 86400 );
                        else
                          who.setlightlevel( amt, duration + Cint(duration / 12) );
                        endif

    MOD_HAIRS:          var OldHairs := GetEquipmentByLayer(who,LAYER_HAIR);
                        var OldHairsProps := {-1,0};
                        if(OldHairs.objtype)
                          OldHairsProps[1] := OldHairs.objtype;
                        endif
                        OldHairsProps[2] := CInt(OldHairs.color);
                        if(OldHairs)
                          if(!DestroyItem(OldHairs))
                            return;
                          endif
                        endif
                        SetModOldAmount(who, Type, {OldHairsProps, Amt});
                        var NewHairs := CreateItemAtLoc(6104, 1182, 0, Amt[1], 1);
                        NewHairs.color := Amt[2];
                        if(NewHairs)
                          if(!EquipItem(who,Newhairs))
                            DestroyItem(NewHairs);
                          else
                            Newhairs.movable := 0;
                          endif
                        endif
      MOD_LIFE:
      MOD_MANA:
      MOD_STAMINA:
      MOD_HP_REGEN:
      MOD_MANA_REGEN:
      MOD_STAM_REGEN:   SetTempModAmount(who, type, PrevAmt + amt);
                        RecalcVitals(who);

      MOD_FIRERESIST:
      MOD_COLDRESIST:
      MOD_LIGHTRESIST:
      MOD_CTH:
      MOD_PENETRATE:
      MOD_UNDEAD_SLAY:
      MOD_DAEMON_SLAY:
      MOD_ORDER_SLAY:
      MOD_DEFENSE:
      MOD_TREE_SUPPORT:
      MOD_MONSTER_AR:
      MOD_SILVER_AR:
      MOD_FIRE_BLOOD:
      MOD_LOWER_RC:
      MOD_FASTER_CAST:
      MOD_MANA_COST:
      MOD_SPELL_DMG:
      MOD_POISON_DMG:
      MOD_FIRE_DMG:
      MOD_EXPLO_DMG:
      MOD_COLD_DMG:
      MOD_LIGHT_DMG:
      MOD_RETORT:
      MOD_CRIT_HIT:
      MOD_STAM_DMG:
      MOD_DISARM:
      MOD_STUPOR:
      MOD_2H_DMG:
      MOD_BLOCK_CAST:
      MOD_BLOCK_FIGHT:
      MOD_BLOCK_SELFHEAL:
      MOD_RA:           
      MOD_CASTSIGHT:   
      MOD_DMG_REDUCE: 
                        SetTempModAmount(who, type, PrevAmt + amt);

      MOD_ONHIT_AR:     SetTempModAmount(who, type, 1);
                        who.ar_mod := who.ar_mod + 1;

      MOD_STATUS:       if(!(ModParms.Flags & MODF_FORCE_MODIFY))
                          var Stat := {who.criminal,who.murderer};
                          if(Amt == 3)
                            Stat[1] := 1;
                          endif
                          SetModOldAmount(who, Type, Stat);
                        endif

                        case(Amt)
                          1: SetCriminal(who, 0);
                             who.SetMurderer(0);
                          2: SetCriminal(who, 1, "DoTempMod", "Ustawienie krima poprzez Status Mod na czas: "+__IfElse(Duration == MODDUR_INFINITE, "nieskonczonosc", RealTime(Duration) )+", Amt: 2, Byl krimem: "+who.criminal+".");
                             who.SetMurderer(0);
                          3: SetCriminal(who, 1, "DoTempMod", "Ustawienie krima i murda poprzez Status Mod na czas: "+__IfElse(Duration == MODDUR_INFINITE, "nieskonczonosc", RealTime(Duration) )+", Amt: 3, Byl krimem: "+who.criminal+".");
                             who.SetMurderer(1);
                        endcase
  endcase

  PlayDoModEffect(who, Type);

  NotifyAddBuff(who, type, amt, duration);
  SetDefaultDelayForMod(who, Type, PrevAmt + amt);

  if(duration != MODDUR_INFINITE)

    if(PrevMod and ExPid(PrevMod[4]))
      var uProc := GetProc(PrevMod[4]);
      var rEv := struct;
      rEv.+refresh := 1;

      if(uProc)
        uProc.sendevent(rEv);
      endif
      CurMod[4] := PrevMod[4];
    else
      var uProc := start_script(":magiceffects:undomodcheck", ModParms);
      CurMod[4] := uProc.pid;
    endif
  endif

  SetObjProperty(who, PROP_TEMP_MOD + type, CurMod);

  case(type)
    MOD_RUN_SPEED:         RecalcVitals(who);
  endcase

endfunction

//-----------------------------------------------------------------------------

function DoTempSkMod(byref Parms)

  if(who.dead)
    return 0;
  endif

  if(Parms.skid > GetMaxSkillId())
    return 0;
  endif

  Parms.+source  := who;
  Parms.+serial  := WhoSer;

  var amt            := CInt(Parms.amt);
  var duration       := CInt(Parms.duration);

  if(!who or !duration or Parms.skid == error)
    return 0;
  endif

  if(!CanSkillMod(who,Parms.skid) or !CanMagicSkillMod(who,Parms.skid))
    return 0;
  endif

  if(Parms.sound)
    PlaySoundEffect(who,Parms.sound);
  endif

  NoLongerBusy(who);

  ModifySkillTemporaryMod(who, Parms.skid, amt);

  var uProc;
  
  if(duration != MODDUR_INFINITE)
    uProc := start_script(":magiceffects:undomodcheck", Parms);
  endif

  SetObjProperty(who, PROP_TEMP_SKMOD + Parms.skid, array(amt, Duration, ReadGameClock(), CInt(uProc.pid)));
  return 1;

endfunction

//-----------------------------------------------------------------------------

function DoTempMagicMod(byref Parms)

  if(CanMod(who,Parms.modtype) and CanMagicMod(who,Parms.modtype) and !who.dead)
    SetMagicModItemSerial(who, Parms.item, Parms.modtype);

    case(Parms.modtype)
      MOD_STR:         SetStrengthMod(who, GetStrengthMod(who) + Parms.mlvl);
      MOD_DEX:         SetDexterityMod(who, GetDexterityMod(who) + Parms.mlvl);
      MOD_INT:         SetIntelligenceMod(who, GetIntelligenceMod(who) + Parms.mlvl);

      MOD_HP_REGEN:
      MOD_MANA_REGEN:
      MOD_STAM_REGEN:
      MOD_LIFE:
      MOD_MANA:
      MOD_STAMINA:     SetTempModAmount(who, Parms.modtype, Parms.mlvl);
                       RecalcVitals(who);

      MOD_FIRERESIST:
      MOD_COLDRESIST:
      MOD_LIGHTRESIST:
      MOD_CTH:
      MOD_PENETRATE:
      MOD_UNDEAD_SLAY:
      MOD_DAEMON_SLAY:
      MOD_ORDER_SLAY:
      MOD_DEFENSE:
      MOD_TREE_SUPPORT:
      MOD_MONSTER_AR:
      MOD_SILVER_AR:
      MOD_FIRE_BLOOD:
      MOD_LOWER_RC:
      MOD_FASTER_CAST:
      MOD_MANA_COST:
      MOD_SPELL_DMG:
      MOD_POISON_DMG:
      MOD_FIRE_DMG:
      MOD_EXPLO_DMG:
      MOD_COLD_DMG:
      MOD_LIGHT_DMG:
      MOD_RETORT:
      MOD_CRIT_HIT:
      MOD_STAM_DMG:
      MOD_DISARM:
      MOD_STUPOR:
      MOD_2H_DMG:
      MOD_CASTSIGHT:    
      MOD_DMG_REDUCE: 
                       SetTempModAmount(who,Parms.modtype,Parms.mlvl);

      MOD_ONHIT_AR:    SetTempModAmount(who, Parms.modtype,Parms.mlvl);
                       who.ar_mod := who.ar_mod + Parms.mlvl;

      MOD_LIGHT:       who.setlightlevel(0, 186400);

      MOD_RUN_SPEED:   SendMountSpeedPacket(who, Parms.mlvl);
                       RecalcVitals(who);
    endcase

    NotifyAddBuff(who, Parms.modtype, Parms.mlvl, 0);

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DoTempMagicSkMod(byref Parms)

  if(CanSkillMod(who, Parms.skid) and CanMagicSkillMod(who, Parms.skid) and !who.dead)
    if(Parms.skid <= GetMaxSkillId())
      SetMagicSkModItemSerial(who, Parms.item, Parms.skid);
      ModifySkillTemporaryMod(who, Parms.skid, Parms.mlvl);
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function UndoTempMod(byref ModParms)

  var type   := ModParms.modtype;

  var PrevMod := GetObjProperty(who, PROP_TEMP_PREVMOD + Type);
  var ActMod  := GetObjProperty(who, PROP_TEMP_MOD + Type);
  var CurMod;
  
  if(ModParms.flags & UMDF_PREVIOUS)
    CurMod := PrevMod;
  else
    CurMod := ActMod;
  endif

  var Amt    := CurMod[1];
  var OldMod := GetModOldAmount(who, Type);

  if(ModParms.flags & UMDF_PREVIOUS)
    OldMod := ActMod[1];
  elseif(PrevMod)
    OldMod := PrevMod[1];
  endif

  if(!who or !type or (CurMod == error and OldMod == error))
    return;
  endif

  if((ModParms.flags & UMDF_PREVIOUS) and ActMod)
    Amt := CurMod[1] + ActMod[1];
  endif

  case(type)
    MOD_STR:               SetStrengthMod(who, GetStrengthMod(who) - Amt);
                           SetObjProperty(who, "#CurseDelay", ReadGameClock() + 10);

    MOD_DEX:               SetDexterityMod(who, GetDexterityMod(who) - Amt);
                           SetObjProperty(who, "#CurseDelay", ReadGameClock() + 10);

    MOD_INT:               SetIntelligenceMod(who, GetIntelligenceMod(who) - Amt);
                           SetObjProperty(who, "#CurseDelay", ReadGameClock() + 10);

    MOD_AR:                who.ar_mod := who.ar_mod - Amt;

    MOD_PARA:              if(amt == 2)
                             who.frozen := 0;
                           else
                             who.SetParalyzed(0);
                           endif

    MOD_INCO:              if(OldMod)
                             who.name := OldMod;
                           endif
                           EraseObjProperty(who, "#TempAge");

    MOD_GRAPHIC:           if(CInt(OldMod))
                             PlaySoundEffect(who, SFX_SPELL_MANA_VAMPIRE );

                             if(who.isa(POLCLASS_NPC))
                               var Mount := FindObjTypeInContainer(who.backpack, UOBJ_MOUNT);

                               if(Mount)
                                 EquipItem(who, Mount);
                               endif
                             endif

                             if(!who.dead)
                               who.graphic := CInt(OldMod);
                             endif
                             PlayStationaryEff( who.x, who.y, who.z, FX_SMOKE, 0xa, 0xa, 0, who.realm);
                           endif

    MOD_MOUNT:             var Mount := FindItemBySerial(OldMod);
                           if(Mount)
                             DestroyItem(Mount);
                           endif

    MOD_RUN_SPEED:         if(who.isa(POLCLASS_NPC))
                             who.run_speed := OldMod;
                           elseif(PrevMod) // zdejmowany jest force modify, wiec zmieniamy na oldMod
                             SendMountSpeedPacket(who, OldMod);
                           else
                             SendMountSpeedPacket(who, 0);
                           endif
    
    MOD_SLOWDOWN:          if(who.isa(POLCLASS_NPC))
                             if(OldMod != error)
                               who.run_speed := OldMod;
                             endif
                           endif

    MOD_COLOR:             if(OldMod != error)
                             who.color := OldMod;
                           endif
    MOD_TPREFIX:           if(OldMod != error)
                             who.title_prefix := OldMod;
                           endif
    MOD_TSUFFIX:           if(OldMod != error)
                             who.title_suffix := OldMod;
                           endif
    MOD_TGUILD:            if(OldMod != error)
                             who.title_guild := OldMod;
                           endif
    MOD_TRACE:             if(OldMod != error)
                             who.title_race := OldMod;
                           endif
    MOD_HGHOSTS:           RevokePrivilege(who,"hearghosts");

    MOD_LIGHT:             who.setlightlevel(31, 1);
                           who.setlightlevel(amt, 1);

    MOD_HAIRS:             if(OldMod)
                             var CurHairs := GetEquipmentByLayer(who,LAYER_HAIR);
                             if(CurHairs)
                               if(CurHairs.objtype == CInt(OldMod[2][1]) and CurHairs.color == CInt(OldMod[2][2]))
                                 if(DestroyItem(CurHairs))
                                   if(CInt(OldMod[1][1]) > 0)
                                     var OldHairs   := CreateItemAtLoc(6104,1182,0,CInt(OldMod[1][1]),1);
                                     OldHairs.color := CInt(OldMod[1][2]);
                                     if(OldHairs)
                                       if(!EquipItem(who,OldHairs))
                                         DestroyItem(OldHairs);
                                       else
                                         OldHairs.movable := 0;
                                       endif
                                     endif
                                   endif
                                 endif
                               endif
                             endif
                           endif

    MOD_HP_REGEN:
    MOD_MANA_REGEN:
    MOD_STAM_REGEN:
    MOD_LIFE:
    MOD_MANA:
    MOD_STAMINA:           DecreaseTempModAmount(who, Type, Amt);
                           RecalcVitals(who);

    MOD_FIRERESIST:
    MOD_COLDRESIST:
    MOD_LIGHTRESIST:
    MOD_CTH:
    MOD_PENETRATE:
    MOD_UNDEAD_SLAY:
    MOD_DAEMON_SLAY:
    MOD_ORDER_SLAY:
    MOD_DEFENSE:
    MOD_LOWER_RC:
    MOD_FIRE_BLOOD:
    MOD_FASTER_CAST:
    MOD_MANA_COST:
    MOD_SPELL_DMG:
    MOD_POISON_DMG:
    MOD_FIRE_DMG:
    MOD_EXPLO_DMG:
    MOD_COLD_DMG:
    MOD_LIGHT_DMG:
    MOD_RETORT:
    MOD_CRIT_HIT:
    MOD_STAM_DMG:
    MOD_DISARM:
    MOD_STUPOR:
    MOD_2H_DMG:
    MOD_RA:
    MOD_TREE_SUPPORT:
    MOD_MONSTER_AR:
    MOD_SILVER_AR:      
    MOD_CASTSIGHT:    
    MOD_DMG_REDUCE:
                           DecreaseTempModAmount(who, Type, Amt);

    MOD_STATUS:            if(OldMod)
                             if(OldMod[2])
                               who.SetMurderer(1);
                             else
                               who.SetMurderer(0);
                             endif

                             if(OldMod[1])
                               SetCriminal(who, 1, "UndoTempMod", "Cofanie Status Moda, ustawianie krima, bo byl nim przed wlaczeniem Moda, lub Amt bylo 3.");
                             else
                               SetCriminal(who, 0);
                             endif
                           endif

    MOD_ONHIT_AR:          var ModAmt := DecreaseTempModAmount(who, Type, Amt);
                           who.ar_mod := who.ar_mod - ModAmt;
  endcase

  PlayUndoModEffect(who, Type);
  SetDefaultDelayForMod(who, Type, 0);

  if(ModParms.Flags & UMDF_PREVIOUS)
    NotifyDelBuff(who, type, amt);
    EraseObjProperty(who, PROP_TEMP_MOD + Type);
    EraseObjProperty(who, PROP_TEMP_PREVMOD + Type);

  elseif(PrevMod) // zamiana starego na biezacy - force_modify skonczyl sie.
    SetObjProperty(who, PROP_TEMP_MOD + Type, PrevMod);
    EraseObjProperty(who, PROP_TEMP_PREVMOD + Type);
    NotifyAddBuff(who, type, PrevMod[1], PrevMod[2] - (ReadGameClock() - PrevMod[3]));

  else
    EraseObjProperty(who, PROP_TMOD_PREV_AMT + Type);
    SetObjProperty(who, PROP_TEMP_MOD + Type, PrevMod);
    NotifyDelBuff(who, type, amt);
    
  endif

  if(ActMod[4])
    var Proc := GetProc(ActMod[4]);

    if(Proc)
      var rEv := struct;
      rEv.+refresh := 1;
      Proc.sendevent(rEv);
    endif
  endif

  case(type)
    MOD_RUN_SPEED:         RecalcVitals(who);
  endcase

  return 1;

endfunction

//-----------------------------------------------------------------------------

function UnMagicMod(who, ItemSer, ModType, MagicLevel)

  if(!CanMagicMod(who,ModType) and GetMagicModItemSerial(who, ModType) == ItemSer)
    EraseObjProperty(who, PROP_MAGIC_MOD + ModType);

    case(ModType)
      MOD_STR:         SetStrengthMod(who, GetStrengthMod(who) - MagicLevel);
      MOD_DEX:         SetDexterityMod(who, GetDexterityMod(who) - MagicLevel);
      MOD_INT:         SetIntelligenceMod(who, GetIntelligenceMod(who) - MagicLevel);
      MOD_HP_REGEN:
      MOD_MANA_REGEN:
      MOD_STAM_REGEN:
      MOD_LIFE:
      MOD_MANA:
      MOD_STAMINA:     EraseTempModAmount(who, ModType);
                       RecalcVitals(who);

      MOD_FIRERESIST:
      MOD_COLDRESIST:
      MOD_LIGHTRESIST:
      MOD_CTH:
      MOD_PENETRATE:
      MOD_UNDEAD_SLAY:
      MOD_DAEMON_SLAY:
      MOD_ORDER_SLAY:
      MOD_DEFENSE:
      MOD_TREE_SUPPORT:
      MOD_MONSTER_AR:
      MOD_SILVER_AR:
      MOD_FIRE_BLOOD:
      MOD_ONHIT_AR:
      MOD_LOWER_RC:
      MOD_FASTER_CAST:
      MOD_MANA_COST:
      MOD_SPELL_DMG:
      MOD_POISON_DMG:
      MOD_FIRE_DMG:
      MOD_EXPLO_DMG:
      MOD_COLD_DMG:
      MOD_LIGHT_DMG:
      MOD_RETORT:
      MOD_CRIT_HIT:
      MOD_STAM_DMG:
      MOD_DISARM:
      MOD_STUPOR:
      MOD_2H_DMG:
      MOD_CASTSIGHT:    
      MOD_DMG_REDUCE: 
                     EraseTempModAmount(who, ModType);

      MOD_RUN_SPEED: SendMountSpeedPacket(who, 0);
                       RecalcVitals(who);

      MOD_LIGHT:       who.setlightlevel(0,1);
    endcase

    NotifyDelBuff(who, ModType, MagicLevel);

  endif

endfunction

//-----------------------------------------------------------------------------

function UndoSkillTempMod(byref Parms)

  var type      := Parms.skid;
  var CurMod    := GetObjProperty(who, PROP_TEMP_SKMOD + Type);
  var amt       := CurMod[1];

  if(!who or !CurMod)
    return;
  endif

  SetScriptController(who);

  if(Parms.sound)
    PlaySoundEffect(who, Parms.sound);
  endif

  ModifySkillTemporaryMod(who, type, 0 - amt);

  NotifyDelBuff(who, type, amt);

  if(CurMod[4])
    var Proc := GetProc(CurMod[4]);

    if(Proc)
      var rEv := struct;
      rEv.+refresh := 1;
      Proc.sendevent(rEv);
    endif
  endif

  EraseObjProperty(who, PROP_TEMP_SKMOD + Type);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PlayDoModEffect(who, type)

  case(type)
    MOD_DEFENSE:
    MOD_TREE_SUPPORT:
    MOD_MONSTER_AR:
    MOD_SILVER_AR:      PlaySoundEffect(who, SFX_ARCANE_POWER_UP);

    MOD_BLOCK_SELFHEAL:
    MOD_BLOCK_FIGHT:
    MOD_BLOCK_CAST:     PlaySoundEffect(who, SFX_SPELL_CURSE);
                        PlayPcEffect(who, FX_SPARK_EFFECT, 1219, 0x10, 0xA, 3);

    MOD_CHARM_ANIMAL:   PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
                        PlayPcEffect(who, FX_HEAL_EFFECT, 1424, 0x10, 0x1F, 2);
    MOD_RED_SPELL:      PlaySoundEffect(who, SFX_GIFT_OF_RENEWAL);
                        PlayPcEffect(who, FX_HEAL_EFFECT, 91, 0x10, 0x1F, 3);
    MOD_DISMOUNT_RES:   PlaySoundEffect(who, SFX_SPELL_MARK);
                        PlayPcEffect(who, FX_HEAL_EFFECT, 1152, 0x10, 0x1F, 5);
                        PlaySoundEffect(who, SFX_MEDIT_END);

    MOD_GRAPHIC:        PlaySoundEffect( who, SFX_SPELL_MANA_VAMPIRE );
                        PlayStationaryEff( who.x, who.y, who.z, FX_SMOKE, 0xa, 0xa, 0, who.realm);

    MOD_HGHOSTS:        PlaySoundEffect(who, SFX_SKILL_SPIRITSPEAK);

    MOD_SLOWDOWN:       PlayPcStEffect( who.x, who.y, who.z, who.realm, FX_MARK_EFFECT, 40, 0x10, 0x10, 3 );
                        PlayPcStEffect( who.x, who.y, who.z, who.realm, FX_HEAL_EFFECT, 40, 0x10, 0x10, 3 );

  endcase

endfunction

//-----------------------------------------------------------------------------

function PlayUndoModEffect(who, type)

  case(type)
    MOD_RA:                PlaySoundEffect(who, SFX_ARCANE_POWER_DOWN);
                           PlayPcEffect(who, FX_GLOW_SPIKE, 997, 0x10, 0xA, 3);

    MOD_BLOCK_SELFHEAL:
    MOD_BLOCK_FIGHT:
    MOD_BLOCK_CAST:        PlaySoundEffect(who, SFX_SPELL_CURE);
                           PlayPcEffect(who, FX_SPARK_EFFECT, 1321, 0x10, 0xA, 2);

    MOD_TREE_SUPPORT:      PlaySoundEffect(who, SFX_ARCANE_POWER_DOWN);
                           PlayPcEffect(who, FX_GLOW_SPIKE, 1421, 0x10, 0xA, 4);

    MOD_MONSTER_AR:        PlaySoundEffect(who, SFX_ARCANE_POWER_DOWN);
                           PlayPcEffect(who, FX_GLOW_SPIKE, 1153, 0x10, 0xA, 3);

    MOD_SILVER_AR:         PlaySoundEffect(who, SFX_ARCANE_POWER_DOWN);
                           PlayPcEffect(who, FX_GLOW_SPIKE, 1301, 0x10, 0xA, 3);

    MOD_ONHIT_AR:          PlaySoundEffect(who, SFX_ARCANE_POWER_DOWN);
                           PlayPcEffect(who, FX_GLOW_SPIKE, 36, 0x10, 0xA, 8);

    MOD_CHARM_ANIMAL:      PlayPcEffect(who, FX_SMOKE, 1424, 0x10, 0xE, 2);
                           PlaySoundEffect(who, SFX_SPELL_POLYMORPH);

    MOD_RED_SPELL:         PlayPcEffect(who, FX_SMOKE, 91, 0x10, 0xE, 3);
                           PlaySoundEffect(who, SFX_GIFT_OF_RENEWAL);

    MOD_DISMOUNT_RES:      PlayPcEffect(who, FX_SMOKE, 1152, 0x10, 0xE, 5);
                           PlaySoundEffect(who, SFX_SPELL_MARK);
                           PlaySoundEffect(who, SFX_MEDIT_END);
                           
    MOD_SLOWDOWN:          PlayPcStEffect( who.x, who.y, who.z, who.realm, FX_MARK_EFFECT, 30, 0x10, 0x10, 3 );
                           PlayPcStEffect( who.x, who.y, who.z, who.realm, FX_HEAL_EFFECT, 30, 0x10, 0x10, 3 );
  endcase

endfunction

//-----------------------------------------------------------------------------

function SetDefaultDelayForMod(who, Type, Amt)

  case(Type)

    MOD_PARA:          SetModDelay(who, Type, Amt + 5 + RandomInt(2));
    MOD_SLOWDOWN:      SetModDelay(who, Type, 280 + RandomInt(20));

  endcase

endfunction

//-----------------------------------------------------------------------------
