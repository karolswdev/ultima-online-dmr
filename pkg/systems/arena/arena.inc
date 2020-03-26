//--------------------------------------------------------------------------------

include "include/items";
include "include/objtypes/misc";
include "include/player/target";

//--------------------------------------------------------------------------------

const PLAYER_FIGHT        := 1;
const NPC_FIGHT           := 2;
const NPC_GROUP           := 3;
const TOUR_FIGHT          := 4;

const ARENA_CFG           := ":arena:arena";
const MIN_ARENA_RANGE     := 5;
const ARENASER_FLAG       := "FightingArenaSerials";
const BOARD_CHANGE_GRAPH  := 0x1E5F;
const CHEST_DROP_TIME     := 43200; // 12h

const UOBJ_ARENACHEST     := 0xA203;
const UOBJ_ARENASTONE     := 0xA200;
const UOBJ_ARENABOARD     := 0xA201;
const UOBJ_BOARDLEG       := 0xA209;

const REQ_STR_ON_TOUR     := 40;
const MAX_TOUR_FIGHTERS   := 16;
const TOUR_START_TIME     := 600;
const FINAL_LVL           := 4;

const UOBJ_TOURMACE       := 0xA222;
const UOBJ_TOURSWORD      := 0xA221;
const UOBJ_TOURFENC       := 0xA223;
const UOBJ_TOURAXE        := 0xA224;
const UOBJ_TOURBOW        := 0xA225;

//--------------------------------------------------------------------------------

