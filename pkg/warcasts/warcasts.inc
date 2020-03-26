//--------------------------------------------------------------------------------

include "include/clock";
include "include/effects";
include "include/logerror";
include "include/regs";
include "include/std";
include "include/move";
include "include/checks/locchecks";
include "include/checks/objchecks";

include ":tourney:tourney";
include ":magic:magicatt";
include ":magic:common";
include ":wearwolf:wearwolf";
include ":vampire:vampire";
include ":warcasts:wcgain";

//--------------------------------------------------------------------------------

const WCT_WEARWOLF           := "WearWolf";
const WCT_VAMPIRE            := "Vampire";
const WCT_PALADIN            := "Paladin";
const WCT_DARKKNIGHT         := "DarkKnight";
const WCT_BARBARIAN          := "Barbarian";
const WCT_KENSAI             := "Kensai";
const WCT_HUNTER             := "Hunter";

//-----------------------------------------------------------------------------

const WCF_CONS_STAMINA       := 0x01;
const WCF_CONS_MANA          := 0x02;

//--------------------------------------------------------------------------------

function WarCast(byref Parms, wcId, wcFlags, TrgType := TRGT_NONE, TrgNoto := TRGO_NONE)

  if(Malloc(Parms.Caster))
	return;
  endif

  var WasOffen := IsCastingOffensiveNear(Parms.Caster);
  var Ret := __WarCast(Parms, wcId, wcFlags, TrgType, TrgNoto);

  if(!WasOffen and !Ret and (TrgNoto & TRGO_HARMFUL))
    EraseOffensiveCast(Parms.Caster);
  endif

endfunction

//--------------------------------------------------------------------------------

