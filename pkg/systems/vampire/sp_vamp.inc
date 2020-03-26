//-----------------------------------------------------------------------------

include ":magic:magicbag";
include "../pkg/magic/sp_words";

//-----------------------------------------------------------------------------
// Vampire Staff / Speech
//-----------------------------------------------------------------------------

function CheckVampStaff(who, byref text, Item := 0)

  if(!IsVampire(who) and !who.cmdlevel)
    return;
  endif

  var vSpellsCfg := ReadConfigFile(":vampire_spells:spells");

  if(!VampSpWords)
    DefineSpWordsDict(VampSpWords, vSpellsCfg);
  endif

  if(who.frozen or who.paralyzed)
    return;
  endif

  var spLen := FindSpWordInDict(VampSpWords, vSpellsCfg, text);

  if(spLen)

    if(IsBusy(who,NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return;
    else
      NoLongerBusy(who);
    endif

    var bag := FindObjTypeInContainer(who.backpack, UOBJ_VAMP_BAG);

    if(bag)
      if(CheckForRuneInBag(bag, vSpellsCfg[spLen].rune_objtype))
        CastSpWordSpell(who, Item, vSpellsCfg, spLen);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
