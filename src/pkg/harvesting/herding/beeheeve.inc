//-----------------------------------------------------------------------------

const MAX_BEEHEEVES := 4;

//-----------------------------------------------------------------------------

include "include/checks/stdchecks";
include "include/items";

//-----------------------------------------------------------------------------

function DestroyBeeHeeve(BeeHeeve,who := 0)

  var Owner   := FindBeeHeeveOwner(BeeHeeve);
  var BasicUl := FindItemBySerial(GetObjProperty(beeheeve,"BasicSer"));
  var Bees    := FindItemBySerial(GetObjProperty(beeheeve,"Bees"));
  var Multi   := GetHouse(BeeHeeve);

  if(!IsGm(who))
    if(Multi)
      if(who != Owner and who.acctname != GetHouseOwner(Multi).acctname)
        SendSysMessage(who, "Nie mozesz zniszczyc tego ula.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    elseif(who != Owner)
      SendSysMessage(who, "Nie mozesz zniszczyc tego ula.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
  endif

  if(!DestroyItem(BeeHeeve))
    return 0;
  endif

  if(BasicUl)
    DestroyItem(basicul);
  endif

  if(Bees)
    DestroyItem(Bees);
  endif

  var Deed := CreateItemInPack(who, UOBJ_BEEHEEVEDEED, 1);

  if(Deed)
    SendSysMessage(who,"Zniszczyl"+ggm(who,3)+" ul i wlozyl"+ggm(who,3)+" deed do plecaka.",FONT_NORMAL,COLOR_GREEN);
  else
    SendSysMessage(who,"Zniszczyl"+ggm(who,3)+" ul.. ale deed gdzies zaginal!!!",FONT_NORMAL,COLOR_ORANGE);
  endif

  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function FindBeeHeeveOwner(BeeHeeve)

  return FindPlayerBySerial(GetObjProperty(beeheeve,"UlOwner"));

endfunction

//-----------------------------------------------------------------------------

function CheckBeeHeeves(who)

  if(Len(GetBeeHeeves(who)) >= MAX_BEEHEEVES)
    return 0;
  endif
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetBeeHeeves(who)

  var BeeHeeves := GetObjProperty(who,"BeeHeeves");
  if(!BeeHeeves or !BeeHeeves.size())
    BeeHeeves := {};
  endif
  var i := 1;
  foreach BeeHeeve in BeeHeeves
    if(SystemFindObjectBySerial(BeeHeeve))
      i := i + 1;
    else
      BeeHeeves.erase(i);
    endif
  endforeach
  SetBeeHeeves(who,BeeHeeves);

  return GetObjProperty(who,"BeeHeeves");

endfunction

//-----------------------------------------------------------------------------

function SetBeeHeeves(who,BeeHeeves)

  SetObjProperty(who,"BeeHeeves",BeeHeeves);

endfunction

//-----------------------------------------------------------------------------

function AddBeeHeeve(Owner,BeeHeeve)

  var BeeHeeves := GetBeeHeeves(Owner);
  if(!(BeeHeeve.serial in BeeHeeves))
    if(BeeHeeves.size() >= MAX_BEEHEEVES)
      DestroyBeeHeeve(BeeHeeve,Owner);
      EraseObjProperty(Owner,"#BeeHeeveChecking");
      exit;
    endif
    BeeHeeves.append(BeeHeeve.serial);
    SetBeeHeeves(Owner,BeeHeeves);
    return 1;
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------
