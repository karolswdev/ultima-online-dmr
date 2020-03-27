//-----------------------------------------------------------------------------

include "include/unicode";
include "include/misc";
include "include/finding";
include "include/msgs";
include "include/client/fonts";
include "include/checks/stdchecks";

//-----------------------------------------------------------------------------

const PARTYCMD_ADD_MEMB        := 0x01;
const PARTYCMD_REM_MEMB        := 0x02;
const PARTYCMD_TELL_MEMB_MSG   := 0x03;
const PARTYCMD_TELL_PARTY_MSG  := 0x04;
const PARTYCMD_CAN_LOOT        := 0x06;
const PARTYCMD_INVITATION      := 0x07;
const PARTYCMD_ACCEPT_JOIN     := 0x08;
const PARTYCMD_DECLINE_JOIN    := 0x09;

//-----------------------------------------------------------------------------

function IsInSameParty(char1, char2)

  // For memory fragmentation
  if(Malloc(char1) && Malloc(char2))
	return 0;
  endif

  if(!IsPlayer(char1) or !IsPlayer(char2))
    return 0;
  endif

  if(char1.party and char2.party and char1.party == char2.party)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function PartySendPrivateMsg(who, byref Packet)

  var Targ := FindPlayerBySerial(Packet.GetInt32(6));

  if(IsInSameParty(who, Targ))
    if(!IsLoggedIn(Targ))
      SendSysMessage(who, Targ.name+" stracil"+ggm(Targ,4)+" przytomnosc!", FONT_NORMAL, COLOR_ORANGE);
      return 0;
    endif

    var orgMsg := Packet.GetUnicodeString(10, (Packet.GetSize()-12)/2);
    var Msg;

    Msg := CAscZ("[do "+Targ.name+"]: ") + orgMsg;
    UniSendSysMessage(who, Msg, FONT_NORMAL, COLOR_GREEN);
    Msg := CAscZ("[od "+who.name+"]: ") + orgMsg;
    UniSendSysMessage(Targ, Msg, FONT_NORMAL, COLOR_GREEN);

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function PartySendPartyMsg(who, byref Packet)

  var orgMsg := Packet.GetUnicodeString(6, (Packet.GetSize()-8)/2);
  UniPrintTextAbove(who, orgMsg);

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendPartyNotice(party, Msg, Color := COLOR_ORANGE)

  if(!party or !party.members)
    return 0;
  endif

  var Members := party.members;

  foreach Memb in Members
    if(IsLoggedIn(Memb))
      SendSysMessage(Memb, Msg, FONT_NORMAL, Color);
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetNearbyPartyMembers(who)

  if(!who.party)
    return 0;
  endif

  var AllMembs    := who.party.members;
  var NearbyMembs := array;

  foreach Memb in AllMembs
    if(IsLoggedIn(Memb) and !Memb.dead and Memb != who)
      if(Distance(who, Memb) <= MAX_VISIBLE_RANGE and !Memb.hidden)
        NearbyMembs.append(Memb);
      endif
    endif
  endforeach

  return NearbyMembs;

endfunction

//-----------------------------------------------------------------------------

function SplitGold(who, Item)

  if(GetObjProperty(Item, "#SplitGold"))
    return 0;
  endif

  var NearbyMembs := GetNearbyPartyMembers(who);

  if(!NearbyMembs or !NearbyMembs.size())
    return 0;
  endif

  var oneAmt := CInt(Item.amount / (NearbyMembs.size()+1));

  if(!oneAmt)
    return 0;
  endif

  if(SubtractAmount(Item, oneAmt * NearbyMembs.size()))
    foreach Memb in NearbyMembs
      CreateItemInPack(Memb, UOBJ_GOLDCOIN, oneAmt);
      SendSysMessage(Memb, "Zebral"+ggm(Memb,3)+" "+oneAmt+" zlota.", FONT_NORMAL, COLOR_GREEN);
    endforeach
  else
    SetObjProperty(Item, "#SplitGold", who.serial);
    return -1;
  endif

  SendSysMessage(who, "Zebral"+ggm(who, 3)+" "+Item.amount+" zlota.", FONT_NORMAL, COLOR_GREEN);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SplitNoto(who, kGain, fGain)

  var NearbyMembs := GetNearbyPartyMembers(who);

  if(!NearbyMembs or !NearbyMembs.size())
    return 0;
  endif

  var okAmt := CInt(kGain / (NearbyMembs.size()+1));
  var ofAmt := CInt(fGain / (NearbyMembs.size()+1));

  if(!okAmt and !ofAmt)
    return 0;
  endif

  var WhoKarma := GetKarma(who);
  var WhoFame  := GetFame(who);

  var rkAmt := kGain - (okAmt * NearbyMembs.size());
  var rfAmt := fGain - (ofAmt * NearbyMembs.size());

  SetFame(who, WhoFame + rfAmt);
  SetKarma(who, WhoKarma + rkAmt);
  RefreshNotoTitle(who);

  foreach Memb in NearbyMembs
    WhoKarma := GetKarma(Memb);
    WhoFame  := GetFame(Memb);

    SetFame(Memb, WhoFame + ofAmt);
    SetKarma(Memb, WhoKarma + okAmt);
    RefreshNotoTitle(Memb);
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PartyCheckStatus(who, Leader)

  __ParamUnused(who);

  var Party := Leader.party;
  var Members := Party.members;
  var lCrim := 0;

  foreach Mob in Members
    if(Mob.murderer != Leader.murderer)
      lCrim := 1;
      break;
    endif
  endforeach

  foreach Mob in Members
    if(!Mob.criminal and !Mob.murderer)
      SetCriminal(Mob, 1, "PartyCheck", "Osoba wstapila do party z mordercami");
      SendBad(Mob, "Ten kto brata sie z mordercami zostanie kryminalista!");
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
