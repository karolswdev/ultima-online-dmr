//-----------------------------------------------------------------------------

include ":npcsets:shared";

//-----------------------------------------------------------------------------

function CheckNpcSetControl(Npc)

  if(GetObjProperty(Npc, PROP_NSET_CTRL_ID) == error)
    return 1;
  endif

  return CheckNpcSetExistance(Npc);

endfunction

//-----------------------------------------------------------------------------

function CheckNpcSetExistance(Npc)

  var ctrlId := CInt(GetObjProperty(Npc, PROP_NSET_CTRL_ID));

  var cElem  := GetNsetCtrlElem(ctrlId);

  if(!cElem)
    LogError("Znaleziono npc, ktorego IS-Ctrl nie istnieje, a ma przypisany ID! "+GetName(Npc)+"/"+hex(Npc.objtype)+" CtrlID: "+ctrlId, LOGF_SCRIPTS);
    return 0;
  endif

  var Npcs := cElem.GetProp("Npcs");
  var myIdx := RetIndex(Npcs, Npc.serial);

  if(!myIdx)
    Npcs.append(Npc.serial);
    cElem.SetProp("Npcs", Npcs);
    cElem.SetProp("NpcCount", Npcs.size());
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