function __WarCast(byref Parms, wcId, wcFlags, TrgType, TrgNoto)

  SetLogRunCycles(LST_SPELL);

  var WarCfg  := GetWarCastCfg();
  var WctCfg  := GetWarCastTypesCfg();
  var Caster  := Parms.Caster;
  var CastOn  := Parms.CastOn;

  if(CanMod(Caster, MOD_BLOCK_CAST) == 0)
    SendBad(Caster,"Nie mozesz rzucic teraz zaklecia. Twe zdolnosci cos blokuje!");
    return 0;
  endif

  if(!Parms.Caster)
    return 0;
  endif

  if(!WarCfg or !WctCfg)
    return 0;
  endif

  if(IsPlayer(Caster))
    if(IsBusy(Caster, NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return 0;
    endif
  endif

  var Cast := WarCfg[wcId];
  
  if(!Cast)
    LogError("WarCast: Nie mozna bylo znalezc WcID: ["+wcId+"] w cfg ["+WARCASTS_CFG+"]");
    return 0;
  endif
 
  if(ChNocZone(Caster))
    SendSysMessage(Caster, "Twe moce zanikaja w tym miejscu...", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(TrgNoto & TRGO_HARMFUL)
    if(IsPeacefulZone(Caster.x, Caster.y, Caster.realm))
      Fizzle(Caster,"Magia w tym miejscu nie dziala...");
      return 0;
    endif

    SetOffensiveCast(Caster);
  endif

  if(!CheckWarCastType(Caster, Cast.Type))
    return 0;
  endif

  if(Parms.wcItem)
    if(!StandardItemCheck(Caster, Parms.wcItem, NO_COMBAT_CHECK))
      return 0;
    endif
  endif

  if(!CheckWarCastProps(Cast, WctCfg, wcId))
    return 0;
  endif

  var Reagents  := GetConfigStringArray(Cast, "reagent");
  var Diff      := CInt(Cast.Diff);
  var MainSkId  := CInt(WctCfg[Cast.Type].MainSkId);
  var MindSkId  := CInt(WctCfg[Cast.Type].MindSkId);
  var Delay     := CInt(WctCfg[Cast.Type].CastDelay);
  var MaxSteps  := CInt(WctCfg[Cast.Type].CastMaxSteps);
  var Mana      := CInt(Cast.Mana);
  var Stamina   := CInt(Cast.Stamina);
  var MainBSk   := GetEffectiveSkillBaseValue(Caster, MainSkId);
  var MindBSk   := GetEffectiveSkillBaseValue(Caster, MindSkId);
  var TimeDelay := CInt(Cast.TimeDelay);
  var EqpReq    := Cast.EqpReq;
  var EqpItem   := 0;
  var EqpResv   := 0;
  var DayCast   := SplitWords(Cast.DayCast);
 

  if(Caster.paralyzed or Caster.frozen)
    WcFizzle(Caster, "Jestes sparalizowan"+ggm(Caster,6)+"!", Cast.Type);
    return 0;
  endif

  if(Caster.hidden)
    UnhidePlayer(Caster);
  endif
  
  if(EqpReq)
    if(!CInt(EqpReq))
      EqpReq := GetObjTypeByName(EqpReq);
    else
      EqpReq := CInt(EqpReq);
    endif

    foreach Item in ListEquippedItems(Caster)
      if(Item.objtype == EqpReq)
        if(ReserveItem(Item))
          EqpItem := Item;
          break;
        else
          EqpResv := Item;
        endif
      endif
    endforeach

    if(!EqpItem)
      if(EqpResv)
        WcFizzle(Caster, "Przedmiot "+EqpResv.desc+" jest teraz uzywany.", Cast.Type);
        return 0;
      else
        WcFizzle(Caster, "Musisz miec zalozony przedmiot "+GetDescForItem(EqpReq)+" by moc rzucic to zaklecie.", Cast.Type);
        return 0;
      endif
    endif
  endif

  if(!(Parms.Flags & SPF_NOMANA))
    if(Mana and !(wcFlags & WCF_CONS_STAMINA))
      if(GetMana(Caster) < Mana)
        WcFizzle(Caster, "Brakuje Ci many.", Cast.Type);
        return 0;
      endif
    endif
  
    if(Stamina and !(wcFlags & WCF_CONS_MANA))
      if(GetStamina(Caster) < Stamina)
        WcFizzle(Caster, "Brakuje Ci staminy.", Cast.Type);
        return 0;
      endif
    endif
  endif

  if(!(Parms.Flags & SPF_NOREGS))
    if(Reagents)
      if(!EatReagents(Caster, Reagents))
        SendReagentsList(Caster, Reagents);
        return 0;
      endif
    endif
  endif

  if(!(Parms.Flags & SPF_NOMANA))
    if(Mana and !(wcFlags & WCF_CONS_STAMINA))
      if(!ConsumeManaAmount(Caster, Mana))
        WcFizzle(Caster, "Brakuje Ci many.", Cast.Type);
        return 0;
      endif
    endif
  
    if(Stamina and !(wcFlags & WCF_CONS_MANA))
      if(!ConsumeStamina(Caster, Stamina))
        WcFizzle(Caster, "Brakuje Ci staminy.", Cast.Type);
        return 0;
      endif
    endif
  endif

  Delay    := ToMin(CInt(Delay - (GetAttrExponent(MainBSk) / 2)), 500);
  MaxSteps := ToMin(CInt(MaxSteps - 2 + (MainBSk / 500)), 1);

  if(!WcDoAnim(Caster, Cast, Delay, MaxSteps))
    return 0;
  endif
  
  if(Caster.paralyzed or Caster.frozen)
    WcFizzle(Caster, "Jestes sparalizowan"+ggm(Caster,6)+"!", Cast.Type);
    return 0;
  endif

  if(!(Parms.Flags & SPF_NOSKILL))
    if(CInt(MindBSk/10) <= (Diff-10))
      WcFizzle(Caster, "Twe umiejetnosci sa zbyt niskie.", Cast.Type);
      return 0;
    elseif(!CheckMoreSkill(Caster, MainSkId, Diff, 0))
      WcFizzle(Caster, "Nie udalo sie.", Cast.Type);
      return 0;
    endif
  endif

  if(!(Parms.Flags & SPF_NODELAY))
    Delay := CInt(Cast.Delay);
    Delay := ToMin(CInt(Delay - ((Delay * 0.5) * (MainBSk * 0.001))), 100);

    sleepms(Delay);
  endif

  if(TimeDelay)
    if(CInt(GetObjProperty(Caster, "#WcDl_"+wcId)) > ReadGameClock())
      WcFizzle(Caster, "Nie jestes jeszcze gotow by rzucic to zaklecie ponownie.", Cast.Type);
      return 0;
    endif
  endif

  if(!(Parms.Flags & SPF_NOSKLOSS))
    foreach AttrId in GetConfigStringArray(WctCfg[Cast.Type], "ReduceAttribute")
      ReduceSkill(Caster, GetSkillIdByAttributeId(AttrId), 0.99);
    endforeach
  endif

  if(!caster.cmdlevel)
    if(Cast.Type in {"Vampire","WearWolf"} and !CanCastDebug(Caster, Cast.Type))
      var hour := GetHour();
      if(IsWearWolf(Caster) and hour < WW_START_HOUR and hour >= WW_END_HOUR)
        if(DayCast[1] != "1")
          WcFizzle(Caster, "Nie mozesz uzyc tego w dzien...", Cast.Type);
          return 0;
        else
          if(GetEffectiveSkill(Caster,SKILLID_WEARWOLF) < CInt(DayCast[2]))
            WcFizzle(Caster, "Masz zbyt malo umiejetnosci by uzyc tego w dzien...", Cast.Type);
            return 0;
          endif
        endif
      elseif(IsVampire(Caster) and hour < VAMPIRE_HOUR and hour >= VAMPIRE_END_HOUR and !IsDungeon(Caster.x, Caster.y, Caster.realm) and GetEffectiveSkill(Caster, SKILLID_VAMPIRE) < 110)
        if(DayCast[1] != "1")
          WcFizzle(Caster, "Nie mozesz uzyc tego w dzien na powierzchni...", Cast.Type);
          return 0;
        else
          if(GetEffectiveSkill(Caster,SKILLID_VAMPIRE) < CInt(DayCast[2]))
            WcFizzle(Caster, "Masz zbyt malo umiejetnosci by uzyc tego w dzien na powierzchni...", Cast.Type);
            return 0;
          endif
        endif
        
      endif
    endif
  endif

  var tgParam := struct;

  tgParam.+Caster      := Caster;
  tgParam.+CasterX     := Caster.x;
  tgParam.+CasterY     := Caster.y;
  tgParam.+CasterRealm := Caster.realm;
  tgParam.+CasterSteps := CInt(MainBSk / 250) + CInt(MindBSk / 250);
  tgParam.+CastType    := Cast.Type;
  tgParam.+StartTime   := ReadGameClock();
  
  if(!CastOn)
    var NotoType;

    case(TrgNoto)
      TRGO_HELPFUL:  NotoType := TGTOPT_HELPFUL + TGTOPT_CHECK_LOS;
      TRGO_HARMFUL:  NotoType := TGTOPT_HARMFUL + TGTOPT_CHECK_LOS;
      TRGO_NOLOS:    NotoType := TGTOPT_NOCHECK_LOS;
      default:       NotoType := TGTOPT_CHECK_LOS;
    endcase
    
    var maxDist := CInt(MainBSk / 100) + CInt(MindBSk / 150);

    case(TrgType)
      TRGT_LOC:     CastOn := PcTargetLoc(Caster, tgParam);
                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardLocCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS  | NEW_DIST_CHECK, maxDist))
                      return 0;
                    endif

      TRGT_MOB:     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                    
                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardMobCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS | NEW_DIST_CHECK, maxDist))
                      return 0;
                    endif

      TRGT_NPC:     CastOn := ObjTargetEx(Caster, NotoType, tgParam);

                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardMobCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS | NEW_DIST_CHECK, maxDist))
                      return 0;
                    elseif(!CastOn.isa(POLCLASS_NPC))
                       WcFizzle(Caster, "To nie zwierze.", Cast.Type);
                      return 0;
                    endif

      TRGT_PLAYER:  CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                    
                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardMobCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS | NEW_DIST_CHECK, maxDist))
                      return 0;
                    elseif(!IsPlayer(CastOn))
                      WcFizzle(Caster, "To nie osoba.", Cast.Type);
                      return 0;
                    endif

      TRGT_ITEM:    CastOn := ObjTargetEx(Caster, NotoType, tgParam);

                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardItemCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS | NEW_DIST_CHECK, maxDist))
                      return 0;
                    endif

      TRGT_CORPSE:  CastOn := ObjTargetEx(Caster, NotoType, tgParam);

                    if(CastOn == error)
                      if(CastOn.errortext)
                        WcFizzle(Caster, CastOn.errortext, Cast.Type);
                      endif
                      return 0;
                    elseif(!StandardItemCheck(Caster, CastOn, NO_COMBAT_CHECK | NO_CHECK_SPECLOS | NEW_DIST_CHECK, maxDist))
                      return 0;
                    elseif(CastOn.objtype != UOBJ_CORPSE)
                      WcFizzle(Caster, "To nie zwloki.", Cast.Type);
                      return 0;
                    endif
    endcase
  endif

  if(coordist(tgParam.CasterX, tgParam.CasterY, Caster.x, Caster.y) > tgParam.CasterSteps)
    WcFizzle(Caster, "Stracil"+ggm(Caster,3)+" koncentracje.", Cast.Type);
    return 0;
  elseif(tgParam.Tele and CastOn.isa(POLCLASS_NPC))
    WcFizzle(Caster, "Stracil"+ggm(Caster,3)+" koncentracje.", Cast.Type);
    return 0;
  endif

  if(Caster.paralyzed or Caster.frozen)
    WcFizzle(Caster, "Jestes sparalizowan"+ggm(Caster,6)+"!", Cast.Type);
    return 0;
  endif

  Detach();
  NoLongerBusy(Caster);

  if(CastOn)
    CheckUseSthWatch(Caster, CastOn);
  endif

  if(Parms.wcItem)
    ReleaseItem(Parms.wcItem);
  endif

  if(EqpItem)
    ReleaseItem(EqpItem);
  endif

  if(Parms.Flags & SPF_RETCASTON)
    if(TimeDelay)
      SetObjProperty(Caster, "#WcDl_"+wcId, ReadGameClock() + TimeDelay);
    endif
    return CastOn;
  else
    if(CastAction(Caster, CastOn))
      if(TimeDelay)
        SetObjProperty(Caster, "#WcDl_"+wcId, ReadGameClock() + TimeDelay);
      endif
    endif
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function PcTargetWaitFunc(byref Param)

  if(!Param.Caster)
    return 0;
  endif

  if(Param.Caster.dead)
    WcFizzle(Param.Caster, "Jestes martw"+ggm(Param.Caster,6)+"!", Param.CastType);
    return 0;
  endif

  if(Param.Caster.paralyzed or Param.Caster.frozen)
    WcFizzle(Param.Caster, "Jestes sparalizowan"+ggm(Param.Caster,6)+"!", Param.CastType);
    return 0;
  endif
  
  if(CoorDist(Param.CasterX, Param.CasterY, Param.Caster.x, Param.Caster.y) > Param.CasterSteps or Param.CasterRealm != Param.Caster.realm)
    var lMove := GetLastMove(Param.Caster);
    if(lMove[1] == MOVETYPE_TELE and lMove[3] and lMove[4])
      if(CoorDist(lMove[3][1], lMove[3][2], Param.CasterX, Param.CasterY) <= Param.CasterSteps and Param.CasterRealm == lMove[3][4])
        Param.CasterX     := lMove[4][1];
        Param.CasterY     := lMove[4][2];
        Param.CasterRealm := lMove[4][4];
        Param.+Tele := 1;
        return 1;
      endif
    endif
  
    WcFizzle(Param.Caster, "Stracil"+ggm(Param.Caster,3)+" koncentracje.", Param.CastType);
    return 0;
  endif

  if(Param.StartTime + 9 < ReadGameClock())
    if(!ConsumeManaAmount(Param.Caster, 4))
      WcFizzle(Param.Caster,"Stracil"+ggm(Param.Caster,3)+" koncentracje.", Param.CastType);
      return 0;
    endif
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetWcType(Id)

  var WarCfg  := GetWarCastCfg();

  return WarCfg[Id].Type;

