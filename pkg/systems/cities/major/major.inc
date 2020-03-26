use uo;
use os;
use cfgfile;

include ":citizens:citizens";
include "include/clock";
include ":character:checks";
include "include/mobile/age";

const MIN_VOTES           := 1;
const MAX_VENDORS         := 5;
const MAX_GUARDS          := 3;
const MAX_CANDIDATES      := 5;

const VOTE_STOP           := 1;
const VOTE_START          := 2;
const VOTE_END            := 3;

const UOBJ_MAJORSTONE     := 0xA202;

const NEEDED_CITIZEN_TIME := 172800;
const NEEDED_CAND_AGE     := 24;

const VOTE_TIME           := 259200;       // 3d
const MAJOR_TIME          := 5443200;      // 3w // goqsane 15.02.2012 was 181440 (3w) is now 5443200 (12w)
const BREAK_TIME          := 21600;       // 6h

const MAJOR_JAIL_TIME     := 180;          // 15min
const MAJOR_JAIL_TIMEOUT  := 14400;

function GetStoneTime(Stone)

  if(!GetObjProperty(Stone,"Time"))
    ResetStoneTime(Stone);
  endif
  return CInt(GetObjProperty(Stone,"Time"));

endfunction

function ResetStoneTime(Stone)

  return SetObjProperty(Stone,"Time",ReadGameClock());

endfunction

function GetAllVStones()

  if(!GetGlobalProperty("VoteStones"))
    SetGlobalProperty("VoteStones",dictionary);
  endif

  return GetGlobalProperty("VoteStones");

endfunction

function SetAllVStones(ExStones)

  SetGlobalProperty("VoteStones",ExStones);

endfunction

function GetVoteStatus(Stone)

  if(!GetObjProperty(Stone,"Status"))
    SetObjProperty(Stone,"Status",VOTE_STOP);
  endif
  
  if(GetOBjProperty(Stone, "FinishNow"))
    return VOTE_END;
  endif
  
  return GetObjProperty(Stone,"Status");

endfunction

function ChangeVoteStatus(Stone,Stat)

  case(Stat)
    VOTE_START:   EraseObjProperty(Stone,"VotingPers");
                  EraseObjProperty(Stone,"Candidates");
    VOTE_STOP:    if(DismissMajor(Stone) != error)
                    EndHistory(GetCity(Stone));
                  endif
    VOTE_END:     ResetStoneTime(Stone);
                  if(!SetMajor(Stone))
                    Stat := VOTE_STOP;
                  endif
  endcase
  
  ResetStoneTime(Stone);
  SetObjProperty(Stone,"Status",Stat);

endfunction

function CanVote(who,Stone)

  if(who.cmdlevel)
    return 0;
  endif

  if(!IsCitizen(who,GetCity(Stone)))
    SendSysMessage(who,"Nie nalezysz do tego miasta!",FONT_NORMAL,RED_COLOR);
    return 0;
  endif

  foreach Cand in Candidates
    if(Cand.acctname == who.acctname)
      SendSysMessage(who,"Kandydujesz na burmistrza w tym miescie!",FONT_NORMAL,RED_COLOR);
      return 0;
    endif
  endforeach

  return 1;

endfunction

function GetCandidates(Stone)

  var Candidates := GetObjProperty(Stone,"Candidates");
  if(!Candidates)
    Candidates := {};
    SetCandidates(Stone,Candidates);
  endif
  
  i := 1;
  foreach Cand in GetObjProperty(Stone,"Candidates")
    if(!FindPlayerBySerial(Cand.serial))
      Candidates.erase(i);
    else
      i := i + 1;
    endif
  endforeach
  
  SetCandidates(Stone,Candidates);
  return Candidates;
  
endfunction

function AddCandidate(who,Stone,Desc)

  var Data       := struct;
  var Candidates := GetCandidates(Stone);
  var NewDesc    := {};

  for(i:=10;i<=17;i:=i+1)
    NewDesc.append(ClStr(CroppedId(Desc[i])));
  endfor

  
  Data.+serial   := who.serial;
  Data.+acctname := who.acctname;
  Data.+name     := GetRealName(who);
  Data.+desc     := NewDesc;
  Data.+votes    := 0;

  Candidates.append(Data);
  SendSysMessage(who,"Kandydujesz na burmistrza miasta "+City+".",FONT_NORMAL,GREEN_COLOR);
  SetCandidates(Stone,Candidates);

endfunction

function SetCandidates(Stone,Candidates)

  SetObjProperty(Stone,"Candidates",Candidates);

endfunction

