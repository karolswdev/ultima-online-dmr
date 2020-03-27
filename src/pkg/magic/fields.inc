//------------------------------------------------------------------------

include "include/portals";

//------------------------------------------------------------------------

function CreateField(Caster, x, y, z, fType, Duration, Direction := DIR_NONE,OnMob := 1)

  if(!Caster)
    return 0;
  endif
  
  //Krim za stawianie scian w miescie
  if(FindCityCoords(x,y,Caster.realm))
    SetCriminal(Caster, 1, "CreateField","Stawianie scian w miescie");
  endif


  foreach itm in ListItemsNearLocation(x,y,z,6,Caster.realm)
    if(itm.objtype == UOBJ_WAYGATE)
      SendSysMessage(Caster, "Nie mozesz postawic sciany tak blisko portalu.",FONT_NORMAL,COLOR_RED);
      return;
    endif
    if(itm.objtype == UOBJ_CORPSE)
      var Corpse := GetObjProperty(itm, "GuardedBy");
      if(!Corpse.errortext)
        SendSysMessage(Caster, "Nie mozesz postawic sciany tak blisko zwlok.",FONT_NORMAL,COLOR_RED);
        return;
      endif
    endif
  endforeach
  
  if(Direction != DIR_E and Direction != DIR_N)
    Direction := GetFieldDirection (Caster, x, y);
  endif

  var fObjType, fObjTypeW, fObjTypeN;
  var Sound := 0;
  var St    := -2;
  var Et    := 2;

  case(fType)
    MFIELDT_FIRE:    fObjTypeW := UOBJ_FIREFIELD_EW;
                     fObjTypeN := UOBJ_FIREFIELD_NS;
                     Sound     := SFX_SPELL_FIRE_FIELD;
    MFIELDT_ENERGY:  fObjTypeW := UOBJ_ENERGYFIELD_EW;
                     fObjTypeN := UOBJ_ENERGYFIELD_NS;
                     Sound     := SFX_SPELL_ENERGY_FIELD;
                     St        := -3;
                     Et        := 3;
    MFIELDT_POISON:  fObjTypeW := UOBJ_POISONFIELD_EW;
                     fObjTypeN := UOBJ_POISONFIELD_NS;
                     Sound     := SFX_SPELL_POISON_FIELD;
    MFIELDT_PARA:    fObjTypeW := UOBJ_PARAFIELD_EW;
                     fObjTypeN := UOBJ_PARAFIELD_NS;
                     Sound     := SFX_SPELL_PARA_FIELD;
    MFIELDT_STONE:   fObjTypeW := UOBJ_STONEFIELD_EW;
                     fObjTypeN := UOBJ_STONEFIELD_NS;
                     Sound     := SFX_SPELL_WALL_OF_STONE;
    MFIELDT_FROST:   fObjTypeW := UOBJ_FROSTFIELD_EW;
                     fObjTypeN := UOBJ_FROSTFIELD_NS;
    default:         return;
  endcase

  case(Direction)
    DIR_E:  fObjType := fObjTypeW;
    DIR_N:  fObjType := fObjTypeN;
  endcase

  if(sound)
    PlaySoundEffect(Caster,sound);
  endif

  var field := {};
  var i, it, bCreate, cx, cy, StandZ;
  var mobOnWall := 0;

  for (i := St;i <= Et;i := i + 1)
    bCreate := 1;

    case(Direction)
      DIR_E:      cx := x + i;
                  cy := y;
      DIR_N:      cx := x;
                  cy := y + i;
    endcase

    StandZ := GetStandZ(cx, cy, Caster.realm, z);
    if(StandZ == error)
      continue;
    endif

    if(ListMobilesNearLocationEx(cx, cy, StandZ,0,LISTEX_FLAG_NORMAL + LISTEX_FLAG_HIDDEN,Caster.realm).size() == 0 or OnMob)
      if(Caster)
        if(!CheckLosAt(Caster, cx, cy, StandZ))
          bCreate := 0;
        endif
      endif
    else
      bCreate := 0;
      mobOnWall := 1;    // Jezeli chociaz jeden mob jest na lini sciany to mobOnWall == 1
    endif

    foreach locItem in ListItemsAtLocation(cx, cy, StandZ, Caster.realm)
      if(IsPortal(locItem))
        bCreate := 0;
      endif
    endforeach

    if(bCreate)
      it := CreateItemAtLoc (cx, cy, StandZ, fObjType, 1,Caster.realm);
      if (it)
        it.movable := 0;
        SetObjProperty (it, PROP_SUMMONED_FIELD, Caster.serial);
        field.append( it );
      endif
    endif
  endfor

  Detach();

  if (( fType == MFIELDT_STONE or fType == MFIELDT_ENERGY ) and mobOnWall ) // jezeli na WallofStones jest mob, to zniszcz sciane po 1 sekundzie
    sleep ( 1 );
  else
    sleep (duration);                         // w przeciwnym wypadku zniszcz po czasie wytrzymalosci
  endif
  
  foreach item in (field)
    PlayStationaryEff(item.x, item.y, item.z, FX_SMOKE, 0xa, 0xa, 0, item.realm);
    DestroyItem (item);
  endforeach

endfunction

//------------------------------------------------------------------------

function GetFieldDirection (Caster, x, y)

  var xDiff := abs(Caster.x - x);
  var yDiff := abs(Caster.y - y);

  if(!xDiff and !yDiff)
    return DIR_E;
  elseif (xdiff == ydiff)
    return DIR_N;
  elseif (xdiff > ydiff)
    return DIR_N;
  else
    return DIR_E;
  endif
endfunction

//------------------------------------------------------------------------
