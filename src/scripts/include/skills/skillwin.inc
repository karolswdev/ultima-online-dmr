//--------------------------------------------------------------------------------

include "include/bits";

//================================================================================
//--------------------------------------------------------------------------------
// Skill Window Gump
//--------------------------------------------------------------------------------
//================================================================================

const NO_SKWIN_ARROWS     := 0x01;
const NO_SKWIN_SKSCRIPT   := 0x02;
const PAGE_COUNT          := 20;

//-----------------------------------------------------------------------------

function BuildAllSkillsWindow(who, Flags := 0, bywho := 0)

  StandSkLayout();
  GPage();

  GButton(  325,487,2453,2454,9997);
  GTextLine(160, 30, 610, "Umiejetnosci");
  GAddPageData(365,55,2650,2651,365,463,2648,2647);

  var ShowBlock, SkVal, SkElem;
  var Skills := GetCharGroupedSkills(who, 0, 1);
  var classSkills := GetCharClassSkills(who);
  var bCanId := IsOnDMRTest() or IsStaff(bywho);
  var IsBlocked;
  var IsGlobalBlocked := IsGainBlockTimed(who);

  foreach GroupData in Skills
    var GroupCfg := GetAttributeGroupConfig(_GroupData_iter);

    GCheckNextPage(PAGE_COUNT);

    if(GNPCount() == PAGE_COUNT-1);
      GCheckNextPage(PAGE_COUNT);
    endif

    GTextLine(70, 67+ (GNPCount()*20), 999, "Grupa:");
    GTextLine(120, 67+ (GNPCount()*20), 1410, GroupCfg.Name);
    GGumpPic(43, 85+ (GNPCount()*20), 0xBB9);
    GGumpPic(117, 85+ (GNPCount()*20), 0xBB9);

    foreach SkID in GroupData 
      if(SkID in SkInGump)
        continue;
      endif

      SkElem := GetSkillConfigElem(SkID);
      SkVal := GetEffectiveSkillBaseValue(who,(SkID));

      if(SkVal == 0 and (SkElem.NotVisible or !(SkID in classSkills)))
        if(!who.cmdlevel)
          if(bywho)
            if(!bywho.cmdlevel)
              continue;
            endif
          else
            continue;
          endif
        endif
      endif

      IsBlocked := (!who.isa(POLCLASS_NPC) and !(SkID in classSkills)) or IsGlobalBlocked;

      GCheckNextPage(PAGE_COUNT);
      GTextLine(70, Cint(67+ (GNPCount()*20)), __IfElse(IsBlocked, 1209, 550) ,SkElem.name);
      
      if(bCanId)
        GTextLine(220,Cint(67+ (GNPCount()*20)), 560, SkID);
      endif

      if(GetSkillBaseMaxValue(who, SkID) > DEF_SK_MAX_VALUE)
        GTextLine(270,Cint(67+ (GNPCount()*20)),1320,SkVal*0.1);
      elseif(SkVal)
        GTextLine(270,Cint(67+ (GNPCount()*20)),__IfElse(IsBlocked, 1209, 550), SkVal*0.1);
      else
        GTextLine(270,Cint(67+ (GNPCount()*20)),__IfElse(IsBlocked, 1209, 997), 0);
      endif

      GButton(  340,Cint(70+ (GNPCount()*20)),2117,2118,(400+SkID));

      if(!(Flags & NO_SKWIN_ARROWS))
        GButton(250,Cint(68+ (GNPCount()*20)),2704,2705,(800+SkID));
      endif

      if(SkElem.script and SkElem.script != "noskill" and !(Flags & NO_SKWIN_SKSCRIPT))
        GButton(50,Cint(73+ (GNPCount()*20)),2103,2104,(SkID+1));
      endif

      if(who.isa(POLCLASS_NPC))
        if(IsNpcSkillGainOn(SkID))
          ShowBlock := IsSetAttrFlag(who, GetAttributeIdBySkillId(SkID), ATTRF_BLOCKED);
        else
          ShowBlock := -1;
        endif
      else
        ShowBlock := IsSetAttrFlag(who, GetAttributeIdBySkillId(SkID), ATTRF_BLOCKED);
      endif

      if(IsSkillMastered(who, SkID))
        GGumpPic(304,Cint(74+ (GNPCount()*20)),0x13A8);
      endif

      if(bCanId)
        if(SkElem.CraftReceptures)
          GButton(200,Cint(70+ (GNPCount()*20)),0x2716,0x2716,(500+SkID));
        endif
      endif

      if(ShowBlock >= 0)
        if(ShowBlock)
          GButton(320,Cint(73+ (GNPCount()*20)),2360,2361,(200+SkID));
        else
          GButton(320,Cint(73+ (GNPCount()*20)),2362,2360,(200+SkID));
        endif
      endif
    endforeach
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GetSkillsInGump(who)

  if(who.isa(POLCLASS_NPC))
    return {};
  endif

  var GumpSkills := GetObjProperty(who,"SkillsInSkWin");
  if(!GumpSkills or !GumpSkills.size())
    GumpSkills := {};
    SetObjProperty(who,"SkillsInSkWin",GumpSkills);
  endif

  return GumpSkills;

