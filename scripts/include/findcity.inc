//--------------------------------------------------------------------------

use uo;
use os;
use cfgfile;

//--------------------------------------------------------------------------

include "include/logerror";
include "include/const/cfgs";
include "include/const/misc";

//================================================================================
//--------------------------------------------------------------------------------
// Find City
//--------------------------------------------------------------------------------
//================================================================================

function FindCity(me)
  return FindCityCoords(me.x,me.y,me.realm);
endfunction

//--------------------------------------------------------------------------

function FindCityCoords(Mx,My,Realm)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);

  if(!CitiesConfig)
    LogError("FindCityCoords: Nie mozna bylo znalezc Configa Miast! ["+CITIES_CFG+"]");
    return error;
  endif

  foreach City in GetConfigStringKeys(CitiesConfig)
    var cfgCity := CitiesConfig[City];
    if(Mx >= cfgCity.sx and Mx <= cfgCity.ex and My >= cfgCity.sy and My <= cfgCity.ey and Realm == cfgCity.Realm)
      return City;
    endif
    sleepms(2);
  endforeach

  return error;

endfunction

//--------------------------------------------------------------------------

function GetCityDropLoc(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];

  return GetLocFromString(cfgCity.DropLoc);

endfunction

//--------------------------------------------------------------------------

function FindNearestCity(Mx, My, Realm, CityAlign := 0)

  var Result := struct;
  Result.+city := 0;
  Result.+dist := -1;
  Result.+realm;

  var CitiesConfig := ReadConfigFile(CITIES_CFG);

  if(!CitiesConfig)
    LogError("FindCityCoords: Nie mozna bylo znalezc Configa Miast! ["+CITIES_CFG+"]");
    return error;
  endif

  var curDist := 0;

  foreach City in GetConfigStringKeys(CitiesConfig)

    var cfgCity := CitiesConfig[City];

    if(CityAlign and cfgCity.al != CityAlign)
      continue;
    endif

    if(Mx >= cfgCity.sx and Mx <= cfgCity.ex and My >= cfgCity.sy and My <= cfgCity.ey and Realm == cfgCity.Realm)
    
      Result.dist := 0;
      Result.city := City;
      Result.realm := cfgCity.realm;
      return Result;

    elseif(Result.dist == -1)

      curDist := coordist(cfgCity.sx, cfgCity.sy, Mx, My);
      curDist := Min(curDist, coordist(cfgCity.sx, cfgCity.ey, Mx, My));
      curDist := Min(curDist, coordist(cfgCity.ex, cfgCity.sy, Mx, My));
      curDist := Min(curDist, coordist(cfgCity.ex, cfgCity.ey, Mx, My));

      Result.dist := curDist;
      Result.city := City;
      Result.realm := cfgCity.Realm;

    elseif(cfgCity.Realm == Realm)

      curDist := coordist(cfgCity.sx, cfgCity.sy, Mx, My);
      curDist := Min(curDist, coordist(cfgCity.sx, cfgCity.ey, Mx, My));
      curDist := Min(curDist, coordist(cfgCity.ex, cfgCity.sy, Mx, My));
      curDist := Min(curDist, coordist(cfgCity.ex, cfgCity.ey, Mx, My));

      if(Result.dist > curDist or Result.realm != Realm)
        Result.dist := curDist;
        Result.city := City;
        Result.realm := cfgCity.Realm;
      endif

    endif
    sleepms(2);
  endforeach

  return Result;

endfunction

//--------------------------------------------------------------------------

function GetCities()

  var CitiesConfig := ReadConfigFile(CITIES_CFG);

  if(!CitiesConfig)
    LogError("GetCities: Nie mozna bylo znalezc Configa Miast! ["+CITIES_CFG+"]");
    return error;
  endif

  return GetConfigStringKeys(CitiesConfig);

endfunction

//--------------------------------------------------------------------------

