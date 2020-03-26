//-----------------------------------------------------------------------------

include "spawnrune";

//-----------------------------------------------------------------------------

const SC_SER_FLAG            := "SpawnChestNr_";
const MAX_CHEST_LEVEL        := 6;

//-----------------------------------------------------------------------------

function GetChestNr(rune)
  return CInt(GetObjProperty(rune,"ChestNr"));
endfunction

//-----------------------------------------------------------------------------

function FindChestByNr(Nr)

  var ChestProp := GetGlobalProperty(SC_SER_FLAG+CStr(Nr));

  if(ChestProp == "Empty")
    return 0;
  endif

  if(!ChestProp[5])
    var Chest := FindItemBySerial(ChestProp[4]);
    if(Chest)
      ChestProp[5] := Chest.realm;
      SetGlobalProperty(SC_SER_FLAG+CStr(Nr), ChestProp);
      return Chest;
    else
      ChestProp[5] := _DEFAULT_REALM;
      LogNotice("[SpawnChest] Chest Nr: ["+Nr+"] nie posiada Realm, a skrzyni nie mozna znalezc ["+ChestProp+"]", LOGF_SCRIPTS);
    endif
  endif

  if(ChestProp.size())
    foreach SChest in ListItemsNearLocationOfType(CInt(ChestProp[1]),CInt(ChestProp[2]),CInt(ChestProp[3]),0,UOBJ_SCHEST, ChestProp[5])
      if(CInt(ChestProp[4]) == SChest.serial)
        return SChest;
      endif
    endforeach

    return FindItemBySerial(ChestProp[4]);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetMaxSChestNr()

  if(!CInt(GetGlobalProperty("LastSpawnChestNr")))

    var i;

    for(i:=2;i<=20000;i:=i+1)
      if(!GetGlobalProperty(SC_SER_FLAG+CStr(i)))
        SetGlobalProperty("LastSpawnChestNr",CInt(i - 1));
        break;
      endif
    endfor
  endif

  return CInt(GetGlobalProperty("LastSpawnChestNr"));

endfunction

//-----------------------------------------------------------------------------

function LoadChestSpawnData(who,Chest)

  var SpawnProps := GetObjProperty(who, "SpawnChestProps");
  if(!SpawnProps)
    SendSysMessage(who, "Ostatnie dane nie istanieja!",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  SetRuneRespawnTime(Chest,   SpawnProps[1]);
  SetRuneTemplate(Chest,      SpawnProps[1]);
  if(SpawnProps[3])
    SetRunning(Chest);
  elseif(SpawnProps[4])
    SetStopped(Chest);
  endif
  
  SendSysMessage(who,"Ostatnie dane wczytane.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function CheckChestSpawn(who,Chest,Group)

  var ChestCfg := ReadConfigFile(SCHEST_CFG);
  var Items    := GetConfigStringArray(Chestcfg[Group],"Stack");
  if(!Items.size())
    SendSysMessage(who,"Grupa jest pusta!",FONT_NORMAL,COLOR_BAD);
    return;
  endif
  SetCheckedType(Chest,CHEST_SPAWN);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetChestRuneFromLink(Chest)

  var RuneSerial := GetObjProperty(Chest, "RuneSerial");

  return FindItemBySerial(RuneSerial);

endfunction

//-----------------------------------------------------------------------------

function SetChestRuneLink(Chest, Rune)

  if(Chest.objtype == UOBJ_SCHEST and Rune.objtype == UOBJ_SRUNE)
    SetObjProperty(Chest, "RuneSerial", Rune.serial);
  endif

endfunction

//-----------------------------------------------------------------------------
