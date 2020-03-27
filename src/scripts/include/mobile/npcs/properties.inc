//--------------------------------------------------------------------------------

function GetNpcElementals(Npc)

  var NpcTemp;

  if(!Npc)
    return array;
  elseif(Npc.isa(POLCLASS_NPC))
    NpcTemp := Npc.npctemplate;
  else
    NpcTemp := Npc;
  endif

  var cfgElem := GetNpcConfig(Npc);
  
  if(!cfgElem)
    return array;
  endif
  
  var Elems := GetConfigStringArray(cfgElem, "elemental");
  if(!Elems or !Elems.size())
    return array;
  endif

  return Elems;

endfunction

//--------------------------------------------------------------------------------

function SetNpcGraphic(Npc, NewGrap)

  if(NewGrap)
    Npc.graphic := NewGrap;
    SetObjProperty(Npc,"Graphic", NewGrap);
  endif

endfunction

//--------------------------------------------------------------------------------

function SetNpcColor(Npc, NewColor)

  Npc.color := NewColor;
  SetObjProperty(Npc, "SetColor", NewColor);

endfunction

//--------------------------------------------------------------------------------

function GetNpcNameFromTemplate( template )

  var cfgElem := GetNpcConfig(template);
  var Graphic := cfgElem.objtype;

  if(!cfgElem)
    return "";
  endif

  if(CInt(cfgElem.objtype) == CID_HUMAN_MALE or CInt(cfgElem.objtype) == CID_HUMAN_FEMALE or CInt(cfgElem.objtype) == CID_ELF_MALE or CInt(cfgElem.objtype) == CID_ELF_FEMALE)
    var Gender := cfgElem.Gender;
    if(!Gender)
      Gender := RandomInt(2)+1;
    endif
    Gender  := Gender - 1;
    Graphic := cfgElem.objtype + Gender;
  endif

  return ConvertName(cfgElem.Name, Graphic, CInt(cfgElem.NameIdx), template);

endfunction

//--------------------------------------------------------------------------------

function GetNpcColorFromTemplate( template )

  var cfgElem := GetNpcConfig(template);
  var Color   := cfgElem.colors;

  if(Color)
    Color := SplitWords(Color);
    Color := Color[RandomDiceRoll("1d"+Color.size())];
  else
    Color := cfgElem.color;
  endif

  return Color;

endfunction

//-----------------------------------------------------------------------------

function IsNpcGuard(Npc)

  if(Npc)
    if(Npc.isa(POLCLASS_NPC))
      if(Npc.script["guard"])
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanNpcShearing(Npc)

  if(IsSummoned(Npc))
    return RetError("To stworzenie nie wydaje sie byc w pelni zywe.");
  endif

  var cfgElem := GetNpcConfig(Npc);

  if(cfgElem.ReGrowGrap)
    var ObjType := CInt(cfgElem.Objtype);
    if(Npc.graphic == ObjType)
      return RetError("To zwierze nie jest jeszcze gotowe do strzyzenia.");
    endif
  else
    return RetError("Tego stworzenia nie da sie ostrzyc.");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function NpcShearing(Npc)

  var cfgElem := GetNpcConfig(Npc);

  if(cfgElem.ReGrowGrap)
    var ObjType := CInt(cfgElem.Objtype);
    if(Npc.graphic != ObjType)
      SetNpcGraphic(Npc, ObjType);
      SetObjProperty(Npc,"NextRegrow",ReadGameClock() + 900 + RandomInt(300));
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function SaveSkillSum(Npc)

  SetObjProperty(Npc, PROP_SK_SUMMONED, ReadGameClock());

endfunction

//--------------------------------------------------------------------------------

function IsSkillSum(Npc)

  return CInt(GetObjProperty(Npc, PROP_SK_SUMMONED));

endfunction

//-----------------------------------------------------------------------------------//

function GetRunSpeedLowerAmount(Mob, Mod)

  // niektore moby maja wiecej run_speeda niz Max, czyli 250
  // wtedy procentowo zmniejszanie nic nie daje
  // nalezy najpierw obnizyc do 250, a potem robic z tego procent.
  // najlepiej byloby poprawic npece, co by nie mialy runspeeda > 250, ale to moze byc ciezkie.
  // dlatego ta funkcja.

  var Diff := 0;

  var RunSpeed := ToMax(Mob.run_speed, MAX_RUN_SPEED);

  if(Mob.run_speed > MAX_RUN_SPEED)
    Diff := Mob.run_speed - MAX_RUN_SPEED;
  endif

  Diff += CInt(RunSpeed * Mod);

  return Diff;

endfunction

//-----------------------------------------------------------------------------

function GetNpcTemplateFromGroup(GroupName)

  var GroupsCfg := ReadConfigFile(":spawnnet:groups");
  var GroupElem := GroupsCfg[GroupName];

  if(!GroupElem)
    var CfgElem := GetNpcConfig(GroupName);
    
    if(!CfgElem)
      return RetError("Nie znaleziono npeca bo template ani grupa ["+GroupName+"] nie istnieje!");
    endif
    
    return GroupName;
  endif
  
  var Templates := GetConfigStringArray(GroupElem, "spawn");
  
  if(Templates.size())
    return Templates[RandomInt(Templates.size())+1];
  else
    return RetError("Nie znaleziono npeca bo grupa spawnnetu ["+GroupName+"] jest pusta!");
  endif

endfunction

//--------------------------------------------------------------------------------
