//-----------------------------------------------------------------------------

const GNDF_BASIC             := 0x0001;
const GNDF_MODS              := 0x0002;
const GNDF_EXTENDED          := 0x0004;
const GNDF_STATS             := 0x0008;
const GNDF_EQUIPS            := 0x0010;
const GNDF_FULL              := 0x00FF;

//-----------------------------------------------------------------------------

function ListNpcProps(Npc)

  var List := GetObjPropertyNames(Npc);
  var Props := dictionary;
  var noSaveProps := array(PROP_CREATED, PROP_QUEST_ITEM, PROP_AISERIAL, "Anchor", "Graphic", PROP_FLAGS, PROP_CREATEDAT, "SetColor", PROP_NSET_CTRL_ID);

  foreach Name in List

    if(Name[1] != "#" and !(Name in noSaveProps))
      Props[Name] := GetObjProperty(Npc, Name);
    endif

  endforeach

  return Props;

endfunction

//-----------------------------------------------------------------------------

function GetNpcDefinition(Npc, Flags := GNDF_FULL, MidLoc := error)

  var Data := struct;

  if(Flags & GNDF_BASIC)
    Data.+name    := GetName(Npc, NAME_CLEAN_AMOUNT);
    Data.+color   := Npc.color;

    Data.+template := Npc.npctemplate;
    Data.+graphic  := CInt(Npc.graphic);
    Data.+facing   := CInt(Npc.facing);
    Data.+murderer := CInt(Npc.murderer);
    Data.+criminal := CInt(Npc.criminal);
    Data.+gender   := CInt(Npc.gender);
  endif

  if(Flags & GNDF_EXTENDED)
    Data.+props   := ListNpcProps(Npc);

    Data.+title_guild  := Npc.title_guild;
    Data.+title_prefix := Npc.title_prefix;
    Data.+title_suffix := Npc.title_suffix;
    Data.+title_race   := Npc.title_race;
  endif

  if(Flags & GNDF_MODS)
    Data.+run_speed := Npc.run_speed;
  endif

  if(Flags & GNDF_STATS)
  
    var maxSkId := GetMaxSkillId();
    var i;

    Data.+Skills := array;
    
    for(i:=0;i<=maxSkId;i:=i+1)
      Data.Skills[i+1] := GetBaseSkillBaseValue(Npc, i);
    endfor

    Data.+Stats := array;
    
    Data.Stats[1] := GetBaseStrength(Npc);
    Data.Stats[2] := GetBaseDexterity(Npc);
    Data.Stats[3] := GetBaseIntelligence(Npc);

    Data.+Vitals := array;

    Data.Vitals[1] := GetHp(Npc);
    Data.Vitals[2] := GetStamina(Npc);
    Data.Vitals[3] := GetMana(Npc);
    
    Data.+MaxVitals := array;

    Data.MaxVitals[1] := GetMaxHp(Npc);
    Data.MaxVitals[2] := GetMaxStamina(Npc);
    Data.MaxVitals[3] := GetMaxMana(Npc);

  endif

  if(MidLoc)
    Data.+x := Npc.x - MidLoc.x;
    Data.+y := Npc.y - MidLoc.y;
    Data.+z := Npc.z - MidLoc.z;
  else
    Data.+x := 0;
    Data.+y := 0;
    Data.+z := 0;
  endif

  Data.+Equips := array;

  if(Flags & GNDF_EQUIPS)
    foreach Item in ListEquippedItems(Npc)
      
      if(Item.objtype != UOBJ_NPCPACK)
        Data.Equips.append( GetItemDefinition(Item, GIDF_FULL | GIDF_NO_LOCATION) );
      endif

    endforeach
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------
// Tworzenie 
//-----------------------------------------------------------------------------

