//=====================================================================
//---------------------------------------------------------------------
// Races
//---------------------------------------------------------------------
//=====================================================================

const ELF_RACE                         := "elf";
const DWARF_RACE                       := "dwarf";
const ISTARI_RACE                      := "istari";
const HUMAN_RACE                       := "human";
const HOBBIT_RACE                      := "hobbit";
const GNOM_RACE                        := "gnom";

const CLASS_WARRIOR                    := "Warrior";
const CLASS_ARCHER                     := "Archer";
const CLASS_MAGE                       := "Mage";

//-----------------------------------------------------------------------------

const RACE_LOW_SK_VALUE                := 500;
const RACE_HIGH_SK_VALUE               := 1050;

//-----------------------------------------------------------------------------

function GetCharClass(who)

  var CharData := GetObjProperty(who, "CharData");

  if(CharData)
    return CharData[1];
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCharClassTitle(who)

  var Title;

  if(who.cmdlevel)
    Title := "Bog";
  else
    var cfgElem := GetCharClassConfig(who);
    Title := cfgElem.Title;

    if(!Title)
      Title := "Nieznana";
    endif
  endif

  return Title;

endfunction

//-----------------------------------------------------------------------------

function GetCharClassConfig(Class)

  var Cfg := ReadConfigFile(CLASSES_CFG);

  if(Class.isa(POLCLASS_MOBILE))
    return Cfg[GetCharClass(Class)];
  endif
    
  return Cfg[Class];

endfunction

//-----------------------------------------------------------------------------

function GetCharRace(who)

  var CharData := GetObjProperty(who, "CharData");

  if(CharData)
    return CharData[2];
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetCharRace(who, RaceId, Color := 0)

  var RacesCfg := ReadConfigFile(RACES_CFG);

  if(!RacesCfg[RaceId])
    return 0;
  endif

  var CharData := GetObjProperty(who, "CharData");

  CharData[2] := RaceId;
  SetObjProperty(who, "CharData", CharData);

  if(!Color)
    Color := SplitWords(RacesCfg[RaceId].Colors);
    COlor := CInt(Color[1]);
  endif

  who.title_race := RacesCfg[RaceId].Name;
  who.truecolor := Color;
  who.color := Color;
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetRaceConfig(Race)

  if(Race.isa(POLCLASS_MOBILE))
    Race := GetCharRace(Race);
  endif

  var Cfg := ReadConfigFile(RACES_CFG);

  if(Race)
    return Cfg[Race];
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetRaceWeightModifier(who)

  var Mod := CDbl(GetRaceConfig(who).WeightMod);

  if(!Mod)
    return 1.0;
  endif

  return Mod;

endfunction

//-----------------------------------------------------------------------------

function GetRaceGraphic(who)

  var RaceElem := GetRaceConfig(who);
  var Graphic;

  case(who.gender)
    0:  Graphic := CInt(RaceElem.MaleGraphic);
    1:  Graphic := CInt(RaceElem.FemaleGraphic);
  endcase

  if(!Graphic)
    // jak nie ma to zwracamy humana - ta funkcja MUSI cos zwrocic sensownego.
    case(who.gender)
      0:  Graphic := CID_HUMAN_MALE;
      1:  Graphic := CID_HUMAN_FEMALE;
    endcase

  endif

  return Graphic;

endfunction

//-----------------------------------------------------------------------------

function GetCharSubClassTitle(who)

  if(who.cmdlevel)
    return "Niesmiertelny";
  endif

  var CraftSkills := {
    SKILLID_ALCHEMY,
    SKILLID_ARMORER,
    SKILLID_BOWCRAFT,
    SKILLID_BLACKSMITHY,
    SKILLID_CARPENTRY,
    SKILLID_CARTOGRAPHY,
    SKILLID_COOKING,
    SKILLID_TAILORING,
    SKILLID_TINKERING,
    SKILLID_FISHING,
    SKILLID_HERDING,
    SKILLID_FARMING,
    SKILLID_LUMBERJACKING,
    SKILLID_MINING
  };

  var CraftMaxVal := 0;
  var CraftMaxID  := 0;
  var CraftSkCnt  := 0;

  foreach SkillID in CraftSkills
    if(GetBaseSkillBaseValue(who, SkillID) > 700)
      CraftSkCnt:= CraftSkCnt + 1;
    endif

    if(GetBaseSkillBaseValue(who, SkillID) > CraftMaxVal)
      CraftMaxVal := GetBaseSkillBaseValue(who, SkillID);
      CraftMaxId := SkillID;
    endif
  endforeach

  var SubClass;
  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);

  var OtherSubClassSkills := {
    SKILLID_NECRO,
    SKILLID_DRUID,
    SKILLID_VAMPIRE,
    SKILLID_WEARWOLF,
    SKILLID_PALADINISM,
    SKILLID_FANATICISM,
    SKILLID_KENSAI,
    SKILLID_BARBARIAN,
    SKILLID_PROVOCATION,
    SKILLID_ENTICEMENT,
    SKILLID_PEACEMAKING
  };

  var OtherSCSkill;
  foreach SkillID in OtherSubClassSkills
    if(GetBaseSkillBaseValue(who, SkillID) > 700)
      OtherSCSkill := SkillID;
    endif
  endforeach

  var BMSkills := GetBaseSkillBaseValue(who, SKILLID_TACTICS) + GetBaseSkillBaseValue(who, SKILLID_ANATOMY);

  var OSks  := {
    GetBaseSkillBaseValue(who, SKILLID_MACEFIGHTING),
    GetBaseSkillBaseValue(who, SKILLID_WRESTLING),
    GetBaseSkillBaseValue(who, SKILLID_FENCING)
  };

  if(    OSks[1] >= OSks[2] and OSks[1] >= OSks[3])
    BMSkills := BMSkills + OSks[1];
  elseif(OSks[2] >= OSks[1] and OSks[2] >= OSks[3])
    BMSkills := BMSkills + OSks[2];
  elseif(OSks[3] >= OSks[1] and OSks[3] >= OSks[2])
    BMSkills := BMSkills + OSks[3];
  endif

  if(OtherSCSkill)
    SubClass := AttrCfg[GetAttributeIdBySkillId(OtherSCSkill)].Title;
  elseif(BMSkills > 1800 and GetCharClass(who) == CLASS_MAGE)
    SubClass := "Mag Bitewny";
  elseif(CraftMaxVal > 700)
    SubClass := "Rzemieslnik";
  else
    SubClass :="Brak";
  endif

  return SubClass;

