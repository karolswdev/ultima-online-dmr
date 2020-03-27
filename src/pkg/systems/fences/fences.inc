//-----------------------------------------------------------------------------

const UOBJ_FENCESIGN_START   := 0x8001;
const UOBJ_FENCESIGN_END     := 0x8004;

//-----------------------------------------------------------------------------

function IsFenceSign(Item)

  return Item.objtype >= UOBJ_FENCESIGN_START and Item.objtype <= UOBJ_FENCESIGN_END;

endfunction

//-----------------------------------------------------------------------------

function GetFenceTroughs(Sign)

  var Troughs := GetObjProperty(Sign,"Troughs");

  if(!Troughs or !Troughs.size())
    Troughs := {};
  endif

  var i := 1;
  foreach Tr in GetObjProperty(Sign,"Troughs")
    if(!SystemFindObjectBySerial(CInt(Tr)))
      Troughs.erase(i);
    else
      i := i + 1;
    endif
  endforeach

  SetObjProperty(Sign,"Troughs",Troughs);

  return Troughs;

endfunction

//-----------------------------------------------------------------------------

function DestroyFence(Cont, sign, CrDeed := 1)

  var Created := GetObjProperty(sign, "fenceparts");
  var Type    := GetObjProperty(sign, "fencetype");
  var Troughs := GetFenceTroughs(sign);
  var Item;

  foreach ItemSer in Created
    Item := SystemFindObjectBySerial(ItemSer);
    if(Item)
      DestroyItem(Item);
    endif
  endforeach

  foreach ItemSer in Troughs
    Item := SystemFindObjectBySerial(ItemSer);
    if(Item)
      DestroyItem(Item);
    endif
  endforeach
  
  DestroyItem(sign);

  if(CrDeed)
    return CreateItemInCont(Cont, type);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
