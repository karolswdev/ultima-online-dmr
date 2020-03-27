//------------------------------------------------------------------------------

  const GODS_DATA              := ":gods:god";
  const GODS_CFG               := ":gods:gods";
  const TYPE_NAME              := "Types";
  const TYPE_TITLE             := "Type_";
  const GODS_MAXID             := 6;
  const PLAYER_TITLE           := "PL_";
  const PROP_ID                := "Id";
  const PROP_HISTORY           := "History_";
  
//------------------------------------------------------------------------------

function GetGodPoints(targ, godId)

  var GodName := GetGodNameById(godId);
  var Pts := GetObjProperty(targ, GodName);

  if(!Pts or Pts.pw == error)
    Pts := struct;
    Pts.+bw := 0;
    Pts.+pw := 0;
  endif

  return Pts;

endfunction

//-----------------------------------------------------------------------------

function SetGodPoints(targ, godId, bw, pw)

  var GodName := GetGodNameById(godId);
  var Pts := GetGodPoints(targ, godId);

  Pts.bw := Pts.bw + Cint(bw);
  Pts.pw := Pts.pw + CInt(pw);
  
  SetObjProperty(targ, GodName, Pts); 
  
endfunction

//-----------------------------------------------------------------------------

function GetGodNameById(Id)

  var Entries := GetGodsList();

  if(Entries.exists(Id))
    return Entries[Id];
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetGodsList()

  var GdCfg := ReadConfigFile(GODS_CFG);
  var List := dictionary;
  
  if(!GdCfg)
    LogError("GetGodsName: Nie udalo sie odczytac GodsCfg: "+GODS_CFG, LOGF_SCRIPTS);
  else
    if(!GdCfg[TYPE_NAME])
      LogError("GetGodsName: Brak grupy z nazwami Bogow ["+GODS_CFG+"]", LOGF_SCRIPTS);
    else
      var i;
      for(i:=1;i<=GODS_MAXID;i:=i+1)
        List[i] := GetConfigString(GdCfg[TYPE_NAME], TYPE_TITLE+i);
      endfor
    endif
  endif

  return List;

endfunction

//-----------------------------------------------------------------------------
