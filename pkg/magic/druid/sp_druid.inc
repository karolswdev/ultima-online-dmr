//-----------------------------------------------------------------------------

include ":magic:magicbag";
include "../pkg/magic/sp_words";

//-----------------------------------------------------------------------------
// Druid Staff / Speech
//-----------------------------------------------------------------------------

function CheckDruidStaff(who, byref text, Item := 0)

  var dSpellsCfg := ReadConfigFile(":druid_spells:spells");

  if(!DruidSpWords)
    DefineSpWordsDict(DruidSpWords, dSpellsCfg);
  endif

  if(who.frozen or who.paralyzed)
    return;
  endif

  var spLen := FindSpWordInDict(DruidSpWords, dSpellsCfg, text);
  
  if(spLen)

    if(IsBusy(who,NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return;
    else
      NoLongerBusy(who);
    endif

    var bag := FindObjTypeInContainer(who.backpack, UOBJ_DRUID_BAG);
    
    if(bag)
      if(CheckForRuneInBag(bag, dSpellsCfg[spLen].rune_objtype))
        CastSpWordSpell(who, Item, dSpellsCfg, spLen);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
