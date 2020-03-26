//-----------------------------------------------------------------------------

include "include/portals";
include "include/items/temp_storage";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna pluc trupami
// 3 - szansa na rzut w loopie
// 4 - czas przygotowania do wyplucia w sec.
// ilosc trupow zalezy natomiast od specjala CorpseEater.
// przyklad: CorpseSpitter 30 50 4
//-----------------------------------------------------------------------------

function CheckCorpseSpitter()

  var Count := GetObjProperty(me, "#CorpseAte");

  if(!Count)
    return 0;
  endif

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_CORPSESPITTER][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_CORPSESPITTER][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[CorpseSpitter] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_CORPSESPITTER][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  var Res := 1;

  if(CheckNpcSpecDelay(NSPEC_CORPSESPITTER, 5))
    Res := PerformCorpseSpitter();
  endif
  
  EraseDelayedAttack(me);
  EraseNpcSpecPerforming(me, NSPEC_CORPSESPITTER);

  return Res;

endfunction

//-----------------------------------------------------------------------------

function PerformCorpseSpitter()

  var i;
  var Secs := Specs[NSPEC_CORPSESPITTER][4];

  SetNpcSpecPerforming(me, NSPEC_CORPSESPITTER, 1);

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*prze¿uwa*"));
         PlaySoundEffect(me, 0x1CD);
         PerformAction(me, ANIM_CAST_NPC);
    endcase

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_CORPSESPITTER))
      PerformNpcSpecialBreak();
      return -1;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_CORPSESPITTER))
      PerformNpcSpecialBreak();
      return -1;
    endif
    
  endfor
  
  var Mob := FindSpecFarestHostile();
  
  if(!Mob)
    return 0;
  endif
 
  var Count := CInt(GetObjProperty(me, "#CorpseAte"));

  if(Count > 0)
    SetObjProperty(me, "#CorpseAte", Count - 1);
  else
    EraseObjProperty(me, "#CorpseAte");
  endif

  while(Count < 10 and Distance(me, Mob) < 5)
    RunAwayFrom(Mob);
    Count += 1;
  endwhile

  PerformAction(me, 0xa);
  PlaySoundEffect(me, 0x5BE);
  PlayMovEffXYZ( me.x, me.y, me.z+15, Mob.x, Mob.y, Mob.z, 0x1B0D, 8, 0, 0, me.realm );
  sleepms(500);
  PlayPcEffect(Mob, FX_EXPLODE_1, 1153, 10, 20, 2);
  DealDamage(Mob, CInt(GetMaxHp(Mob)*0.2)+RandomInt(5), me);
  var Item := PullTempItemAtLoc(Mob.x, Mob.y, Mob.z, 0x1B0D, Mob.realm);
  ReturnTempItemDelayed(Item, 30);
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
