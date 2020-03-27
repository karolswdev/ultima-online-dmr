//-----------------------------------------------------------------------------

include ":npcsets:shared";

//-----------------------------------------------------------------------------

function GetNsetCtrlIds()

  var Data := GetNsetCtrlData();
  var Keys := Data.Keys();

  var gIdx := RetIndex(Keys, NSC_ELEM_GLOBAL);

  if(gIdx)
    Keys.erase(gIdx);
  endif

  return Keys;

endfunction

//-----------------------------------------------------------------------------

function GetNsetCtrlNextId()

  var Data := GetNsetCtrlData();

  var gElem := Data.FindElement(NSC_ELEM_GLOBAL);

  if(!gElem)
    gElem := Data.CreateElement(NSC_ELEM_GLOBAL);
  endif

  var NextId := CInt(gElem.GetProp("NextId"))+1;

  gElem.SetProp("NextId", NextId);

  return NextId;

endfunction

//-----------------------------------------------------------------------------

function AddNpcSetControl(who, Set, Npcs)

  var CtrlData := GetNsetCtrlData();

  if(!CtrlData)
    return CtrlData;
  endif

  var NextId   := GetNsetCtrlNextId();

  if(!NextId)
    return NextId;
  endif

  var newElem  := CtrlData.CreateElement(CStr(NextId));

  if(!newElem)
    return newElem;
  endif

  foreach Npc in Npcs
    SetObjProperty(Npc, PROP_NSET_CTRL_ID, NextId);
    Npc := Npc.serial;
  endforeach

  newElem.SetProp("SetName", Set.Name);
  newElem.SetProp("Npcs", Npcs);
  newElem.SetProp("Creator", who.acctname);
  newElem.SetProp("AddTime", PolCore().systime);
  newElem.SetProp("AddGameTime", ReadGameClock());
  newElem.SetProp("NpcCount", Npcs.size());

  return nextId;

endfunction

//-----------------------------------------------------------------------------

function MoveNsetCtrlToLoc(who, Loc, ctrlId)

  var cElem := GetNsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola NS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Npcs := cElem.GetProp("Npcs");
  var Npc;
  var Res := struct;
  Res.+npcCount := 0;
  Res.+nexCount  := 0;

  // na koncu beda root npcy
  Npcs.reverse();

  foreach NpcSer in Npcs

    Npc := FindNpcBySerial(NpcSer);

    if(!Npc)
      Res.nexCount += 1;
      continue;
    endif

    var itmRes := MoveObject(Npc, Loc.x, Loc.y, Loc.z, Loc.realm, MOVEOBJECT_FORCELOCATION);

    if(!itmRes)
      return error{"errortext" := "Nie udalo sie przeniesc npca: "+GetName(Npc)+"/"+hex(Npc.serial)+"! Zbadaj w czym problem! Res: "+itmRes.errortext};
    endif

    EraseObjProperty(Npc, PROP_NSET_CTRL_ID);
    
    if(!IsGmCreated(Npc))
      SetPropCreated(who, Npc);
    endif

    Res.npcCount += 1;

  endforeach

  var rmRes := RemoveNsetCtrl(ctrlId);

  if(!rmRes)
    return rmRes;
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function CheckNsetCtrl(cElem)
  
  var Npc;
  var Npcs := cElem.GetProp("Npcs");

  foreach NpcSer in Npcs

    Npc := FindNpcBySerial(NpcSer);

    if(Npc)
      if(!IsGmCreated(Npc))
        return error{"errortext" := "Przedmiot "+GetName(Npc)+"/"+hex(Npc.serial)+" nie zostal oznaczony flaga stworzyciela i ciagle istnieje!"};
      endif
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteNsetCtrlNpcs(ctrlId)

  var cElem := GetNsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola IS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Res  := struct;

  Res.+npcCount := 0;
  Res.+nexCount  := 0;
  
  var Npcs := cElem.GetProp("Npcs");

  foreach NpcSer in Npcs

    var Npc := FindNpcBySerial(NpcSer);

    if(Npc)
      if(!KillNpc(Npc))
        return error{"errortext" := "Nie udalo sie usunac npcu "+GetName(Npc)+"/"+hex(Npc.serial)+"! Zbadaj dlaczego."};
      endif

      Res.npcCount += 1;

    else

      Res.nexCount += 1;

    endif

  endforeach

  return Res;

endfunction

//-----------------------------------------------------------------------------

function RemoveNsetCtrl(ctrlId)

  var cElem := GetNsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola IS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Data := GetNsetCtrlData();

  var cRes := CheckNsetCtrl(cElem);
  if(!cRes)
    return cRes;
  endif

  var dRes := Data.DeleteElement(CStr(ctrlId));

  if(!dRes)
    return error{"errortext" := "Nie udalo sie usunac IS kontroli! Blad: ["+dRes+"] Powiadom o tym administratora."};
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
