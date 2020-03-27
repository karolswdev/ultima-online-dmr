//=============================================================================
//-----------------------------------------------------------------------------
// Npc Names
//-----------------------------------------------------------------------------
//=============================================================================

function ConvertName(Name,Graphic,Idx := 0, Template := 0)

  if(!Name["<random>"])
    return Name;
  endif

  if(Idx)
    return AssignName( name, Idx , Graphic);
  endif

  var ngCfg := ReadConfigFile(NPCGRAP_CFG);

  if(Template)
    if(Template[1, 6] == "tokuno" and ngCfg[Graphic].TokunoNIdx)
      return AssignName(Name, ngCfg[Graphic].TokunoNIdx);
    endif
  endif

  if(ngCfg[graphic].NameIdx)
    return AssignName(name, ngCfg[graphic].NameIdx);
  endif

  return " ";

endfunction

//--------------------------------------------------------------------------------

function AssignName( Name, Index, Graphic := 0)

  if(Graphic == CID_HUMAN_FEMALE or Graphic == CID_ELF_FEMALE)
    Index := Index + 1;
  endif

  var RName := GetRandomNpcName( Index );
  Name["<random>"] := "";

  if(RName)
    Name := RName + Name;
  endif
  
  return Name;

endfunction

//--------------------------------------------------------------------------------

function GetRandomNpcName( Index )

  var NamesCfg     := ReadConfigFile( NAMES_CFG );
  var Count        := NamesCfg[Index].Count;
  var RName        := "";

  if(!NamesCfg[index])
    LogError("GetRandomNpcName: Blad konfiga "+NAMES_CFG+", brak Idx: "+index);
    return 0;
  endif

  if(Count)
    var Rand := RandomDiceRoll("1d"+CStr(Count));
    RName := GetConfigString(NamesCfg[index],"Name"+Rand);
    if(!RName)
      LogError("GetRandomNpcName: Blad konfiga "+NAMES_CFG+", brak imienia: Idx: "+Index+" Num: "+Rand+" N: "+RName);
      return 0;
    endif
  else
    LogError("GetRandomNpcName: Blad konfiga "+NAMES_CFG+", brak ilosci: Idx: "+Index+" C: "+Count);
    return 0;
  endif
  
  return RName;

endfunction

//--------------------------------------------------------------------------------

function GetNpcName(Npc)

  var nPos := find(Npc.name," ",1);

  if(nPos)
    return Npc.name[1,nPos-1];
  endif

  return "";

endfunction

//--------------------------------------------------------------------------------

function CheckNpcName(Npc)

  if(!Npc.isa(POLCLASS_NPC))
    return 1;
  endif

  var cfgElem := GetNpcConfig(Npc);

  if(!cfgElem.name["<random>"])
    return 1;
  endif

  var Idx := CInt(cfgElem.NameIdx);

  if(!Idx)
    var ngCfg := ReadConfigFile(NPCGRAP_CFG);
    if(Npc.npctemplate[1, 6] == "tokuno" and ngCfg[Npc.graphic].TokunoNIdx)
      Idx := ngCfg[Npc.graphic].TokunoNIdx;
    else
      Idx := CInt(ngCfg[Npc.graphic].NameIdx);
    endif
  else
    if(Npc.graphic == CID_HUMAN_FEMALE or Npc.graphic == CID_ELF_FEMALE)
      Idx += 1;
    endif
  endif

  if(Idx)
    var NpcName := GetNpcName(Npc);
    if(NpcName)
      if(!NpcNameExists(Idx, NpcName))
        var NewName := GetRandomNpcName(Idx);
        var CurName := GetName(Npc);
        CurName[NpcName] := NewName;
        SetName(Npc, CurName);
        return 0;
      endif
    endif
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function NpcNameExists(Index, Name)

  var NamesCfg     := ReadConfigFile( NAMES_CFG );
  var Count        := NamesCfg[Index].Count;
  var nName;

  if(!NamesCfg[index])
    return 0;
  endif

  if(Count)
    for i := 1 to Count
      nName := GetConfigString(NamesCfg[index], "Name"+i);
      if(!nName)
        LogError("GetRandomNpcName: Blad konfiga "+NAMES_CFG+", brak imienia: Idx: "+Index+" Num: "+i+" N: "+nName);
        return 0;
      elseif(nName == Name)
        return 1;
      endif
      if(!(i % 1000))
        Sleepms(1);
      endif
    endfor
  else
    LogError("GetRandomNpcName: Blad konfiga "+NAMES_CFG+", brak ilosci: Idx: "+Index+" C: "+Count);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function AssignTitle( Name, Index )

  var NamesCfg     := ReadConfigFile( NAMES_CFG );
  var Count        := CStr(NamesCfg["T"+Index].Count);
  var RTitle       := GetConfigString(NamesCfg[index],"Title"+RandomDiceRoll("1d"+Count));

  if(RTitle)
    Name := Name + " " + RTitle;
  endif

  return Name;

endfunction

//-----------------------------------------------------------------------------
