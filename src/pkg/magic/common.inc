//-----------------------------------------------------------------------------

const ICN_ONLYCITY := 0x01;

//-----------------------------------------------------------------------------

function IsCastingOffensiveNear(who, Range := 15, flags := 0)

  if(IsBitOn(flags, ICN_ONLYCITY))
    if(!FindCity(who))
      return 0;
    endif
  endif

  var Prop := GetObjProperty(who, "#OffensiveCast");

  if(Prop)
    if(Prop[1] + 600 > ReadGameClock())
      var Dist := GetLocDist(Prop[2], who);
      if(Dist != error and Dist <= Range)
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetOffensiveCast(who)

  __ParamUnused(who);
/* na razie wylaczam, bo byl problem z wojnami gildii.. nie mogli czarowac przez to w miastach.
  if(FindCity(who))
    SetObjProperty(who, "#OffensiveCast", array(ReadGameClock(), GetLocStruct(who)));
  endif
*/
endfunction

//-----------------------------------------------------------------------------

function EraseOffensiveCast(who)

  __ParamUnused(who);
//  EraseObjProperty(who, "#OffensiveCast");

endfunction 

//-----------------------------------------------------------------------------

function IsSpellLearned(Caster, SpellId)
  
  var SpellBook := GetEquipmentByLayer(Caster,LAYER_HAND1);

  if(SpellBook.objtype != UOBJ_SPELLBOOK)
    SpellBook := GetEquipmentByLayer(Caster,LAYER_HAND2);
  endif

  if(SpellBook.objtype != UOBJ_SPELLBOOK)
    SpellBook := FindObjTypeInContainer(Caster.backpack,UOBJ_SPELLBOOK);
  endif

  if(SpellBook.objtype == UOBJ_SPELLBOOK)
    var Scroll := GetSpellValue(SpellID,"scroll");
    Scroll := FindObjTypeInContainer(SpellBook,Scroll);
    if(!Scroll)
      return RetError("Nie znasz tego czaru.");
    endif

    return 1;
  endif

  return RetError("Nie masz ksiegi czarow przy sobie.");

endfunction

//-----------------------------------------------------------------------------
