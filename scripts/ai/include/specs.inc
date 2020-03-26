//-----------------------------------------------------------------------------

include "specs_funcs";

//================================================================================
//-----------------------------------------------------------------------------
// Npc Specs
//-----------------------------------------------------------------------------
//================================================================================

const THOLD_DIST_CHECK       := 5;

//-----------------------------------------------------------------------------

function InitSpecs()

  if(SpecFlag & NSPECF_AUTODISPEL)
    Specs[NSPEC_AUTODISPEL][3] := ReadGameClock() + Specs[NSPEC_AUTODISPEL][2];
  endif
  
  if(SpecFlag & NSPECF_PROVOKETAMED)
    Specs[NSPEC_PROVOKETAMED][3] := ReadGameClock() + Specs[NSPEC_PROVOKETAMED][2];
  endif
  
  if(SpecFlag & NSPECF_FLAMEBREATH)
    Specs[NSPEC_FLAMEBREATH][2] := Cint(ReadGameClock() + (RandomInt(4) + 4));
  endif
  
  if(SpecFlag & NSPECF_PLAGUE)
    Specs[NSPEC_PLAGUE][4] := ReadGameClock() + RandomInt(5);
  endif
  
  if(SpecFlag & NSPECF_HPDRAIN)
    Specs[NSPEC_PLAGUE][4] := ReadGameClock() + RandomInt(5);
  endif

  if(SpecFlag & NSPECF_AUTOHEALING)
    Specs[NSPEC_AUTOHEALING][4] := ReadGameClock() + RandomInt(5);
  endif

  ResetNpcSpecDelay(NSPECF_MINDRAGE, NSPEC_MINDRAGE);
  ResetNpcSpecDelay(NSPECF_CONFLAGRATION, NSPEC_CONFLAGRATION);
  ResetNpcSpecDelay(NSPECF_ENERGYSHIELD, NSPEC_ENERGYSHIELD);
  ResetNpcSpecDelay(NSPECF_SUMMONMINIONS, NSPEC_SUMMONMINIONS);
  ResetNpcSpecDelay(NSPECF_CORPSEEATER, NSPEC_CORPSEEATER);
  ResetNpcSpecDelay(NSPECF_CORPSESPITTER, NSPEC_CORPSESPITTER);
  ResetNpcSpecDelay(NSPECF_ABYSSFORCE, NSPEC_ABYSSFORCE);
  ResetNpcSpecDelay(NSPECF_CREATEITEM, NSPEC_CREATEITEM);
  ResetNpcSpecDelay(NSPECF_STONEFALL, NSPEC_STONEFALL);
  ResetNpcSpecDelay(NSPECF_BLADEDANCE, NSPEC_BLADEDANCE);
  ResetNpcSpecDelay(NSPECF_REVEALTERROR, NSPEC_REVEALTERROR);
  ResetNpcSpecDelay(NSPECF_MINDCONTROL, NSPEC_MINDCONTROL);
  ResetNpcSpecDelay(NSPECF_REGENERATION, NSPEC_REGENERATION);
  ResetNpcSpecDelay(NSPECF_POLYMORPH, NSPEC_POLYMORPH);
  ResetNpcSpecDelay(NSPECF_CURSE, NSPEC_CURSE);
  
  EraseNpcSpecPerforming(me, NSPEC_CONFLAGRATION);
  EraseNpcSpecPerforming(me, NSPEC_ENERGYSHIELD);   
  EraseNpcSpecPerforming(me, NSPEC_MINDRAGE);
  EraseNpcSpecPerforming(me, NSPEC_SUMMONMINIONS);
  EraseNpcSpecPerforming(me, NSPEC_CORPSEEATER);
  EraseNpcSpecPerforming(me, NSPEC_CORPSESPITTER);
  EraseNpcSpecPerforming(me, NSPEC_ABYSSFORCE);
  EraseNpcSpecPerforming(me, NSPEC_CREATEITEM);
  EraseNpcSpecPerforming(me, NSPEC_STONEFALL);
  EraseNpcSpecPerforming(me, NSPEC_BLADEDANCE);
  EraseNpcSpecPerforming(me, NSPEC_REVEALTERROR);
  EraseNpcSpecPerforming(me, NSPEC_MINDCONTROL);
  EraseNpcSpecPerforming(me, NSPEC_REGENERATION);
  EraseNpcSpecPerforming(me, NSPEC_POLYMORPH);
  EraseNpcSpecPerforming(me, NSPEC_CURSE);

  Specs[NSPEC_GLOBAL_DELAY] := ReadGameClock() + 5;