endfunction

//-----------------------------------------------------------------------------

function GetHairGrowTime(who)

  return Max(CInt(GetRaceConfig(who).HairGrowTime), 3600);

endfunction

//-----------------------------------------------------------------------------

function GetBeardGrowTime(who)

  return Max(CInt(GetRaceConfig(who).BeardGrowTime), 3600);

endfunction

//-----------------------------------------------------------------------------

function IsCharCreationFinished(who)

  var CharData := GetObjProperty(who, "CharData");

  if(CharData != error)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCreationConfig(Name := 0)

  var Cfg := ReadConfigFile(":character:creation");

  if(Name)
    return Cfg[Name];
  else
    return Cfg["Creation"];
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCreationMaxStats(Account)

  var Time := GetAccountExistsTime(Account);
  var Cfg  := GetCreationConfig();

  if(Time > 0 and Time < CInt(Cfg.PrivMaxTime))
    return CInt(Cfg.MaxPrivStats);
  else
    return CInt(Cfg.MaxStats);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCreationMaxSkills(Account)

  var Time := GetAccountExistsTime(Account);
  var Cfg  := GetCreationConfig();

  if(Time > 0 and Time < CInt(Cfg.PrivMaxTime))
    return CInt(Cfg.MaxPrivSkills);
  else
    return CInt(Cfg.MaxSkills);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCharClassSkills(who)

  var cfgClass := GetCharClassConfig(who);
  var Skills := GetConfigStringArray(cfgClass, "Attribute");

  foreach Skill in Skills
    Skill := GetSkillIdByAttributeId(Skill);
  endforeach

  return Skills;

endfunction

//-----------------------------------------------------------------------------

function GetCreationStartSkills()

  var List := array;
  var i, maxId := GetMaxSkillId();

  for(i:=0; i<=maxId; i+=1)
    if(!GetSkillConfigElem(i).NoStartSk)
      List.append(i);
    endif
  endfor

  return List;

endfunction

//-----------------------------------------------------------------------------

function GetCharGroupedSkills(who, limitSkills := 0, bAll := 0)

  var SkillCfg := ReadConfigFile(ATTRIBUTES_CFG);
  var Attributes;
  var Groups := dictionary;
  var cfgClass := GetCharClassConfig(who);

  if(!IsStaff(who) and !bAll and !who.isa(POLCLASS_NPC) and cfgClass)
    Attributes := GetConfigStringArray(cfgClass, "Attribute");
  else
    Attributes := GetConfigStringKeys(SkillCfg);
  endif

  foreach Attribute in Attributes

    var SkillId := SkillCfg[Attribute].SkillId;

    if(limitSkills and !(SkillId in limitSkills))
      continue;
    endif

    if(!Groups.exists(SkillCfg[Attribute].AttrGroup))
      Groups[SkillCfg[Attribute].AttrGroup] := array;
    endif

    Groups[SkillCfg[Attribute].AttrGroup].append(SkillId);
  endforeach

  return Groups;

endfunction

//-----------------------------------------------------------------------------

function IsInStartLocation(who)

  var cfgElem := GetCreationConfig();
  var Loc     := SplitWords(cfgElem.Location);

  if(who.realm != Loc[4])
    return 0;
  endif

  var Start   := SplitWords(cfgElem.LocStart);
  var End     := SplitWords(cfgElem.LocEnd);

  if(IsInRect(who.x, who.y, Start, End))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ValidatePlayerCreation(who)

  SendSysMessage(who, "ValidatePlayerCreation()");
  var CharData := GetObjProperty(who, "CharData");
  
  if(CharData)
    return 0;
  endif
  
  SendSysMessage(who, "ValidatePlayerCreation() 2");

  MoveObject(who, 5135, 2063, 0, "britannia", MOVEOBJECT_FORCELOCATION);
  who.frozen := 1;
  start_script(":character:choose", array(who));
  return 1;

endfunction

//-----------------------------------------------------------------------------

function MoveToStartPlace(who, ClassID)

  var Cfg := ReadConfigFile(CLASSES_CFG);

  if(!Cfg[ClassID])
    return 0;
  endif

  var Loc := GetLocFromString(Cfg[ClassID].StartLoc);

  if(MoveObject(who, Loc.x, Loc.y, Loc.z, Loc.realm, MOVEOBJECT_FORCELOCATION))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetClassRepresentative(MercType)

  var Cfg := ReadConfigFile(CLASSES_CFG);

  foreach Class in GetConfigStringKeys(Cfg)
    var Temps := GetConfigStringArray(Cfg[Class], "Representative");

    if(MercType in Temps)
      return Class;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
