//-----------------------------------------------------------------------------

include ":magic:magicatt";
include "include/effects";

//-----------------------------------------------------------------------------

const GHEAL_SPID             := 29;
const CURE_SPID              := 11;
const ARCHCURE_SPID          := 25;
const DISPEL_SPID            := 41;
const MDISPEL_SPID           := 54;
const RES_SPID               := 59;
const REFLECT_SPID           := 36;
const POISON_SPID            := 20;
const TELEPORT_SPID          := 22;

//-----------------------------------------------------------------------------

function CastSpell(Caster, CastOn, SpellID, Flags := 0)

  if(!CheckSpecialLos(Caster, CastOn))
    return 0;
  endif

  if(DistZ(Caster,CastOn) >= 20)
    return 0;
  endif

  if(!(Flags & SPF_NOMANA))
    if(GetMana(Caster) < 4)
      return 0;
    endif
  endif

  if(SpellID == TELEPORT_SPID)
    if(Distance(Caster, CastOn) < 3)
      return 0;
    endif
  endif

  if(Caster.paralyzed or Caster.frozen)
    return 0;
  endif

  if(Caster.run_speed >= 180)
    if(Distance(Caster, CastOn) > 10)
      return 0;
    endif
  endif

  SpellID := CInt(SpellID);
  if(!Caster or !CastOn or !SpellID)
    return 0;
  endif

  var SpellsCfg  := ReadConfigFile(SPELLS_CFG);

  if(SpellsCfg[SpellID].NpcCast != "On")
    LogError("CastSpell: Npc ["+me.serial+"] probuje wyczarowac wylaczony czar ["+SpellsCfg[SpellID].Name+"] SpID: ["+SpellId+"]");
    return 0;
  endif

  if(!(Flags & SPF_NOMANA))
    var CirclesCfg := ReadConfigFile(CIRCLES_CFG);
    var Mana := CInt(CirclesCfg[SpellsCfg[SpellId].Circle].UMana);

    if(IsTamed(me) or IsBitOn(Flags, SPF_USEMOREMANA))
      Mana := CInt(Mana*1.5);
      SetBitOn(Flags, SPF_USEMOREMANA);
    endif

    if(GetMana(me) < Mana)
      return 0;
    endif
  endif

  var SpellScript := SpellsCfg[SpellID].Script;

  if(!SpellScript)
    LogError("CastSpell: NPC "+Caster.name+" chcial wyczarowac "+SpellsCfg[SpellID].Name+" SPID: ["+SpellID+"] ale brakuje SPScript ["+SpellScript+"]");
    return 0;
  endif

  sleepms(100);

  var SpellParms := {};
  SpellParms[1] := Caster;
  SpellParms[2] := CAST_NPC;
  SpellParms[3] := CastOn;
  SpellParms[4] := SPF_NOREGS | CInt(Flags);
  
  if(!IsHuman(Caster))
    SpellParms[4] := SpellParms[4] | SPF_NOWORDS;
  endif

  if(!start_script(SpellScript,SpellParms))
    return 0;
  endif

  //sleep(1);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CastOffensiveSpell(caster, CastOn)

  if(!spells.size() or !spells)
    return;
  endif

  var thespell := 0;

  if(CastOn.isa(POLCLASS_NPC))
    var CastOnElems := GetNpcElementals(CastOn);

    if(CastOnElems.size() and (NPCELEM_FIRE in CastOnElems or NPCELEM_COLD in CastOnElems or NPCELEM_LIGHT in CastOnElems))
      var SpellElems, isGood;
      var RightSpells := array;

      foreach SpellID in spells
        SpellElems := GetSpellValue(SpellID, "Element", STRING_ID);
        if(!SpellElems)
          RightSpells.append(SpellID);
        else
          SpellElems := SplitWords(SpellElems);
          isGood     := 1;

          foreach SpellElem in SpellElems
            case(SpellElem)
              SPELLELEM_FIRE:       if(NPCELEM_FIRE in CastOnElems)
                                      isGood := 0;
                                    endif
              SPELLELEM_COLD:       if(NPCELEM_COLD in CastOnElems)
                                      isGood := 0;
                                    endif
              SPELLELEM_LIGHT:      if(NPCELEM_LIGHT in CastOnElems)
                                      isGood := 0;
                                    endif
            endcase
          endforeach

          if(isGood)
            RightSpells.append(SpellID);
          endif
        endif
      endforeach

      if(!RightSpells.size())
        LogError("CastOffensiveSpell: NPC ["+caster.serial+"/"+caster.npctemplate+"] nie ma zadnego czaru, ktorym moglby rzucic w NPCa ["+CastOn.npctemplate+"].");
        return;
      endif

      thespell := CInt(RightSpells[RandomInt(RightSpells.size()) + 1]);

      if(!thespell)
        LogError("CastOffensiveSpell: NPC ["+caster.serial+"/"+caster.npctemplate+"] wylosowal nieprawidlowy spellid, RightSpellsSize ["+RightSpells.size()+"].");
        return;
      endif
    endif
  endif

  if(!thespell)
    thespell := CInt(spells[RandomInt(spells.size()) + 1]);

    if(!thespell)
      LogError("CastOffensiveSpell: NPC ["+caster.serial+"/"+caster.npctemplate+"] wylosowal nieprawidlowy spellid, SpellsSize ["+spells.size()+"].");
      return;
    endif
  endif

  CastSpell(caster, CastOn, thespell);

