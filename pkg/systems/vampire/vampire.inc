//-----------------------------------------------------------------------

include "include/packets";
include ":magic:magicbag";
include ":magic:magicatt";
include "include/tempmods";
include "include/regs";

include "common";
include ":jail:common";
include ":warcasts:warcasts";
include ":magicitems:magicitems";
include ":artifacts:artifacts";

//-----------------------------------------------------------------------

const VP_MBS_TNB                        := 100; //minimalna wartosc bazowa skilla, do niepalenia przez slonce


const VP_ICESERP_GRAPHIC               := 21;
const VP_RUNES_CFG                     := ":vampire:runes";

//-----------------------------------------------------------------------

function CheckVampireAcceptation(who)

  if(GetHour() == 22)
    if(!ExPid(GetObjProperty(who,"#VampChange")))
      start_script(":vampire:vampchange",who);
    endif
  endif

endfunction

//-----------------------------------------------------------------------

function VampireChange(who)

  // Make sure single threaded mode is enabled
  Malloc(who);

  if(who.dead or !who)
    return;
  endif

  if(!IsInVampireChange(who))
    SetObjProperty(who,"#VampireStartSkill",GetEffectiveSkill(who,SKILLID_VAMPIRE));
    UnEquipMagicItems(who, array(MAGICMOD_STRENGTH, MAGICMOD_DEXTERITY, MAGICMOD_INTELLIGENCE, MAGICMOD_LIGHT));

    UndoMod(who, MOD_STR, UMDF_WAIT);
    UndoMod(who, MOD_DEX, UMDF_WAIT);
    UndoMod(who, MOD_INT, UMDF_WAIT);
    UndoMod(who, MOD_LIGHT, UMDF_WAIT);
    
    DoMod(who,MOD_STR,RandomInt(4) + Cint(GetVpSk(who) / 4.2) + RandomInt(2), 7200);
    DoMod(who,MOD_DEX,RandomInt(4) + Cint(GetVpSk(who) / 4.2) + RandomInt(2), 7200);
    DoMod(who,MOD_INT,RandomInt(4) + Cint(GetVpSk(who) / 4.2) + RandomInt(2), 7200);
    DoMod(who,MOD_LIGHT,1, 7200);

    SetObjProperty(who,"#VampChangeComplete", ReadGameClock());
    PlayPcEffect( who, FX_MARK_EFFECT, 431, 0xf, 0xf);
    EraseObjProperty(who,"EndedVampire");
  else
  
    if(CanMod(who, MOD_DEX))
      DoMod(who,MOD_DEX,4 + Cint(GetVpSk(who) / 4) + RandomInt(2), 7200);
    endif

    if(CanMod(who, MOD_STR))
      DoMod(who,MOD_STR,4 + Cint(GetVpSk(who) / 5) + RandomInt(2), 7200);
    endif
    
    if(CanMod(who, MOD_INT))
      DoMod(who,MOD_INT,4 + Cint(GetVpSk(who) / 5) + RandomInt(2), 7200);
    endif
    
    if(CanMod(who, MOD_LIGHT))
      DoMod(who, MOD_LIGHT, 1, 7200);
    endif

  endif

endfunction

//-----------------------------------------------------------------------

function VampireEnd(who)

  if(who.dead or !who)
    return;
  endif

  EraseObjProperty(who,"#VampChangeComplete");

  if(!GetObjProperty(who,"EndedVampire"))
    UndoMod(who,MOD_STR);
    UndoMod(who,MOD_DEX);
    UndoMod(who,MOD_INT);
    SetObjProperty(who,"EndedVampire",1);
  endif

  if(CanMod(who,MOD_LIGHT))
    DoMod(who,MOD_LIGHT,1,3600); // swiatlo 24h/d 
  endif

  if(!who.cmdlevel)
    if(!IsJailed(who))
      if(who.graphic != INVIS_MOB_GRAP)
        if(GetBaseSkill(who,SKILLID_VAMPIRE) < VP_MBS_TNB or who.color !=  GetColorBefVamp(who))   // Jezeli wampiryzm mniejszy od progu niepalenia i nie ma pozorow zycia
          if(IsInVampBurningArea(who))
            if(CInt(GetObjProperty(who,"ResTimer")) - RES_TIME + 30 < ReadGameClock())
              var ItemCfg := ReadConfigFile(ITEMS_CFG);
              if(ItemCfg[GetEquipmentByLayer(who,LAYER_ROBE).objtype].VampRobe and GetBaseSkill(who,SKILLID_VAMPIRE) >= 90)
                var Skill := ToMin(GetEffectiveSkill(who,SKILLID_VAMPIRE)-90, -40); //
                Skill := 10 - CInt(Skill/2);                                        // Jezeli wampiryzm > 109 to nie pali slonce
                if(Skill > 0)                                                       //
                  DealManaDamage(who, ToMin(Skill*2+RandomInt(5), 10));
                  DealDamage(who, 1);
                  SetObjProperty(who,"#HealBreak", ReadGameClock() + 50);
                  PlaySoundEffect( who, SFX_SPELL_FLAME_STRIKE );
                endif
              else
                DealDamage(who,1+RandomInt(5));
                SetObjProperty(who,"#HealBreak",ReadGameClock() + 10);
                PlaySoundEffect( who, SFX_SPELL_FLAME_STRIKE );
              endif
            endif
          endif
        endif
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------

