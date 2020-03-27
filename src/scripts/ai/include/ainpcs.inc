//-----------------------------------------------------------------------------

include "include/std";
include "include/events";
include "include/items";
include "include/checks/stdchecks";
include "include/checks/locchecks";
include "include/move";
include "include/coords";
include "include/attack";
include "include/bits";
include "include/reputation/criminal";
include "ai/include/walking";
include "ai/include/specs";
include "ai/include/anchor";
include "include/player/young";

include "include/mobile/npcs/specs";
include "include/mobile/npcs/attack";

include ":doors:doors";

//-----------------------------------------------------------------------------
// AI Flags
//-----------------------------------------------------------------------------

const AIF_IMMOBILE           := 0x00000001;

const AIF_TMP_LOS            := 0x00010000;
const AIF_TMP_SPECLOS        := 0x00020000;

const IDLE_REGEN_RATE        := 27000;
const MAX_WANDERS_UNTIL_SLEEP := 60;

//-----------------------------------------------------------------------------

var ev      := 0;
var WalkProps;

//-----------------------------------------------------------------------------

function check_frozen()

  if(IsSetFlag(me, NPCF_FROZEN))
    me.frozen := 1;
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function check_alignment()
  
  case(GetNpcConfig(me).alignment)
   "good":       me.SetMurderer(0);
                 SetCriminal(me, 0);
   "evil":       me.SetMurderer(1);
                 SetCriminal(me, 1, "check_alignment", "Krim + Murd z powodu alignment'u Npca = evil.");
   default:      me.SetMurderer(0);
                 SetCriminal(me, 1, "check_alignment", "Krim z powodu alignment'u Npca = "+GetNpcConfig(me).alignment+".");
  endcase
  return 1;

endfunction

//-----------------------------------------------------------------------------

const UOBJ_TREEFORM          := 0x6740;
const UOBJ_TREEFORM_LEAVES   := 0x6741;

//-----------------------------------------------------------------------------

function UnTreeForm()

  var Tree   := FindItemBySerial(GetObjProperty(me, "TfTreeSer"));
  var Leaves := FindItemBySerial(GetObjProperty(me, "TfLeavesSer"));

  if(Tree)
    DestroyItem(Tree);
    EraseObjProperty(me, "TfTreeSer");
  endif

  if(Leaves)
    DestroyItem(Leaves);
    EraseObjProperty(me, "TfLeavesSer");
  endif

  if(me.concealed)
    me.concealed := 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function TreeFormChange()

  var cfgElem := GetNpcConfig(me);

  if(!cfgElem.TreeForm or me.concealed)
    return;
  endif

  case(cfgElem.TreeFormLand)
    "green":  if(!IsOnGreen(me.x, me.y, me.realm))
                return;
              endif
    "dirt":   if(!IsOnDirt(me.x, me.y, me.realm))
                return;
              endif
    default:  LogError("TreeFormChange: ["+me.npctemplate+"] ma TreeForm bez odpowiedniego TreeFormLand ["+cfgElem.TreeFormLand+"]");
  endcase

  var TreeForms := SplitWords(cfgElem.TreeForm);
  var LeavesObj := cfgElem.TreeFormLeaves;
  var TreeColor := CInt(cfgElem.TreeFormColor);

  DisableMainEvents();
  EnableEvents(EVID_ENTEREDAREA, CInt(cfgElem.TreeFormRange));

  var Tree   := FindItemBySerial(GetObjProperty(me, "TfTreeSer"));
  var Leaves := 0;
  
  if(LeavesObj != error)
    Leaves := FindItemBySerial(GetObjProperty(me, "TfLeavesSer"));
  endif

  if(Tree)
    DestroyItem(Tree);
  endif

  if(Leaves)
    DestroyItem(Leaves);
  endif

  Tree         := CreateItemAtLoc(me.x, me.y, me.z, UOBJ_TREEFORM, 1, me.realm);
  Tree.movable := 0;
  SetObjProperty(me, "TfTreeSer", Tree.serial);
  SetFlag(Tree, ITEMF_NO_LOOT);

  if(LeavesObj != error)
    Leaves := CreateItemAtLoc(me.x, me.y, me.z, UOBJ_TREEFORM_LEAVES, 1, me.realm);
    Leaves.movable := 0;
    SetObjProperty(me, "TfLeavesSer", Leaves.serial);
    SetFlag(Leaves, ITEMF_NO_LOOT);
  endif

  var TreeForm := CInt(TreeForms[RandomInt(TreeForms.size())+1]);

  if(!TreeForm)
    TreeForm := CInt(TreeForms[1]);
    if(!TreeForm)
      LogError("TreeFormChange: Brak TreeForm ["+me.npctemplate+"]");
      return;
    endif
  endif

  Tree.graphic   := TreeForm;
  Tree.color     := TreeColor;

  if(Leaves)
    LeavesObj := CInt(LeavesObj);
    if(LeavesObj == 0)
      Leaves.graphic := TreeForm+1;
    else
      Leaves.graphic := LeavesObj;
    endif
  endif

  if(!me.concealed)
    me.concealed   := 1;
  endif

  while(me)
    ev := wait_for_event(15);

    if(ev.type != EVID_LEFTAREA)
      if(ev.type == EVID_ENTEREDAREA and !ev.source.concealed)
        if(CanFightWith(ev.source))
          DisableEvents(EVID_ENTEREDAREA);
          EnableMainEvents();
          UnTreeForm();
          return;
        elseif(ev.source.weapon.objtype == UOBJ_HATCHET and IsPlayer(ev.source))
          ev.type := EVID_ENGAGED;
          DisableEvents(EVID_ENTEREDAREA);
          EnableMainEvents();
          UnTreeForm();
          PlayIdleSound();
          return;
        endif
      endif
    endif

    foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, 2, me.realm)
      if(IsPlayer(Mob))
        if(Mob.weapon.objtype == UOBJ_HATCHET)
          ev := struct;
          ev.+type   := EVID_ENGAGED;
          ev.+source := Mob;
          DisableEvents(EVID_ENTEREDAREA);
          EnableMainEvents();
          UnTreeForm();
          PlayIdleSound();
          return;
        endif
      endif
    endforeach

    if(RandomInt(1000) < 5)
      break;
    endif
  endwhile

  UnTreeForm();
  ev := 0;
  DisableEvents(EVID_ENTEREDAREA);
  EnableMainEvents();

