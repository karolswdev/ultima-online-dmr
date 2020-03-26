use uo;
use os;
use cfgfile;

include "include/finding";
include "include/findcity";

const CITY_LEAVE_TIME   := 172800;

const UOBJ_CITYSTONE    := 0xA205;

const TAXES_TIME        := 84600;
const DEF_TAXRATE       := 100;
const MAX_TAXRATE       := 2000;
const TAX_CHANGETIME    := 21600;

const CITIZEN_BREAKTIME := 172800;

const MAX_BOUGHT_GUARDS := 10;
const GUARD_PRICE       := 2500;

const GUARD_TIME        := 300;

function GetCity(Stone)

    if(!GetObjProperty(Stone,"City"))
        SetObjProperty(Stone,"City",FindCity(Stone));
    endif

    return GetObjProperty(Stone,"City");

endfunction

function GetAllCStones()

    if(!GetGlobalProperty("CitizenStones"))
        SetGlobalProperty("CitizenStones",dictionary);
    endif

    return GetGlobalProperty("CitizenStones");

endfunction

function SetAllCStones(ExStones)
    SetGlobalProperty("CitizenStones",ExStones);
endfunction

function FindCStone(City)

    var AllStones := GetAllCStones();
    return FindItemBySerial(AllStones[City]);

endfunction

function GetCitizens(Stone)

    if(!Stone.isa(POLCLASS_ITEM))
        var AllStones   := GetAllCStones();
        Stone           := FindItemBySerial(AllStones[Stone]);
    endif

    if(!Stone)
        return 0;
    endif

    var Citizens := GetObjProperty(Stone, "Citizens");
    if(!Citizens or !Citizens.size())
        Citizens := {};
    endif

    var CitId := 1;
    foreach Citizen in GetObjProperty(Stone, "Citizens")
        Citizen := FindPlayerBySerial(Citizen);
        if(!Citizen)
            Citizens.erase(CitId);
        else
            CitId := CitId + 1;
        endif
    endforeach

    SetCitizens(Stone,Citizens);

    return Citizens;

endfunction

function AddCitizen(Stone, who)

    var WhoData  := CheckCitizenData(who);
    if(WhoData.city)
        return 0;
    endif

    var Align := GetCityAlignment(City);
    if(Align == "evil")
        if(!who.murderer)
            return 0;
        endif
    elseif(Align == "good")
        if(who.murderer)
            return 0;
        endif
    endif

    var Citizens := GetCitizens(Stone);
    Citizens.append(who.serial);
    SetCitizens(Stone,Citizens);
    SaveCitizenData(who,Stone);

    return 1;

endfunction

function SetCitizens(Stone, Citizens)
    SetObjProperty(Stone,"Citizens", Citizens);
endfunction

function GetCitizenData(who)
    return GetObjProperty(who,"Citizen");
endfunction

function SaveCitizenData(who, Stone)

    var Data := struct;
    Data.+city     := GetCity(Stone);
    Data.+signtime := ReadGameClock();

    SetObjProperty(who, "Citizen", Data);

endfunction

function IsCitizen(who, City)

    if(CheckCitizenData(who).city == City)
        return 1;
    endif

    return 0;

endfunction

function CheckCitizenData(who)

    var Data := GetCitizenData(who);
    if(Data and Data.city)
        var Citizens := GetCitizens(Data.city);
        if(!(who.serial in Citizens))
            EraseObjProperty(who,"Citizen");
            return 0;
        endif
        return Data;
    endif

    return 0;

endfunction


function CanDestroyStone(Stone)

    if(IsAdmin(FindPlayerBySerial(GetObjProperty(Stone,"Destroying"))))
        return 1;
    endif

    return 0;

endfunction

function GetTaxRate(Stone)

    if(!GetObjProperty(Stone, "TaxRate"))
        SetObjProperty(Stone, "TaxRate",DEF_TAXRATE);
    endif

    return CInt(GetObjProperty(Stone, "TaxRate"));

endfunction

function SetTaxRate(Stone,Rate)
    SetObjProperty(Stone,"TaxRate",CInt(Rate));
endfunction

function GetCityHistory(Stone)

    if(!GetObjProperty(Stone, "History"))
        SetObjProperty(Stone ,"History", {});
    endif

    return GetObjProperty(Stone, "History");

endfunction

function GetCityGold(Stone)
    return CInt(GetObjProperty(Stone, "CityGold"));
endfunction

function SetCityGold(Stone, Amt)
    SetObjProperty(Stone, "CityGold", CInt(Amt));