endfunction

//-----------------------------------------------------------------------------

function IsSpecOn(Flag)

  return (SpecFlag & Flag) == Flag;

endfunction

//-----------------------------------------------------------------------------

function SetSpecs()

  var cfgElem := GetNpcConfig(me);

  Specs    := dictionary;
  SpecFlag := 0;

  Specs[NSPEC_THRESHOLD] := HALT_THRESHOLD;
  Specs[NSPEC_GLOBAL_DELAY] := ReadGameClock() + 5;

  if(CInt(cfgElem.HaltThreshold))
    Specs[NSPEC_THRESHOLD] := CInt(cfgElem.HaltThreshold);
  endif

  if(cfgElem.SpecialSight)
    Specs[NSPEC_THRESHOLD] := Specs[NSPEC_THRESHOLD] + CInt(Specs[NSPEC_THRESHOLD] * 0.35);
  endif
  
  Specs[NSPEC_THRESHOLD] := ToMax(Specs[NSPEC_THRESHOLD], 17);

  if(cfgElem.AutoDispel)
    Specs[NSPEC_AUTODISPEL] := array( CInt(cfgElem.AutoDispel), CInt(cfgElem.AutoDispelTime), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_AUTODISPEL);
  endif

  if(cfgElem.ProvokeTamed)
    Specs[NSPEC_PROVOKETAMED] := array( CInt(cfgElem.ProvokeTamedChance), CInt(cfgElem.ProvokeTamedTime), ReadGameClock(), SplitWords(cfgElem.ProvokeTamed) );
    SetBitOn(SpecFlag, NSPECF_PROVOKETAMED);
  endif

  if(cfgElem.HpDrainSkillTime and cfgElem.HpDrainSkillAmt and cfgElem.HpDrainSkillRange)
    Specs[NSPEC_HPDRAIN] := array( CInt(cfgElem.HpDrainSkillAmt), CInt(cfgElem.HpDrainSkillTime), CInt(cfgElem.HpDrainSkillRange), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_HPDRAIN);
  endif

  if(cfgElem.PlagueSkillTime and cfgElem.PlagueSkillLvl and cfgElem.PlagueSkillRange)
    Specs[NSPEC_PLAGUE] := array(CInt(cfgElem.PlagueSkillLvl), CInt(cfgElem.PlagueSkillTime), CInt(cfgElem.PlagueSkillRange), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_PLAGUE);
  endif

  if(cfgElem.AutoSpell)
    Specs[NSPEC_AUTOSPELL] := array( CInt(cfgElem.AutoSpell), CInt(cfgElem.AutoSpellTime), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_AUTOSPELL);
  endif

  if(cfgElem.AutoHealing)
    Specs[NSPEC_AUTOHEALING] := array( CInt(cfgElem.AutoHealing) * 0.01, CInt(cfgElem.AutoHealingTime), CInt(cfgElem.AutoHealingChance), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_AUTOHEALING);
  endif

  if(cfgElem.AcidPool)
    Specs[NSPEC_ACIDPOOL] := array( CInt(cfgElem.AcidPool), 5 );
    SetBitOn(SpecFlag, NSPECF_ACIDPOOL);
  endif

  if(cfgElem.SpiderwebPool)
    Specs[NSPEC_SPIDERWEBPOOL] := array( CInt(cfgElem.SpiderwebPool), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_SPIDERWEBPOOL);
  endif

  if(cfgElem.WhirlwindAttack)
    Specs[NSPEC_WHIRLWIND] := CInt(cfgElem.WhirlwindAttack);
    SetBitOn(SpecFlag, NSPECF_WHIRLWIND);
  endif

  if(cfgElem.ConcussionBlow)
    Specs[NSPEC_CONCUSSION] := CInt(cfgElem.ConcussionBlow);
    SetBitOn(SpecFlag, NSPECF_CONCUSSION);
  endif

  if(cfgElem.AutoSummonTemp)
    Specs[NSPEC_AUTOSUMMON] := array( cfgElem.AutoSummonTemp, CInt(cfgElem.AutoSummonAmt),CInt(cfgElem.AutoSummonTime) );
    SetBitOn(SpecFlag, NSPECF_AUTOSUMMON);
  endif

  if(CInt(cfgElem.FlameStrength))
    Specs[NSPEC_FLAMEBREATH] := array( CInt(cfgElem.FlameStrength), ReadGameClock() );
    SetBitOn(SpecFlag, NSPECF_FLAMEBREATH);
  endif

  if(cfgElem.KamikazeExplode)
    Specs[NSPEC_KAMIKAZEDMG] := cfgElem.KamikazeExplode;
    SetBitOn(SpecFlag, NSPECF_KAMIKAZE);
  endif

  if(cfgElem.DispelAllNear)
    Specs[NSPEC_DISPELALL] := ReadGameClock();
    SetBitOn(SpecFlag, NSPECF_DISPELALL);
  endif
  
  if(cfgElem.MindRage)
    var Data := SplitWords(cfgElem.MindRage);
    if(Data.size() == 6)
      Specs[NSPEC_MINDRAGE] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), CDbl(Data[6])/100.0, 0); // opis w specs/mindrage.inc
      SetBitOn(SpecFlag, NSPECF_MINDRAGE);
    endif
  endif
  
  if(cfgElem.AbyssForce)
    var Data := SplitWords(cfgElem.AbyssForce);
    if(Data.size() == 5)
      Specs[NSPEC_ABYSSFORCE] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), 0); // opis w specs/abyssforce.inc
      SetBitOn(SpecFlag, NSPECF_ABYSSFORCE);
    endif
  endif

  if(cfgElem.Conflagration)
    var Data := SplitWords(cfgElem.Conflagration);
    if(Data.size() == 7)
      Specs[NSPEC_CONFLAGRATION] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), CDbl(Data[6])/100.0, Data[7], 0); // opis w specs/conflagration.inc
      SetBitOn(SpecFlag, NSPECF_CONFLAGRATION);
    endif
  endif
  
  if(cfgElem.EnergyShield)
    var Data := SplitWords(cfgElem.EnergyShield);
    if(Data.size() == 4)
      Specs[NSPEC_ENERGYSHIELD] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), 0); // opis w specs/energyshield.inc
      SetBitOn(SpecFlag, NSPECF_ENERGYSHIELD);
    endif
  endif

  if(cfgElem.SummonMinions)
    var Data := SplitWords(cfgElem.SummonMinions);
    if(Data.size() == 8)
      Specs[NSPEC_SUMMONMINIONS] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), Data[6], CInt(Data[7]), Data[8], 0); // opis w specs/summonminions.inc
      SetBitOn(SpecFlag, NSPECF_SUMMONMINIONS);
    endif
  endif
  
  if(cfgElem.CorpseEater)
    var Data := SplitWords(cfgElem.CorpseEater);
    if(Data.size() == 4)
      Specs[NSPEC_CORPSEEATER] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), 0); // opis w specs/corpseeater.inc
      SetBitOn(SpecFlag, NSPECF_CORPSEEATER);
    endif
  endif
  
  if(cfgElem.CorpseSpitter)
    var Data := SplitWords(cfgElem.CorpseSpitter);
    if(Data.size() == 4)
      Specs[NSPEC_CORPSESPITTER] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), 0); // opis w specs/corpsespitter.inc
      SetBitOn(SpecFlag, NSPECF_CORPSESPITTER);
    endif
  endif
  
  if(cfgElem.CreateItem)
    var Data := SplitWords(cfgElem.CreateItem);
    if(Data.size() == 6)
      Specs[NSPEC_CREATEITEM] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), CInt(Data[6]), 0); // opis w specs/corpsespitter.inc
      SetBitOn(SpecFlag, NSPECF_CREATEITEM);
    endif
  endif
  
  if(cfgElem.StoneFall)
    var Data := SplitWords(cfgElem.StoneFall);
    if(Data.size() == 5)
      Specs[NSPEC_STONEFALL] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), 0); // opis w specs/stonefall.inc
      SetBitOn(SpecFlag, NSPECF_STONEFALL);
    endif
  endif
  
  if(cfgElem.BladeDance)
    var Data := SplitWords(cfgElem.BladeDance);
    if(Data.size() == 5)
      Specs[NSPEC_BLADEDANCE] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), 0); // opis w specs/bladedance.inc
      SetBitOn(SpecFlag, NSPECF_BLADEDANCE);
    endif
  endif

  if(cfgElem.RevealTerror)
    var Data := SplitWords(cfgElem.RevealTerror);
    if(Data.size() == 5)
      Specs[NSPEC_REVEALTERROR] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), 0); // opis w specs/revealterror.inc
      SetBitOn(SpecFlag, NSPECF_REVEALTERROR);
    endif
  endif
  
  if(cfgElem.MindControl)
    var Data := SplitWords(cfgElem.MindControl);
    if(Data.size() == 5)
      Specs[NSPEC_MINDCONTROL] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), Data[5], 0); // opis w specs/mindcontrol.inc
      SetBitOn(SpecFlag, NSPECF_MINDCONTROL);
    endif
  endif

  if(cfgElem.Regeneration)
    var Data := SplitWords(cfgElem.Regeneration);
    if(Data.size() == 6)
      Specs[NSPEC_REGENERATION] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CDbl(Data[5]) / 100.0, Data[6], 0); // opis w specs/regeneration.inc
      SetBitOn(SpecFlag, NSPECF_REGENERATION);
    endif
  endif
  
  if(cfgElem.Polymorph)
    var Data := SplitWords(cfgElem.Polymorph);
    if(Data.size() == 7)
      Specs[NSPEC_POLYMORPH] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), Data[5], Data[6], CInt(Data[7]), 0); // opis w specs/energyshield.inc
      SetBitOn(SpecFlag, NSPECF_POLYMORPH);
    endif
  endif
  
  if(cfgElem.SpecCurse)
    var Data := SplitWords(cfgElem.SpecCurse);
    if(Data.size() == 8)
      Specs[NSPEC_CURSE] := array(CInt(Data[1]), CDbl(Data[2]) / 100.0, CInt(Data[3]), CInt(Data[4]), CInt(Data[5]), Data[6], CInt(Data[7]), CInt(Data[8]), 0); // opis w specs/curse.inc
      SetBitOn(SpecFlag, NSPECF_CURSE);
    endif
  endif

  if(GetProperty("Debug"))
    SetBitOn(SpecFlag, NSPECF_DEBUG);
  endif

