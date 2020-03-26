//-----------------------------------------------------------------------------

include "include/tempmods";
include "include/packets";
include "include/events";
include "include/effects";
include ":glyphs:glyphs";

//-----------------------------------------------------------------------------

const POISON_HIT_SCRIPT       := "PH";
const MPOISON_HIT_SCRIPT      := "MP";
const MACE_HIT_SCRIPT         := "MC";
const MAGIC_HIT_SCRIPT        := "MG";
const EELEM_HIT_SCRIPT        := "EL";
const VORTEX_HIT_SCRIPT       := "VH";
const SLAYER_HIT_SCRIPT       := "SL";
const LEECH_HIT_SCRIPT        := "LH";

//-----------------------------------------------------------------------------

const SILVER_COLOR            := 0x515;

//-----------------------------------------------------------------------------

function GetOnHitScripts(weapon)

  var OnHitScripts := GetObjProperty(weapon,"OnHit");
  if(!OnHitScripts)
    OnHitScripts := array;
  else
    OnHitScripts := SplitWords(OnHitScripts);
  endif
  return OnHitScripts;

endfunction

//-----------------------------------------------------------------------------

function SetOnHitScripts(weapon,OnHitScripts)

  SetObjProperty(weapon,"OnHit",CombineWords(OnHitScripts));

endfunction

//-----------------------------------------------------------------------------

function AddNewOnHitScript(weapon,Hit)

  var OnHit := GetOnHitScripts(weapon);
  if(Hit in OnHit)
    return 0;
  endif

  OnHit.append(Hit);

  if(OnHit)
    SetOnHitScripts(weapon,OnHit);
  endif
  return 1;

endfunction

//-----------------------------------------------------------------------------

function EraseOnHitScript(weapon,script)

  var OnHitScripts := GetOnHitScripts(weapon);
  if(script in OnHitScripts)
    OnHitScripts.erase(retindex(OnHitScripts,script));
    SetOnHitScripts(weapon,OnHitScripts);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendEquip(who, item)
  var proc := GetProcess(Cint(GetObjProperty(item, "#Pid")));
  if(proc)
    var ev   := {};
    ev.+source := who;
    ev.+type := EVID_EQUIP;
    proc.sendevent(ev);
    return 1;
  endif

  return 0;
endfunction

//-----------------------------------------------------------------------------

function SendUnEquip(item)
  var proc := GetProcess(Cint(GetObjProperty(item, "#Pid")));
  if(proc)
    var ev   := {};
    ev.+type := EVID_UNEQUIP;
    proc.sendevent(ev);
  endif
  EraseObjProperty(item, "equipped");
endfunction

//-----------------------------------------------------------------------------

function DamageDefenderStamina(defender, attacker)

  if(RandomInt(100) < 20)
    var sdr := RandomInt(2) + 1;
    DealStaminaDamage(defender, sdr, attacker);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
// Hits
//-----------------------------------------------------------------------------