endfunction

//-----------------------------------------------------------------------------

function MakePeace(source, SkID := SKILLID_PEACEMAKING)

  if(IsWalking())
    SetWalkPause();
  endif

  if(SkId == -1)
    ChangeOpponent(0);
    sleepms(500);
    clear_event_queue();
    return;
  endif
  
  SetOpponent(0);
  SetWarMode(0);
  
  if(!SkID)
    SkID := SKILLID_PEACEMAKING;
  endif

  DisableEvents(EVID_ENTEREDAREA + EVID_LEFTAREA + EVID_SPEECH + EVID_ITEM_GIVEN + EVID_DOUBLECLICKED);
  EnableEvents(EVID_DAMAGED + EVID_ENGAGED);

  ev := wait_for_event(CInt(PEACEMAKING_DELAY + CInt(GetEffectiveSkill(source,SkID)/25)) + RandomInt(3));
  
  case(ev.type)
    EVID_ENGAGED:
    EVID_DAMAGED:  if(ev.source)
                     ChangeOpponent(ev.source);
                   endif

    default:       ChangeOpponent(opponent);
  endcase
  
  EnableMainEvents();

endfunction

//-----------------------------------------------------------------------------

function SetUpAr()

  me.ar_mod := CInt(GetNpcConfig(me).AR);
  ApplyStaticMods(me);

endfunction

//-----------------------------------------------------------------------------

function SaveAiSerial()

  SetObjProperty(me, PROP_AISERIAL, me.serial);

endfunction

//-----------------------------------------------------------------------------