function SetArenaFight(stone,who,Opponent,type)

  if(!CheckArena(stone,who))
    return 0;
  endif
  if(IsArenaInUse(stone))
    SendSysMessage(who,"Nie mozesz teraz walczyc.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  var ArenaCfg := ReadConfigFile(ARENA_CFG);
  var Gold     := GetConfigInt(ArenaCfg["Gold"],"Level_"+Opponent);
  var Monsters := ArenaCfg["Monsters"];
  if(!Monsters)
    LogError("SetArenaFight: Arena Config: Nie mozna bylo znalezc potworow!");
    return;
  endif
  
  case(type)
    PLAYER_FIGHT:   Gold := GetConfigInt(ArenaCfg["Gold"],"Player");
                    if(!SubtractGold(who.backpack,Gold))
                      SendSysMessage(who,"Potrzebujesz "+Gold+"gp.",FONT_NORMAL,COLOR_RED);
                      return 0;
                    endif
                    SetObjProperty(stone,"StartFight",{who.serial,opponent.serial,type});
    NPC_FIGHT:      if(!SubtractGold(who.backpack,Gold))
                      SendSysMessage(who,"Potrzebujesz "+Gold+"gp.",FONT_NORMAL,COLOR_RED);
                      return 0;
                    endif
                    Monsters     := GetConfigStringArray(Monsters,"Level_"+Opponent);
                    var Monster  := Monsters[(RandomInt(len(Monsters))+1)];
                    if(!Monster)
                      Monster := Monsters[1];
                    endif
                    if(!Monster)
                      return 0;
                    endif
                    SetObjProperty(stone,"StartFight",{who.serial,Monster,type,Opponent});
    NPC_GROUP:      if(!SubtractGold(who.backpack,Gold))
                      SendSysMessage(who,"Potrzebujesz "+Gold+"gp.",FONT_NORMAL,COLOR_RED);
                      return 0;
                    endif
                    Monsters     := GetConfigStringArray(Monsters,"GroupLevel_"+Opponent);
                    var Group    := Monsters[(RandomInt(len(Monsters))+1)];
                    if(!Group)
                      Group := Monsters[1];
                    endif
                    if(!Group)
                      return 0;
                    endif
                    var Check := SplitWords(Group);
                    if(Check[1] == "Random")
                      var i,Monster;
                      Group := {};
                      Monsters     := GetConfigStringArray(ArenaCfg["Monsters"],"Level_"+Opponent);
                      for(i:=1;i<=CInt(Check[2]);i:=i+1)
                        Monster      := Monsters[(RandomInt(len(Monsters))+1)];
                        if(!Monster)
                          Monster := Monsters[1];
                        endif
                        Group.append(Monster);
                      endfor
                      Group := CombineWords(Group);
                    endif
                    SetObjProperty(stone,"StartFight",{who.serial,Group,type,Opponent});
    TOUR_FIGHT:     SetObjProperty(stone,"StartFight",{who.serial,opponent.serial,type});
    default:        return 0;
  endcase
  
  SendSysMessage(who,"Przygotuj sie.",FONT_NORMAL,COLOR_GREEN);
  if(IsPlayer(Opponent))
    SendSysMessage(Opponent,"Przygotuj sie.",FONT_NORMAL,COLOR_GREEN);
  endif
  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function CheckArena(stone,who := 0)

  if(!FindArenaCenter(stone))
    if(who and IsAdmin(who))
      SendSysMessage(who,"Nie zostal okreslony srodek areny!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(!FindArenaChest(stone))
    if(who and IsAdmin(who))
      SendSysMessage(who,"Nie zostala okreslona skrzynka areny!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(!FindArenaOppPlace(stone))
    if(who and IsAdmin(who))
      SendSysMessage(who,"Nie zostalo okreslone miejsce przeciwnika!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(!FindArenaFighterPlace(stone))
    if(who and IsAdmin(who))
      SendSysMessage(who,"Nie zostalo okreslone miejsce smialka!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(!FindArenaRange(stone))
    if(who and IsAdmin(who))
      SendSysMessage(who,"Nie zostal okreslony zasieg areny!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function FindArenaCenter(stone)

  var Place  := struct;
  var Coords := GetObjProperty(stone,"CenterPos");
  if(!CInt(Coords[1]) or !CInt(Coords[2]))
    return 0;
  endif
  
  Place.+x := CInt(Coords[1]);
  Place.+y := CInt(Coords[2]);
  Place.+z := CInt(Coords[3]);
  
  return Place;
  
endfunction

//--------------------------------------------------------------------------------

function FindArenaOppPlace(stone)

  var Place  := struct;
  var Coords := GetObjProperty(stone,"OpponentPos");
  if(!CInt(Coords[1]) or !CInt(Coords[2]))
    return 0;
  endif

  Place.+x := CInt(Coords[1]);
  Place.+y := CInt(Coords[2]);
  Place.+z := CInt(Coords[3]);

  return Place;

endfunction

//--------------------------------------------------------------------------------

function FindArenaFighterPlace(stone)

  var Place  := struct;
  var Coords := GetObjProperty(stone,"FighterPos");
  if(!CInt(Coords[1]) or !CInt(Coords[2]))
    return 0;
  endif

  Place.+x := CInt(Coords[1]);
  Place.+y := CInt(Coords[2]);
  Place.+z := CInt(Coords[3]);

  return Place;

endfunction

//--------------------------------------------------------------------------------

function FindArenaRange(stone)

  return CInt(GetObjProperty(stone,"ArenaRange"));

endfunction

//--------------------------------------------------------------------------------

function FindArenaChest(stone)

  return SystemFindObjectBySerial(CInt(GetObjProperty(stone,"ArenaChest")));

endfunction

//--------------------------------------------------------------------------------

function IsArenaInUse(stone)

  if(CInt(GetObjProperty(stone,"#InUse")) or GetObjProperty(stone,"StartFight"))
    return 1;
  endif
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function SetArenaCenter(stone,who)

  var Place := LocTarget(who);
  if(!Place)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  DestroyArenaFence(stone);
  SetObjProperty(stone,"CenterPos",{Place.x,Place.y,Place.z});
  SendSysMessage(who,"Zmienil"+ggm(who,3)+" pozycje srodka areny. x: "+Place.x+" y: "+Place.y+" z: "+Place.z+".",FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function SetArenaOppPlace(stone,who)

  var Place := LocTarget(who);
  if(!Place)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  SetObjProperty(stone,"OpponentPos",{Place.x,Place.y,Place.z});
  SendSysMessage(who,"Zmienil"+ggm(who,3)+" pozycje wroga. x: "+Place.x+" y: "+Place.y+" z: "+Place.z+".",FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function SetArenaFighterPlace(stone,who)

  var Place := LocTarget(who);
  if(!Place)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  SetObjProperty(stone,"FighterPos",{Place.x,Place.y,Place.z});
  SendSysMessage(who,"Zmienil"+ggm(who,3)+" pozycje smialka. x: "+Place.x+" y: "+Place.y+" z: "+Place.z+".",FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function SetArenaRange(stone,who)

  var Range := SendTextEntryGump( who, "Wprowadz nowa wartosc ",TE_CANCEL_ENABLE,TE_STYLE_NUMERICAL,500);
  if(!Range)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  Range := CInt(Range);
  if(Range < MIN_ARENA_RANGE)
    SendSysMessage(who,"Za maly zasieg, minimalny to "+MIN_ARENA_RANGE+".",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  SetObjProperty(stone,"ArenaRange",CInt(Range));
  SendSysMessage(who,"Zmienil"+ggm(who,3)+" zasieg areny na "+Range+".",FONT_NORMAL,COLOR_GREEN);
  DestroyArenaFence(stone);

endfunction

//--------------------------------------------------------------------------------

function SetArenaChest(stone,who)

  var ArenaChest := FindArenaChest(stone);
  if(ArenaChest)
    if(len(EnumerateItemsInContainer(ArenaChest)))
      SendSysMessage(who,"Skrzynka istnieje juz i nie jest pusta!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
    DestroyItem(ArenaChest);
  endif

  var Place := LocTarget(who);
  if(!Place)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  var Chest := CreateItemAtLoc(Place.x,Place.y,Place.z,UOBJ_ARENACHEST,1,who.realm);

  if(Chest)
    SetObjProperty(stone,"ArenaChest",Chest.serial);
    SetObjProperty(Chest,"ArenaSerial",stone.serial);
  endif
  SendSysMessage(who,"Postawil"+ggm(who,3)+" skrzynke areny. x: "+Place.x+" y: "+Place.y+" z: "+Place.z+".",FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function FindBoardArena(board)

  return SystemFindObjectBySerial(CInt(GetObjProperty(board,"ArenaSerial")));
  
endfunction

//--------------------------------------------------------------------------------

function SetBoardArena(board,who)

  SendSysMessage(who,"Wybierz Kamien Areny.",FONT_NORMAL,COLOR_GREEN);
  var Arena := ObjTarget(who);
  if(Arena.objtype != UOBJ_ARENASTONE)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  LinkArenaWithBoard(Arena,board);
  SendSysMessage(who,"Ustawil"+ggm(who,3)+" tablice dla areny "+GetObjProperty(arena,"ArenaNr")+".",FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function LinkArenaWithBoard(stone,board)

  SetObjProperty(board,"ArenaSerial",stone.serial);
  SetObjProperty(stone,"BoardSerial",Board.serial);

endfunction

//--------------------------------------------------------------------------------

function FindArenaScoreBoard(stone)

  var Board := SystemFindObjectBySerial(CInt(GetObjProperty(stone,"BoardSerial")));
  if(Board)
    if(CInt(GetObjProperty(board,"ArenaSerial")) == stone.serial)
      return Board;
    endif
  endif
  return 0;

endfunction

//--------------------------------------------------------------------------------

function UnLinkArenaWithBoard(stone,board)

  EraseObjProperty(board,"ArenaSerial");
  EraseObjProperty(stone,"BoardSerial");

endfunction

//--------------------------------------------------------------------------------

function CheckScores(stone)

  var Scores := GetObjProperty(stone,"Scores");
  if(!Scores.size())
    return 1;
  endif

  var i := 1;
  foreach Sc in GetObjProperty(stone,"Scores")
    if(!FindPlayerBySerial(CInt(Sc[1])))
      Scores.erase(i);
    endif
    i := i + 1;
  endforeach
  SetObjProperty(stone,"Scores",Scores);

  i := 1;
  Scores := GetObjProperty(stone,"PlayerScores");
  foreach Sc in GetObjProperty(stone,"PlayerScores")
    if(!FindPlayerBySerial(CInt(Sc[1])))
      Scores.erase(i);
    endif
    i := i + 1;
  endforeach
  SetObjProperty(stone,"PlayerScores",Scores);

  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function IsTour(stone)

  return CInt(GetObjProperty(stone,"Tournament"));
  
endfunction

//--------------------------------------------------------------------------------

function SetTour(stone,who := 0)

  if(!CheckArena(stone,who))
    return 0;
  endif
  if(IsTour(stone))
    return 0;
  endif
  BroadCast("Rozpoczeto Turniej Areny "+GetArenaName(stone)+"!",FONT_NORMAL,COLOR_BLUE);
  SetObjProperty(stone,"Tournament",ReadGameClock());
  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function BeginTour(stone)

  if(StartedTour(stone))
    return 0;
  endif
  var TourFighters := GetTourFighters(stone);
  if(len(TourFighters) != MAX_TOUR_FIGHTERS)
    return 0;
  endif
  var FirstLvl := {},i,Add;
  while(len(FirstLvl) < len(TourFighters))
    Add := TourFighters[(RandomInt(len(TourFighters))+1)];
    if(!(Add in FirstLvl))
      FirstLvl.append(Add);
    endif
  endwhile
  SetTourFights(stone,FirstLvl);
  SetObjProperty(stone,"StartedTournament",1);

endfunction

//--------------------------------------------------------------------------------

function StartedTour(stone)

  return CInt(GetObjProperty(stone,"StartedTournament"));

endfunction

//--------------------------------------------------------------------------------

function AddTourFighter(stone,who)

  if(who.cmdlevel)
    SendSysMessage(who,"Nie mozesz zapisac sie na liste.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(IsTourFighter(stone,who))
    SendSysMessage(who,"Jestes juz zapisany.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  var TourFighters := GetTourFighters(stone);
  if(len(TourFighters) >= MAX_TOUR_FIGHTERS)
    SendSysMessage(who,"Niestety juz lista jest pelna.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  TourFighters.append({who.acctname,who.serial});
  SetTourFighters(stone,TourFighters);
  SendSysMessage(who,"Zapisal"+ggm(who,3)+" sie na turniej.",FONT_NORMAL,COLOR_GREEN);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetTourFighters(stone)

  var TourFighters := GetObjProperty(stone,"TourFighters"),i:=1;
  if(!TourFighters)
    TourFighters := {};
  endif
  foreach Data in GetObjProperty(stone,"TourFighters")
    if(!FindPlayerBySerial(CInt(Data[2])))
      TourFighters.erase(i);
    endif
    i := i + 1;
  endforeach
  SetTourFighters(stone,TourFighters);
  return TourFighters;
  
endfunction

//--------------------------------------------------------------------------------

function SetTourFighters(stone,TourFighters)

  SetObjProperty(stone,"TourFighters",TourFighters);

endfunction

//--------------------------------------------------------------------------------

function IsTourFighter(stone,who)

  var TourFighters := GetTourFighters(stone);
  foreach Data in TourFighters
    if(who.acctname == CStr(Data[1]) or who.serial == CInt(Data[2]))
      return 1;
    endif
  endforeach
  return 0;

endfunction

//--------------------------------------------------------------------------------

function EraseTour(stone,who := 0)

  if(IsArenaInUse(stone) and who != 1)
    if(who)
      SendSysMessage(who,"Nie mozesz teraz przerwac turnieju.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif
  EraseObjProperty(stone,"Tournament");
  EraseObjProperty(stone,"TourFighters");
  EraseObjProperty(stone,"TourFight");
  EraseObjProperty(stone,"StartedTournament");
  EraseObjProperty(stone,"TourCounting");
  EraseObjProperty(stone,"TourLvl");
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetTourFights(stone)

  if(!GetObjProperty(stone,"TourFight"))
    SetObjProperty(stone,"TourFight",{});
  endif
  return GetObjProperty(stone,"TourFight");

endfunction

//--------------------------------------------------------------------------------

function SetTourFights(stone,TourFights)

  return SetObjProperty(stone,"TourFight",TourFights);

endfunction

//--------------------------------------------------------------------------------

function GetNextTourFight(stone)

  if(!IsTour(stone) or !StartedTour(stone))
    return;
  endif
  var TourFights := GetTourFights(stone),i,Data,Data2,Ret := {},FirstFighter,SecFighter;
  for(i:=1;i<=len(TourFights);i:=i+2)
    Data  := TourFights[i];
    Data2 := TourFights[i+1];
    if(!Data[3] and !Data2[3])
      FirstFighter := FindPlayerBySerial(CInt(Data[2]));
      SecFighter   := FindPlayerBySerial(CInt(Data2[2]));
      if(!FirstFighter)
        Data2[3] := "Won";
        Data[3]  := "Lost";
      elseif(!SecFighter)
        Data[3]  := "Won";
        Data2[3] := "Lost";
      else
        Ret.append(FirstFighter);
        Ret.append(SecFighter);
        return Ret;
      endif
      SetTourFights(stone,TourFights);
    endif
  endfor
  if(GetTourLvl(stone) == 1)
    for(i:=1;i<=FINAL_LVL;i:=i+1)
      EraseObjProperty(stone,"LastTourFightLvl_"+i);
    endfor
  endif
  SetObjProperty(stone,"LastTourFightLvl_"+GetTourLvl(stone),TourFights);
  if(GetTourLvl(stone) == FINAL_LVL)
    FinalEndTour(stone);
    return 0;
  else
    SetTourLvl(stone,GetTourLvl(stone) + 1);
    var NewTourFights := {};
    for(i:=1;i<=len(TourFights);i:=i+2)
      Data  := TourFights[i];
      Data2 := TourFights[i+1];
      if(Data[3] == "Won")
        Data[3] := 0;
        NewTourFights.append(Data);
      elseif(Data2[3] == "Won")
        Data2[3] := 0;
        NewTourFights.append(Data2);
      endif
    endfor
    SetTourFights(stone,NewTourFights);
  endif

endfunction

//--------------------------------------------------------------------------------

function EndTourFight(stone,Winner,Looser)

  EraseObjProperty(Winner,"#TourFight");
  EraseObjProperty(Looser,"#TourFight");
  var TourFights := GetTourFights(stone),i := 1,Data;
  for(i:=1;i<=len(TourFights);i:=i+1)
    Data := TourFights[i];
    if(CInt(Data[2]) == Winner.serial)
      Data[3] := "Won";
    elseif(CInt(Data[2]) == Looser.serial)
      Data[3] := "Lost";
    endif
    TourFights[i] := Data;
  endfor
  SetTourFights(stone,TourFights);
  GetNextTourFight(stone);
  
endfunction

//--------------------------------------------------------------------------------

function GetTourLvl(stone)

  if(!GetObjProperty(stone,"TourLvl"))
    SetObjProperty(stone,"TourLvl",1);
  endif
  return CInt(GetObjProperty(stone,"TourLvl"));
  
endfunction

//--------------------------------------------------------------------------------

function SetTourLvl(stone,Lvl)

  return SetObjProperty(stone,"TourLvl",CInt(Lvl));
  
endfunction

//--------------------------------------------------------------------------------

function FinalEndTour(stone)

  if(!IsTour(stone) or !StartedTour(stone))
    return;
  endif
  var TourFights := GetTourFights(stone), i, Data, Data2, Winner;
  for(i:=1;i<=len(TourFights);i:=i+2)
    Data  := TourFights[i];
    Data2 := TourFights[i+1];
    if(Data[3] == "Won")
      Winner := FindPlayerBySerial(Data[2]);
      break;
    elseif(Data2[3] == "Won")
      Winner := FindPlayerBySerial(Data2[2]);
      break;
    endif
  endfor
  if(Winner)
    var Arenacfg := ReadConfigFile(ARENA_CFG);
    var Prices   := GetConfigStringArray(ArenaCfg["Prices"],"Price");
    var Price    := Prices[(RandomInt(len(Prices))+1)];
    if(!Price)
      Price := Prices[1];
    endif
    var Items    := GetConfigStringArray(ArenaCfg[Price],"Item");
    var StackItm := GetConfigStringArray(ArenaCfg[Price],"Stack");
    var Random   := GetConfigStringArray(ArenaCfg[Price],"Random");
    
    CreateItemFromItemString(Winner.backpack,Items);
    CreateItemFromStackString(Winner.backpack,StackItm);
    CreateItemFromRandomString(Winner.backpack,Random);

    var TourWinners := GetTourWinners(stone);
    TourWinners.append(Winner.serial);
    SetTourWinners(stone,TourWinners);
  endif
  
  BroadCast(Cstr(Winner.name) + " wygral Turniej na Arenie "+GetArenaName(stone)+"!",FONT_NORMAL,COLOR_BLUE);
  EraseTour(stone,1);

endfunction

//--------------------------------------------------------------------------------

function GetTourWinners(stone)

  if(!GetObjProperty(stone,"TourWinners"))
    SetObjProperty(stone,"TourWinners",{});
  endif
  return GetObjProperty(stone,"TourWinners");
  
endfunction

//--------------------------------------------------------------------------------

function SetTourWinners(stone,Winners)

  return SetObjProperty(stone,"TourWinners",Winners);
  
endfunction

//--------------------------------------------------------------------------------

function DestroyPackTourItems(who,cont := 0)

  if(!Cont)
    cont := who.bacpkack;
  endif
  foreach item in EnumerateItemsInContainer(cont)
    if(GetObjProperty(item,"TourItem"))
      DestroyItem(item);
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------

function GetArenaName(stone)

  if(!stone)
    return "";
  endif
  if(!GetObjProperty(stone,"ArenaName"))
    SetObjProperty(stone,"ArenaName","");
  endif
  return GetObjProperty(stone,"ArenaName");

endfunction

//--------------------------------------------------------------------------------

function SetArenaName(stone,Name)

  if(Name)
    return SetObjProperty(stone,"ArenaName",Name);
  endif
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function CreateItemFromStackString(cont,Strin,Flag := 0,FlagValue := 1)

    var Item;
    foreach Str in Strin
      Str  := SplitWords(Str);
      if(!CInt(Str[2]))
        Str[2] := GetObjTypeByName(Str[2]);
      endif
      item := CreateItemInCont(cont,CInt(Str[2]),RandomDiceRoll(Str[1]));
        if(Flag)
          SetObjProperty(item,Flag,FlagValue);
        endif
      if(!item)
        LogError("CreateItemFromStackString: Arena TourWinPrice: Nie mozna bylo stworzyc przedmiotu "+Str[2]+"/"+Str[1]+"!");
      endif
    endforeach

endfunction

//--------------------------------------------------------------------------------

function CreateItemFromItemString(cont,Strin,Flag := 0,FlagValue := 1)

    var Item,i;
    foreach Str in Strin
      Str  := SplitWords(Str);
      if(!CInt(Str[2]))
        Str[2] := GetObjTypeByName(Str[2]);
      endif
      for(i:=1;i<=CInt(Str[1]);i:=i+1)
        item := CreateItemInCont(cont,CInt(Str[2]),1);
        if(Flag)
          SetObjProperty(item,Flag,FlagValue);
        endif
        if(!item)
          LogError("CreateItemFromItemString: Arena TourWinPrice: Nie mozna bylo stworzyc przedmiotu Item "+Str[2]+"/"+Str[1]+"!");
        endif
      endfor
    endforeach

endfunction

//--------------------------------------------------------------------------------

function CreateItemFromRandomString(cont,Strin,Flag := 0,FlagValue := 1)

    var Item,i;
    foreach Str in Strin
      Str  := SplitWords(Str);
      if(!CInt(Str[2]))
        Str[2] := GetObjTypeByName(Str[2]);
      endif
      for(i:=1;i<=CInt(Str[1]);i:=i+1)
        if(CInt(Str[3]) > RandomInt(100))
          item := CreateItemInCont(cont,CInt(Str[2]),1);
          if(Flag)
            SetObjProperty(item,Flag,FlagValue);
          endif
          if(!item)
            LogError("CreateItemFromRandomString: Arena TourWinPrice: Nie mozna bylo stworzyc przedmiotu "+Str[2]+"/"+Str[1]+"!");
          endif
        endif
      endfor
    endforeach

endfunction

//--------------------------------------------------------------------------------

function MoveItemToArenaChest(item,ArenaPack)

  return MoveItemToContainer(item,ArenaPack);

endfunction

//--------------------------------------------------------------------------------

function FindArenaPack(ArenaChest,who,Cr := 1)

  foreach item in ListRootItemsInContainer(ArenaChest)
    if(item.objtype == UOBJ_ARENAPACK and FindItemOwner(item) == who)
      return item;
    endif
  endforeach

  if(Cr)
    var NewPack := CreateItemInCont(ArenaChest,UOBJ_ARENAPACK,1);
    if(NewPack)
      SetItemOwner(NewPack,who);
      SetName(NewPack,"Plecak z rzeczami osoby "+who.name);
    endif
    return NewPack;
  endif
  
  return 0;

endfunction

//--------------------------------------------------------------------------------

function BuildArenaFenceAround(stone)

  var ArenaRange  := FindArenaRange(stone);
  var ArenaCenter := FindArenaCenter(stone);
  if(!ArenaRange or !ArenaCenter)
    return 0;
  endif

  if(!DestroyArenaFence(stone))
    return 0;
  endif
  var AFenOt := GetArenaFenceOt(stone),CreatedArray := {};
  if(!AFenOt)
    return 0;
  endif
  var Item,z,i;

  for(i:=1;i<=(ArenaRange*2);i:=i+1)
    Item := CreateItemAtLoc(ArenaCenter.x+ArenaRange,ArenaCenter.y+i-ArenaRange,GetStandZ(ArenaCenter.x+ArenaRange,ArenaCenter.y+i-ArenaRange,stone.realm,stone.z),CInt(AFenOt[1]),1,stone.realm);
    CreatedArray := AddFenceElem(item,CreatedArray);
    Item := CreateItemAtLoc(ArenaCenter.x+i-ArenaRange,ArenaCenter.y-ArenaRange,GetStandZ(ArenaCenter.x+i-ArenaRange,ArenaCenter.y-ArenaRange,stone.realm,stone.z),CInt(AFenOt[2]),1,stone.realm);
    CreatedArray := AddFenceElem(item,CreatedArray);
    Item := CreateItemAtLoc(ArenaCenter.x+i-ArenaRange,ArenaCenter.y+ArenaRange,GetStandZ(ArenaCenter.x+i-ArenaRange,ArenaCenter.y+ArenaRange,stone.realm,stone.z),CInt(AFenOt[2]),1,stone.realm);
    CreatedArray := AddFenceElem(item,CreatedArray);
    Item := CreateItemAtLoc(ArenaCenter.x-ArenaRange,ArenaCenter.y+i-ArenaRange,GetStandZ(ArenaCenter.x-ArenaRange,ArenaCenter.y+i-ArenaRange,stone.realm,stone.z),CInt(AFenOt[1]),1,stone.realm);
    CreatedArray := AddFenceElem(item,CreatedArray);
  endfor

  z := GetStandZ(ArenaCenter.x+ArenaRange,ArenaCenter.y+ArenaRange,stone.realm,stone.z);

  if(z != error)
    foreach item in ListItemsNearLocationOfType(ArenaCenter.x+ArenaRange,ArenaCenter.y+ArenaRange,z,0,CInt(AFenOt[1]),stone.realm)
      DestroyItem(item);
    endforeach

    foreach item in ListItemsNearLocationOfType(ArenaCenter.x+ArenaRange,ArenaCenter.y+ArenaRange,,0,CInt(AFenOt[2]),stone.realm)
      DestroyItem(item);
    endforeach
  endif

  Item := CreateItemAtLoc(ArenaCenter.x+ArenaRange,ArenaCenter.y+ArenaRange,GetStandZ(ArenaCenter.x+ArenaRange,ArenaCenter.y+ArenaRange,stone.realm,stone.z),CInt(AFenOt[3]),1,stone.realm);
  CreatedArray := AddFenceElem(item,CreatedArray);
  Item := CreateItemAtLoc(CInt(ArenaCenter.x-ArenaRange),CInt(ArenaCenter.y-ArenaRange),GetStandZ(CInt(ArenaCenter.x-ArenaRange),CInt(ArenaCenter.y-ArenaRange),stone.realm,stone.z),CInt(AFenOt[4]),1,stone.realm);
  CreatedArray := AddFenceElem(item,CreatedArray);
  Item := CreateItemAtLoc(ArenaCenter.x-ArenaRange,ArenaCenter.y+ArenaRange,GetStandZ(ArenaCenter.x-ArenaRange,ArenaCenter.y+ArenaRange,stone.realm,stone.z),CInt(AFenOt[4]),1,stone.realm);
  CreatedArray := AddFenceElem(item,CreatedArray);
  Item := CreateItemAtLoc(ArenaCenter.x+ArenaRange,ArenaCenter.y-ArenaRange,GetStandZ(ArenaCenter.x+ArenaRange,ArenaCenter.y-ArenaRange,stone.realm,stone.z),CInt(AFenOt[4]),1,stone.realm);
  CreatedArray := AddFenceElem(item,CreatedArray);
  SetObjProperty(stone,"CreatedArenaFence",CreatedArray);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function AddFenceElem(byref item,CreatedArray)

  Item.movable := 0;
  if(Item)
    CreatedArray.append(item.serial);
  endif
  SetObjProperty(Item,"ArenaFence",1);
  return CreatedArray;

endfunction

//--------------------------------------------------------------------------------

function DestroyArenaFence(stone)

  var CreatedArray := GetObjProperty(stone,"CreatedArenaFence");
  var item;
  if(CreatedArray.size())
    foreach serial in CreatedArray
      item := SystemFindObjectBySerial(CInt(serial));
      DestroyItem(item);
    endforeach
    EraseObjProperty(stone,"CreatedArenaFence");
  endif
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetArenaFenceOt(stone)

  var Ot := GetObjProperty(stone,"ArenaFenceObjType");
  if(!Ot)
    var ArenaConfig := ReadConfigFile(ARENA_CFG);
    var ObjTypes    := GetConfigStringArray(ArenaConfig["ArenaFences"],"Fence");
    if(!ObjTypes)
      LogError("GetArenaFenceOt: ArenaConfig: Fences: Nie mozna bylo znalezc ObjTypow!");
      return 0;
    endif
    ObjTypes        := ObjTypes[1];
    Ot              := GetConfigStringArray(ArenaConfig[ObjTypes],"FencePart");
    if(!Ot)
      LogError("GetArenaFenceOt: ArenaConfig: Nie mozna bylo znalezc czesci plotu! "+ObjTypes);
      return 0;
    endif
    SetObjProperty(stone,"ArenaFenceObjType",Ot);
  endif

  return Ot;

endfunction

//--------------------------------------------------------------------------------

function ChangeFenceOt(stone,Nr)

  var ArenaConfig := ReadConfigFile(ARENA_CFG);
  var ObjTypes    := GetConfigStringArray(ArenaConfig["ArenaFences"],"Fence");
  var Ot          := ObjTypes[Nr];

  if(!ArenaConfig[Ot])
    return 0;
  endif

  Ot              := GetConfigStringArray(ArenaConfig[Ot],"FencePart");
  if(Ot)
    SetObjProperty(stone,"ArenaFenceObjType",Ot);
    return 1;
  endif
  return 0;

endfunction

//--------------------------------------------------------------------------------
