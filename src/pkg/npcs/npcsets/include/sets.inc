//-----------------------------------------------------------------------------

include ":npcsets:shared";
include ":npcsets:definition";

include "include/portals";

//-----------------------------------------------------------------------------

const PROP_NPC_SETS         := "NpcSets";

//-----------------------------------------------------------------------------

const NST_NPC                := "npc";
const NST_LOCATION           := "loc";

//-----------------------------------------------------------------------------

const CNSF_QUEST_NPCS       := 0x01;
const CNSF_PLAYER_NPCS      := 0x02;
const CNSF_FROM_PORTAL      := 0x04;

//-----------------------------------------------------------------------------

function GetPersonNpcSets(who)

  var Sets := GetObjProperty(who, PROP_NPC_SETS);

  if(!Sets)
    Sets := dictionary;
  endif

  return Sets;

endfunction

//-----------------------------------------------------------------------------

function AddPersonNpcSet(who, Name, byref Data)

  var Sets := GetPersonNpcSets(who);

  Sets[Name] := Data;

  SetObjProperty(who, PROP_NPC_SETS, Sets);

endfunction

//-----------------------------------------------------------------------------

function RemoveNpcSet(who, Name)

  var Sets := GetPersonNpcSets(who);

  if(Sets.exists(Name))
    Sets.erase(Name);
    SetObjProperty(who, PROP_NPC_SETS, Sets);
    return 1;
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Funkcje generujace definicje seta
//-----------------------------------------------------------------------------
//=============================================================================

function NpcSetStruct()

  var Data := struct;

  Data.+Name    := "";
  Data.+Count   := 0;
  Data.+addTime := 0;
  Data.+Creator := 0;
  Data.+Npcs    := array;
  Data.+type    := 0;

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetNpcSetDefinitionFromNpc(who, Name, Npc)

  var Data := NpcSetStruct();

  Data.Name    := Name;
  Data.addTime := PolCore().systime;
  Data.Creator := who.acctname;

  Data.Npcs.append(GetNpcDefinition(Npc, GNDF_FULL));
  Data.Count   := 0;
  Data.type := NST_NPC;

  Data.Count := 1;

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetNpcSetDefinitionFromLoc(who, Name, nwLoc, seLoc, midLoc)

  var Data := NpcSetStruct();

  Data.Name    := Name;
  Data.addTime := PolCore().systime;
  Data.Creator := who.acctname;

  Data.Count   := 0;
  Data.type := NST_LOCATION;

  if(coordist(nwLoc.x, nwLoc.y, seLoc.x, seLoc.y) > 50)
    var Res := error;
    Res.errortext := "Zasieg pomiedzy wskazanymi lokacjami przekracza 50! To stanowczo za duzo.";
    return Res;
  elseif(nwLoc.realm != seLoc.realm)
    var Res := error;
    Res.errortext := "Wth?! Rozne realmy?";
    return Res;
  elseif(nwLoc.x > midLoc.x or seLoc.x < midLoc.x or nwLoc.y > midLoc.y or seLoc.y < midLoc.y)
    var Res := error;
    Res.errortext := "Wskazany srodek nie jest wewnatrz wybranego wczesniej zasiegu...";
    return Res;
  endif

  foreach Npc in ListObjectsInBox(nwLoc.x, nwLoc.y, -127, seLoc.x, seLoc.y, 127, nwLoc.realm)
    if(Npc.isa(POLCLASS_NPC))
      Data.Npcs.append(GetNpcDefinition(Npc, GNDF_FULL, midLoc));
      Data.Count := Data.Npcs.size();
    endif
  endforeach

  if(Data.Count == 0)
    var Res := error;
    Res.errortext := "Nie znaleziono npcow w wybranym zasiegu.";
    return Res;
  endif

  return Data;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Tworzenie setow
//-----------------------------------------------------------------------------
//=============================================================================

function CreateNpcFromSet(who, Set, Flags := 0)

  if(!Set or !Set.type)
    SendSysMessage(who, "Argh Bledne dane!", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Dest := LocTarget(who);

  if(!Dest)
    SendSysMessage(who, "Anulowano.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif
  
  var Portals := array;

  if(Flags & CNSF_FROM_PORTAL)
  
    PlaySoundEffect( who, SFX_SPELL_GATE_TRAVEL );


    foreach NpcData in (Set.Npcs)
      var Portal := CreatePortal(UOBJ_BLUEGATE, Dest.x+NpcData.x, Dest.y+NpcData.y, Dest.z+NpcData.z, Dest.realm, 0);
      Portals.append( Portal );
    endforeach

    ShowPortal(Portals);
    Sleep(1);
  endif

  var Res := CreateNpcsFromDefinition(Dest, Set.Npcs);

  if(!Res)
    SendSysMessage(who, CStr(Res.errortext), FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  SendSysMessage(who, "Stworzono set ["+Set.Name+"]. Ilosc npecow: "+Res.Npcs.size(), FONT_NORMAL, COLOR_WHITE);

  foreach Npc in (Res.Npcs)

    if(!(Flags & CNSF_PLAYER_NPCS))

      if(Flags & CNSF_QUEST_NPCS)
        SetPropQuestItem(who, Npc);
      else
        SetPropCreated(who, Npc);
      endif

      LogMsg(QSITEMS_LOG, who, npc);
    endif

  endforeach

  if(Flags & (CNSF_PLAYER_NPCS | CNSF_QUEST_NPCS))
    if(Flags & CNSF_QUEST_NPCS)
      var ctrlRes := AddNpcSetControl(who, Set, Res.Npcs);
      if(ctrlRes)
        SendSysMessage(who, "Dodano npcy do globalnej kontroli.", FONT_NORMAL, COLOR_BLUE);
      else
        SendSysMessage(who, "Wystapil blad podczas dodawania npcow do globalnej kontroli! Blad: "+ctrlRes.errortext, FONT_NORMAL, COLOR_RED);
      endif
    endif

    SendSysMessage(who, "Zalogowano utworzenie "+Res.Npcs.size()+" npcow.", FONT_NORMAL, COLOR_ORANGE);
  endif

  if(Portals.size())
    Sleep(15);
    HidePortal(Portals);

    foreach Portal in Portals
      DestroyItem(Portal);
    endforeach
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