function CheckLifeTimers()

  var cfgElem := GetNpcConfig(me);

  if(GetObjProperty(me, PROP_SK_SUMMONED))
    if(CInt(GetObjProperty(me, PROP_SK_SUMMONED)) + LIFETIME_SUMMONED < ReadGameClock())
      SetFlag(me, NPCF_NO_LOOT);
      DealDamage(me, GetMaxHp(me));
      exit;
    endif
  elseif(GetObjProperty(me, PROP_UNTAMED))
    if(CInt(GetObjProperty(me, PROP_UNTAMED)) + LIFETIME_UNTAMED < ReadGameClock())
      SetFlag(me, NPCF_NO_LOOT);
      DealDamage(me, GetMaxHp(me));
      exit;
    endif
  elseif(cfgElem.MaxLifeTime)
    if(!CInt(GetObjProperty(me, PROP_CREATEDAT)))
      SetObjProperty(me, PROP_CREATEDAT, ReadGameClock());
    endif
    if(CInt(GetObjProperty(me, PROP_CREATEDAT)) + CInt(cfgElem.MaxLifeTime) < ReadGameClock())
      SetFlag(me, NPCF_NO_LOOT);
      DealDamage(me, GetMaxHp(me));
      exit;
    endif      
  endif
  
endfunction

//-----------------------------------------------------------------------------

function RunAwayFeared(opponent)

  return 0;

  if(AiFlags & AIF_IMMOBILE)
    return 1;
  endif

  if(IsWalking())
    SetWalkPause();
  endif
  var Count := Specs[NSPEC_THRESHOLD]*2;
  
  while(Distance(me,opponent) <= Specs[NSPEC_THRESHOLD]+5 and Count > 0)
    if(RunAwayFrom(opponent))
      ev := wait_for_event(0);
    else
      ev := wait_for_event(1);
    endif
    
    if(ev.type)
      case(ev.type)
        EVID_ENGAGED:
        EVID_DAMAGED:      if(RandomInt(10) == 1 and !(opponent.multi.isa(POLCLASS_BOAT) and opponent.multi != me.multi))
                             Fight(ev.source);
                             return 0;
                           endif
                           
        EVID_PEACEMADE:    MakePeace(ev.source,ev.others);
                           return 0;
      endcase
    endif
    Count := Count - 1;
  endwhile

  return 1;

endfunction

//-----------------------------------------------------------------------------

function StdMovNpcHandleEvent(byref ev)

  case(ev.type)
    EVID_RUN:                NpcMoveTo(ev, NMT_RUN);
                             return 1;

    EVID_WALK:               NpcMoveTo(ev, NMT_WALK);
                             return 1;
    
    EVID_WALK_FOLLOW:        NpcFollow(ev, NMT_WALK);
                             return 1;

    EVID_RUN_FOLLOW:         NpcFollow(ev, NMT_RUN);
                             return 1;
  endcase

   ev := 0;
   return 0;

endfunction

//-----------------------------------------------------------------------------

const NMT_WALK               := 0x00000001;
const NMT_RUN                := 0x00000002;
const NMT_SMART              := 0x00000004;
const NMT_NO_BREAK           := 0x00000008;

//-----------------------------------------------------------------------------

function NpcMoveTo(byref ev, Flags := NMT_WALK)

  if(AiFlags & AIF_IMMOBILE or me.frozen)
    ev := 0;
    return 0;
  endif

  var bRes := 0;
  var Source   := ev.source;
  var ShouldAttack := ev.attack;

  if(ev.source.x and ev.source.y)
    if(IsWalking())
      SetWalkPause();
    endif

    var x        := CInt(ev.source.x);
    var y        := CInt(ev.source.y);
    var z        := GetStandZ(x, y, me.realm);
    var Count    := coordist(x,y,me.x,me.y)*2;
    var Prior    := ev.others == "priormax";
    var wTime    := 1;

    if(Count > 200)
      ev := 0;
      return 0;
    elseif(Flags & NMT_RUN)
      Count *= 2;
      Flags := Flags | NMT_SMART;
    endif
    
    InitSmartRunLoc();

    SetObjProperty(me,"#RunningTo",{x,y});

    while(coordist(x,y,me.x,me.y) > 1 and Count > 0)
      if(Flags & NMT_SMART)
        wTime := !SmartRunTowardLoc(x, y, z);
      elseif(Flags & NMT_RUN)
        wTime := !RunTowardLocation(x, y);
      else
        wTime := !WalkTowardLocation(x, y);
      endif

      ev := wait_for_event(wTime);

      if(ev.type != EVID_ENTEREDAREA and ev.type != EVID_LEFTAREA)
        if(Prior)
          if(ev)
            if(ev.type == EVID_RUN or ev.type == EVID_WALK)
              EraseObjProperty(me,"#RunningTo");
              return -1;
            endif
          endif
        elseif(!(Flags & NMT_NO_BREAK) and CanHandleEvent(ev))
          EraseObjProperty(me,"#RunningTo");
          return -1;
        endif
      endif

      Count := Count - 1;
    endwhile
    
    EraseObjProperty(me,"#RunningTo");
    bRes := coordist(x,y,me.x,me.y) <= 1;
  endif

  if(ShouldAttack)
    SendChEvent(me, Source, EVID_ENGAGED);
  endif

  ev := 0;
  return bRes;

