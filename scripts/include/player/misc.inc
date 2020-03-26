//-----------------------------------------------------------------------------

include "include/const/props";
include "include/const/misc";
include "include/checks/stdchecks";

//================================================================================
//--------------------------------------------------------------------------------
// Player Vendor
//--------------------------------------------------------------------------------
//================================================================================

function SetVendorMaster(Vendor,Master)

  SetObjProperty(Vendor,PROP_MASTER,Master.serial);

endfunction

//--------------------------------------------------------------------------------

function SetVendorSeller(Vendor,Seller)

  SetObjProperty(Vendor,"Seller",Seller.serial);

endfunction

//-----------------------------------------------------------------------------

function GetAllFightSkIDs()

  return array(
    SKILLID_SWORDSMANSHIP,
    SKILLID_MACEFIGHTING,
    SKILLID_FENCING,
    SKILLID_ARCHERY,
    SKILLID_WRESTLING,
    SKILLID_AXESMANSHIP);

endfunction

//-----------------------------------------------------------------------------

function GetFightSkId(who)

  var Skills := dictionary;
  var SkIDs := GetAllFightSkIDs();

  foreach SkID in SkIDs
    Skills[GetBaseSkillBaseValue(who, SkID)] := SkID;
  endforeach

 var Keys := Skills.keys();

  
  return Skills[ Keys[Keys.size()] ];

endfunction

//-----------------------------------------------------------------------------

function GetFightSkIds(who)

  var RetSkIDs := array;
  var SkIDs := GetAllFightSkIDs();

  foreach SkID in SkIDs
    if(GetBaseSkillBaseValue(who, SkID) > 0)
      RetSkIDs.append(SkID);
    endif
  endforeach

  if(RetSkIDs.size() > 0)
    return RetSkIDs;
  else
    return error;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetOldestCharTime(acc)

  var i,char,time,tMax := 0;

  for(i:=1;i<=5;i:=i+1)
    char := acc.GetCharacter(i);
    if(char)
      time := CInt(GetObjProperty(char,"onlinetimer"));
      if(time > tMax)
        tMax := time;
      endif
    endif
  endfor

  return tMax;

endfunction

//-----------------------------------------------------------------------------

function UpdateStatus(who)

  //var UpdPc := CrUpdStatsPacket(who);
  //UpdPc.SendPacket(who);
  who.luck := who.luck;

endfunction

//-----------------------------------------------------------------------------

function StartDeathControl(who)

  if(who.dead and IsYoung(who))
    start_script("::death_control", who);
  endif

endfunction

//-----------------------------------------------------------------------------

function DisarmPlayer(who)

  if(MoveToPack(GetEquipmentByLayer(who, LAYER_HAND1), who) or MoveToPack(GetEquipmentByLayer(who, LAYER_HAND2), who))
    SetObjProperty(who, "#WpnEquipDelay", ReadGameClock() + 3);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
