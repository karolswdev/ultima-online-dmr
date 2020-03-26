//--------------------------------------------------------------------------------

include "include/objects/location";

//================================================================================
//--------------------------------------------------------------------------------
// Creation
//--------------------------------------------------------------------------------
//================================================================================

const DEF_PARMS              := struct;

//--------------------------------------------------------------------------------

function CreateNpc(template, x, y, z ,Realm := _DEFAULT_REALM, Parms := DEF_PARMS, Flags := 0)

  x := CInt(x);
  y := CInt(y);

  if(!Parms or (TypeOf(Parms) != "Struct" and TypeOf(Parms) != "Array"))
    Parms := DEF_PARMS;
  endif
  
  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif

  if(x < 0 or y < 0 or z == error)
    return error{"errortext" := "Bledna pozycja"};
  endif

  var cfgElem := GetNpcConfig(Template);

  if(!cfgElem)
    return cfgElem;
  endif

  var Graphic := cfgElem.objtype;
  
  z := CInt(z);
  
  if(CInt(cfgElem.objtype) == CID_HUMAN_MALE or CInt(cfgElem.objtype) == CID_HUMAN_FEMALE or CInt(cfgElem.objtype) == CID_ELF_MALE or CInt(cfgElem.objtype) == CID_ELF_FEMALE)
    if(!Parms.gender)
      var Gender := cfgElem.Gender;
      if(!Gender)
        Gender := RandomInt(2)+1;
      endif
      Gender         := Gender - 1;
      Parms.+gender  := Gender;
      Parms.+graphic := cfgElem.objtype + Gender;
      Graphic        := Parms.graphic;
    endif
  endif

  if(!Parms.Name)
    var Name := ConvertName(cfgElem.Name,Graphic,CInt(cfgElem.NameIdx), template);
  
    if(!Name)
      LogError("CreateNpc: Blad imienia - Template: "+template+" N: "+Name);
      return 0;
    endif

    if(cfgElem.TitleIdx)
      Name := AssignTitle(Name,CInt(cfgElem.TitleIdx));
    endif
    
    Parms.+Name := Name;
  endif
  
  Parms.+concealed   := 1;
  
  var Color := cfgElem.colors;

  if(Color and !Parms.color)
    Color := SplitWords(Color);
    Color := Color[RandomDiceRoll("1d"+Color.size())];
    if(Color)
      Parms.+color := Color;
    endif
  endif

  var Npc := CreateNpcFromTemplate(template,x,y,z,Parms,Realm);
  
  if(!Npc)
    return Npc;
  endif

  if(Npc.backpack)
    DestroyItem(Npc.backpack);
  endif

  GetNpcBackpack(Npc);
  
  SetObjProperty(Npc, PROP_CREATEDAT, ReadGameClock());

  if(cfgElem.NoCorpse)
    SetFlag(Npc, NPCF_NO_CORPSE);
  endif

  if(cfgElem.NoLooting)
    SetFlag(Npc, NPCF_NO_LOOTING);
  endif

  /*
  __NONPVP__ DELETED
  if(cfgElem.TameSkill and IsInNonPvPArea(Npc.x, Npc.y, Npc.realm))
    SetFlag(Npc, NPCF_NO_LOOTING);
  endif
  */

  SetFlag(Npc,flags);
 

  if(!IsBitOn(Flags, NPCF_NO_UPKEEP))
    if(!start_script("::npcupkeep",Npc))
      LogError("CreateNpc: Nie mozna bylo uruchomic [npcupkeep], NpcKill");
      KillNPC(Npc);
      return error;
    endif
  else
    Npc.concealed := 0;
  endif

  if(!GetObjProperty(Npc, "SetColor"))
    SetObjProperty(Npc,"SetColor", Npc.color);
  endif

  if(!GetObjProperty(Npc, "Graphic"))
    SetObjProperty(Npc,"Graphic", Npc.graphic);
  endif

  SetObjProperty(Npc,"AiSerial", Npc.serial);
  
  return Npc;

endfunction

//--------------------------------------------------------------------------------

function CreateNpcAtTempLoc(Template, Parms := DEF_PARMS, Flags := 0)

  return CreateNpc(Template, CR_TEMP_NPC_X, CR_TEMP_NPC_Y, CR_TEMP_NPC_Z, CR_TEMP_NPC_REALM, Parms, Flags);

endfunction

//--------------------------------------------------------------------------------

function CreateRPNpc(Temp,Loc,Realm,Flags := 0, CrFlags := 0)

  var Mods := GetLocMods(2);

  var i := 1,Npc := 0, Rand, Stand;
  
  while(i <= 30 and !Npc)
    Rand  := RandomInt(Mods.size())+1;
    i     := i + 1;
    Stand := GetStand(Loc.x+Mods[Rand][1], Loc.y+Mods[Rand][2], Realm, Loc.z);

    if(Stand == error or Stand.z == error)
      continue;
    endif

    if(DistZ(Loc, Stand) >= 20)
      continue;
    endif

    if(CrFlags & CRNPCF_NOMULTI)
      if(Stand.multi)
        continue;
      endif
    endif

    if(CrFlags & CRNPCF_CHECKLOS)
      if(coordist(Loc.x, Loc.y, Loc.x+Mods[Rand][1], Loc.y+Mods[Rand][2]) > 1)
        if(!CheckSpecialLosAtPos(Loc.x, Loc.y, Loc.z, Realm, Loc.x+Mods[Rand][1], Loc.y+Mods[Rand][2], Stand.z, Realm))
          continue;
        endif
      endif
    endif

    Npc := CreateNpc(Temp,Loc.x+Mods[Rand][1],Loc.y+Mods[Rand][2],Stand.z,Realm,DEF_PARMS,Flags);
  endwhile

  return Npc;

endfunction

//--------------------------------------------------------------------------------
