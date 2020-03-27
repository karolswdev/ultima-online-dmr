//-----------------------------------------------------------------------------

include "include/findcity";
include "include/effects";
include "include/npcs";
include "include/coords";
include "include/misc";
include "include/checks/locchecks";
include "include/client/sounds";
include "ai/include/tamed";

//-----------------------------------------------------------------------------

function SummonCreatureGroup(Caster, byref NpcTemps, Amount, CastLoc, Duration, byref oparms)
  
  if(!NpcTemps.size() or !NpcTemps or !Amount or !CastLoc or Duration <= 0)
    return 0;
  endif

  var NpcTemp, Mod;
  var Mods  := GetLocMods(2);
  var CastZ := CastLoc.z;
  var Count := 0;

  while(Amount > 0 and Count < 20)
    Count   := Count + 1;
    NpcTemp := NpcTemps[RandomInt(NpcTemps.size())+1];
    Mod     := Mods[RandomInt(Mods.size())+1];
    
    if(NpcTemp)
      if(Mod and oparms.curamt)
        CastLoc.x := CastLoc.x + Mod[1];
        CastLoc.y := CastLoc.y + Mod[2];
        CastLoc.z := GetStandZ(CastLoc.x, CastLoc.y, Caster.realm, CastZ);
        if(CastLoc.z == error)
          continue;
        endif
      endif

      if(!SummonCreature(Caster, NpcTemp, CastLoc, Duration, oparms))
        break;
      endif
      Amount := Amount - 1;
    endif
  endwhile

  return oparms.curamt > 0;

endfunction

//-----------------------------------------------------------------------------

