//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------

const MERC_TRAIN_CFG         := ":merc_train:training";

const MT_MAX_TRAIN_SKILL     := 250;
const MT_TRAIN_AMOUNT        := 100;

//=============================================================================
//-----------------------------------------------------------------------------
// Train Skills
//-----------------------------------------------------------------------------
//=============================================================================

function GetMerchantTeachSkills(Npc)

  var TrainCfg  := ReadConfigFile(MERC_TRAIN_CFG);
  var TrainType := lower(GetMercType(Npc));
  var SkillsIDs := GetConfigString(TrainCfg["SkillList"], TrainType);
  
  if(!SkillsIDs)
    return dictionary;
  endif
  
  SkillsIDs      := SplitWords(SkillsIDs);
  var Skills     := dictionary;
  var SkElem;
  
  foreach SkillID in SkillsIDs
    SkElem := GetSkillConfigElem(Cint(SkillID));
    if(SkElem.name)
      Skills[SkillID] := CStr(SkElem.name);
    endif
  endforeach

  return Skills;

endfunction

//-----------------------------------------------------------------------------

function TrainSkill(Npc, who)

  if(!CheckLineOfSight(Npc, who))
    return;
  endif

  sleepms(RandomInt(1000));

  if(GetObjProperty(who, "#SkTraining"))
    PrintTextAbove(Npc, "Wybierz lepiej umiejetnosc.");
    return;
  endif

  var whoSer := who.serial;
  SetObjProperty(who, "#SkTraining", Npc.serial);
  NpcTrainPlayerSkill(Npc, who);

  if(!who)
    who := FindPlayerBySerial(whoSer);
  endif

  EraseObjProperty(who, "#SkTraining");

endfunction

//-----------------------------------------------------------------------------

function CanNpcTrainSkill(Npc, who, SkId)

  if(SkId == SKILLID_MAGERY or SkId == SKILLID_ARCHERY)
    if(!IsNewPlayer(who))
        PrintTextAbove(Npc, "Nie moge Cie nauczyc tej umiejetnosci, juz za pozno na nauke takiej umiejetnosci.");
        return 0;
    elseif(GetBaseSkill(who, __ifelse(SkID == SKILLID_ARCHERY, SKILLID_MAGERY, SKILLID_ARCHERY)))
        PrintTextAbove(Npc, "Nie moge Cie nauczyc tej umiejetnosci, ani lucznik nie zrozumie magii, ani mag lucznictwa.");
        return 0;
    endif
  elseif(SkId == SKILLID_NECRO or SkId == SKILLID_DRUID)
    if(GetBaseSkill(who, SKILLID_MAGERY) == 0)
      if(!IsNewPlayer(who))
        PrintTextAbove(Npc, "Nie moge Cie nauczyc tej umiejetnosci, nie wygladasz ani mlodo ani na maga.");
        return 0;
      else
        PrintTextAbove(Npc, "Mlodys, dam Ci wiec jeszcze szanse nauczyc sie tej magii, ucz sie i korzystaj.");
      endif
    endif
  elseif(SkId == SKILLID_VAMPIRE)
    if(!IsVampire(who))
      PrintTextAbove(Npc, "Nie widze by w twoich zylach plynela krew wampira! Wynos sie stad zanim wyssam z ciebie ostatnia krople krwi!");
      return 0;
    endif
  else
    case(SkId)
      SKILLID_PALADINISM:
      SKILLID_FANATICISM:   PrintTextAbove(Npc, "Tej umiejetnosci nie da sie w taki prosty sposob nauczyc.");
                            return 0;
      SKILLID_KENSAI:
      SKILLID_BARBARIAN:    if(IsCharClass(who, CLASS_MAGE))
                              PrintTextAbove(Npc, "Pff... maga? Idz lepiej do liceum w Moonglow postudiowac prastare ksiegi.");
                              return 0;
                            else
                              if(GetBaseSkillBaseValue(who, SKILLID_PALADINISM))
                                PrintTextAbove(Npc, "Jestes juz Paladynem, czy Ci to nie wystarcza?");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_FANATICISM))
                                PrintTextAbove(Npc, "Mroczny Rycerz? Hm.. nie wydaje mi sie.");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_KENSAI) and SkId != SKILLID_KENSAI)
                                PrintTextAbove(Npc, "Umiesz juz sztuke walki Kensai, czyz to nie wystarczajaco?");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_BARBARIAN) and SkId != SKILLID_BARBARIAN)
                                PrintTextAbove(Npc, "Barbarzynca? Nie, nie, twoj umysl tyle nie pomiesci.");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_VAMPIRE) or GetBaseSkillBaseValue(who, SKILLID_WEARWOLF) or IsVampire(who) or IsWearWolf(who))
                                PrintTextAbove(Npc, "Sadze, ze nie uda mi sie nauczyc Ciebie tej umiejetnosci.");
                                return 0;
                              endif
                            endif
    endcase
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
