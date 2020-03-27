//-----------------------------------------------------------------------------

use cfgfile;

//-----------------------------------------------------------------------------

include "include/effects";
include "include/player/notes";
include "common";

//-----------------------------------------------------------------------------

const UOBJ_JAILMESS    := 0xe3a;

const JAIL_CFG           := ":jail:jail";
const PROP_JAILEDPERSONS := "JailedPersons";

//-----------------------------------------------------------------------------

function GetJailGroups()

  var JailCfg   := ReadConfigFile(JAIL_CFG);
  var cfgGroups := GetConfigStringKeys(JailCfg);
  var Groups    := array;

  foreach Group in cfgGroups

    if(JailCfg[Group].Status)
      Groups.append(Group);
    endif

  endforeach

  return Groups;

endfunction

//-----------------------------------------------------------------------------

function GetJailGroupStatus(GroupName)

  var JailCfg := ReadConfigFile(JAIL_CFG);
  
  return JailCfg[GroupName].Status;

endfunction

//-----------------------------------------------------------------------------

function GetJailExitCoords(GroupName)

  var JailCfg := ReadConfigFile(JAIL_CFG);
  
  var ExitCoords := SplitWords(JailCfg[GroupName].ExitPlace);
  
  ExitCoords[1] := CInt(ExitCoords[1]);
  ExitCoords[2] := CInt(ExitCoords[2]);
  ExitCoords[3] := CInt(ExitCoords[3]);
  ExitCoords[4] := ExitCoords[4];

  return ExitCoords;

endfunction

//-----------------------------------------------------------------------------

function GetJailCoords(GroupName)

  var JailCfg    := ReadConfigFile(JAIL_CFG);
  var JailCoords := GetConfigStringArray(JailCfg[GroupName], "Place");

  foreach JailCoord in JailCoords
    JailCoord  := SplitWords(JailCoord);

    JailCoord[1] := CInt(JailCoord[1]);
    JailCoord[2] := CInt(JailCoord[2]);
    JailCoord[3] := CInt(JailCoord[3]);
  endforeach

  return JailCoords;

endfunction

//-----------------------------------------------------------------------------

function GetJailCoord(Cell)

  var JailCfg := ReadConfigFile(JAIL_CFG);
  var Coords  := GetJailCoords(Cell[1]);

  return Coords[Cell[2]];

endfunction

//-----------------------------------------------------------------------------

function GetJailTimes()

  var JailCfg := ReadConfigFile(JAIL_CFG);
  return GetConfigStringArray(JailCfg["Times"],"Time");

endfunction

//-----------------------------------------------------------------------------

function IsJailPlaceTaken(JailCoord)

  var Jailed := GetGlobalProperty(PROP_JAILEDPERSONS);

  if(!Jailed)
    return 0;
  endif

  foreach JailEntry in Jailed

    if(JailEntry["Coords"] == JailCoord)
      return 1;
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetJailed(Victim, Cell)

  var Jailed := GetGlobalProperty(PROP_JAILEDPERSONS);

  if(!Jailed)
    Jailed := dictionary;
  endif

  var JailEntry := dictionary;
  
  JailEntry["Coords"] := GetJailCoord(Cell);
  JailEntry["Cell"]   := Cell;

  Jailed[Victim.serial] := JailEntry;

  SetGlobalProperty(PROP_JAILEDPERSONS, Jailed);
  SetObjProperty(Victim, PROP_JAILED, 1);

endfunction

//-----------------------------------------------------------------------------

function EraseJailed(whoSerial)

  var Jailed    := GetGlobalProperty(PROP_JAILEDPERSONS);
  var JailEntry := Jailed[whoSerial];

  var Person := FindPlayerBySerial(whoSerial);
  var Scroll;

  if(Person)

    var JailProps := GetJailProps(Person);
    Scroll        := FindItemBySerial(JailProps[6]);

    EraseObjProperty(Person,PROP_JAILED);
    EraseObjProperty(Person,"JailProps");

  endif

  // jesli nie ma who (skasowana postac np.) 
  // lub zaginal prop - szukamy scrolla bezposrednio w danej lokacji

  if(!Scroll)

    var JailCoords := JailEntry["Coords"];

    foreach Item in ListItemsAtLocation( CInt(JailCoords[1])+1, CInt(JailCoords[2]), CInt(JailCoords[3]), JailCoords[4]);
      if(Item.objtype == UOBJ_JAILMESS)
        DestroyItem(Item);
      endif
    endforeach
    
  else

    DestroyItem(Scroll);

  endif

  Jailed.erase(whoSerial);
  SetGlobalProperty(PROP_JAILEDPERSONS, Jailed);

endfunction

//-----------------------------------------------------------------------------

function LookForFreeJail(Victim)

  var Groups := GetJailGroups();

//wylaczony podzial na redow i blue
//  var IsMurd := IsMurderer(Victim);
//  var LookIn := 0;

  foreach GroupName in Groups
    
    // sprawdzamy, czy status osoby jest odpowiedni do grupy jaili
//    case(GetJailGroupStatus(GroupName))
//      "Murderer":   LookIn := IsMurd;
//      "Order":      LookIn := !IsMurd;
 //   endcase

