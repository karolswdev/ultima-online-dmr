//-----------------------------------------------------------------------------

include "include/const/eventid";
include "include/const/props";
include "include/finding";
include "include/misc";
include "include/checks/stdchecks";

//================================================================================
//--------------------------------------------------------------------------------
// Events
//--------------------------------------------------------------------------------
//================================================================================

function SendChEvent(who,source,type := EVID_ENGAGED, Others := 0)

  var nev     := array;
  nev.+source := source;
  
  if(!type)
    type := EVID_ENGAGED;
  endif
  
  nev.+type   := type;
  nev.+others := Others;

  SendEvent(who,nev);

endfunction

//--------------------------------------------------------------------------------

function SendDelayedEvent(Mob, source, evId, Delay)

  if(Mob.isa(POLCLASS_NPC))
    if(CInt(GetObjProperty(Mob, PROP_NPCNEXT_SENDEV+evId)) < ReadGameClock())
      SendChEvent(Mob, source, evId);
      SetObjProperty(Mob, PROP_NPCNEXT_SENDEV+evId, ReadGameClock() + Delay);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SendHealToHostile(CastOn, Caster, Healed)

  if(CastOn and Caster and !CastOn.dead and !Caster.dead)
    var ev     := struct;
    ev.+source := Caster;
    ev.+healed := Healed;
    ev.+type   := EVID_FRIEND_HEAL;

    foreach Mob in ListHostiles(CastOn, 15)
      if(Mob.isa(POLCLASS_NPC))
        SendEvent(Mob, ev);
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function SendCorpseLootEvent(Corpse, who, Owner := 0, Range := 5)

  if(!Corpse or !who)
    return error;
  endif

  if(!Owner or !Owner.isa(POLCLASS_MOBILE))
    Owner := FindPlayerBySerial(GetObjProperty(Corpse, PROP_CORPSE_OWNER));
    if(!Owner)
      return error;
    endif
  endif

  if(Range != 0 and !CInt(Range))
    Range := 5;
  endif

  var ev     := struct;
  ev.+type   := EVID_CORPSE_LOOT;
  ev.+corpse := Corpse;
  ev.+source := who;

  var Cnt    := 0;

  foreach Npc in ListMobilesNearLocationEx(Corpse.x, Corpse.y, Corpse.z, Range, LISTEX_FLAG_NORMAL | LISTEX_FLAG_HIDDEN, Corpse.realm)
    if(Npc.isa(POLCLASS_NPC) and IsTamed(Npc) and GetMaster(Npc) == Owner)
      SendEvent(Npc, ev);
      Cnt += 1;
    endif
  endforeach

  return Cnt;

endfunction

//-----------------------------------------------------------------------------

function GetEventName(EvId)

  case(EvId)
    EVID_SPEECH:             return "Speech";
    EVID_GHOST_SPEECH:       return "GhostSpeech";
    EVID_ENGAGED:            return "Engaged";
    EVID_DISENGAGED:         return "Disengaged";
    EVID_MOUNT:              return "Mount";
    EVID_WAKEUP:             return "WakeUp";
    EVID_DAMAGED:            return "Damaged";
    EVID_MOUNT_LOGON:        return "MountLogon";
    EVID_MOUNT_LOGOFF:       return "MoungLogoff";
    EVID_WALK_FOLLOW:        return "WalkFollow";
    EVID_RUN_FOLLOW:         return "RunFollow";
    EVID_FOLLOW_STOP:        return "FollowStop";
    EVID_FLEE:               return "Flee";
    EVID_RUN:                return "Run";
    EVID_GOLDCHECK:          return "GoldCheck";

    EVID_REPORTMAKRO:        return "ReportMakro";
    EVID_MOD:                return "Mod";
    EVID_SKMOD:              return "SkMod";
    EVID_MAGICMOD:           return "MagicMod";
    EVID_MAGICSKMOD:         return "MagicSkMod";
    EVID_UNDOMOD:            return "UndoMod";
    EVID_UNDOSKMOD:          return "UndoSkMod";
    EVID_WALK:               return "Walk";
    EVID_PUSH:               return "Push";
    EVID_MASTER_DEAD:        return "MasterDead";
    EVID_CORPSE_LOOT:        return "CorpseLoot";
    EVID_FRIEND_HEAL:        return "FriendHeal";
    EVID_QUEST_MSG:          return "QuestMsg";
    EVID_ENTEREDAREA:        return "EnteredArea";

    EVID_PLAYER_LOGIN:       return "PlayerLogin";
    EVID_PLAYER_LOGOFF:      return "PLayerLogoff";
    EVID_PLAYER_DELAY:       return "PlayerDelay";
    EVID_PLAYER_RECONNECT:   return "PlayerReconnect";
    EVID_SEND_SEER:          return "SendSeer";
    EVID_SEND_GM:            return "SendGM";
    EVID_SEND_ADMIN:         return "SendAdmin";

    EVID_SK_AWARD:           return "SkAward";
    EVID_ST_AWARD:           return "StAward";
    EVID_SK_SET:             return "SkSet";
    EVID_ST_SET:             return "StSet";
    EVID_AT_REC:             return "AtRec";
    EVID_SETSPECABS:         return "SetSpecAbs";

    EVID_ALL_ATTACK_CMD:     return "AllAttackCmd";
    EVID_ALL_FOLLOW_CMD:     return "AllFollowCmd";
    EVID_ALL_GUARD_CMD:      return "AllGuardCmd";
    EVID_ALL_TRANSFER_CMD:   return "AllTransferCmd";

    EVID_LEFTAREA:           return "LeftArea";

    EVID_OPPONENT_MOVED:     return "OpponentMoved";

    EVID_HOSTILE_MOVED:      return "HostileMoved";

    EVID_MERCHANT_BOUGHT:    return "MerchantBought";
    EVID_MERCHANT_SOLD:      return "MerchantSold";

    EVID_CMERC_BOUGHT:       return "CustomMerchantBought";
    EVID_CMERC_SOLD:         return "CustomMerchantSold";

    EVID_ITEM_GIVEN:         return "ItemGiven";
    EVID_DOUBLECLICKED:      return "DoubleClicked";

    EVID_PEACEMADE:          return "PeaceMade";
    EVID_UNEQUIP:            return "UnEquip";

    EVID_QUESTION:           return "Question";

  endcase

  return "[Unknown]";

endfunction

//-----------------------------------------------------------------------------
