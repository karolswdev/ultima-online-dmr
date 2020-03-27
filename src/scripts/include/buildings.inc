//-----------------------------------------------------------------------------

include "include/logerror";
include "include/coords";

//-----------------------------------------------------------------------------

const BUILDINGS_CFG := ":datacfg:bld_";

//-----------------------------------------------------------------------------

var BuildCfg, BuildCfgRealm;

//=============================================================================
//-----------------------------------------------------------------------------
// Buildings
//-----------------------------------------------------------------------------
//=============================================================================

function IsInBuilding(x,y,Realm)

  // goqsane: This is annoying.
  return 0;
  
  if(!x or !y or !Realm)
    return 0;
  endif
  
  if(!BuildCfg or BuildCfgRealm != Realm)
    BuildCfg := ReadConfigFile(BUILDINGS_CFG+Realm);
    BuildCfgRealm := Realm;
  endif

  if(!BuildCfg)
    LogError("IsInBuilding: Nie znaleziono konfiga budynkow R: "+Realm+" Cfg: "+BuildCfg);
    return 0;
  endif

  if(BuildCfg[CStr(x)+"-"+CStr(y)])
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInBuildingEx(x,y,Realm, z := error)

  // goqsane: This is annoying.
  return 0;

  if(z == error)
    z := GetStandZ(x, y, Realm);
  endif

  if(z == error)
    return 0;
  endif

  var mZ := GetStandZ(x, y, Realm, z+20);
  if(DistZ(mZ, z) >= 20 or mZ == error)
    return 1;
  else
    mZ := GetStandZ(x, y, Realm, z+30);
    if(DistZ(mZ, z) >= 30 or mZ == error)
      return 1;
    else
      mZ := GetStandZ(x, y, Realm, z+40);
      if(DistZ(mZ, z) >= 40 or mZ == error)
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AddBuildingCoords(x,y,City)

  UnloadConfigFile(BUILDINGS_CFG);
  BuildCfg := ReadConfigFile(BUILDINGS_CFG);

  if(CInt(x) and CInt(y))
    if(!IsInBuilding(x,y,GetCityRealm(City)))
      AppendConfigFileElem(BUILDINGS_CFG+GetCityRealm(City),"Bd",CStr(CInt(x))+"-"+CStr(CInt(y)),{});
      return 1;
    endif
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------