endfunction

//--------------------------------------------------------------------------------

function CheckWarCastType(Caster, CastType)

  if(Caster.cmdlevel)
    return 1;
  endif

  case(CastType)
    WCT_WEARWOLF:  if(!IsWearWolf(Caster))
                     SendSysMessage(Caster,"W twoich zylach nie plynie krew wilkolaka!", FONT_NORMAL, COLOR_RED);
                     return 0;
                   endif

    WCT_VAMPIRE:   if(!IsVampire(Caster))
                     SendSysMessage(Caster,"W twoich zylach nie plynie krew wampira!", FONT_NORMAL, COLOR_RED);
                     return 0;
                   endif
                   

    WCT_PALADIN:   if(IsRealMurderer(Caster))
                     if(!IsInTourneyIsland(Caster.x, Caster.y) and !IsInTourneyLoc(TRLOC_STABLES, Caster.x, Caster.y) or GetTrTeam(Caster) != TRLOC_RED_ROOM)
                       SendSysMessage(Caster,"Zhanbil"+ggm(Caster,3)+" sie i zostal"+ggm(Caster,3)+" morderca, twym obowiazkiem jest pokuta!", FONT_NORMAL, COLOR_RED);
                       return 0;
                     endif
                   endif
  endcase

  return 1;

endfunction