//    if(LookIn)
      
      var Coords := GetJailCoords(GroupName);
      var i;

      for(i:=1; i<=Coords.size(); i:=i+1)

        // sprawdzamy, czy ta cela jest zajeta
        if(!IsJailPlaceTaken(Coords[i]))

          // jesli nie, zwracamy array identyfikujacy cele
          return array(GroupName, i);
        endif
        
      endfor

 //   endif

  endforeach

  return error;

endfunction

//-----------------------------------------------------------------------------

function FreeFromJail(who)

  if(!IsJailed(who))
    return;
  endif

  var JailProps  := GetJailProps(who);
  var JailedFrom := JailProps[1];

  if(!JailedFrom[4])
    JailedFrom[4] := 1;
  endif

  sleep(5);

  PlayStationaryEff(who.x,who.y,who.z, 0x3709, 0x0a, 0x1e, 0, who.realm );
  PlaySoundEffect(who, 0xf8);
  sleep(1);

  EraseJailed(who.serial);
  who.frozen := 0;


  var ExitCoords := GetJailExitCoords(JailProps[7][1]);
  
  MoveObject(who, ExitCoords[1], ExitCoords[2], ExitCoords[3], ExitCoords[4], MOVECHAR_FORCELOCATION);

print(ExitCoords);
  
  // Castle Rock
  //MoveObject(who, 1726, 860, 0, "britannia");

  who.frozen := 0;

  SendSysMessage(who, "Ciesz sie wolnoscia i lepiej by twoje przewinienie sie nie powtorzylo!",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function Jail(who, victim, Time, Cell, Reason)

  if(!IsPlayer(Victim))
    SendSysMessage(who, "To nie gracz!", FONT_NORMAL, COLOR_RED);
    return;
  endif

  if(IsJailed(Victim))
    SendSysMessage(who, "Ta osoba jest zamknieta!", FONT_NORMAL, COLOR_RED);
    return;
  endif

  Time := CInt(Time);

  if(Cell == 1)

    Cell := LookForFreeJail(Victim);

    if(!Cell)
      SendSysMessage(who,"Nie ma juz wolnych cel!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  
  endif

  if(!Victim or !Time or !Cell or !Reason)
    SendSysMessage(who,"Brak odpowiednich danych.",FONT_NORMAL,COLOR_BAD);
    return 0;
  endif

  var JailCoord := GetJailCoord(Cell);

  if(!JailCoord[1] or !JailCoord[2])
    SendSysMessage(who,"Lokacja jest nieprawidlowa.",FONT_NORMAL,COLOR_BAD);
    return 0;
  endif

  if(IsJailPlaceTaken(JailCoord))
    SendSysMessage(who,"Ktos juz siedzi w tej celi!",FONT_NORMAL,COLOR_GOOD);
    return 0;
  endif

  PlaySoundEffect(victim, SFX_SPELL_LIGHTNING);
  PlayLightningBoltEffect(victim);
  sleep(1);

  if(!victim)
    SendSysMessage(who, "Cel nie istnieje!", FONT_NORMAL, COLOR_GREEN);
    return 0;
  endif

  var Scroll     := CreateItemAtLoc(JailCoord[1]+1, JailCoord[2], JailCoord[3], UOBJ_JAILMESS, 1, JailCoord[4]);
  Scroll.movable := 0;
  SetScrollName(Scroll, who, Victim, Reason, Time);

  var JailProps := {};

  JailProps[1]  := {Victim.x, Victim.y, Victim.z, Victim.realm};
  JailProps[2]  := ReadGameClock();
  JailProps[3]  := Time;
  JailProps[4]  := Reason;
  JailProps[5]  := who.serial;
  JailProps[6]  := Scroll.serial;
  JailProps[7]  := Cell;

  var JailMsg   := GetRealName(Victim)+" [loc: "+Victim.x+","+Victim.y+","+Victim.z+","+Victim.realm+"] czas: "+RealHourTime(Time)+" powod: '"+Reason+"'.";

  AddPlayerNote(who, Victim, NOTETYPE_JAIL, JailMsg);
  SetJailProps(Victim, JailProps);
  SetJailed(Victim, Cell);

  MoveObject(victim, JailCoord[1], JailCoord[2], JailCoord[3], JailCoord[4], MOVECHAR_FORCELOCATION);
  Victim.frozen := 1;
  SendSysMessage(victim, "Zostal"+ggm(victim,3)+" zamkniet"+ggm(victim,6)+" w wiezieniu za: "+Reason+".",FONT_NORMAL,COLOR_RED);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetScrollName(Scroll, Gm,Victim,Reason,Time)

  SetName(Scroll,"Zostal"+ggm(victim,3)+" zamkniet"+ggm(victim,6)+" " + victim.name + " w wiezieniu");
  SetToolTipProp(Scroll, "Czas", RealHourTime(Time));
  SetToolTipProp(Scroll, "Powod", Reason);
  SetToolTipProp(Scroll, "Sedzia", Gm.name);

endfunction

//-----------------------------------------------------------------------------

function GetJailProps(who)

  return GetObjProperty(who,"JailProps");

endfunction

//-----------------------------------------------------------------------------

function SetJailProps(who, Props)

  SetObjProperty(who,"JailProps", Props);

endfunction

//-----------------------------------------------------------------------------
