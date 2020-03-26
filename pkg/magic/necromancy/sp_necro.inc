//-----------------------------------------------------------------------------

include "necrobook";
include "../pkg/magic/sp_words";

//-----------------------------------------------------------------------------
// Necro Staff / Speech
//-----------------------------------------------------------------------------

function CheckNecroStaff(who, byref text, Item := 0)

  var nSpellsCfg := ReadConfigFile(":necro_spells:spells");

  if(!NecroSpWords)
    DefineSpWordsDict(NecroSpWords, nSpellsCfg);
  endif

  if(who.frozen or who.paralyzed)
    return;
  endif

  var spLen := FindSpWordInDict(NecroSpWords, nSpellsCfg, text);

  if(spLen)

    if(IsBusy(who,NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return;
    else
      NoLongerBusy(who);
    endif
    
    var NecroBook := GetEquipmentByLayer(who,LAYER_HAND1);

    if(NecroBook.objtype != UOBJ_NECROBOOK)
      NecroBook := GetEquipmentByLayer(who,LAYER_HAND2);
    endif
    
    if(NecroBook.objtype != UOBJ_NECROBOOK)
      NecroBook := GetEquipmentByLayer(who,LAYER_HAND2);
    endif
    
    if(NecroBook.objtype != UOBJ_NECROBOOK)
      NecroBook := FindObjTypeInContainer(who.backpack,UOBJ_NECROBOOK);
    endif
    
    if(NecroBook.objtype == UOBJ_NECROBOOK)
      if(CheckNecroSpellInBook(NecroBook, spLen))
        CastSpWordSpell(who, Item, nSpellsCfg, spLen);
      endif
    endif
    
  endif

endfunction

//-----------------------------------------------------------------------------