//--------------------------------------------------------------------------------

function WcDoAnim(Caster, byref Cast, sDelay, MaxSteps)

  var WctCfg   := GetWarCastTypesCfg();
  var WctElem  := WctCfg[Cast.Type];
  var Times    := CInt(Cast.Times);
  
  var AnimId   := CInt(WctElem.AnimId);
  var EffId    := CInt(WctElem.EffId);
  var EffClr   := CInt(WctElem.EffClr);
  var EffRnd   := CInt(WctElem.EffRnd);
  var Sound    := CInt(WctElem.Sound);
  var i;
  
  var x        := Caster.x;
  var y        := Caster.y;
  var Realm    := Caster.realm;
  var tx       := x;
  var ty       := y;
  var aDelay   := 0;

  if(AnimId)
    PerformAction(Caster, AnimId);
  endif

  if(Sound)
    PlaySoundEffect(Caster, Sound);
  endif

  for(i:=1;i<=Times;i:=i+1)

    if(!Caster or !IsLoggedIn(Caster))
      return 0;
    endif

    if(Caster.dead)
      WcFizzle(Caster, "Jestes martw"+ggm(Caster,6)+"!", Cast.Type);
      return 0;
    endif

    if(Caster.paralyzed or Caster.frozen)
      WcFizzle(Caster, "Jestes sparalizowan"+ggm(Caster,6)+"!", Cast.Type);
      return 0;
    endif

    if(EffId)
      PlayPcEffect(Caster, EffId, EffClr, 0x1, 0x1f, EffRnd);
    endif

    sleepms(sDelay);
    
    if(!CInt(Cast.MoveCast))
      if(coordist(Caster.x, Caster.y, x, y) > MaxSteps or Caster.realm != Realm)
        WcFizzle(Caster,"Oddalil"+ggm(Caster, 3)+" sie.", Cast.Type);
        return 0;
      endif
    endif
    
    aDelay := coordist(Caster.x, Caster.y, tx, ty) * 250;
    tx     := Caster.x;
    ty     := Caster.y;

    sleepms(aDelay);
  endfor

  return 1;