function GetVotingPersons(Stone)

  if(!GetObjProperty(Stone,"VotingPers"))
    SetObjProperty(Stone,"VotingPers",{});
  endif

  return GetObjProperty(Stone,"VotingPers");

endfunction

function AddVotingPerson(Stone,who)

  var Persons := GetVotingPersons(Stone);
  Persons.append(who.acctname);
  SetObjProperty(Stone,"VotingPers",Persons);

endfunction

function GetMajor(Stone)

  if(!Stone.isa(POLCLASS_ITEM))
    Stone := FindVStone(Stone);
  endif

  return FindPlayerBySerial(GetObjProperty(Stone,"Major"));
  
endfunction

function DismissMajor(Stone)

  if(!Stone.isa(POLCLASS_ITEM))
    Stone := FindVStone(Stone);
  endif

  if(Stone)
    var Major   := GetMajor(Stone),x;

    for(x:=1;x<=MAX_GUARDS;x:=x+1)
      DismissGuard(Stone,1);
    endfor

    for(x:=1;x<=MAX_VENDORS;x:=x+1)
      DismissVendor(Stone,1);
    endfor

    var CStone := FindCStone(GetCity(Stone));
    
    if(CStone)
      for(x:=1;x<=MAX_BOUGHT_GUARDS;x:=x+1)
        DismissBoughtGuard(CStone,1);
      endfor
    endif

    if(Major)
      if(!CanMod(Major,MOD_TRACE))
        UndoMod(Major,MOD_TRACE);
        sleepms(300);
      endif
    
      Major.title_suffix := "";
      EraseObjProperty(Major,"Major");
      return 1;
    endif

    if(!GetObjProperty(Stone,"Major"))
      return error;
    endif

    EraseObjProperty(Stone,"Major");
    return 0;
  endif

  return error;
   
endfunction

function SetMajor(Stone)

  var Candidates := GetCandidates(Stone);
  var Winner     := 0;
  var MajorCity  := GetCity(Stone);
  var x;
  
  for(i:=1;i<=Candidates.size();i:=i+1)
    for(x:=1;x<=Candidates.size();x:=x+1)
      if(x != i)
        if(Candidates[i].votes > Candidates[x].votes)
          Winner := Winner + 1;
        endif
      endif
    endfor
    if(Winner == Candidates.size() - 1)
      Winner := Candidates[i];
      break;
    endif
    Winner := 0;
  endfor

  var Major := FindPlayerBySerial(Winner.serial);
  
  if(Winner.votes < MIN_VOTES or !Major or !Winner.votes or !Winner.serial)
    BroadCast("Burmistrz miasta "+MajorCity+" nie zostal wybrany!",FONT_NORMAL,RED_COLOR);
    return 0;
  endif

  BroadCast("Nowym Burmistrzem miasta "+MajorCity+" zostal"+ggm(Major,4)+" "+Major.name+"!",FONT_NORMAL,GREEN_COLOR);
  SetObjProperty(Stone,"Major",Major.serial);
  SetObjProperty(Stone,"WinVotes",Winner.votes);
  AddToHistory(MajorCity,"Wygral"+ggm(Major,4)+" wybory "+Winner.votes+" glosami");
  SetObjProperty(Major,"Major",MajorCity);
  EraseObjProperty(Stone,"VotingPers");
  Major.title_suffix := " [Burmistrz "+MajorCity+"]";
  return 1;

endfunction

function IsMajorByLocation(source, major)

	var cityName := GetCity(source);
	var majorCity := GetObjProperty(major, "Major");
	
	if(cityName == majorCity)
		return 1;
	else
		return 0;
		
	endif
	
	return 0;

endfunction

function IsGuardByLocation(source, guard)

	var cityName := GetCity(source);
	var guardCity := GetObjProperty(guard, "MajorGuard");
	
	if(cityName == guardCity)
		return 1;
	else
		return 0;
	endif
	
	return 0;
	
endfunction
	

function IsMajor(who)

  var MajorCity  := GetObjProperty(who,"Major");
  
  if(MajorCity)
    if(GetMajor(MajorCity) == who)
      return MajorCity;
    else
      EraseObjProperty(who,"Major");
      who.title_suffix := "";
    endif
  endif
  
  return 0;

endfunction


function FindVStone(City)

  var AllStones := GetAllVStones();
  return FindItemBySerial(AllStones[City]);

endfunction

function IsGuard(who)

  var CityGuard := GetObjProperty(who,"MajorGuard");
  
  if(CityGuard)
    if(who.serial in GetMajorGuards(FindVStone(CityGuard)))
      return CityGuard;
    else
      EraseObjProperty(who,"MajorGuard");
      who.title_suffix := "";
    endif
  endif
  return 0;