function CreateNpcFromDefinition(byref Res, Dest, NpcData)

  if(!Res.Npcs)
    Res := struct;
    Res.+Npcs := array;
  endif

  if(!NpcData.template or !NpcData.name)
    return error{"errortext" := "Nieprawidlowa definicja npcu ["+NpcData+"]. Brak objtype lub name!"};
  endif

  if(!Dest.exists("x") or !Dest.exists("y") or !Dest.exists("z") or !Dest.exists("realm"))
    return error{"errortext" := "Nieprawidlowa lokacja!"};
  endif

  Dest.x += NpcData.x;
  Dest.y += NpcData.y;
  Dest.z += NpcData.z;

  var Parms := struct;

  Parms.+color   := NpcData.color;
  Parms.+graphic := NpcData.graphic;
  Parms.+Name    := NpcData.name;
  Parms.+gender  := NpcData.gender;
  Parms.+facing  := NpcData.facing;
  Parms.+CProps  := NpcData.props;

  var Npc := CreateNpc(NpcData.template, Dest.x, Dest.y, Dest.z, Dest.realm, Parms, NPCF_NO_UPKEEP);

  if(!Npc)
    return error{"errortext" := "Nie udalo sie stworzyc npeca: "+NpcData.name+"/"+NpcData.template+": "+Npc.errortext};
  endif

  Npc.SetCriminal(NpcData.criminal);
  Npc.SetMurderer(NpcData.murderer);
  Npc.run_speed := NpcData.run_speed;
  
  Npc.title_prefix := NpcData.title_prefix;
  Npc.title_suffix := NpcData.title_suffix;
  Npc.title_race   := NpcData.title_race;
  Npc.title_guild  := NpcData.title_guild;

  var eqRes;
  var Loc := LocStruct();
  Loc.x := CR_TEMP_ITEM_X;
  Loc.y := CR_TEMP_ITEM_Y;
  Loc.z := CR_TEMP_ITEM_Z;
  Loc.realm := CR_TEMP_ITEM_REALM;

  foreach ItemData in (NpcData.Equips)

    var Item := CreateItemFromDefinition(eqRes, Loc, ItemData);

    if(!Item)
      return Item;
    endif

  endforeach

  foreach Item in (eqRes.Items)

    var eqpRes := EquipItem(Npc, Item);

    if(!eqpRes)
      foreach Item in (eqRes.Items)
        DestroyItem(Item);
      endforeach
      return RetError("Nie udalo sie zalozyc itema: "+GetName(Item)+"/"+hex(Item.objtype)+" Res: "+eqpRes.errortext);
    endif

  endforeach
  
  if(NpcData.Skills)
    var maxSkId := GetMaxSkillId();
    var i;

    for(i:=0;i<=maxSkId;i:=i+1)
      SetBaseSkillBaseValue(Npc, i, NpcData.Skills[i+1]);
    endfor
  endif

  if(NpcData.Stats)
    SetBaseStrength(Npc, NpcData.Stats[1]);
    SetBaseDexterity(Npc, NpcData.Stats[2]);
    SetBaseIntelligence(Npc, NpcData.Stats[3]);
  endif
  
  if(NpcData.MaxVitals)
    SetMaxHp(Npc, NpcData.MaxVitals[1]);
    SetMaxStamina(Npc, NpcData.MaxVitals[2]);
    SetMaxMana(Npc, NpcData.MaxVitals[3]);
  endif

  if(NpcData.Vitals)
    SetHp(Npc, NpcData.Vitals[1]);
    SetStamina(Npc, NpcData.Vitals[2]);
    SetMana(Npc, NpcData.Vitals[3]);
  endif

  Res.Npcs.append(Npc);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CreateNpcsFromDefinition(Dest, byref Npcs)

  var Res;

  foreach NpcData in Npcs

    var cRes := CreateNpcFromDefinition(Res, Dest, NpcData);

    if(!cRes)
      return cRes;
    endif

  endforeach

  return Res;

endfunction

//-----------------------------------------------------------------------------