endfunction

//--------------------------------------------------------------------------------

function WcFizzleId(Caster, Msg, wcId)

  var WarCfg   := GetWarCastCfg();

  if(WarCfg[wcId])
    WcFizzle(Caster, Msg, WarCfg[wcId].Type);
  endif

endfunction

//--------------------------------------------------------------------------------

function WcFizzle(Caster, Msg, CastType)

  var WctCfg   := GetWarCastTypesCfg();
  var WctElem  := WctCfg[CastType];

  if(Msg and !Caster.isa(POLCLASS_NPC))
    SendSysMessage(Caster, Msg, FONT_NORMAL, COLOR_RED);
  endif

  if(!WctElem)
    return;
  endif

  if(WctElem.FzEffId)
    PlayPcEffect(Caster, CInt(WctElem.FzEffId), CInt(WctElem.FzEffClr), 0xF, 0xF, CInt(WctElem.FzEffRnd));
  endif

  if(WctElem.FzSound)
    PlaySoundEffect(Caster, CInt(WctElem.FzSound));
  endif

endfunction

//--------------------------------------------------------------------------------

function CheckWarCastProps(byref Cast, byref WctCfg, Id)

  if(!Cast.Diff or !Cast.Points or !Cast.Times or Cast.Delay == error or !Cast.Type)
    LogError("CheckWarCastProps: WcID ["+Id+"]: Bledne wlasciwosci");
    return 0;
  endif

  if(!Cast.Stamina and !Cast.Mana)
    LogError("CheckWarCastProps: WcID ["+Id+"]: nie zawiera Stam/Mana");
    return 0;
  endif

  if(!WctCfg[Cast.Type])
    LogError("CheckWarCastProps: WcID ["+Id+"]: Nieistniejacy typ ["+Cast.Type+"]");
    return 0;
  endif

  if((!WctCfg[Cast.Type].AnimId and !WctCfg[Cast.Type].EffId) or !WctCfg[Cast.Type].MainSkId or !WctCfg[Cast.Type].MindSkId)
    LogError("CheckWarCastProps: WcID ["+Id+"]: Typ ["+Cast.Type+"] posiada bledne wlasciwosci");
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetWarCastCfg()

  var WarCfg  := ReadConfigFile(WARCASTS_CFG);

  if(!WarCfg)
    LogError("GetWarCastCfg: Nie mozna bylo znalezc cfg ["+WARCASTS_CFG+"] !");
  endif

  return WarCfg;