function GetCityCoords(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);

  if(!CitiesConfig)
    LogError("GetCityCoords: Nie mozna bylo znalezc Configa Miast! ["+CITIES_CFG+"]");
    return error;
  endif

  var cfgCity := CitiesConfig[City];

  if(!cfgCity)
    return 0;
  endif

  return {CInt(cfgCity.sx),CInt(cfgCity.sy),CInt(cfgCity.ex),CInt(cfgCity.ey)};

endfunction

//--------------------------------------------------------------------------

function IsInCity(who,City)

  if(!City)
    return 0;
  endif

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  if(!CitiesConfig)
    LogError("IsInCity: Nie mozna bylo znalezc Configa Miast! ["+CITIES_CFG+"]");
    return 0;
  endif
  
  var cfgCity := CitiesConfig[City];

  if(!cfgCity)
    return 0;
  endif
  
  if(cfgCity.Realm != who.realm)
    return 0;
  endif
  
  return (who.x >= cfgCity.sx and who.y >= cfgCity.sy and who.x <= cfgCity.ex and who.y <= cfgCity.ey);

endfunction

//--------------------------------------------------------------------------

function GetCityAlignment(x,y := 0,Realm := 0)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cCity        := x;

  if(!CitiesConfig[cCity])
    cCity := FindCityCoords(x,y,Realm);
  endif
  
  if(cCity)
    return CStr(CitiesConfig[cCity].al);
  endif
  
  return "";
  
endfunction

//--------------------------------------------------------------------------

function GetCityGuardTemplate(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];

  return cfgCity.GuardTemp;

endfunction

//==========================================================================
//--------------------------------------------------------------------------
//                                Buildings
//--------------------------------------------------------------------------
//==========================================================================

function GetCityBuildingsGround(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];

  var Grounds := dictionary,x;

  foreach Ground in GetConfigStringArray(cfgCity,"Building")
    Ground := SplitWords(Ground);
    for(x:=CInt(Ground[1]);x<=CInt(Ground[2]);x:=x+1)
      Grounds[CInt(x)] := 1;
    endfor
  endforeach
  
  return Grounds;

endfunction

//--------------------------------------------------------------------------

function GetCityBuildingsExGround(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];

  var Grounds := GetConfigStringArray(cfgCity,"ExGround");
  
  foreach Ground in Grounds
    Ground := SplitWords(Ground);
    Ground[1] := CInt(Ground[1]);
    Ground[2] := CInt(Ground[2]);
    Ground[3] := CInt(Ground[3]);
    Ground[4] := CInt(Ground[4]);
  endforeach
  
  var ExGrounds := dictionary,x,y;
  
  foreach Ground in Grounds
    for(x:=Ground[1];x<=Ground[3];x:=x+1)
      for(y:=Ground[2];y<=Ground[4];y:=y+1)
        ExGrounds[x+"-"+y] := 1;
      endfor
    endfor
  endforeach
  
  return ExGrounds;

endfunction

//--------------------------------------------------------------------------

function GetCityBuildGrounds(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];

  var Grounds := GetConfigStringArray(cfgCity,"BuildGround");

  foreach Ground in Grounds
    Ground := SplitWords(Ground);
    Ground[1] := CInt(Ground[1]);
    Ground[2] := CInt(Ground[2]);
    Ground[3] := CInt(Ground[3]);
    Ground[4] := CInt(Ground[4]);
  endforeach

  return Grounds;

endfunction

//--------------------------------------------------------------------------

function GetCityRealm(City)

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];
  
  if(cfgCity.Realm)
    return cfgCity.Realm;
  else
    return REALM_BRITANNIA;
  endif

endfunction

//--------------------------------------------------------------------------

function IsVampireCity(City)

  if(!City)
    return 0;
  endif

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];
  return CInt(cfgCity.Vampire);

endfunction

//--------------------------------------------------------------------------

function IsWearwolfCity(City)

  if(!City)
    return 0;
  endif

  var CitiesConfig := ReadConfigFile(CITIES_CFG);
  var cfgCity := CitiesConfig[City];
  return CInt(cfgCity.Wearwolf);

endfunction

//--------------------------------------------------------------------------