endfunction


function DismissGuard(Stone,Nr)

  var Guards := GetMajorGuards(Stone);
  var Guard  := Guards[Nr];

  if(Guard)
  
    Guard := FindPlayerBySerial(Guard);
    if(Guard)
      EraseObjProperty(Guard,"MajorGuard");
      if(!CanMod(Guard,MOD_TRACE))
        UndoMod(Guard,MOD_TRACE);
        sleepms(300);
      endif
      Guard.title_suffix := "";
    endif
    Guards.erase(Nr);
    SetObjProperty(Stone,"Guards",Guards);
    return 1;
  endif
  return 0;

endfunction


function GetMajorGuards(Stone)

  var Guards := GetObjProperty(Stone,"Guards");
  if(!Guards or !Guards.size())
    Guards := {};
  endif

  i := 1;
  foreach Guard in Guards
    if(!FindPlayerBySerial(Guard))
      Guards.erase(i);
    else
      i := i + 1;
    endif
  endforeach

  SetObjProperty(Stone,"Guards",Guards);
  return Guards;

endfunction

function AddMajorGuard(Stone,who)

  var Guards := GetMajorGuards(Stone);

  if(Guards.size() >= MAX_GUARDS)
    return 0;
  endif
  
  if(who.serial in Guards)
    return 0;
  endif
  
  if(IsGuard(who) or IsMajor(who))
    return 0;
  endif
  
  if(!IsCitizen(who,GetCity(Stone)))
    return 0;
  endif
  
  Guards.append(who.serial);
  AddToHistory(GetCity(Stone),"Powolal straznika, "+GetRealName(who));
  who.title_suffix := " [Straznik "+GetCity(Stone)+"]";
  SetObjProperty(who,"MajorGuard",GetCity(Stone));
  SetObjProperty(Stone,"Guards",Guards);

  return 1;

endfunction



function GetMajorVendors(Stone)

  var Vendors := GetObjProperty(Stone,"Vendors");
  if(!Vendors or !Vendors.size())
    Vendors := {};
  endif
  
  i := 1;
  foreach Vend in Vendors
    if(!FindNpcBySerial(Vend))
      Vendors.erase(i);
    else
      i := i + 1;
    endif
  endforeach

  SetObjProperty(Stone,"Vendors",Vendors);
  return Vendors;

endfunction

function DismissVendor(Stone,Nr)

  var Vendors := GetMajorVendors(Stone);
  var Vendor  := Vendors[Nr];

  if(Vendor)
    Vendor := FindNpcBySerial(Vendor);
    if(Vendor)
      SetObjProperty(Vendor,"KillMe",1);
      RestartScript(Vendor);
    endif
    Vendors.erase(Nr);
    SetObjProperty(Stone,"Vendors",Vendors);
    return 1;
  endif
  return 0;

endfunction

function AddMajorVendor(Stone,Loc,who)

  var Vendors := GetMajorVendors(Stone);

  if(Vendors.size() >= MAX_VENDORS)
    return 0;
  endif
  
  var Vendor := CreateNpc("playervendor",Loc.x,Loc.y,Loc.z);
  if(!Vendor)
    SendSysMessage(who,"Nie udalo Ci sie sprowadzic kupca!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  Vendors.append(Vendor.serial);
  SetVendorMaster(Vendor,who);
  SetObjProperty(Stone,"Vendors",Vendors);
  SendSysMessage(who,"Sprowadzil"+ggm(who,3)+" nowego kupca.",FONT_NORMAL,COLOR_GREEN);

  return 1;

endfunction

function AddToHistory(City,Text)

  var Stone   := FindCStone(City);
  var MStone  := FindVStone(City);
  var History := GetCityHistory(Stone);
  var Major   := GetMajor(GetCity(Stone));
  var Arr;

  if(Major)
    if(!History.size() or !History)
      History := {};
      History.append({});
    endif
    Arr := History[History.size()];
    if(!Arr or !Arr.size())
      Arr := {};
    endif

    if(ClStr(Text))
      Arr.append({GetRealName(Major),GetDate() + " - "+Text});
    endif

    History[History.size()] := Arr;
    SetObjProperty(Stone,"History",History);
  endif

endfunction

function EndHistory(City)

  var Stone   := FindCStone(City);
  var History := GetCityHistory(Stone);
    
  History[History.size() + 1] := {};
  SetObjProperty(Stone,"History",History);

endfunction