function SummonCreature( caster, npctemplate, cast_loc, duration, byref oparms := 0)

  if(Cast_Loc.z == error)
    Fizzle(Caster,"Nie widzisz tego miejsca.");
    return RetError("Nie widzisz tego miejsca.");
  endif

  if(!caster.isa(POLCLASS_NPC) and !CheckLosAt(Caster,Cast_Loc.x,Cast_Loc.y,Cast_Loc.z))
    Fizzle(Caster,"Nie widzisz tego miejsca.");
    return RetError("Nie widzisz tego miejsca.");
  endif
  
  if(!Caster.cmdlevel and !caster.isa(POLCLASS_NPC))
    if(IsInGa(Cast_Loc.x, Cast_Loc.y, Caster.realm))
      Fizzle(Caster,"Nie mozesz przywolac tu stworzenia!");
      return RetError("Nie mozesz przywolac tu stworzenia!");
    elseif(GetStandZ(Cast_Loc.x, Cast_Loc.y, Caster.realm, Cast_Loc.z).multi)
      Fizzle(Caster,"Nie mozesz przywolac tu stworzenia!");
      return RetError("Nie mozesz przywolac tu stworzenia!");
    endif
  endif

  var thecritter;
  var parms  := struct;

  if(oparms.onlyone and GetObjProperty(caster,"summonedserial"))
    var summoned := FindNpcBySerial( GetObjProperty(caster, "summonedserial") );
    if(summoned)
      if(GetMaster(summoned) == Caster)
        Fizzle(caster, "Nie uda Ci sie przywolac wiecej tych stworzen!");
        return RetError("Nie uda Ci sie przywolac wiecej tych stworzen!");
      endif
    endif
  endif

  if(oparms)
    if(!oparms.curamt)
      oparms.+curamt := 0;
    endif
  endif

  if(oparms.maxamt)
    if(!CheckCurrentAmount(Caster, oparms.maxamt, oparms.curamt))
      return RetError("Za duzo stworzen.");
    endif
  endif

  if(oparms.script)
    parms.+script := oparms.script;
  endif
  if(oparms.name)
    parms.+name := oparms.name;
  endif
  if(oparms.color)
    parms.+color := oparms.color;
  endif
  
  // goqsane: fire elemental glyph
  var hpMod := 0;
  var manaMod := 0;
  var arMod := 0;
  
  if(oparms.boostMod)
    parms.+name := "Zywiolak Potegi Ognia";
	parms.+color := 1171;
	hpMod := 2000;
	manaMod := 1000;
	arMod := 30;
  endif

  var SpellParms := oparms.spellparms;

  if(npctemplate.size())
    npctemplate := npctemplate[RandomInt(npctemplate.size())+1];
  endif
  
  Detach();
  sleepms(100);
  thecritter := CreateNpc(npctemplate, cast_loc.x, cast_loc.y,cast_loc.z, Caster.realm, parms, NPCF_NO_UPKEEP | __IfElse(oparms.canloot, NPCF_MIN_LOOT, NPCF_NO_LOOT));

  if(!thecritter)
    return thecritter;
  endif

  if(caster.isa(POLCLASS_NPC))
    SetObjProperty(thecritter, "NpcSummoned", caster.serial);
  endif

  if(oparms.onlyone)
    SetObjProperty(caster, "summonedserial", thecritter.serial);
  endif

  if(oparms.offensive)
    SetObjProperty(thecritter, "SummonOffensive", caster.serial);
  endif
  
  if(oparms.alwayscrim)
    SetSumCrim(caster,thecritter);
  elseif(!oparms.nocrim)
    if(FindCity(caster))
      SetSumCrim(caster,thecritter);
    endif
  endif

  var Elems := GetNpcElementals(npctemplate);

  if(NPCELEM_DAEMON in Elems)
    PlaySoundEffect(caster, SFX_SPELL_SUMMON_DAEMON);
  elseif(NPCELEM_ELEMENTAL in Elems)
    PlaySoundEffect(caster, SFX_SPELL_SUMMON_ELEMENTAL);
  else
    PlaySoundEffect(caster, SFX_SPELL_SUMMON_CREATURE);
  endif

  if(oparms.sumaction)
    sleepms(200);
    PlayAction(thecritter, oparms.sumaction, 2);
  endif

  SetScriptController(thecritter);
  
  if(oparms.controlable)
    Tame(thecritter, Caster);
  else
    SetObjProperty(thecritter, PROP_MASTER, Caster.serial);
  endif

  if(oparms.skillsmod)
    var SkMod  := oparms.skillsmod;

    var SkMain := GetEffectiveSkillBaseValue(Caster,SkMod[1]);
    var SkMind := GetEffectiveSkillBaseValue(Caster,SkMod[2]);

    if(!IsPlayer(Caster))
      SkMain := ToMax(SkMain, DEF_SK_MAX_VALUE + 20);
      SkMind := ToMax(SkMind, DEF_SK_MAX_VALUE + 20);
    endif
    
    if(!thecritter)
      return thecritter;
    endif

    if(SkMain != error and SkMind != error)
      var RunMod := 0;
      
      if(thecritter.run_speed < 100)
        RunMod := CInt(thecritter.run_speed * (SkMain * 0.0008));
      else
        RunMod := CInt(SkMain/10) - 100;
      endif

      thecritter.run_speed := thecritter.run_speed + RunMod;
      SetBaseDexterity(thecritter,GetBaseDexterity(thecritter) + CInt(GetBaseDexterity(thecritter) * (SkMain * 0.0002)));
      SetBaseStrength(thecritter,GetBaseStrength(thecritter) + CInt(GetBaseStrength(thecritter) * (SkMain * 0.0002)));
      SetMaxHp(thecritter,GetMaxHp(thecritter) + CInt(GetMaxHp(thecritter) * (SkMain * 0.0002)) + hpMod);
      SetHp(thecritter, GetMaxHp(thecritter) + hpMod);
	  SetMaxMana(thecritter, GetMaxMana(thecritter) + manaMod);

      SetBaseSkillBaseValue( thecritter, SKILLID_MAGICRESISTANCE, ToMax(SkMain, NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_TACTICS,      ToMax(SkMind, NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_WRESTLING,    ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_WRESTLING)     - (970 - SkMain), NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_SWORDSMANSHIP,ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_SWORDSMANSHIP) - (970 - SkMain), NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_MACEFIGHTING, ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_MACEFIGHTING)  - (970 - SkMain), NPC_MAX_SK_VALUE));

      SetBaseSkillBaseValue( thecritter, SKILLID_MAGERY,       ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_MAGERY )      - (980 - SkMain), NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_EVALINT,      ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_EVALINT )     - (980 - SkMind), NPC_MAX_SK_VALUE));

      SetBaseSkillBaseValue( thecritter, SKILLID_DRUID,        ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_DRUID )       - (980 - SkMain), NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_ANIMALLORE,   ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_ANIMALLORE)   - (980 - SkMind), NPC_MAX_SK_VALUE));

      SetBaseSkillBaseValue( thecritter, SKILLID_SPIRITSPEAK,  ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_SPIRITSPEAK ) - (980 - SkMain), NPC_MAX_SK_VALUE));
      SetBaseSkillBaseValue( thecritter, SKILLID_NECRO,        ToMax(GetBaseSkillBaseValue( thecritter, SKILLID_NECRO )       - (980 - SkMain), NPC_MAX_SK_VALUE));
    endif

  endif

  if(oparms.maxamt)
    AddSummonedToCurSum(Caster,thecritter);
  endif

  oparms.curamt := oparms.curamt + 1;
  SetSummoned(thecritter,duration);
  RestartScript(thecritter);
  
  if(SpellParms[2] == CAST_NPC and SpellParms[3].isa(POLCLASS_MOBILE))
    sleep(1);
    SendChEvent(thecritter,thecritter,EVID_PEACEMADE);
    SendChEvent(thecritter,SpellParms[3],EVID_ENGAGED);
  endif

  if(!oparms.nosleep)
    sleep(duration);
    KillSummonedCreature(thecritter,caster);
  endif
  
  return thecritter;