function CheckHit(Hit,attacker,defender,weapon)

  if(ADamage)
    case(Hit)
      POISON_HIT_SCRIPT:       CheckPoisonHit(attacker,defender,weapon);
      MPOISON_HIT_SCRIPT:      CheckMobPoisonHit(attacker,defender);
      MAGIC_HIT_SCRIPT:        CheckMagicHit(attacker,defender,weapon);
      EELEM_HIT_SCRIPT:        CheckEarthElemHit(defender,attacker);
      VORTEX_HIT_SCRIPT:       CheckVortexHit(defender);
      SLAYER_HIT_SCRIPT:       CheckSlayerHit(defender,weapon);
      LEECH_HIT_SCRIPT:        CheckLeechHit(defender,weapon,attacker);
    endcase
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckModArmors(attacker, defender)

  var Amt := GetTempModAmount(defender, MOD_RA);

  if(Amt > 0)
    if(Distance(attacker,defender) <= 1)
      PlayObjectCenteredEffect(defender,8902,0xa,0xa);
    else
      PlayMovingEff(defender,attacker,8902,0xa,0xa);
    endif

    PlaySoundEffect(attacker, SFX_SPELL_HARM);

    DealDamage(attacker, ToMin(CInt(ADamage * 0.10),1), defender);
    ADamage := CInt(ADamage * 0.80);
    FDamage := CInt(FDamage * 0.80);
    DecModAmount(defender, MOD_RA, Amt);
  endif

  Amt := GetTempModAmount(defender, MOD_SILVER_AR);

  if(Amt > 0)
    if(attacker.weapon.color == SILVER_COLOR)
      PlaySoundEffect(defender, SFX_SPELL_MAGIC_ARROW);
      PlayPcEffect(defender, FX_MARK_EFFECT, 1301, 0x10, 0x10, 3);
      ADamage := CInt(ADamage * 0.80);
      FDamage := CInt(FDamage * 0.80);
      DecModAmount(defender, MOD_SILVER_AR, Amt);
    endif
  endif

  Amt := GetTempModAmount(defender, MOD_MONSTER_AR);

  if(Amt > 0)
    if(attacker.isa(POLCLASS_NPC))
      CheckNecroAttackGain(defender, attacker);
      PlaySoundEffect(attacker, SFX_SPELL_MAGIC_ARROW);
      PlayPcEffect(defender, FX_HEAL_EFFECT, 1152, 0x10, 0x1A, 5);

      ADamage := CInt(ADamage * 0.80);
      FDamage := CInt(FDamage * 0.80);
      DecModAmount(defender, MOD_MONSTER_AR, Amt);
    endif
  endif

  Amt := GetTempModAmount(defender, MOD_TREE_SUPPORT);

  if(Amt > 0)
    if(attacker.isa(POLCLASS_NPC))
      if(IsTreeNear(defender.x, defender.y, defender.realm, 5))
        CheckDruidAttackGain(defender, attacker);
        PlaySoundEffect(attacker, SFX_SPELL_MAGIC_ARROW);
        PlayPcEffect(defender, FX_HEAL_EFFECT, 1424, 0x10, 0x1A, 3);

        ADamage := CInt(ADamage * 0.80);
        FDamage := CInt(FDamage * 0.80);
        DecModAmount(defender, MOD_TREE_SUPPORT, Amt);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function DecModAmount(who, Type, Amt)

  if(Amt == 1)
    UndoMod(who, Type);
  else
    SetTempModAmount(who, Type, Amt - 1);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckPoisonHit(attacker,defender,weapon)

  if(HaveChance(25 + CInt(GetEffectiveSkill(attacker,SKILLID_POISONING) / 4)))
    CheckItemPoisonCharges(weapon);
    SetPoison(defender,GetItemPoisonLvl(weapon),attacker);
    FDamage := FDamage + (GetItemPoisonLvl(weapon) * 1.5);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckMobPoisonHit(attacker,defender)

  if(RandomInt(100) >= 50)
    var poison_strength := CInt(GetObjProperty(attacker, "poison_strength"));
    if(poison_strength)
      SetPoison(defender,poison_strength,attacker);
      FDamage := FDamage + (poison_strength * 1.5);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckEarthElemHit(defender, attacker)

  DamageDefenderStamina(defender, attacker);

endfunction

//-----------------------------------------------------------------------------

