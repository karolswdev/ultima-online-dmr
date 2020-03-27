//-----------------------------------------------------------------------------

include "include/client/effects";
include ":combat:combat";
include ":magicitems:magicitems";

//-----------------------------------------------------------------------------

function IsForceField(who)

  if(CInt(GetObjProperty(who,"ForceField")) > 0)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveForceField(who,Attacker := 0)

  if(CInt(GetObjProperty(who,"ForceField")) > 0)
    PlayObjectCenteredEffect(who,FX_HEAL_EFFECT,0x9,0x9);
    PlaySoundEffect(who,0x2F5);
    if(IsPlayer(Attacker))
      var ForceFieldDamaged := BreakForceField(who, Attacker); 
        if(!CInt(GetObjProperty(Attacker,"#NFFMsg")))
          SetObjProperty(Attacker,"#NFFMsg",ReadGameClock() + 15);
        endif
        if(CInt(GetObjProperty(Attacker,"#NFFMsg")) < ReadGameClock())
          if (ForceFieldDamaged)
            SendSysMessage(Attacker,"Twa bron nieznacznie oslabila magiczna oslone.",FONT_NORMAL,COLOR_GREEN);
          else
            SendSysMessage(Attacker,"Twa bron nie moze przebic magicznej oslony!",FONT_NORMAL,COLOR_RED);
          endif
          SetObjProperty(Attacker,"#NFFMsg",ReadGameClock() + 15);
        endif
    endif
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

const NO_FIELD_REDUCE := 1;

//-----------------------------------------------------------------------------

function CheckForceField(who,Dmg,NoReduce := 0)

  if(CInt(GetObjProperty(who,"ForceField")))
    if(CInt(GetObjProperty(who,"ForceField")) > 0)
      if(!NoReduce)
        SetObjProperty(who,"ForceField",CInt(GetObjProperty(who,"ForceField")) - CInt(Dmg));
      endif
      if(GetObjProperty(who,"ForceField") <= 0)
        SetObjProperty(who,"ForceField",-1);
        PlayObjectCenteredEffect(who,FX_FLAMESTRIKE,0xf,0xf);
      else
        PlayObjectCenteredEffect(who,FX_HEAL_EFFECT,0xf,0xf);
        if (Dmg and !NoReduce)
          PlayObjectCenteredEffect(who,FX_GLOW,0xf,0xf);
        endif
        PlaySoundEffect(who,0x2F5);
      endif
      return 0;
    else
      return 0;
    endif
  endif
  return Dmg;

endfunction

//-----------------------------------------------------------------------------

function BreakForceField(Who,Attacker)
  var Weapon := Attacker.weapon;
  
  if(weapon.objtype == UOBJ_WRESTLE)
    var Gloves := GetEquipmentByLayer(attacker, LAYER_GLOVES);
    if(Gloves)
      weapon := Gloves;
    endif
  endif

  foreach OnHit in GetOnHitScripts(Weapon)
    if ( OnHit == MAGIC_HIT_SCRIPT )
      var Dmg := GetMagicItemLevel(Weapon);
      if (!Dmg)
        Dmg := 4;
      endif
      CheckForceField(who,dmg);
      
      case(GetMagicItemPrefixMod(Weapon))
        MAGICMOD_FIREDMG:        PlaySoundEffect( who, SFX_SPELL_EXPLOSION );
                                 PlayObjectCenteredEffect(who, FX_EXPLODE_3, 0x7, 0x10);
                                 break;

        MAGICMOD_COLDDMG:        PlaySoundEffect( who, 0x15);
                                 PlayPcEffect(who, FX_CURSE_EFFECT, 1151, 0xF, 0xE);
                                 break;

        MAGICMOD_LIGHTDMG:       PlaySoundEffect( who, SFX_LIGHTNING_STRIKE);
                                 PlayPcEffect(who, FX_HEAL_EFFECT, 90, 0xF, 0x8, 0x2);
                
        default:                 break;                               
      endcase 
      return 1;
    endif
  endforeach
  return 0;
endfunction

//-----------------------------------------------------------------------------