endfunction

//-----------------------------------------------------------------------------

function KamikazeExplode(me,Master := 0)

  var Dmg;

  if(Master)
    Dmg := RandomDiceRoll(CStr(CInt(GetEffectiveSkill(Master,SKILLID_ANIMALLORE) / 30))+"d"+CStr(CInt(GetEffectiveSkill(Master,SKILLID_DRUID) / 8)));
  else
    if(Specs[NSPEC_KAMIKAZEDMG])
      Dmg := RandomDiceRoll(CStr(Specs[NSPEC_KAMIKAZEDMG]));
    else
      Dmg := RandomDiceRoll("2d10");
    endif

    Master := me;
  endif

  PlaySoundEffect(Me, SFX_SPELL_EXPLOSION );
  PlayStationaryEff( Me.x,Me.y,Me.z, FX_EXPLODE_3, 7, 0x10, 0, me.realm);
  foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, 2, me.realm)
    if(CanVSAttack(Master,Mob))
      DoMagicDamage(Mob, Master, Dmg, 8, DMGTYPE_FIRE, SPTYPE_NONE, NO_RESIST_GAIN);
    endif
  endforeach
  KillNPC(me);
  exit;

endfunction

//-----------------------------------------------------------------------------

function CastFlameBreath(Caster, CastOn, FlameDmg)

  if(!CheckLineOfSight(Caster, CastOn) or DistZ(Caster, CastOn) >= 15)
    return;
  endif

  var cfgElem := GetNpcConfig(Caster);

  if(cfgElem.FlameAnim)
    if(Distance(Caster,CastOn) > 4)
      return;
    endif
  elseif(Distance(Caster,CastOn) > 8)
    return;
  endif

  PlaySoundEffect(Caster, 0x15F);
  TurnToward(CastOn);

  if(cfgElem.FlameAnim)
    PlayAction(Caster, CInt(cfgElem.FlameAnim), 4);
    sleepms(3000);
    PlayHitAnim(CastOn);
    PlayObjectCenteredEffect(CastOn, FX_EXPLODE_1, 0xF, 0xF);
    sleepms(500);
    PlayObjectCenteredEffect(CastOn, FX_EXPLODE_1, 0xF, 0xF);
    sleepms(500);
    PlayObjectCenteredEffect(CastOn, FX_EXPLODE_1, 0xF, 0xF);
  elseif(Caster and CastOn)
    PlayMovingEff(Caster, CastOn, FX_FIREBALL, 5, 1, 1);
  endif

  var Dmg := Cint(((FlameDmg * GetHp(me)) / GetMaxHp(me)));
  Dmg     := RandomInt(CInt(Dmg/2)) + CInt(Dmg/2);

  DoMagicDamage(CastOn, Caster ,Dmg, 6, DMGTYPE_FIRE, SPTYPE_NONE, NO_RESIST_GAIN);

endfunction

//-----------------------------------------------------------------------------