endfunction

//-----------------------------------------------------------------------------

function SetSumCrim(caster,critter)

  if(!FindNpcBySerial(CInt(GetObjProperty(caster,"SumCrim"))))
    if(caster.criminal)
      SetObjProperty(caster, "WasCrim",1);
    endif
  endif
  
  SetObjProperty(caster,"SumCrim",critter.serial);
  SetCriminal(caster, 1, "SetSumCrim", "Przywolal "+critter.name+"/"+critter.npctemplate+" ["+critter.serial+"].");

endfunction

//-----------------------------------------------------------------------------

function KillSummonedCreature(critter, Caster := 0)

  CheckSumDie(critter,Caster);
  sleepms(200);
  KillNpc(critter);

endfunction

//-----------------------------------------------------------------------------

function CheckSumDie(critter,Caster := 0)

  if(!Caster)
    Caster := GetMaster(critter);
  endif

  if(Caster)
    EraseObjProperty(caster,"SumCrim");
    EraseObjProperty(caster,"WasCrim");

    var CurSum := GetObjProperty(Caster,"CurSum");
    var i      := 1;

    if(CurSum)
      foreach Sum in CurSum
        if(Sum == critter.serial)
          CurSum.erase(i);
          break;
        endif
        i := i + 1;
      endforeach
      SetObjProperty(Caster,"CurSum",CurSum);
    endif
  endif
  
  if(critter.isa(POLCLASS_MOBILE))
    critter.hidden := 1;
  elseif(critter.isa(POLCLASS_CORPSE) and !IsSetFlag(critter, NPCF_MIN_LOOT))
    critter.invisible := 1;
  endif
  
  PlaySoundEffect( critter, SFX_SPELL_MANA_VAMPIRE );
  PlayStationaryEff( critter.x, critter.y, critter.z, FX_SMOKE, 0xa, 0xa, 0, critter.realm );

endfunction

//-----------------------------------------------------------------------------

function CheckSummonedCreature(critter)

  if(IsSummoned(critter))
    if(GetObjProperty(critter,"summonedtime") < ReadGameClock())
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetSummoned(critter,time)

  SetObjProperty(critter, "summoned",1);
  setobjproperty(critter, "summonedtime",Readgameclock() + time);

endfunction

//-----------------------------------------------------------------------------

function CheckCurrentAmount(Caster, maxamt, curamt)

  var Current := GetObjProperty(Caster,"CurSum"),i := 1;
  if(!Current.size() or !Current)
    Current := {};
  endif
  
  for(i:=1;i<=Current.size();i:=i+1)
    if(!FindNpcBySerial(Current[i]))
      Current.erase(i);
      i := i - 1;
    endif
  endfor
  
  SetObjProperty(Caster,"CurSum",Current);
  
  if(Current.size() >= maxamt)
    if(!curamt)
      Fizzle(Caster,"Nie zdolasz przywolac wiecej stworzen!");
    endif
    return 0;
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddSummonedToCurSum(Caster, Summoned)

  if(Summoned.serial)
    var Current := GetObjProperty(Caster,"CurSum");
    Current.append(Summoned.serial);
    SetObjProperty(Caster,"CurSum",Current);
  else
    return 0;
  endif
  return Summoned;

endfunction

//-----------------------------------------------------------------------------
