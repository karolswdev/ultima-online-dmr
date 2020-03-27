//--------------------------------------------------------------------------------

include ":glyphs:glyphs";

var START_MED_MANA_UP := 5;
const MED_BREAK_TIME    := 30;

//--------------------------------------------------------------------------------

function Meditate(who)

  var mod := GetGlyphMod(who, "MincreasedMeditation");
  if(mod)
	START_MED_MANA_UP := START_MED_MANA_UP + mod;
  endif

  if(ExPid(GetObjProperty(who,"#MedTimer")))
    SendSysMessage(who,"Juz medytujesz...",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  if(CInt(GetObjProperty(who,"#MedBreak")) + MED_BREAK_TIME > ReadGameClock())
    SendSysMessage(who,"Nie mozesz teraz medytowac...",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if (GetPoisonLevel(who) >= 1)
    SendSysMessage( who, "Nie mozesz medytowac kiedy jestes zatrut"+ggm(who,6)+".",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if (GetMana(who) >= GetMaxMana(who))
    SendSysMessage(who, "Nie brakuje Ci many.",FONT_NORMAL,COLOR_GREEN);
    return 0;
  endif

  SetBusyTime(who,GetSkillDelay(SKILLID_MEDITATION));

  if(who.hidden)
    who.hidden := 0;
  endif

  InitSpellGlob(who, 0, 0);

  SendSysMessage(who,"Rozpoczynasz medytacje...",FONT_NORMAL,COLOR_BLUE);
  PlaySoundEffect(who,0xf8);

  SetObjProperty(who,"#MedTimer",GetPid());
  var Res := ProcessMeditation(who);
  EraseObjProperty(who,"#MedTimer");
  
  return Res;

endfunction

//--------------------------------------------------------------------------------

function ProcessMeditation(who)

  var hp      := GetHp(who);
  var MaxHp   := GetMaxHp(who);
  var Mana    := GetMana(who);
//  var Perc    := CInt((Mana * 100) / GetMaxMana(who));
//  var Diff    := 65 - Perc;
  var Points  := GetDefaultPointsValue(SKILLID_MEDITATION);
  var Time    := GetTransMeditationTime(who);
  var i;

/*  if(diff <= 0)
    diff := 1;
  endif
  if (diff > 100)
    diff := 100;
  endif*/

  // Sec was initially set to 5. Decreased to 4.
  var Sec := 4 - CInt(GetEffectiveSkill(who,SKILLID_MEDITATION) / 50);

  for(i:=1;i<=Sec;i:=i+1)
    sleep(1);
    if (!CheckDisturb(who, MaxHp, Hp) or (GetMana(who) >= GetMaxMana(who)) or (GetMana(who) < mana) )
      SendSysMessage(who,"Nie mozesz sie skoncentrowac.",FONT_NORMAL,COLOR_RED);
      PlaySoundEffect(who,0xf6);
      return 0;
    endif
    if(CInt(GetObjProperty(who,"#MedTimer")) != GetPid())
      SendSysMessage(who,"Stracil"+ggm(who,3)+" koncentracje.",FONT_NORMAL,COLOR_RED);
      PlaySoundEffect(who,0xf6);
      return 0;
    endif
  endfor
  
//  if ( CheckSkill(who,SKILLID_MEDITATION,Diff,0) )
    AwardPoints(who,SKILLID_MEDITATION,Points);
    SetMana(who, GetMana(who) + START_MED_MANA_UP);

    if (GetMana(who) > GetMaxMana(who))
      SetMana(who, GetMaxMana(who));
      SendSysMessage(who,"Konczysz medytacje.",FONT_NORMAL,COLOR_GREEN);
      PlaySoundEffect(who, SFX_MEDIT_END);
      return;
    endif

    PrintTextAbove(who, "*medytuje*");
    PlaySoundEffect(who, SFX_MEDITATION);

    repeat
      SetMana(who, GetMana(who) + 1);
      sleepms(Time);

      if(CInt(GetObjProperty(who,"#MedTimer")) != GetPid())
        SendSysMessage(who,"Stracil"+ggm(who,3)+" koncentracje.",FONT_NORMAL,COLOR_RED);
        PlaySoundEffect(who,SFX_MEDIT_END);
        return;
      endif
    until(!CheckDisturb(who, MaxHp, Hp) or (GetMana(who) >= GetMaxMana(who)) or (GetMana(who) < mana) );
		
    SendSysMessage(who,"Konczysz medytacje.",FONT_NORMAL,COLOR_GREEN);
    PlaySoundEffect(who,SFX_MEDIT_END);
    return 1; 

/*  else
  
    SendSysMessage(who,"Nie mozesz sie skoncentrowac.",FONT_NORMAL,COLOR_RED);
    PlaySoundEffect(who,SFX_MEDIT_END);
    return 0;
    
  endif*/

endfunction

//--------------------------------------------------------------------------------

function GetTransMeditationTime(who)

  var WhoMed     := GetEffectiveSkill(who,SKILLID_MEDITATION);
  var Time       := 2150;
  Time           := Time - CInt(WhoMed * 15) - GetIntelligence(who)*5;
  var Circle     := 8;
  var ItmC       := 0;
  var ItemCfg    := ReadConfigFile(ITEMS_CFG);

  foreach item in ListEquippedItems(who)
    if(item != who.weapon)
      if(!ItemCfg[Item.objtype].Gloves)
        ItmC := CInt(ItemCfg[item.objtype].blockcircle);
        if(ItmC < Circle and ItmC)
          Circle := ItmC;
        endif
      endif
    endif
  endforeach
  
//////////////////////////DELAY DLA WAMPA MAGA//////////////////////////////////////////
  var VampEffSkill;
  var hour := GetHour();
  var cycles :=0; 
  if(IsVampire(who) and hour < VAMPIRE_HOUR and hour >= VAMPIRE_END_HOUR and IsInVampBurningArea(who))
  
    VampEffSkill := CInt(GetEffectiveSkill(who, SKILLID_VAMPIRE));
      if (VampEffSkill < 110)
        case (CInt(GetBaseSkill(who, SKILLID_VAMPIRE)))
             101:     cycles  := cycles + 10;  break;
             102:     cycles  := cycles + 8;  break;
             103:     cycles  := cycles + 6;  break;
             104:     cycles  := cycles + 4;  break;
             105:     break;
             default: cycles  := cycles + 100;  break;
        endcase
      endif
  endif
  

  var mana := CInt(GetMaxMana(who));
  var vampdelay := CInt(cycles * 1000 / mana);
////////////////////////////////////////////////////////////////////////////////////
  Time := Time + CInt(((8 - Circle) * 150) + vampdelay);
 
  if(Time < 150)
    Time := 150;
  endif
  
  return Time;
  
endfunction

//--------------------------------------------------------------------------------