function CheckMagicHit(attacker, defender, weapon)

  if(HaveChance(75))
    var Dmg := 0;
    var Mod := 0;

    if(attacker.isa(POLCLASS_NPC))
      var cfgElem := GetNpcConfig(attacker);
      Dmg := CInt(cfgElem.MagicHitLvl);
      Mod := CInt(cfgElem.MagicHitMod);

      Dmg := CInt(Dmg/2) + RandomInt(CInt(Dmg*0.75));

      if(!Dmg or !Mod)
        return;
      endif
    else
      var Charges := GetMagicItemPrefixCharges(weapon);

      if(!IsMagicItem(weapon) or !IsIdentified(weapon) or !Charges)
        return;
      endif

      if(!CheckMagicPrefixItemCharges(weapon,attacker))
        return;
      endif

      Dmg := Cint(GetMagicItemLevel(weapon) / 2);
      Mod := GetMagicItemPrefixMod(weapon);
      SetMagicItemPrefixCharges(weapon,Charges - 1);
    endif

    if(Mod == MAGICMOD_POISONDMG)
      Dmg := CInt(Dmg * 0.7);
    endif

    DoMagicHitDmg(attacker, defender, Mod, Dmg);
    FDamage := FDamage + Dmg;
  endif

endfunction

//-----------------------------------------------------------------------------

function DoMagicHitDmg(attacker, defender, mMod, byref mDmg)

  if(mMod != MAGICMOD_POISONDMG) // dla poisona nie mozna ruszac dmg, bo to level! Wyjdzie kosmiczny.
    if(defender.isa(POLCLASS_NPC))
      if(IsNpcElemental(defender, NPCELEM_WOLF))
        mDmg := mDmg * 1.5;
      endif
    else
      if(IsWearWolfMorphed(defender))
        mDmg := mDmg * 1.5;
      endif
    endif
  endif

  case(mMod)
    MAGICMOD_FIREDMG:          PlaySoundEffect( defender, SFX_SPELL_EXPLOSION );
                               PlayObjectCenteredEffect(defender, FX_EXPLODE_3, 0x7, 0x10);
                               DoMagicDamage(defender, attacker, mDmg, 5, DMGTYPE_FIRE, SPTYPE_NONE, NO_RESIST_GAIN);
    MAGICMOD_COLDDMG:          PlaySoundEffect( defender, 0x15);
                               PlayPcEffect(defender, FX_CURSE_EFFECT, 1151, 0xF, 0xE);
                               DoMagicDamage(defender, attacker, mDmg, 5, DMGTYPE_COLD, SPTYPE_NONE, NO_RESIST_GAIN);
    MAGICMOD_LIGHTDMG:         PlaySoundEffect( defender, SFX_LIGHTNING_STRIKE);
                               PlayPcEffect(defender, FX_HEAL_EFFECT, 90, 0xF, 0x8, 0x2);
                               DoMagicDamage(defender, attacker, mDmg, 5, DMGTYPE_ELECTRIC, SPTYPE_NONE, NO_RESIST_GAIN);
    MAGICMOD_POISONDMG:        
                               if(HaveChance(45))
                                 PlaySoundEffect( defender, SFX_SPELL_POISON);
                                 SetPoison(defender, mDmg, attacker);
                               endif

  endcase

endfunction

//-----------------------------------------------------------------------------

