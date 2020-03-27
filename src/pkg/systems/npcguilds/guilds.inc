//-----------------------------------------------------------------------------
// Modified: 2005-05-28
//-----------------------------------------------------------------------------

include "include/mobile/age";
include "include/mobile/titles";

include "guildsdata";
include "exping";

//-----------------------------------------------------------------------------

function AddNpcGuild(byref GdData, who := 0)

  var Res := VerifyNpcGuild(GdData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data   := GetNGdData();
  var NextId := GetNextNpcGuildID();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac Daty Gildii! ["+Data+"]";
    return Res;
  endif

  if(!NextId)
    Res.errortext := "Nie udalo sie pobrac NextID Gildii";
    return Res;
  endif

  var NewGuild := Data.CreateElement(NGUILD_TITLE+NextId);

  if(!NewGuild)
    Res.errortext := "Nie udalo sie stworzyc nowego elementu Gildii ["+NewGuild+"]";
    return Res;
  endif

  NewGuild.SetProp(GDPROP_NAME,     GdData.Name);
  NewGuild.SetProp(GDPROP_TYPE,     GdData.Type);
  NewGuild.SetProp(GDPROP_STATUS,   GdData.Status);
  NewGuild.SetProp(GDPROP_QTYPES,   GdData.QuestTypes);
  NewGuild.SetProp(GDPROP_BUILDINGS,GdData.Buildings);
  NewGuild.SetProp(GDPROP_MEMBERS,  GdData.Members);
  NewGuild.SetProp(GDPROP_SKIDS,    GdData.SkIDs);
  NewGuild.SetProp(GDPROP_MAINSKIDS,GdData.MainSkIDs);
  NewGuild.SetProp(GDAPROP_FLAGS,   GdData.Flags);
  NewGuild.SetProp(GDPROP_MASTER,   GdData.Master);
  NewGuild.SetProp(GDPROP_CITIES,   GdData.Cities);
  NewGuild.SetProp(GDPROP_BOARD,    GdData.Board);

  SetNGdElemCrInfo(NewGuild, GDGPROP_GDCRINFO, who, NextID);

  return NextId;

endfunction

//-----------------------------------------------------------------------------

function SetNpcGuild(GdId, byref GdData, who := 0)

  var Res := VerifyNpcGuild(GdData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data := GetNGdData();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac Daty Gildii! ["+Data+"]";
    return Res;
  endif

  var Guild := Data.FindElement(NGUILD_TITLE+GdId);

  if(!Guild)
    Res.errortext := "Nie znaleziono Gildii ID: ["+Guild+"]";
    return Res;
  endif

  Guild.SetProp(GDPROP_NAME,     GdData.Name);
  Guild.SetProp(GDPROP_TYPE,     GdData.Type);
  Guild.SetProp(GDPROP_STATUS,   GdData.Status);
  Guild.SetProp(GDPROP_QTYPES,   GdData.QuestTypes);
  Guild.SetProp(GDPROP_BUILDINGS,GdData.Buildings);
//  Guild.SetProp(GDPROP_MEMBERS,  GdData.Members);
  Guild.SetProp(GDPROP_SKIDS,    GdData.SkIDs);
  Guild.SetProp(GDPROP_MAINSKIDS,GdData.MainSkIDs);
  Guild.SetProp(GDAPROP_FLAGS,   GdData.Flags);
  Guild.SetProp(GDPROP_MASTER,   GdData.Master);
  Guild.SetProp(GDPROP_CITIES,   GdData.Cities);
  Guild.SetProp(GDPROP_BOARD,    GdData.Board);

  SetNGdElemUpInfo(Guild, GDGPROP_GDUPINFO, who, GdId);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteNpcGuild(GdId)

  var Data := GetNGdData();
  var Res;

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac Daty Gildii! ["+Data+"]";
    return Res;
  endif

  var Guild := Data.FindElement(NGUILD_TITLE+GdId);

  if(!Guild)
    Res.errortext := "Nie znaleziono Gildii ID: ["+GdId+"]";
    return Res;
  endif

  var Member;
  var Members := Guild.GetProp(GDPROP_MEMBERS);

  ResignNpcsFromGuild(GdId);

  foreach MembSer in Members
    Member := FindPlayerBySerial(MembSer);
    if(Member)
      ResignPlayerFromGuild(Member, GdId);
    endif
  endforeach

  var Board := FindItemBySerial(Guild.GetProp(GDPROP_BOARD));

  if(Board)
    DestroyItem(Board);
  endif

  return Data.DeleteElement(NGUILD_TITLE+GdId);

endfunction

//-----------------------------------------------------------------------------

function VerifyNpcGuild(byref GdData)

  var Err := error;

  if(!GdData.Name)
    Err.errortext := "Brak nazwy Gildii.";
  elseif(!GdData.Type)
    Err.errortext := "Brak typu Gildii.";
  elseif(!GdData.Status)
    Err.errortext := "Brak statusu Gildii.";
  elseif(!GdData.QuestTypes or !GdData.QuestTypes.size())
    Err.errortext := "Brak typow questow Gildii.";
  elseif(!GdData.SkIDs or !GdData.SkIDs.size())
    Err.errortext := "Brak umiejetnosci Gildii.";
  elseif(!GdData.MainSkIDs or !GdData.MainSkIDs.size())
    Err.errortext := "Brak kluczowych umiejetnosci Gildii.";
  endif

  if(Err.errortext)
    return Err;
  endif

  var i    := 1;
  var Rlms := Realms();
  var Width, Height, Realm;

  foreach Build in (GdData.Buildings)
    if(!Build)
      Err.errortext := "Bledny budynek Nr: "+i;
      break;
    elseif(Build.size() != 5)
      Err.errortext := "Za malo parametrow ["+Build.size()+"] (5)";
      break;
    endif

    Build[1] := CInt(Build[1]);
    Build[2] := CInt(Build[2]);
    Build[3] := CInt(Build[3]);
    Build[4] := CInt(Build[4]);
    Build[5] := CInt(Build[5]);

    if(!Build[1] or !Build[3])
      Err.errortext := "Bledne xSet ["+Build[1]+"] lub ySet ["+Build[2]+"]";
      break;
    endif

    if(Build[1] >= Build[3])
      Err.errortext := "xSet >= xEnd ["+Build[1]+" >= "+Build[3]+"]";
      break;
    endif

    if(Build[2] >= Build[4])
      Err.errortext := "ySet >= yEnd ["+Build[2]+" >= "+Build[4]+"]";
      break;
    endif

    Realm := GetRealmById(Build[5]);

    if(!Realm)
      Err.errortext := "Bledny RealmID ["+Build[5]+"]";
      break;
    endif

    Width  := Rlms[Realm].width;
    Height := Rlms[Realm].height;

    if(Build[2] > Width)
      Err.errortext := "xEnd > "+Width+" (max X "+Realm+") ["+Build[2]+"]";
      break;
    endif

    if(Build[4] > Height)
      Err.errortext := "yEnd > "+Height+" (max Y "+Realm+") ["+Build[4]+"]";
      break;
    endif

    i := i + 1;
  endforeach

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AssignNpcsToGuild(NGdId)

  var NGdData := GetNGdData();
  var Guild   := NGdData.FindElement(NGUILD_TITLE+NGdId);
  var Err     := error;
  var x, y, tx, ty, Res, Num := 0;

  if(!Guild)
    Err.errortext := "Nie znaleziono Gildii ["+NGdId+"]: "+Guild;
    return Err;
  endif

  foreach Building in (Guild.GetProp(GDPROP_BUILDINGS))
    for(x:=Building[1];x<=Building[3];x:=x+64)
      for(y:=Building[2];y<=Building[4];y:=y+64)
        tx := x + 64;
        ty := y + 64;
        if(tx > Building[3])
          tx := Building[3];
        endif
        if(ty > Building[4])
          ty := Building[4];
        endif
        foreach Npc in ListObjectsInBox(x, y, -127, tx, ty, 127, GetRealmById(Building[5]))
          if(Npc.isa(POLCLASS_NPC))
            if(GetNpcConfig(Npc).CityNpc)
              Res := AssignNpcToGuild(Npc, NGdId, Guild);
              if(Res == error)
                return Res;
              elseif(Res)
                Num := Num + 1;
              endif
            endif
          endif
        endforeach
      endfor
    endfor
  endforeach

  return Num;

endfunction

//-----------------------------------------------------------------------------

function AssignNpcToGuild(Npc, NGdId, byref Guild, Level := 0)
 
  if(!Npc.isa(POLCLASS_NPC))
    return 0;
  endif

  var NpcCurData := GetObjProperty(Npc, PROP_NGDDATA);

  if(NpcCurData and NpcCurData.GuildId != NGdId)
    var Err := error;
    Err.errortext := "Npc ["+Npc.serial+"] istnieje w Gildii ID: "+NpcCurData.GuildId+" !";
    return Err;
  elseif(!NpcCurData)
    var NpcData;
    InitNGdNpcData(NpcData);
    NpcData.GuildId := NGdId;
    NpcData.Level   := Level;
    SetObjProperty(Npc, PROP_NGDDATA, NpcData);
    Npc.title_guild := ClStr(Guild.GetProp(GDPROP_NAME));
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ResignNpcsFromGuild(NGdId)

  var NGdData := GetNGdData();
  var Guild   := NGdData.FindElement(NGUILD_TITLE+NGdId);
  var Err     := error;
  var x, y, tx, ty, Res, Num := 0;

  if(!Guild)
    Err.errortext := "Nie znaleziono Gildii ["+NGdId+"]: "+Guild;
    return Err;
  endif

  foreach Building in (Guild.GetProp(GDPROP_BUILDINGS))
    for(x:=Building[1];x<=Building[3];x:=x+64)
      for(y:=Building[2];y<=Building[4];y:=y+64)
        tx := x + 64;
        ty := y + 64;
        if(tx > Building[3])
          tx := Building[3];
        endif
        if(ty > Building[4])
          ty := Building[4];
        endif
        foreach Npc in ListObjectsInBox(x, y, -127, tx, ty, 127, GetRealmById(Building[5]))
          if(Npc.isa(POLCLASS_NPC))
            Res := ResignNpcFromGuild(Npc, NGdId);
            if(Res == error)
              return Res;
            elseif(Res)
              Num := Num + 1;
            endif
          endif
        endforeach
      endfor
    endfor
  endforeach

  return Num;

endfunction

//-----------------------------------------------------------------------------

function ResignNpcFromGuild(Npc, NGdId := 0)
 
  if(!Npc.isa(POLCLASS_NPC))
    return 0;
  endif

  var NpcCurData := GetObjProperty(Npc, PROP_NGDDATA);
  if(!NGdId)
    NGdId := NpcCurData.GuildId;
  endif

  if(NpcCurData.GuildId and NpcCurData.GuildId == NGdId)
    if(NpcCurData.Level == 5)
      var NGdData := GetNGdData();
      var Guild   := NGdData.FindElement(NGUILD_TITLE+NGdId);
      if(Guild)
        Guild.SetProp(GDPROP_MASTER, 0);
      endif
    endif

    EraseObjProperty(Npc, PROP_NGDDATA);
    Npc.title_guild := "";
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AssignNpcGuildMaster(Npc, NGdId)

  var Data   := GetNGdData();
  var Guild  := Data.FindElement(NGUILD_TITLE+NGdId);
  var Err    := error;
  var Master := FindNpcBySerial(Guild.GetProp(GDPROP_MASTER));
  var NpcData;

  if(Master)
    NpcData := GetObjProperty(Master, PROP_NGDDATA);
    NpcData.Level := 0;
    Master.title_guild := ClStr(Guild.GetProp(GDPROP_NAME));
    Guild.SetProp(GDPROP_MASTER, 0);
  endif

  if(!Guild)
    Err.errortext := "Nie znaleziono Gildii ["+NGdId+"]: "+Guild;
    return Err;
  endif

  Err := AssignNpcToGuild(Npc, NGdId, Guild);

  if(Err != error)
    NpcData := GetObjProperty(Npc, PROP_NGDDATA);
    NpcData.Level := 5;
    SetObjProperty(Npc, PROP_NGDDATA, NpcData);
    Guild.SetProp(GDPROP_MASTER, Npc.serial);
    Npc.title_guild := "Mistrz Gildii "+ClStr(Guild.GetProp(GDPROP_NAME));
    return 1;
  endif

  return Err;

endfunction

//-----------------------------------------------------------------------------

function IsInNpcGuild(Npc, GdId)

  if(Npc.isa(POLCLASS_NPC))
    var NpcData := GetObjProperty(Npc, PROP_NGDDATA);
    return (NpcData and NpcData.GuildId == GdId);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsOnGuildGround(GdId, x, y, realm)

  var Data  := GetNGdData();
  var Guild := Data.FindElement(NGUILD_TITLE+GdId);

  if(Guild)
    foreach Build in (Guild.GetProp(GDPROP_BUILDINGS))
      if(GetRealmById(Build[5]) == realm)
        if(Build[1] <= x and Build[3] >= x and Build[2] <= y and Build[4] >= y)
          return 1;
        endif
      endif
    endforeach
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function FindGuildNpcs(GdId, byref Guild, byref Npcs)

  Npcs := array;
  var x,y;

  foreach Build in (Guild.GetProp(GDPROP_BUILDINGS))
    for(x:=Build[1]-4;x<=Build[3]+4;x:=x+24)
      for(y:=Build[2]-4;y<=Build[4]+4;y:=y+24)
        foreach Npc in ListObjectsInBox(x, y, -127, x+24, y+24, 127, GetRealmById(Build[5]))
          if(Npc.isa(POLCLASS_NPC))
            if(IsInNpcGuild(Npc, GdId))
              if(!(Npc in Npcs))
                Npcs.append(Npc);
              endif
            endif
          endif
        endforeach
      endfor
    endfor
  endforeach

endfunction

//-----------------------------------------------------------------------------
// Player
//-----------------------------------------------------------------------------

function ResignPlayerFromGuild(Player, NGdId, bBySelf := 0)

  if(!IsPlayer(Player) or !NGdId)
    return RetError("Gildia nie istnieje!");
  endif

  var CurData := GetObjProperty(Player, PROP_NGDDATA);
  
  var Data    := GetNGdData();
  var Guild   := Data.FindElement(NGUILD_TITLE+NGdId);
  var Members := Guild.GetProp(GDPROP_MEMBERS);

  if(Player.serial in Members)
    Members.erase(RetIndex(Members, Player.serial));
    Guild.SetProp(GDPROP_MEMBERS, Members);
  endif

  if(CurData.Guilds.exists(NGdId))

    if(CurData.Guilds[NGdId].Flags & NGPF_MAXSK_CHOOSEN)
      EraseMaxSkillDelayed(Player, CurData.MaxSkID);
      CurData := GetObjProperty(Player, PROP_NGDDATA);
    endif

    // tracimy 5% expa w gildii za wyrzucenie
    // nie zerujemy calkowicie
    if(!bBySelf)
      // ale tylko, gdy nie zrezygnowalismy sami!
      LoseGuildExp(Player, CurData, NGdId, Guild, CInt(CDbl(CurData.Guilds[NGdId].Exp) * NGD_RESIGN_EXP_LOSS), 0); // 0 na koncu bo nie chcemy wiadomosci przy wypisie = poo co to.
    endif

    CurData.Guilds[NGdId].Flags    := 0;
    CurData.Guilds[NGdId].Delay    := ReadGameClock();

    CurData.Guilds[NGdId].LastQuest.Ticket := 0;
    CurData.Guilds[NGdId].LastQuest.Time   := 0;
    CurData.Guilds[NGdId].LastQuest.From   := 0;

    SetObjProperty(Player, PROP_NGDDATA, CurData);
    SendSysMessage(Player, "Zostal"+ggm(Player,3)+" wypisan"+ggm(Player,6)+" z Gildii "+ClStr(Guild.GetProp(GDPROP_NAME))+".", FONT_NORMAL, COLOR_RED);

    var sTitle := GetPlayerNGDTitleByData(CurData);
    SetTitleSuffix(Player, sTitle);

    return 1;
  endif

  return RetError("Osoba nie jest zapisana w gildii.");

endfunction

//-----------------------------------------------------------------------------

function ErasePlayerFromGuild(Player, GdId)

  var CurData := GetObjProperty(Player, PROP_NGDDATA);

  if(CurData.Guilds[GdId].Flags & NGPF_MAXSK_CHOOSEN)
    EraseMaxSkillDelayed(Player, CurData.MaxSkID);
    CurData := GetObjProperty(Player, PROP_NGDDATA);
  endif

  CurData.Guilds.erase(GdId);
  
  var GuildData := GetNGdData();
  var Guild     := GuildData.FindElement(NGUILD_TITLE+GdId);

  if(Guild)
    var Members := Guild.GetProp(GDPROP_MEMBERS);
    if(Player.serial in Members)
      Members.erase(RetIndex(Members, Player.serial));
      Guild.SetProp(GDPROP_MEMBERS, Members);
    endif
  endif

  SetObjProperty(Player, PROP_NGDDATA, CurData);
  
  var sTitle := GetPlayerNGDTitleByData(CurData);
  SetTitleSuffix(Player, sTitle);

endfunction

//-----------------------------------------------------------------------------

function GetNgdQuestDelay(Level)

  Level := MinMax(CInt(Level), 0, 5);

  var Cfg := ReadConfigFile(NGD_CFG);

  return GetConfigInt(Cfg["QuestDelay"], "Level_"+Level);

endfunction

//-----------------------------------------------------------------------------