endfunction

//-----------------------------------------------------------------------------

function NpcFollow(byref ev, Flags := NMT_WALK)

  if(AiFlags & AIF_IMMOBILE or me.frozen)
    ev := 0;
    return;
  elseif(!ev.source.isa(POLCLASS_MOBILE))
    ev := 0;
    return;
  elseif(ev.source.realm != me.realm)
    ev := 0;
    return;
  elseif(Distance(me, ev.source) > 40)
    ev := 0;
    return;
  endif

  if(IsWalking())
    SetWalkPause();
  endif

  var Prior    := ev.others == "priormax";
  var Source   := ev.source;

  SetObjProperty(me,"#Following", ev.source.serial);

  while(Source and me)
    if(Distance(me, Source) > 1)
      if(Flags & NMT_RUN)
        ev := wait_for_event( !RunTowardLocation(Source.x, Source.y) );
      else
        ev := wait_for_event( !WalkTowardLocation(Source.x, Source.y) );
      endif
    else
      ev := wait_for_event(2);
    endif

    if(ev.type == EVID_FOLLOW_STOP)
      break;
    endif

    if(ev.type != EVID_ENTEREDAREA and ev.type != EVID_LEFTAREA)
      if(Prior)
        if(ev)
          if(ev.type == EVID_RUN_FOLLOW or ev.type == EVID_WALK_FOLLOW or ev.type == EVID_FOLLOW_STOP)
            EraseObjProperty(me,"#Following");
            return;
          endif
        endif
      elseif(CanHandleEvent(ev))
        EraseObjProperty(me,"#Following");
        return;
      endif
    endif

  endwhile
    
  ev := 0;

endfunction

//-----------------------------------------------------------------------------

function RunLikeHell(opponent)

  if(AiFlags & AIF_IMMOBILE)
    return;
  endif

  if(IsWalking())
    SetWalkPause();
  endif

  var Count := Specs[NSPEC_THRESHOLD] * 2;

  while(opponent and !(opponent.dead or opponent.hidden or opponent.concealed) and Distance(me,opponent) < Specs[NSPEC_THRESHOLD]+5 and Count > 0)

    if(RunAwayFrom(opponent))
      ev := wait_for_event(0);
    else
      ev := wait_for_event(1);
    endif

    case(ev.type)
      EVID_ENGAGED:
      EVID_DAMAGED:     if(RandomInt(3)==1)
                          Fight(ev.source);
                          return;
                        endif
    endcase

    Count := Count - 1;
  endwhile

endfunction

//-----------------------------------------------------------------------------

function RunAwayFromLoc(x, y)

  if(AiFlags & AIF_IMMOBILE)
    return;
  endif

  if(IsWalking())
    SetWalkPause();
  endif

  var Count := Specs[NSPEC_THRESHOLD] * 2;

  while(coordist(me.x, me.y, x, y) < Specs[NSPEC_THRESHOLD]+5 and Count > 0)

    if(RunAwayFromLocation(x, y))
      ev := wait_for_event(0);
    else
      if(me.x == x and me.y == y)
        if(!GetStand(x, y+1, me.realm, me.z) and !GetStand(x-1, y-1, me.realm, me.z) and !GetStand(x+1, y-1, me.realm, me.z) and GetStand(x, y-1, me.realm, me.z))
          y += 1;
        endif
      endif
      ev := wait_for_event(1);
    endif

    case(ev.type)
      EVID_ENGAGED:
      EVID_DAMAGED:     if(RandomInt(3)==1)
                          Fight(ev.source);
                          return;
                        endif
    endcase

    Count := Count - 1;
  endwhile

endfunction

//-----------------------------------------------------------------------------

function GetNpcSpells()

  var cfgElem := GetNpcConfig(me);

  if(cfgElem.spell)
    spells := {};
    foreach Spell in GetConfigStringArray(cfgElem, "spell")
      spells.append(CInt(Spell));
    endforeach
    return spells;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function AnimalIdle()

  CheckAnimalSpecs();
  NpcIdle();

