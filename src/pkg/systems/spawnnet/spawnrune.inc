//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

const MAX_CREATE_TRYES       := 25;
const NEXT_CHECK_TIME        := 600;

const RUNE_CH_GRAPHIC        := 0x20f8;
const NPC_GROUP_CFG          := ":spawnnet:groups";
const ITEM_GROUP_CFG         := ":spawnnet:igroups";
const SPAWNRUNES_CFG         := ":datacfg:spawnrunes";
const SPAWNCHEST_CFG         := ":datacfg:spawnchests";
const SCHEST_CFG             := ":spawnnet:cgroups";
const SR_SER_FLAG            := "SpawnRuneNr_";

const DEF_SPAWN_DELAY_PERC   := 4;

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

const NPC_SPAWN              := "Npc";
const NPCGROUP_SPAWN         := "NpcGroup";
const ITEM_SPAWN             := "Item";
const CHEST_SPAWN            := "Chest";

//-----------------------------------------------------------------------------
//  Flags
//-----------------------------------------------------------------------------

const RUNEF_FROZEN           := 0x0001;
const RUNEF_BASE_Z           := 0x0002;
const RUNEF_NOCITY           := 0x0004;
const RUNEF_ONLYCITY         := 0x0008;
const RUNEF_BITEMS           := 0x0010;
const RUNEF_NOWANDER         := 0x0020;
const RUNEF_GUARDIGNORE      := 0x0040;

//-----------------------------------------------------------------------------
// CProperties
//-----------------------------------------------------------------------------

const PROP_CREATED_OBJECTS   := "CreatedCreatures";

//-----------------------------------------------------------------------------

function CheckExistingObjects(rune)

  var CreatedObjects := GetObjProperty(rune, PROP_CREATED_OBJECTS);
  if(!CreatedObjects or !CreatedObjects.size())
    CreatedObjects := {};
  else
    var i := 1;
    foreach serial in GetObjProperty(rune,PROP_CREATED_OBJECTS)
      if(!SystemFindObjectBySerial(Cint(serial)))
		PrintTextAbove(rune, "Cannot find serial " + serial + " erasing!");
        CreatedObjects.erase(i);
      else
        i := i + 1;
      endif
    endforeach
  endif

  SetObjProperty(rune,PROP_CREATED_OBJECTS,CreatedObjects);

  return CreatedObjects;

endfunction

//-----------------------------------------------------------------------------

function GetRuneNr(rune)
  return CInt(GetObjProperty(rune,"RuneNr"));
endfunction

//-----------------------------------------------------------------------------

function FindRuneByNr(Nr)

  var RuneProp := GetGlobalProperty(SR_SER_FLAG+CStr(Nr));

  if(RuneProp == "Empty")
    return 0;
  endif

  if(RuneProp and RuneProp.size() and !RuneProp[5])
    var Rune := FindItemBySerial(RuneProp[4]);
    if(Rune)
      RuneProp[5] := Rune.realm;
      SetGlobalProperty(SR_SER_FLAG+CStr(Nr), RuneProp);
      return Rune;
    else
      RuneProp[5] := _DEFAULT_REALM;
      LogNotice("[Spawnrune] Rune Nr: ["+Nr+"] nie posiada Realm, a runa nie mozna znalezc ["+RuneProp+"]", LOGF_SCRIPTS);
    endif
  endif

  if(RuneProp and RuneProp.size())
    foreach SRune in ListItemsNearLocationOfType(CInt(RuneProp[1]),CInt(RuneProp[2]),CInt(RuneProp[3]),0,UOBJ_SRUNE, RuneProp[5])
      if(CInt(RuneProp[4]) == SRune.serial)
        return SRune;
      endif
    endforeach

    return FindItemBySerial(RuneProp[4]);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetMaxSRuneNr()

  if(!CInt(GetGlobalProperty("LastSpawnRuneNr")))

    var i;

    for(i:=2;i<=20000;i:=i+1)
      if(!GetGlobalProperty(SR_SER_FLAG+CStr(i)))
        SetGlobalProperty("LastSpawnRuneNr",CInt(i - 1));
        break;
      endif
    endfor
  endif

  return CInt(GetGlobalProperty("LastSpawnRuneNr"));

endfunction

//-----------------------------------------------------------------------------

function IsRunning(rune)
  return CInt(GetObjProperty(rune,"running"));
endfunction

//-----------------------------------------------------------------------------

function SetRunning(rune)
  SetObjProperty(rune,"running",ReadGameClock());
  if(GetObjProperty(rune,"lasttype") != GetRuneTemplate(rune))
    AnnihilateSpawns(rune);
  endif
  if(IsStopped(rune))
    SetSpawnTime(rune,GetSpawnTime(rune) + (ReadGameClock() - IsStopped(rune)));
  else
    SetSpawnTime(rune,ReadGameClock());
  endif
  EraseObjProperty(rune,"stopped");
endfunction

//-----------------------------------------------------------------------------

function IsStopped(rune)
  return CInt(GetObjProperty(rune,"stopped"));
endfunction

//-----------------------------------------------------------------------------

function SetStopped(rune)
  EraseObjProperty(rune,"running");
  SetObjProperty(rune,"stopped",ReadGameClock());
  SetObjProperty(rune,"lasttype",GetRuneTemplate(rune));