function CheckVortexHit(defender)

  var Dmg := RandomInt(CInt(GetIntelligence(defender) / 10));
  if(Dmg > 5)
    Dmg := 5;
  elseif(Dmg < 1)
    Dmg := 1;
  endif
  
  ADamage := ADamage + Dmg;
  FDamage := FDamage + Dmg;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcAdds(attacker, defender)

  var cfgElem := GetNpcConfig(attacker);

  if(cfgElem.CrushingBlow)
    if(CInt(cfgElem.CrushingBlow) > RandomInt(100))
      PlayPcEffect(defender, FX_MARK_EFFECT, 1000, 0x1F, 0x10, 0x2);
      ADamage := CInt(Adamage * 1.2);
      FDamage := CInt(Fdamage * 1.2);
      if(!Blood)
        Blood := CreateNpcBlood(defender, 40, CB_SMALL);
      endif
    endif
  endif

  if(cfgElem.DecreaseHealth)
    if(CInt(cfgElem.DecreaseHealth) > RandomInt(100))
      start_script(":combat:dechealth",{defender,ADamage});
    endif
  endif

  if(cfgElem.HitDisMount)
    if(CInt(cfgElem.HitDisMount) > RandomInt(100))
      if(GetEquipmentByLayer(defender,LAYER_MOUNT))
        if(CanMod(defender, MOD_DEFENSE) == 0)
          var Mod := GetTempModAmount(defender, MOD_DEFENSE);
          if(RandomInt(100) < Mod)
            DisMount(defender);
          endif
        else
          DisMount(defender);
        endif
      endif
    endif
  endif

  if(cfgElem.HitHealBreak)
    if(IsPlayer(defender))
      var HealBreak := SplitWords(cfgElem.HitHealBreak);
      if(CInt(HealBreak[1]) > RandomInt(100))
        PlayPcEffect(attacker, FX_CURSE_EFFECT, 52, 0x10, 0x10, 0x2);
        SetObjProperty(defender,"#HealBreak",ReadGameClock() + CInt(HealBreak[2]));
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckSlayerHit(defender,weapon)

  if(defender.isa(POLCLASS_NPC))
    if(IsNpcElemental(defender, GetObjProperty(weapon,"SlayerType")))
      ADamage := CInt(Adamage * 1.6);
      FDamage := CInt(Fdamage * 1.6);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckLeechHit(defender,weapon,attacker)

  if(RandomInt(100) < 25)
    case(GetObjProperty(weapon,"LeechType"))
      1: DealStaminaDamage(defender,5 + RandomInt(4),attacker);
      2: DealManaDamage(defender,5 + RandomInt(4),attacker);
    endcase
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckModsDmg(attacker,defender)

  var Dmg := 0;
  var WpnObj := attacker.weapon.objtype;
  var VampGain := 0;
  var HuntGain := 0;
  
  var mod := GetGlyphMod(attacker, "EffectiveShot");
  if(mod)
	SendSysMessage(attacker, "Witamy w krainie efektownego ciosu.", FONT_NORMAL, RED_COLOR);
	start_script(":glyphs:specials/effectiveShot", {attacker, defender});
  endif

  if(HaveChance(75))
    Dmg := GetTempModAmount(attacker,MOD_POISON_DMG);
    if(Dmg)
      CheckWearWolfAttackGain(attacker, defender);
      DoMagicHitDmg( attacker, defender, MAGICMOD_POISONDMG, Dmg );
      FDamage := FDamage + Dmg;
    endif

    Dmg := GetTempModAmount(attacker,MOD_FIRE_DMG);
    if(Dmg)
      CheckWearWolfAttackGain(attacker, defender);
      DoMagicHitDmg( attacker, defender, MAGICMOD_FIREDMG, Dmg );
      FDamage := FDamage + Dmg;
      HuntGain := 1;
    endif
    
    Dmg := GetTempModAmount(attacker,MOD_EXPLO_DMG);
    if(Dmg)
      DoMagicHitDmg( attacker, defender, MAGICMOD_FIREDMG, Dmg );

      foreach mob in ListMobilesNearLocation(defender.x, defender.y, defender.z, 1, defender.realm)
        if(defender != mob and attacker != mob and CanDamage(attacker, mob))
          DoMagicHitDmg( attacker, mob, MAGICMOD_FIREDMG, Dmg );
        endif
      endforeach

      FDamage := FDamage + Dmg;
      HuntGain := 1;
    endif

    Dmg := GetTempModAmount(attacker,MOD_COLD_DMG);
    if(Dmg)
      VampGain := 1;
      HuntGain := 1;
      DoMagicHitDmg( attacker, defender, MAGICMOD_COLDDMG, Dmg );
      FDamage := FDamage + Dmg;

      if(defender.isa(POLCLASS_NPC) and CanMod(defender, MOD_SLOWDOWN) and CInt(GetObjProperty(defender, "#LastColdSlow")) < ReadGameClock())
        DoMod(defender, MOD_SLOWDOWN, Dmg*2, MinMax(2, 6, CInt(2+Dmg/3)));
        SetObjProperty(defender, "#LastColdSlow", ReadGameClock() + 15);
      endif
    endif

    Dmg := GetTempModAmount(attacker,MOD_LIGHT_DMG);
    if(Dmg)
      DoMagicHitDmg( attacker, defender, MAGICMOD_LIGHTDMG, Dmg );
      FDamage := FDamage + Dmg;
    endif
    
    if(defender.isa(POLCLASS_NPC))

      Dmg := GetTempModAmount(attacker, MOD_UNDEAD_SLAY);
      if(Dmg)
        if(IsNpcElemental(defender, NPCELEM_UNDEAD))
          CheckPaladinAttackGain(attacker, defender);
          Dmg     := MinMax(Dmg, -50, 100) * 0.01;
          Dmg     := CInt(ADamage * Dmg);
          ADamage := ADamage + Dmg;
          FDamage := FDamage + Dmg;
        endif
      endif

      Dmg := GetTempModAmount(attacker, MOD_DAEMON_SLAY);
      if(Dmg)
        if(IsNpcElemental(defender, NPCELEM_DAEMON))
          CheckPaladinAttackGain(attacker, defender);
          Dmg     := MinMax(Dmg, -50, 100) * 0.01;
          Dmg     := CInt(ADamage * Dmg);
          ADamage := ADamage + Dmg;
          FDamage := FDamage + Dmg;
          HuntGain := 1;
        endif
      endif

      Dmg := GetTempModAmount(attacker, MOD_ORDER_SLAY);
      if(Dmg)
        if(IsGoodNpc(defender))
          CheckDarkKnightAttackGain(attacker, defender);
          Dmg     := MinMax(Dmg, -50, 100) * 0.01;
          Dmg     := CInt(ADamage * Dmg);
          ADamage := ADamage + Dmg;
          FDamage := FDamage + Dmg;
        endif
      endif

    else

      Dmg := GetTempModAmount(attacker, MOD_ORDER_SLAY);
      if(Dmg)
        if(!defender.murderer)
          var lAddDmg := (Dmg / 12 + RandomInt(3));
          ADamage := ADamage + lAddDmg;
          FDamage := FDamage + lAddDmg;
        endif  
      endif

    endif

  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[WpnObj].FireBlood and !IsMagicItem(attacker.weapon) and !attacker.shield)
    Dmg := GetTempModAmount(attacker, MOD_FIRE_BLOOD);

    if(Dmg and Distance(attacker, defender) <= 2 and CanMod(defender, MOD_PARA) and RandomInt(100) < 80)
      var Pid := GetProc(GetObjProperty(defender, "#FireBloodPid"));
      if(!Pid)
        var fbTime := CInt(GetObjProperty(defender, "#LastFireBloodTime"));
        if(fbTime + 30 < ReadGameClock())
          start_script(":combat:fireblood", {defender,attacker});
          VampGain := 1;
          PlaySoundEffect(defender, SFX_FIRE_STRIKE);
          PlayPcEffect(defender, FX_SPARK_EFFECT, 31, 0x10, 0x10, 5);
          if(IsPlayer(defender))
            SendSysMessage(defender, "Palacy bol przeszywa twe cialo!", FONT_NORMAL, COLOR_RED);
          endif
          DecModAmount(attacker, MOD_FIRE_BLOOD, Dmg);
        endif
      endif
    endif
  endif

  Dmg := GetTempModAmount(attacker, MOD_ONHIT_AR);
  if(Dmg)
    if(Dmg < GetModAmount(attacker, MOD_ONHIT_AR))
      SetTempModAmount(attacker, MOD_ONHIT_AR, Dmg + 1);
      VampGain := 1;
      attacker.ar_mod := attacker.ar_mod + 1;
      PlaySoundEffect(attacker, SFX_FOCUSATTACK);

      if((Dmg + 1) == GetModAmount(attacker, MOD_ONHIT_AR))
        PlayPcEffect(attacker, FX_FLAMESTRIKE, 36, 0x10, 0x1F, 8);
      else
        PlayPcEffect(attacker, FX_BLESS_EFFECT, 36, 0x10, 0x10, 8);
      endif
    endif
  endif

  if(RandomInt(100) < 80)
    if(CanMod(attacker, MOD_LIFE_STOLEN) == 0)
      VampGain := 1;
      var ModAmt := Max(CInt(ADamage * (GetModAmount(attacker, MOD_LIFE_STOLEN) * 0.01)), 1);
      HealMobile(attacker, ModAmt, attacker);
      PlayPcEffect(attacker, FX_HEAL_EFFECT, 31, 0x10, 0x10, 7);
      PlayPcEffect(defender, FX_MARK_EFFECT, 31, 0x10, 0x10, 7);
      PlaySoundEffect(attacker, SFX_SPELL_MANA_VAMPIRE);
    endif
  endif

  if(RandomInt(100) < 80)
    if(CanMod(attacker, MOD_MANA_STOLEN) == 0)
      VampGain := 1;
      var ModAmt := Max(CInt(ADamage * (GetModAmount(attacker, MOD_MANA_STOLEN) * 0.01)), 1);
      HealMana(attacker, ModAmt, attacker);
      DealManaDamage(defender, ModAmt, attacker);
      PlayPcEffect(attacker, FX_HEAL_EFFECT, 86, 0x10, 0x10, 7);
      PlayPcEffect(defender, FX_MARK_EFFECT, 86, 0x10, 0x10, 7);
      PlaySoundEffect(attacker, SFX_SPELL_MANA_VAMPIRE);
    elseif(CanMod(attacker, MOD_MANA_DMG) == 0)
      HuntGain := 1;
      var ModAmt := Max(CInt(ADamage * (GetModAmount(attacker, MOD_MANA_DMG) * 0.01)), 1);
      DealManaDamage(defender, ModAmt, attacker);
      PlayPcEffect(attacker, FX_HEAL_EFFECT, 86, 0x10, 0x10, 7);
      PlayPcEffect(defender, FX_MARK_EFFECT, 86, 0x10, 0x10, 7);
      PlaySoundEffect(attacker, SFX_SPELL_MANA_DRAIN);
    endif
  endif

  if(CanMod(defender, MOD_RETORT) == 0)
    Dmg := GetTempModAmount(defender, MOD_RETORT);
    SetTempModAmount(defender, MOD_RETORT, ToMax(Dmg + ADamage, 100));
    PlayPcEffect(defender, FX_HEAL_EFFECT, 999, 0x10, 0xD, 32);
  endif

  Dmg := GetTempModAmount(attacker, MOD_RETORT);

  if(Dmg)
    if(Dmg > RandomInt(100))
      Dmg := ADamage * (ToMax(Dmg, 100) * 0.01);
      ADamage := ADamage + Dmg;
      SetTempModAmount(attacker, MOD_RETORT, 1);
      PlayPcEffect(defender, FX_MARK_EFFECT, 999, 0x10, 0x10, 32);
      PlaySoundEffect(attacker, SFX_FOCUSATTACK);
      CheckKensaiAttackGain(attacker, defender);
      CheckWearWolfAttackGain(attacker, defender);
    endif
  endif

  Dmg := GetTempModAmount(attacker, MOD_STAM_DMG);

  if(Dmg)
    if(RandomInt(100) < 60)
      Dmg := CInt(Dmg * ((100 - RandomInt(30)) * 0.01));
      if(defender.isa(POLCLASS_NPC))
        Dmg := CInt(Dmg / 2);
      endif
      DealStaminaDamage(defender, Dmg, attacker);
      PlayPcEffect(defender, FX_MARK_EFFECT, 1324, 0x10, 0x10, 0);
    endif
  endif

  Dmg := GetTempModAmount(attacker, MOD_DISARM);

  if(Dmg and IsPlayer(defender))
    if(RandomInt(100) < Dmg)
      DisarmPlayer(defender);
      PlayPcEffect(defender, FX_HEAL_EFFECT, 1340, 0x10, 0xA, 0);
    endif
  endif
  
  Dmg := GetTempModAmount(attacker, MOD_STUPOR);

  if(Dmg and GetEquipmentByLayer(defender, LAYER_MOUNT) and !IsMagicItem(attacker.weapon) and !ItemCfg[WpnObj].poisonable)
    var StuporChance := 20;
    if(WpnObj >= UOBJ_ART_START and WpnObj <= UOBJ_ART_END)
      StuporChance += 25;
    else
      StuporChance += CheckItemQuality(attacker.weapon);
    endif
    if(RandomInt(100) < StuporChance - CInt(StuporChance * GetModAmount(defender, MOD_DISMOUNT_RES) * 0.01))
      Dismount(defender);
      PlayHitAnim(defender);
      DoMod(defender, MOD_PARA, 1, 1+RandomInt(2));
      SetTempModAmount(attacker, MOD_STUPOR, Dmg - 1);
    endif
  endif
  
  Dmg := GetTempModAmount(attacker, MOD_2H_DMG);

  if(Dmg)
    if(RandomInt(100) < Dmg and ItemCfg[WpnObj].TwoHanded)
      ADamage := CInt(ADamage * 1.25);
      DealStaminaDamage(attacker, 2 + RandomInt(3), attacker);
      PlayPcEffect(defender, FX_MARK_EFFECT, 1150, 0x10, 0x10, 64);
      DealStaminaDamage(defender, 10 + RandomInt(3 + CInt(Dmg/20)), attacker);
    endif
  endif

  if(VampGain)
    CheckVampireAttackGain(attacker, defender);
  endif

  if(HuntGain)
    CheckHunterAttackGain(attacker, defender);
  endif