function CanBeVampire(who)

  return CInt(GetObjProperty(who,"CanBeVampire"));

endfunction

//-----------------------------------------------------------------------

function GetVpSk(who)

  return CInt(GetObjProperty(who,"#VampireStartSkill"));

endfunction

//-----------------------------------------------------------------------

function GetVpSkAbove(who)

  return ToMin(GetVpSk(who) - 100, 0);

endfunction

//-----------------------------------------------------------------------

function VampireRuneCast(byref Parms, wcId, TrgType := TRGT_NONE, TrgNoto := TRGO_NONE)

  if(!Parms.Caster.cmdlevel)
    if(!IsVampire(Parms.Caster))
      SendSysMessage(Parms.Caster, "W twoich zylach nie plynie krew wampira!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    if(IsCharClass(Parms.Caster, CLASS_MAGE))
      SendSysMessage(Parms.Caster, "Te runo jest niezwykle tajemnicze dla Ciebie...",FONT_NORMAL,COLOR_BLUE);
      return 0;
    endif
  endif

  return WarCast(Parms, wcId, 0, TrgType, TrgNoto);

endfunction

//-----------------------------------------------------------------------

function GetVampireWarSpDur(caster)

  var CasterVamp          := GetEffectiveSkill(caster,SKILLID_VAMPIRE);
  var CasterSp            := GetEffectiveSkill(caster,SKILLID_SPIRITSPEAK);

  return 10 + CInt((CasterSp + CasterVamp) * 3);

endfunction


//-----------------------------------------------------------------------

function CalcVampWarModDur(caster)

  var CasterVamp          := GetEffectiveSkill(caster,SKILLID_VAMPIRE);
  var CasterSp            := GetEffectiveSkill(caster,SKILLID_SPIRITSPEAK);

  return CInt( ((CasterSp/2) + (CasterVamp)) * 6);

endfunction

//-----------------------------------------------------------------------

function SetVampire(who)

  if(IsVampire(who))
     return 0;
  endif

  UndoMod(who, MOD_COLOR);
  SetObjProperty(who, "IsVampire", ReadGameClock());
  SetObjProperty(who, "ColorBeforeVamp", who.color);
  who.truecolor := VAMP_COLOR;
  who.color     := VAMP_COLOR;

  return 1;

endfunction

//-----------------------------------------------------------------------

function EraseVampire(who)

  var Skill := GetBaseSkillBaseValue(who, SKILLID_VAMPIRE);

  if(!IsVampire(who) and !Skill)
    return 0;
  endif

  EraseObjProperty(who, "IsVampire");
  UndoMod(who, MOD_STR);
  UndoMod(who, MOD_DEX);
  UndoMod(who, MOD_INT);
  UndoMod(who, MOD_GRAPHIC);
  UndoMod(who, MOD_COLOR);
  UndoMod(who, MOD_LIGHT);
  EraseObjProperty(who, "#VampChangeComplete");
  EraseObjProperty(who, "EndedVampire");
  EraseObjProperty(who, "#VampireStartSkill");
  EraseObjProperty(who, "CanBeVampire");

  if(CInt(GetObjProperty(who, "ColorBeforeVamp")) or who.color == VAMP_COLOR)
    var OldColor  := GetColorBefVamp(who);
    who.color     := OldColor;
    who.truecolor := OldColor;
  endif
  EraseObjProperty(who, "ColorBeforeVamp");
  SetBaseSkill(who, SKILLID_VAMPIRE, 0);

  return 1;

endfunction

//-----------------------------------------------------------------------------