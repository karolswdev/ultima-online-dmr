//-----------------------------------------------------------------------------

include "../magicitems/magicitems";

//-----------------------------------------------------------------------------

function CheckWand(who, text)

  if(who.frozen or who.paralyzed)
    return;
  endif

  var mSpellsCfg := ReadConfigFile(SPELLS_CFG);
  var ItemCfg    := ReadConfigFile(ITEMS_CFG);

  if(text == mSpellsCfg[ItemCfg[who.weapon.objtype].SpellID].PowerWords)

    if(IsBusy(who,NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return;
    else
      NoLongerBusy(who);
    endif
    
    WandCast(who, who.weapon, SPF_NOWORDS);
  endif

endfunction

//-----------------------------------------------------------------------------

function WandCast(who, wand, AddOpt := 0)

  if(!IsIdentified(wand))
    return;
  endif

  if(Cint(GetObjProperty(wand, "charges")))
    var mSpellsCfg := ReadConfigFile(SPELLS_CFG);
    var ItemCfg    := ReadConfigFile(ITEMS_CFG);

    var SpellID    := ItemCfg[wand.objtype].SpellID;
    var SpellType  := mSpellsCfg[SpellID].SpellType;
    var Script     := mSpellsCfg[SpellID].script;

    if(!SpellType or !Script)
      SendSysMessage(who,"Ta magia jest dla Ciebie nie znana.",FONT_NORMAL,COLOR_RED);
      return;
    endif

    var SpellParms := {};
    SpellParms[1] := who;
    SpelLParms[2] := CAST_WAND;
    SpellParms[3] := wand;
    SpellParms[4] := SPF_NOREGS | SPF_NOMANA | SPF_NOGAIN | AddOpt;
    Start_script(Script, SpellParms);

  endif

endfunction

//-----------------------------------------------------------------------------
