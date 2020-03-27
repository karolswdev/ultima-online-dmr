//-----------------------------------------------------------------------------

include "include/portals";
include ":blades:corpse";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na szukanie trupow w loopie wyrazona w procentach
// 4 - zasieg specjala - czyli zasieg szukania trupow
// przyklad: CorpseEater 30 50 15 20
//-----------------------------------------------------------------------------

function CheckCorpseEater()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_CORPSEEATER][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_CORPSEEATER][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[CorpseEater] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_CORPSEEATER][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_CORPSEEATER, 5))
    return PerformCorpseEater();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function LocateCorpse()

  var minDist := Specs[NSPEC_CORPSEEATER][4]+2;
  var Corpse := 0;

  foreach item in ListItemsNearLocationOfType(me.x, me.y, me.z,Specs[NSPEC_CORPSEEATER][4], UOBJ_CORPSE, me.realm)
    
    if(GetBaseSpecialAbility(item, NPC_ABLID_ENDURANCE) < 1000)
      var Dist := Coordist(item.x, item.y, me.x, me.y);
      if(Dist < minDist or !Corpse)
        Corpse := Item;
        minDist := Dist;
      endif
    endif
  endforeach

  return Corpse;

endfunction

//-----------------------------------------------------------------------------

function PerformCorpseEater()

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[CorpseEater] "+GetName(me)+": Perform...");
  endif
  
  var Corpse := LocateCorpse();

  if(!Corpse)
    syslog("[CorpseEater] "+GetName(me)+": Cant find corpse!");
    return;
  endif

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[CorpseEater] "+GetName(me)+": Perform on corpse: "+GetName(Corpse));
  endif
  
  var lAttackDelayed := SetDelayedAttack(me, 2);

  ev := struct;
  ev.+type := EVID_RUN;
  ev.+source :=  LocStruct();
  ev.source.x := Corpse.x;
  ev.source.y := Corpse.y;
  ev.source.z := Corpse.z;
        
  if(NpcMoveTo(ev, NMT_SMART) > 0)
    if(!Corpse or Distance(me, Corpse) > 1)
      Say("*grrhrh*");
      Say("*nie czuje juz zwlok*");
    else
      TurnToward(Corpse);
      PerformAction(me, ANIM_ATTACK_2HAND_DOWN);
      PlaySoundEffect(me, 0x3D);
      HealMobile(me, CInt(GetMaxHp(me)*0.08), me);
      PlayPcStEffect(Corpse.x, Corpse.y, Corpse.z, Corpse.realm, FX_SMOKE, 1400, 16, 16, 4);
      EatCorpse(Corpse);
      SetObjProperty(me, "#CorpseAte", CInt(GetObjProperty(me, "#CorpseAte"))+1);
      sleepms(1500);
    endif
  else
    Say("*grrhrh*");
    Say("*czuje gdzies zwloki*");
  endif

  SetNpcSpecPerforming(me, NSPEC_CORPSEEATER, 1);

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_CORPSEEATER);

  return 1;

endfunction

//-----------------------------------------------------------------------------