endfunction

//-----------------------------------------------------------------------------

function PlayFightEffect(who, weapon, x, y, z, Realm, Hitted)

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var Anim      := ANIM_ATTACK_1HAND_WIDE;
  var HitSound  := CInt(ItemCfg[weapon.objtype].HitSound);
  var MissSound := CInt(ItemCfg[weapon.objtype].MissSound);
  
    var mod := GetGlyphMod(who, "EffectiveShot");
  if(mod)
	SendSysMessage(who, "Witamy w krainie efektownego ciosu.", FONT_NORMAL, RED_COLOR);
	start_script(":glyphs:specials/effectiveShot", {who, Hitted});
  endif

  if(ItemCfg[weapon.objtype].Anim)
    Anim := CInt(ItemCfg[weapon.objtype].Anim);
  endif

  if(ItemCfg[weapon.objtype].ProjecTile)
    var AmmoType := CInt(ItemCfg[weapon.objtype].ProjectileAnim);
    PlayMovEffXYZ( who.x, who.y, who.z+9, x, y, z+5, AmmoType, 0xA, 0, 0, Realm);
  endif
  
  var cfgElem := GetNpcConfig(who);

  if(who.isa(POLCLASS_NPC))

    if(Cint(cfgElem.AttackHitSound))
      HitSound  := Cint(cfgElem.AttackHitSound);
    endif
    if(Cint(cfgElem.AttackMissSound))
      MissSound := Cint(cfgElem.AttackMissSound);
    endif

  endif

  if(!IsHuman(who))
    var HitAnimArr := {0x5,0x6,0xc};
    if(cfgElem.HitAnim)
      HitAnimArr := SplitWords(cfgElem.HitAnim);
    endif

    Anim := CInt(HitAnimArr[RandomInt(HitAnimArr.size())+1]);
    if(!Anim)
      Anim := CInt(HitAnimArr[1]);
    endif
  endif

  PlayAction(who, Anim, 1);

  if(Hitted)
    PlaySoundEffect(who,HitSound);
  else
    PlaySoundEffect(who,MissSound);
  endif

endfunction

//-----------------------------------------------------------------------------