endfunction

//-----------------------------------------------------------------------------

function ResetNpcSpecDelay(Flag, SpecId)

  if(SpecFlag & Flag)
    var lIdx := Specs[SpecId].size(); // zakladamy, ze clock bedzie zawsze na ostatnim elemencie danych specjala
    Specs[SpecId][lIdx] := ReadGameClock() + CInt(Specs[SpecId][1] * 0.1); 
  endif

endfunction

//-----------------------------------------------------------------------------

function DoNpcSpecDelay(SpecId, GlobDelay := 20)

  var lIdx := Specs[SpecId].size(); // zakladamy, ze clock bedzie zawsze na ostatnim elemencie danych specjala
  Specs[SpecId][lIdx] := ReadGameClock() + Specs[SpecId][1];
  Specs[NSPEC_GLOBAL_DELAY] := ReadGameClock() + GlobDelay;

endfunction

//-----------------------------------------------------------------------------

function GetNpcSpecDelayTime(SpecId)

  if(Specs[NSPEC_GLOBAL_DELAY] > ReadGameClock())
    return Specs[NSPEC_GLOBAL_DELAY] - ReadGameClock();
  endif
  
  if(IsNpcSpecPerforming(me, SpecId))
    return error;
  endif

  var lIdx := Specs[SpecId].size(); // zakladamy, ze clock bedzie zawsze na ostatnim elemencie danych specjala
  
  if(Specs[SpecId][lIdx] > ReadGameClock())
    return Specs[SpecId][lIdx] - ReadGameClock();
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcSpecDelay(SpecId, GlobDelay := 20)

  if(Specs[NSPEC_GLOBAL_DELAY] > ReadGameClock())
    return 0;
  endif
  
  if(IsNpcSpecPerforming(me, SpecId))
    return 0;
  endif

  var lIdx := Specs[SpecId].size(); // zakladamy, ze clock bedzie zawsze na ostatnim elemencie danych specjala
  
  if(Specs[SpecId][lIdx] > ReadGameClock())
    return 0;
  endif

  Specs[SpecId][lIdx] := ReadGameClock() + Specs[SpecId][1];
  Specs[NSPEC_GLOBAL_DELAY] := ReadGameClock() + GlobDelay;
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcSpecial()

  if(SpecFlag & NSPECF_DISPELALL)
    DispelAllNear();
  endif

  if(!me.paralyzed and !me.frozen)
    if(SpecFlag & NSPECF_HPDRAIN)
      CheckHpDrainSkill();
    endif
      
    if(SpecFlag & NSPECF_PLAGUE)
      CheckPlagueSkill();
    endif
      
    if(SpecFlag & NSPECF_AUTOSPELL)
      CheckAutoSpell();
    endif

    if(SpecFlag & NSPECF_ACIDPOOL)
      MakeAcidPool();
    endif

    if(SpecFlag & NSPECF_SPIDERWEBPOOL)
      MakeSpiderwebPool();
    endif

    if(SpecFlag & NSPECF_WHIRLWIND)
      WhirlwindAttack();
    endif

    if(SpecFlag & NSPECF_CONCUSSION)
      ConcussionBlow();
    endif

    if(SpecFlag & NSPECF_AUTOSUMMON)
      CheckAutoSummon();
    endif

    if(SpecFlag & NSPECF_FLAMEBREATH)
      FlameBreathAttack();
    endif

    if(SpecFlag & NSPECF_AUTOHEALING)
      CheckAutoHealing();
    endif

    if(Specs[NSPEC_GLOBAL_DELAY] < ReadGameClock())
      if(SpecFlag & NSPECF_MINDRAGE)
        CheckMindRage();
      endif
      
      if(SpecFlag & NSPECF_CONFLAGRATION)
        CheckConflagration();
      endif
      
      if(SpecFlag & NSPECF_ENERGYSHIELD)
        CheckEnergyShield();
      endif
      
      if(SpecFlag & NSPECF_SUMMONMINIONS)
        CheckSummonMinions();
      endif
      
      if(SpecFlag & NSPECF_CORPSEEATER)
        CheckCorpseEater();
      endif
      
      if(SpecFlag & NSPECF_CORPSESPITTER)
        CheckCorpseSpitter();
      endif

      if(SpecFlag & NSPECF_ABYSSFORCE)
        CheckAbyssForce();
      endif
      
      if(SpecFlag & NSPECF_CREATEITEM)
        CheckItemSpecCreation();
      endif
      
      if(SpecFlag & NSPECF_STONEFALL)
        CheckStoneFall();
      endif
      
      if(SpecFlag & NSPECF_BLADEDANCE)
        CheckBladeDance();
      endif

      if(SpecFlag & NSPECF_REVEALTERROR)
        CheckRevealTerror();
      endif
      
      if(SpecFlag & NSPECF_MINDCONTROL)
        CheckMindControl();
      endif
      
      if(SpecFlag & NSPECF_REGENERATION)
        CheckSpecRegeneration();
      endif
      
      if(SpecFlag & NSPECF_POLYMORPH)
        CheckPolymorph();
      endif
      
      if(SpecFlag & NSPECF_CURSE)
        CheckSpecCurse();
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAnimalSpecs()

  var cfgElem := GetNpcConfig(me);

  if(cfgElem.LayItem)
    if(!GetObjProperty(me,"NextLay"))
      SetObjProperty(me,"NextLay",ReadGameClock() + 900 + RandomInt(600));
    endif

    if(GetObjProperty(me,"NextLay") < ReadGameClock())
      SetObjProperty(me,"NextLay",ReadGameClock() + 900 + RandomInt(600));
      var LayItem := SplitWords(cfgElem.LayItem);
      LayItem[1] := CInt(LayItem[1]);
      if(LayItem[2]["d"])
        LayItem[2] := RandomDiceRoll(LayItem[2]);
      else
        LayItem[2] := CInt(LayItem[2]);
      endif

      var Res := CreateItemAtLoc(me.x, me.y, me.z, LayItem[1], LayItem[2], me.realm);
      if(!Res)
        LogError("AnimalIdle: LayItem: Bledny Nie udalo sie stworzyc przedmiotu Obj: ["+cfgElem.LayItem+"] ["+Res.errortext+"]");
      endif
    endif
  endif
  
  if(cfgElem.ReGrowGrap)
    if(!GetObjProperty(me,"NextRegrow"))
      SetObjProperty(me,"NextRegrow",ReadGameClock() + 900 + RandomInt(300));
    endif

    if(GetObjProperty(me,"NextRegrow") < ReadGameClock())
      SetObjProperty(me,"NextRegrow",ReadGameClock() + 900 + RandomInt(300));
      if(me.graphic != CInt(cfgElem.ReGrowGrap))
        SetNpcGraphic(me, CInt(cfgElem.ReGrowGrap));
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
