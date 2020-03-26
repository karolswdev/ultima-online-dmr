//--------------------------------------------------------------------------------

function BlankOn(who,Bed)

  SendSysMessage(who,"Scielisz lozko.",FONT_NORMAL,COLOR_GREEN);

  if(!ReserveBed(Bed))
    SendSysMessage(who,"Ktos juz cos robi z tym lozkiem.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!MakeNoise(who))
    return 0;
  endif

  ChangeBedGraphic(who,Bed,"Blanket");
  SendSysMessage(who,"Poscielil"+ggm(who,3)+" lozko.",FONT_NORMAL,COLOR_BLUE);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function ChangeBedGraphic(who,Bed,Type)

  var Parts := GetBedPartsSerials(Bed);

  foreach Part in Parts
    Part := FindItemBySerial(Part);
    if(!ItemCfg[Part.objtype])
      LogError("ChangeBedGraphic: Nie zdefiniowany Bed ObjType ["+Part.objtype+"]");
    else
      if(!GetConfigInt(ItemCfg[Part.objtype], Type))
        LogError("ChangeBedGraphic: Nie zdefiniowany Typ ["+Type+"] Lozka ["+Part.objtype+"]");
      else
        Part.graphic := GetConfigInt(ItemCfg[Part.objtype], Type);
        SetObjProperty(Part, Type, who.serial);
        if(Type == "Blanket")
          EraseObjProperty(Part,"Dirty");
          EraseObjProperty(Part,"Pillowed");
        endif
      endif
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------

function ReserveBed(Bed)

  var Parts := GetBedPartsSerials(Bed);

  if(!Parts)
    return 0;
  endif

  foreach Part in Parts
    Part := FindItemBySerial(Part);
    if(!ReserveItem(Part))
      return 0;
    endif
  endforeach
  
  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function GetBedPartsSerials(Bed)
  
  if(GetObjProperty(Bed, "#Crap"))
    return 0;
  endif

  var Parts := GetObjProperty(Bed, "PartsSerials");

  if(!Parts or !Parts.size())
    var Items := FindBedItems(Bed);
    if(!Items)
      //SetObjProperty(Bed, "#Crap", 1);
      return 0;
    endif

    Parts := array;

    foreach Item in Items
      Parts.append(Item.serial);
    endforeach

    foreach Item in Items
      SetObjProperty(Item, "PartsSerials", Parts);
    endforeach
  endif

  return Parts;

endfunction

//--------------------------------------------------------------------------------

function FindBedItems(Bed)

  var ObjTypes := FindDeedObjTypes(Bed.objtype);

  if(!ObjTypes)
    return 0;
  endif

  var Items := array;
  var Loc   := struct;

  foreach Obj in ObjTypes
    if(CheckBedObjType(_Obj_iter, Bed.objtype))
      Loc.+x := Bed.x - Obj[1];
      Loc.+y := Bed.y - Obj[2];
      Loc.+z := Bed.z - Obj[3];
      break;
    endif
  endforeach

  foreach Obj in ObjTypes
    foreach Item in ListItemsAtLocation(Loc.x+Obj[1], Loc.y+Obj[2], Loc.z+Obj[3], Bed.realm)
      if(CheckBedObjType(_Obj_iter, Item.objtype))
        Items.append(Item);
        break;
      endif
    endforeach
  endforeach

  if(Items.size() == ObjTypes.size())
    return Items;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function FindDeedObjTypes(ObjType)

  var ObjTypes;
  var DeedCfg := ReadConfigFile(":carpentry:deeds");
  
  if(!CheckBedDeedObjs(DeedCfg, ObjTypes, ObjType, 0xB009))
    if(!CheckBedDeedObjs(DeedCfg, ObjTypes, ObjType, 0xB012))
      if(!CheckBedDeedObjs(DeedCfg, ObjTypes, ObjType, 0xB013))
        return 0;
      endif
    endif
  endif

  return ObjTypes;

endfunction

//--------------------------------------------------------------------------------

function CheckBedDeedObjs(DeedCfg, byref ObjTypes, BedObj, DeedObj)

  if(DeedCfg[DeedObj].ObjType_S)
    ObjTypes := SplitWords(DeedCfg[DeedObj].ObjType_S);
    if(IsInBedDeedObjTypes(ObjTypes, BedObj))
      BuildObjTypes(ObjTypes, SplitWords(DeedCfg[DeedObj].Mods_S));
      return 1;
    endif
  endif

  if(DeedCfg[DeedObj].ObjType_E)
    ObjTypes := SplitWords(DeedCfg[DeedObj].ObjType_E);
    if(IsInBedDeedObjTypes(ObjTypes, BedObj))
      BuildObjTypes(ObjTypes, SplitWords(DeedCfg[DeedObj].Mods_E));
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInBedDeedObjTypes(byref ObjTypes, BedObj)

  foreach Obj in ObjTypes
    if(CheckBedObjType(CInt(Obj), BedObj))
      return 1;
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function BuildObjTypes(byref ObjTypes, Mods)

  var Objs := ObjTypes;
  ObjTypes := dictionary;

  MakeIntArray(Mods);

  foreach Obj in Objs
    ObjTypes[CInt(Obj)] := array(Mods[(_Obj_iter-1)*3+1], Mods[(_Obj_iter-1)*3+2], Mods[(_Obj_iter-1)*3+3]);
  endforeach

endfunction

//--------------------------------------------------------------------------------

function CheckBedObjType(ObjType, BedObj)

  if(ObjType == BedObj)
    return 1;
  elseif(ItemCfg[ObjType].Blanket == BedObj)
    return 1;
  elseif(ItemCfg[ObjType].Dirty == BedObj)
    return 1;
  elseif(ItemCfg[ObjType].Pillowed == BedObj)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CheckSleepLogon(who)

  if(GetObjProperty(who,"BedSleep"))
    SendSysMessage(who, "Budzisz sie ze snu.",FONT_NORMAL,COLOR_GREEN);
    var Bed := FindItemBySerial(GetObjProperty(who,"BedSleep"));
    if(Bed)
      ChangeBedGraphic(who,Bed,"Dirty");
      EraseObjProperty(Bed,"Sleeping");
    endif
    EraseObjProperty(who,"BedSleep");
  endif

endfunction

//--------------------------------------------------------------------------------