endfunction

//-----------------------------------------------------------------------------

function GetRuneType(rune)
  return GetObjProperty(rune,"type");
endfunction

//-----------------------------------------------------------------------------

function SetRuneType(rune,type)
  if(type)
    SetObjProperty(rune,"type",type);
  endif
endfunction

//-----------------------------------------------------------------------------

function GetRuneTemplate(rune)
  return GetObjProperty(rune,"template");
endfunction

//-----------------------------------------------------------------------------

function SetRuneTemplate(rune,template)
  if(template)
    SetObjProperty(rune,"template",template);
    return 1;
  endif
  return error;
endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Runes Props
//-----------------------------------------------------------------------------
//=============================================================================

function GetRuneRespawnTime(rune)
  return CInt(GetObjProperty(rune,"respawntime"));
endfunction

//-----------------------------------------------------------------------------

function SetRuneRespawnTime(rune,respawntime)
  if(CInt(respawntime) > 0)
    SetObjProperty(rune,"respawntime",CInt(respawntime));
    return 1;
  endif
  return error;
endfunction

//-----------------------------------------------------------------------------

function GetSpawnTime(rune)
  return CInt(GetObjProperty(rune,"lastspawntime"));
endfunction

//-----------------------------------------------------------------------------

function SetSpawnTime(rune,time)
  if(CInt(time))
    SetObjProperty(rune,"lastspawntime",CInt(time));
    return 1;
  endif
  return error;
endfunction

//-----------------------------------------------------------------------------

function DelaySpawn(rune, DelayPerc := DEF_SPAWN_DELAY_PERC)

  if(DelayPerc <= 0)
    return;
  elseif(DelayPerc > 100)
    DelayPerc := 100;
  endif

  var DelayTime := CInt( (GetRuneRespawnTime(Rune) * 60) * (DelayPerc * 0.01) );

  DelayTime := CInt(DelayTime/2);
  DelayTime := DelayTime + RandomInt(DelayTime);

  SetSpawnTime(rune, GetSpawnTime(Rune) + DelayTime);

endfunction

//-----------------------------------------------------------------------------

function GetRuneMaxAmount(rune)
  return CInt(GetObjProperty(rune,"maxamount"));
endfunction

//-----------------------------------------------------------------------------

function SetRuneMaxAmount(rune,MaxAmount)
  if(CInt(MaxAmount) >= 1)
    SetObjProperty(rune,"maxamount",CInt(MaxAmount));
    return CheckSpawnAmount(rune);
  endif
  return error;
endfunction

//-----------------------------------------------------------------------------

function GetRuneAmount(rune)
  return CInt(GetObjProperty(rune,"amount"));
endfunction

//-----------------------------------------------------------------------------

function SetRuneAmount(rune,Amount)
  if(CInt(Amount) >= 1 and CInt(Amount) <= GetRuneMaxAmount(rune))
    SetObjProperty(rune,"amount",CInt(Amount));
    return 1;
  endif
  return error;
endfunction

//-----------------------------------------------------------------------------

function GetRuneRange(rune)
  return CInt(GetObjProperty(rune,"range"));
endfunction

//-----------------------------------------------------------------------------

function SetRuneRange(rune,Range)
  if(IsSetFlag(Rune,RUNEF_BASE_Z))
    Range := 0;
  endif
  if(CInt(Range) >= 0)
    SetObjProperty(rune,"range",CInt(Range));
    return 1;
  endif
  return error;
endfunction

//-----------------------------------------------------------------------------

function GetRuneFacing(Rune)

  if(!CInt(GetObjProperty(Rune,"Fac")))
    return "N/A";
  else
    return CInt(GetObjProperty(Rune,"Fac")) - 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetRuneFacing(Rune,Fac)

  SetObjProperty(Rune,"Fac",Fac + 1);

endfunction

//-----------------------------------------------------------------------------

function EraseRuneFacing(Rune)

  EraseObjProperty(Rune,"Fac");

endfunction

//-----------------------------------------------------------------------------

function GetRuneOverColor(Rune)

  return CInt(GetObjProperty(Rune,"OverColor"));

endfunction

//-----------------------------------------------------------------------------

function SetRuneOverColor(Rune,Color)

  SetObjProperty(Rune,"OverColor",CInt(Color));

endfunction

//-----------------------------------------------------------------------------

function GetCheckedType(rune)

  return GetObjProperty(rune,"CheckedType");

endfunction

//-----------------------------------------------------------------------------

function SetCheckedType(rune,type)

  if(type)
    SetObjProperty(rune,"CheckedType",type);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckRuneType(rune)

  if(GetRuneType(rune) != GetCheckedType(rune) or !GetRuneType(rune) or !GetCheckedType(rune))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsSpawning(Rune)

  if(ExPid(GetObjProperty(Rune,"#SpawnPid")))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function DestroyChestSpawns(Chest)

  foreach item in EnumerateItemsInContainer(Chest)
    DestroyItem(item);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function HaveRuneLiveSpawns(rune)

  return CheckExistingObjects(rune).size() > 0;

endfunction

//-----------------------------------------------------------------------------