endfunction

function GetBoughtGuards(Stone)

  var Guards := GetObjProperty(Stone,"BoughtGuards");
  if(!Guards or !Guards.size())
    Guards := {};
  endif

  var GuardId := 1;
  foreach Guard in GetObjProperty(Stone,"BoughtGuards")
    Guard := FindItemBySerial(Guard);
    if(!Guard)
      Guards.erase(GuardId);
    else
      GuardId := GuardId + 1;
    endif
  endforeach

  SetObjProperty(Stone,"BoughtGuards",Guards);
  return Guards;
  
endfunction

function BuyGuard(Stone, who)

    var City   := IsMajor(who);
    var Guards := GetBoughtGuards(Stone);
    if(!City)
        return;
    endif

    if(Guards.size() >= MAX_BOUGHT_GUARDS)
        SendSysMessage(who, "Nie mozesz kupic wiecej straznikow.",FONT_NORMAL,RED_COLOR);
        return;
    endif

    if(GetCityGold(Stone) < GUARD_PRICE)
        SendSysMessage(who, "Miasto ma za malo pieniedzy.",FONT_NORMAL,RED_COLOR);
        return;
    endif

    SendSysMessage(who, "Gdzie chcesz by straznik stal?",FONT_NORMAL,GREEN_COLOR);

    var Loc := TargetCoordinates(who);
    if(!Loc)
        SendSysMessage(who, "Anulowano.",FONT_NORMAL,RED_COLOR);
        return;
    endif

    if(FindCityCoords(Loc.x,Loc.y, Loc.Realm) != City)
        SendSysMessage(who, "Jestes poza terytorium swojego miasta.",FONT_NORMAL,RED_COLOR);
        return;
    endif

    foreach Npc in ListMobilesNearLocation(Loc.x,Loc.y,Loc.z,20)
        if(GetObjProperty(Npc,"MajorGuard") and Npc.isa(POLCLASS_NPC))
            SendSysMessage(who, "Nie daleko stoi juz straznik.",FONT_NORMAL,RED_COLOR);
            return;
        endif
    endforeach

    //dodane przez AsYlum-
    if(GetObjProperty(who, "BuyingGuard") <= CInt(ReadGameClock()))
        SendSysMessage(who, "Nie mozesz sprowadzic nowego straznika tak szybko.",FONT_NORMAL,RED_COLOR);
        return;
    endif

    var cfgfile := ReadConfigFile("::gzone");
    var entries := GetConfigStringKeys(cfgfile);
    var element;
    var range;
    var guardzone := 0;
    foreach listing in entries
        element := cfgfile[listing];
        range   := element.range;
        range   := SplitWords(range);
        if((Loc.x >= CInt(range[1])) && (Loc.x <= CInt(range[3])) && (Loc.y >= CInt(range[2])) && (Loc.y <= CInt(range[4])))
            guardzone := 1;
            SetObjProperty(who, "BuyingGuard", CInt(ReadGameClock() + GUARD_TIME));
            sleep(GUARD_TIME);
            var Guard := CreateNpc(GetCityGuardTemplate(CIty),Loc.x,Loc.y,Loc.z);
            if(Guard)
                SetCityGold(Stone, GetCityGold(Stone) - GUARD_PRICE);
                SetObjProperty(Guard, "MajorGuard", City);
                SendSysMessage(who, "Sprowadzil"+ggm(who,3)+" nowego straznika.",FONT_NORMAL,GREEN_COLOR);
                Guards.append(Guard.serial);
                SetObjProperty(Stone, "BoughtGuards", Guards);
                AddToHistory(City, "Sprowadzil "+Guards.size()+" dodatkowego straznika.");
                EraseObjProperty(who, "BuyingGuard");
            else
                SendSysMessage(who,"Nie udalo Ci sie sprowadzic nowego straznika.",FONT_NORMAL,RED_COLOR);
            endif
            break;
        endif
    endforeach
    if(guardzone == 0)
        SendSysMessage(who, "Ten obszar jest poza twoja kontrola.",FONT_NORMAL,RED_COLOR);
        return;
    endif
    //END dodane przez AsYlum-

endfunction

function DismissBoughtGuard(Stone, Nr)

    var Guards := GetBoughtGuards(Stone);
    var Guard  := Guards[Nr];
    if(Guard)
        if(Guard)
            Guard := FindNpcBySerial(Guard);
            KillNPC(Guard);
        endif
        Guards.erase(Nr);
        SetObjProperty(Stone,"BoughtGuards",Guards);
    endif

endfunction