endfunction

//-----------------------------------------------------------------------------

function NpcIdle()

  if(NextIdle < ReadGameClock() and !ev)
    NextIdle := ReadGameClock() + 30 + RandomInt(60);
  
    var cfgElem := GetNpcConfig(me);

    if(cfgElem.IdlePlayGood and RandomInt(100) < 20)
      if(RandomInt(100) < 50)
        PlaySoundEffect(me, cfgElem.IdlePlayGood);
      else
        PlaySoundEffect(me, cfgElem.IdlePlayBad);
      endif

      if(cfgElem.IdlePlayAnim)
        PerformAction(me, cfgElem.IdlePlayAnim);
      endif

      NextIdle := NextIdle - RandomInt(30);
    else
      for i := 1 to (2+RandomInt(3))
        PlayIdleSound();
        case(RandomInt(5))
          1:        if(cfgElem.FidgetAnim1)
                      PerformAction(me, cfgElem.FidgetAnim1);
                    elseif(IsHuman(me))
                      PerformAction(me, ANIM_FIDGET_2);
                    else
                      PerformAction(me, ANIM_NPC_FIDGET1);
                    endif

          3:        if(cfgElem.FidgetAnim2)
                      PerformAction(me, cfgElem.FidgetAnim2);
                    elseif(IsHuman(me))
                      PerformAction(me, ANIM_FIDGET_2);
                    else
                      PerformAction(me, ANIM_NPC_FIDGET2);
                    endif

          default:  if(cfgElem.IdleAnim)
                      PerformAction(me, cfgElem.IdleAnim);
                    elseif(IsHuman(me))
                      PerformAction(me, ANIM_FIDGET_1);
                    else
                      PerformAction(me, ANIM_NPC_IDLE);
                    endif
        endcase

        ev := wait_for_event(2+RandomInt(4));
        if(CanHandleEvent(ev))
          return;
        elseif(ev)
          sleepms(500+RandomInt(500));
        endif
      endfor
    endif

  elseif(RandomInt(100) < 8)
    PlayIdleSound();
  endif

endfunction

//-----------------------------------------------------------------------------

function PlayIdleSound()

  var cfgElem := GetNpcConfig(me);

  if(cfgElem.idlesound)
    var Sound := SplitWords(cfgElem.idlesound);
    Sound     := CInt(Sound[RandomInt(Sound.size())+1]);
    if(Sound)
      PlaySoundEffect(me, Sound);
    endif
  else
    if(RandomInt(2) == 1)
      PlaySoundEffect(me, CInt(cfgElem.idlesound1));
    else
      PlaySoundEffect(me, CInt(cfgElem.idlesound2));
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function StartSleep()

  var cfgElem := GetNpcConfig(me);

  if(cfgElem.TreeForm)
    UnTreeForm();
  endif

  if(!cfgElem.NoStSleep)
    return 0;
  endif

  return MAX_WANDERS_UNTIL_SLEEP;

endfunction

//-----------------------------------------------------------------------------

function OpenDoors()

  var cfgElem := GetNpcConfig(me);

  if(cfgElem.OpensDoors or IsTamed(me))
    foreach door in ListItemsNearLocation(me.x, me.y, me.z, 1, me.realm)
      if(door.isa(POLCLASS_DOOR))
        if(!IsOpened(door))
          if(CanOpenDoor(me, door))
            start_script(":doors:opendoor", door);
          endif
        endif
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function DoWander()

  if(!IsSetFlag(me,NPCF_NO_WANDER) and !me.hidden and !me.concealed)
    if(RandomInt(100) < 40)
      Wander();
    endif
  endif

endfunction

//------------------------------------------------------------------------------

function UnHide(byref opp)

  if(opp.hidden)
    opp.hidden := 0;
  endif

endfunction

//------------------------------------------------------------------------------

function GetStrengthFamilyMax(SkVal)

  var Str := FAMILY_MAXSTR * (CDbl(SkVal-50)/50.0);

  if(Str < 100)
    return 100;
  endif

  return Str;

endfunction 

//------------------------------------------------------------------------------