endfunction

//--------------------------------------------------------------------------------

function GetWarCastTypesCfg()

  var WctCfg  := ReadConfigFile(WCTYPES_CFG);

  if(!WctCfg)
    LogError("GetWarCastTypesCfg: Nie mozna bylo znalezc cfg ["+WCTYPES_CFG+"] !");
  endif

  return WctCfg;

endfunction

//--------------------------------------------------------------------------------

function StartWarCast(who, Id, Item := 0, CastOn := 0, Flags := 0)

  if(IsPlayer(who))
    if(IsBusy(who, NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return 0;
    endif

    NoLongerBusy(who);
  endif

  var Parms   := array;
  var WarCfg  := GetWarCastCfg();
  var WctCfg  := GetWarCastTypesCfg();

  if(!WarCfg[Id].Script)
    LogError("StartWarCast: Nie znaleziono skryptu WcID: ["+Id+"]: "+WarCfg[Id].Script);
    return 0;
  endif

  if(CInt(WctCfg[WarCfg[Id].Type].NoSkill))
    Flags := Flags | SPF_NOSKILL;
  endif
  
  if(CInt(WctCfg[WarCfg[Id].Type].NoGain))
    Flags := Flags | SPF_NOGAIN;
  endif

  Parms.+Caster  := who;
  Parms.+CastOn  := CastOn;
  Parms.+Flags   := Flags;
  Parms.+wcItem  := Item;

  var Res := start_script(WarCfg[Id].Script, Parms);

  if(!Res)
    LogError("StartWarCast: WcID: ["+Id+"]: "+WarCfg[Id].Script+" Err: "+Res);
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function CanCastWarCast(who, wcId)

  var WarCfg := GetWarCastCfg();

  case(WarCfg[wcId].Type)
    WCT_KENSAI:
    WCT_BARBARIAN:
    WCT_WEARWOLF:  
    WCT_HUNTER:
    WCT_VAMPIRE:    return FindItemInContainer(who.backpack, CInt(WarCfg[wcId].ItemObj));

    WCT_PALADIN:    if(who.murderer)
                      return 0;
                    endif
    
                    var Book := FindItemInContainer(who.backpack, UOBJ_PD_WCBOOK);
                    if(Book)
                      return IsInWarCastBook(Book, wcId);
                    endif


    WCT_DARKKNIGHT: var Book := FindItemInContainer(who.backpack, UOBJ_DK_WCBOOK);
                    if(Book)
                      return IsInWarCastBook(Book, wcId);
                    endif
  endcase

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CalcWarCastDamage(who, wcId, CastOn := 0)

  var WarCfg := GetWarCastCfg();
  var Dmg    := 1;

  if(WarCfg[wcId])
    var WctCfg := GetWarCastTypesCfg();
    var Diff   := CInt(WarCfg[wcId].Diff);
    var MainSk, MindSk;
    
    MainSk := GetEffectiveSkill(who, WctCfg[WarCfg[wcId].Type].MainSkId);
    MindSk := GetEffectiveSkill(who, WctCfg[WarCfg[wcId].Type].MindSkId);

    Dmg := CInt(((Diff/10)*((MainSk) + (MindSk/2)))*4) + RandomInt(Diff*4);
    Dmg := CInt(Dmg / 115);

//    Dmg := MainSk + MindSk + ( ( GetDexterity(who) + GetIntelligence(who) ) / 6 );
//    Dmg := CInt(Dmg * (Diff * 0.01));

    if(IsPlayer(who))
      Dmg := (Dmg * 0.8) + CInt((Dmg/2) * (CInt((GetStrength(who)*2) * (Diff * 0.009)) * 0.016));
    endif

    if(Dmg < 1)
      Dmg := 1;
    elseif(Dmg > Diff + 5)
      Dmg := Diff + 5;
    endif

    if(CastOn and CastOn.isa(POLCLASS_NPC))
      Dmg := Dmg * 2;
    endif

    ModifyGainByNutr(who, Dmg, ATTRIBUTEID_INTELLIGENCE);
  endif

  return Dmg;

endfunction

//--------------------------------------------------------------------------------

function GetWarCastDuration(Caster, Type)

  var WctCfg := GetWarCastTypesCfg();

  if(!WctCfg[Type])
    return 0;
  endif
  
  var MainSk := GetEffectiveSkillBaseValue(Caster, WctCfg[Type].MainSkId);
  var MindSk := GetEffectiveSkillBaseValue(Caster, WctCfg[Type].MindSkId);

  return CInt((MindSk + MainSk) / 3.333); // 0-600 sec, 0-10 min

endfunction

//--------------------------------------------------------------------------------

function GetSortedWcIDsByDiff(byref IdsArr, IdSet, IdEnd)

  var WarCfg := ReadConfigFile(WARCASTS_CFG);
  var Ids    := dictionary;
  var i;

  for(i:=IdSet;i<=IdEnd;i:=i+1)
    if(WarCfg[i])
      Ids[ZeroFill(WarCfg[i].Diff, 3)+"_"+i] := i;
    endif
  endfor

  IdsArr := array;

  foreach Id in Ids
    IdsArr.append(Id);
  endforeach

endfunction

//--------------------------------------------------------------------------------

function CanCastDebug(who, Type)

  if(IsOnDMRTest())
    if(Type == "WearWolf" and GetObjProperty(who, "WWDebug"))
      return 1;
    elseif(Type == "Vampire" and GetObjProperty(who, "VWDebug"))
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsDungeonWarCasted(wcId)

  var WarCfg  := GetWarCastCfg();
  var Cast := WarCfg[wcId];

  if(Cast.Type != "Vampire")
    return 0;
  endif

  if(!Cast.DayCast)
    return 1;
  endif

  var DayCast := SplitWords(Cast.DayCast);

  if(DayCast[1] != "1")
    return 1;
  endif

  return 0;

endfunction 

//--------------------------------------------------------------------------------

