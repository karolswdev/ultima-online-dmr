//-----------------------------------------------------------------------------

function IsRaceEvilForSkill(who, SkID, byref newPrefix)

  var skElem := GetSkillConfigElem(SkID);

  if(skElem.MakesEvil)

    var SkVal := GetBaseSkill(who, SkID);

    if(CInt(skElem.MakesEvil) <= SkVal)
      newPrefix := ClStr(skElem.RacePrefix);
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsRaceEvilForStatus(who)

  if(who.murderer)
    if(GetMurdersCount(who) >= 15)
      return 1;  
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetProperRaceTitle(who)

  if(who.cmdlevel)
    return "Bog";
  endif

  var newPrefix;
  var newTitle;

  var bRes := IsRaceEvilForSkill(who, SKILLID_NECRO, newPrefix);

  if(!bRes)
    bRes := IsRaceEvilForSkill(who, SKILLID_WEARWOLF, newPrefix);
  endif
  
  if(!bRes)
    bRes := IsRaceEvilForSkill(who, SKILLID_VAMPIRE, newPrefix);
  endif
  
  bRes := IsRaceEvilForStatus(who);

  if(newPrefix)
    newTitle := newPrefix + " ";
  else
    newTitle := "";
  endif

  if(bRes)
    newTitle += ClStr(GetRaceConfig(who).EvilName);
  else
    newTitle += ClStr(GetRaceConfig(who).Name);
  endif

  return newTitle;

endfunction

//-----------------------------------------------------------------------------

function ValidateRace(who)

  // TODO: goqsane
  return;

  var newTitle := GetProperRaceTitle(who);

  if(GetTitleRace(who) != newTitle)
    SetTitleRace(who, newTitle);
    SendSysMessage(who, "Twoj charakter zmienil sie. Jestes teraz znan"+ggm(who,6)+" jako "+newTitle+".", FONT_NORMAL, COLOR_WHITE);
  endif

endfunction

//-----------------------------------------------------------------------------

function QualifyForEvilRace(who)

endfunction

//-----------------------------------------------------------------------------