function IsFamily(mob)

  var BaseStr := GetBaseStrength(me);

  if(BaseStr <= FAMILY_MAXSTR)

    var Wearwolf := GetBaseSkill(mob, SKILLID_WEARWOLF);
    var Vampire := GetBaseSkill(mob, SKILLID_VAMPIRE);
    var Necro := GetBaseSkill(mob, SKILLID_NECRO);
    var Druid := GetBaseSkill(mob, SKILLID_DRUID);
    var AlLore := GetBaseSkill(mob, SKILLID_ANIMALLORE);

    if(Wearwolf >= 60 and AlLore >= 60)

      var Elementals := GetNpcElementals(me);
      var StrMax := GetStrengthFamilyMax(Wearwolf);

      if(BaseStr <= StrMax)
        if(Elementals.size())
          if(NPCELEM_WOLF in Elementals)
            return 1;
          endif
        endif
      endif

    elseif((Vampire >= 60 or Necro >= 60) and GetBaseSkill(mob, SKILLID_SPIRITSPEAK) >= 60)
    
      var Elementals := GetNpcElementals(me);
      var StrMax := GetStrengthFamilyMax(__IfElse(Vampire > Necro, Vampire, Necro));

      if(BaseStr <= StrMax)
        if(Elementals.size())
          if(NPCELEM_UNDEAD in Elementals)
            return 1;
          endif
        endif
      endif
    elseif(Druid >= 60 and AlLore >= 60)

      var StrMax := GetStrengthFamilyMax(Druid);

      if(BaseStr <= StrMax)
        var cfgElem := GetNpcConfig(me);

        if(cfgElem.LikeDruid)
          return 1;
        endif
      endif

    endif
    
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsPeaceTime()

  var Time := GetObjProperty(me, "#PeaceTime");

  if(Time != error)
    if(CInt(Time) > ReadGameClock())
      return 1;
    endif
    EraseObjProperty(me, "#PeaceTime");
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWithPlayer(Mob)

  if(IsPeaceTime())
    return 0;
  endif

  if(IsPlayer(Mob))
    if(CanMod(Mob, MOD_UNTOUCHABLE) == 0)
      var Amt := GetModAmount(Mob, MOD_UNTOUCHABLE);

      if(Amt[1] >= CInt(GetMaxHp(me) / 2))
        foreach Elem in (Amt[2])
          if(IsNpcElemental(me, Elem))
            return 1;
          endif
        endforeach

        return 0;
      endif
    endif

    if(Mob.graphic == INVIS_MOB_GRAP)
      return 0;
    endif

/*    if(IsYoung(Mob))
      if(GetBaseStrength(me) <= YOUNG_UNTOUCH_MAXSTR or IsSummoned(me))
        return 0;
      endif
    else*/
    if(IsPlayer(GetMaster(me))) //__NOPVP__  and GetPvPStatus(Mob) == PVP_NO)
      return 0;
    endif

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetFleePoint()

  var cfgElem := GetNpcConfig(me);

  var fleePoint := cfgElem.FleePoint;

  if(fleePoint == error)
    return DEF_FLEEPOINT;
  else
    return CInt(fleePoint);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetSpellTime()

  var cfgElem := GetNpcConfig(me);

  var SpellTime := cfgElem.SpellTime;

  if(SpellTime == error)
    return DEF_SPELLTIME;
  else
    return CInt(SpellTime);
  endif

endfunction

//-----------------------------------------------------------------------------

function MeHaveFlag(Name)

  var cfgElem := GetNpcConfig(me);
  return GetConfigInt(cfgElem, Name);

endfunction

//-----------------------------------------------------------------------------

function CheckBegPurse()

  var amt := Cint(GetObjProperty(me, "begpurse"));
  if(!amt)
    amt := 50;
  endif
  
  var cap := 50;
  var cfgElem := GetNpcConfig(me);

  case(cfgElem.wealth)
    "upper":  cap := 150;
    "middle": cap := 100;
    "poor":   cap := 50;
    "broke":  cap := 0;
  endcase
  
  if(amt < cap)
    amt := amt + 1;
    SetObjProperty(me, "begpurse", amt);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function GetHateList()

  var cfgElem := GetNpcConfig(me);
  var list    := cfgElem.HateList;

  if(!list)
    list := error;
  else
    list := SplitWords(list);
  endif

  return list;

endfunction

//-----------------------------------------------------------------------------