endfunction

//-----------------------------------------------------------------------------

function CountAllSkills(who,SkillsArr := 0)

  var Count := 0;

  if(who.isa(POLCLASS_NPC))
    var i, maxSkId := GetMaxSkillId();
    for(i:=0;i<maxSkId;i:=i+1)
      Count := Count + GetBaseSkillBaseValue(who,i);
    endfor
  else
    if(!SkillsArr)
      Count := GetObjProperty(who, PROP_SKILLSUM);
      if(Count)
        Count := Count[1] + Count[2];
      endif
    elseif(SkillsArr.size())
      foreach Sk in SkillsArr
        Count := Count + GetBaseSkillBaseValue(who,Sk);
      endforeach
    endif
  endif

  return CInt(Count*0.1);

endfunction

//-----------------------------------------------------------------------------

function StandSkLayout(Multiplier := 20,Width := 10, Uni := 0)

  GInit(0,0);
  GResPic(0,  20, 2520,(200+(Width*20)),(100+(Multiplier*20)));
  GResPic(28, 54, 3600,(144+(Width*20)),(30+(Multiplier*20)));

  if(Uni)
    GMode(GMODE_NORMAL, GSMODE_PACKET);
    GSetUnicode();
  else
    GMode(GMODE_NORMAL, GSMODE_NORMAL);
  endif

endfunction

//-----------------------------------------------------------------------------

function StandMultiSkLayout(Multiplier := 20,Width := 10)

  GInit(0,0);
  GMultiHeaderInit();
  GResPic(0,  20, 2520,(200+(Width*20)),(100+(Multiplier*20)));
  GResPic(28, 54, 3600,(144+(Width*20)),(30+(Multiplier*20)));

endfunction

//-----------------------------------------------------------------------------

function CountAllStats(who)

  var Count := GetBaseStrengthBaseValue(who) + GetBaseDexterityBaseValue(who) + GetBaseIntelligenceBaseValue(who);
  Count := Count / 10.0;
  return Count;
  
endfunction

//-----------------------------------------------------------------------------

function BlockSkill(who, SkId, bywho := 0)

  if(bywho and bywho != who and !IsAdmin(bywho) and !who.npctemplate)
    SendSysMessage(bywho,"Nie mozesz tego zrobic.",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  var AttrId := GetAttributeIdBySkillId(SkId);
  var SkElem := GetSkillConfigElem(SkId);

  case(IsSetAttrFlag(who, AttrId, ATTRF_BLOCKED))
   1:        UnSetAttrFlag(who, AttrId, ATTRF_BLOCKED);
             SendSysMessage(who, "Odblokowal"+ggm(who,3)+" umiejetnosc "+SkElem.name+".",FONT_NORMAL,COLOR_GOOD);
    default: SetAttrFlag(who, AttrId, ATTRF_BLOCKED);
             SendSysMessage(who, "Zablokowal"+ggm(who,3)+" umiejetnosc "+SkElem.name+".",FONT_NORMAL,COLOR_BAD);
  endcase

  if(SkId == SKILLID_KENSAI or SkId == SKILLID_BARBARIAN)
    SetObjProperty(who, "Blocked_"+SkId, ReadGameClock());
  endif

endfunction

//-----------------------------------------------------------------------------

function BlockStat(who, sId, bywho := 0)

  if(bywho and bywho != who and !IsAdmin(bywho) and !who.npctemplate)
    SendSysMessage(bywho,"Nie mozesz tego zrobic.",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  var msg := "";
  var AttrId;

  case(sId)
    1:  msg    := "sily";
        AttrId := ATTRIBUTEID_STRENGTH;
    2:  msg    := "zrecznosci";
        AttrId := ATTRIBUTEID_DEXTERITY;
    3:  msg    := "inteligencji";
        AttrId := ATTRIBUTEID_INTELLIGENCE;
  endcase

  case(IsSetAttrFlag(who, AttrId, ATTRF_BLOCKED))
   1:       UnSetAttrFlag(who, AttrId, ATTRF_BLOCKED);
            SendSysMessage(who, "Odblokowal"+ggm(who,3)+" przyrost "+msg+".",FONT_NORMAL,COLOR_GOOD);
   default: SetAttrFlag(who, AttrId, ATTRF_BLOCKED);
            SendSysMessage(who, "Zablokowal"+ggm(who,3)+" przyrost "+msg+".",FONT_NORMAL,COLOR_BAD);
  endcase

endfunction

//--------------------------------------------------------------------------------
