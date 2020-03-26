//-----------------------------------------------------------------------------

function GetSkillCap(who)

  var Cap         := DEFAULT_SKILL_CAP;
  var OnlineTimer := CInt(GetObjProperty(who, PROP_ONLINETIMER));

  if(OnlineTimer >= SKCAP_TIME_BARIER)
    var Count := CInt((OnlineTimer - SKCAP_TIME_BARIER) / SKCAP_TIME_DIVIDER);
    Cap += SKCAP_ADD_BASE_POINTS * Count;
  endif
  
  if(Cap > MAX_SKILL_CAP)
    Cap := MAX_SKILL_CAP;
  endif
  
  if(GetObjProperty(who, "GMDebug"))
	SendSysMessage(who, "Skillcap: " + Cap + " before adding " + CInt(GetObjProperty(who, "AddSkCapBonus")) + " to it.");
  endif
  
  Cap := Cap + CInt(GetObjProperty(who, "AddSkCapBonus"));
  
  if(GetObjProperty(who, "GMDebug"))
	SendSysMessage(who, "Skillcap is now: " + Cap);
  endif

  who.skillcap := CInt((Cap + SK_CRAFT_SKILLCAP_ADD) / 10);

  return Cap;

endfunction

//-----------------------------------------------------------------------------

function GetStatCap(who)

  var Cap         := DEFAULT_STAT_CAP;
  var OnlineTimer := CInt(GetObjProperty(who, PROP_ONLINETIMER));

  if(OnlineTimer >= STCAP_TIME_BARIER)
    var Count := CInt((OnlineTimer - STCAP_TIME_BARIER) / STCAP_TIME_DIVIDER);
    Cap += STCAP_ADD_BASE_POINTS * Count;
  endif

  if(Cap > MAX_STAT_CAP)
    Cap := MAX_STAT_CAP;
  endif

  who.statcap := CInt(Cap / 10);

  return Cap;

endfunction

//-----------------------------------------------------------------------------

function CanFitSkill(who, SkId, byref SkBaseVal)

  var SkillCap := GetSkillCap(who);
  var SkillSum := GetObjProperty(who, PROP_SKILLSUM);
  var CurCap := SkillCap;
  var CurSum := SkillSum[1];
  var IsCraft := IsCraftHarvestAttribute(GetAttributeIdBySkillId(SkId));

  if(IsCraft)
    CurCap += SK_CRAFT_SKILLCAP_ADD;
    CurSum += SkillSum[2];
  else
    CurSum += Max(SkillSum[2] - SK_CRAFT_SKILLCAP_ADD, 0); // dodajemy, ale odejmujac 100% z rzemiosla
  endif

  if(CurSum + SkBaseVal > CurCap)
    SkBaseVal := CurCap - CurSum;
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
