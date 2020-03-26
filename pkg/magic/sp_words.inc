//-----------------------------------------------------------------------------

function DefineSpWordsDict(byref SpWords, byref SpellsCfg)

  SpWords := dictionary;

  var spLen;

  foreach SpId in GetConfigIntKeys(SpellsCfg)
    spLen := len(SpellsCfg[SpId].PowerWords);

    if(!SpWords.exists(spLen))
      SpWords[spLen] := array(SpId);
    else
      SpWords[spLen].append(SpId);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function FindSpWordInDict(byref SpWords, byref SpellsCfg, byref text)

  var spLen := len(text);

  if(SpWords.exists(spLen))

    foreach SpId in (SpWords[spLen])
      if(SpellsCfg[SpId].PowerWords == text)
        return SpId;
      endif
    endforeach

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CastSpWordSpell(who, Item, byref SpellsCfg, SpId)

  var MainElem := {0,0,0,0};
  MainElem[1] := who;

  if(Item.isa(POLCLASS_ITEM) and Item.objtype != UOBJ_WRESTLE)
    MainElem[2] := CAST_WAND;
    MainElem[3] := Item;
  endif

  MainElem[4] := SPF_NOWORDS;
  start_script(SpellsCfg[SpId].Script, MainElem);
  MainElem    := array;

endfunction

//-----------------------------------------------------------------------------
