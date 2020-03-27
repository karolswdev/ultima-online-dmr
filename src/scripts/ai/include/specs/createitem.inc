//-----------------------------------------------------------------------------

include "include/portals";
include "include/items/temp_storage";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna tworzyc pod soba itemy
// 3 - szansa na create
// 4 - zasieg specjala
// 5 - objtype itema do stworzenia
// 6 - damage jaki dostanie gosc, gdy wejdzie na item, Dmg + RandomInt(Dmg/2)

// przyklad: CreateItem 15 50 6 20 0xa3c0 20

//-----------------------------------------------------------------------------

function CheckItemSpecCreation()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_CREATEITEM][2])
    return 0;
  endif

  var HpMod := CInt((50-HpPerc*100)/4);
  var Chance := Specs[NSPEC_CREATEITEM][3] + HpMod + RandomInt(CInt(HpMod/2));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[ItemSpecCreation] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_CREATEITEM][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_CREATEITEM))
    return PerformItemSpecCreation();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformItemSpecCreation()

  var Loc;
  var bFound := 0;
  var Range := Specs[NSPEC_CREATEITEM][4];
  var Count := 0;

  syslog("[ItemSpecCreation] "+GetName(me)+": Create item!");

  while(!bFound and Count < 5)
    Loc := FindStandableLocNear(me, Range);

    if(Loc)
      bFound := 1;

      foreach Item in ListItemsAtLocation(Loc.x, Loc.y, Loc.z, Loc.realm)
        if(Item.objtype == Specs[NSPEC_CREATEITEM][5])
          bFound := 0;
        endif
      endforeach
    endif

    Count += 1;
  endwhile

  if(!bFound)
    LogError("[ItemSpecCreation] "+me.npctemplate+"/"+me.serial+" nie udalo sie znalezc odpowiedniej lokacji do spawnu itema.");
    return 0;
  endif

  ev := struct;
  ev.+type := EVID_RUN;
  ev.+source :=  Loc;
        
  if(NpcMoveTo(ev, NMT_SMART | NMT_NO_BREAK) > 0)
    sleepms(200);
    PlayAction(me, ANIM_STUMBLE);
    sleepms(800);
    PlayPcEffect(me, FX_MARK_EFFECT, 1421, 0xF, 0xF, 2);
    sleepms(200);

    var item := CreateItemAtLoc(me.x, me.y, me.z, Specs[NSPEC_CREATEITEM][5], 1, me.realm);

    if(!Item)
      LogError("[ItemSpecCreation] "+me.npctemplate+"/"+me.serial+" nie udalo sie stworzyc itema!! Objtype: "+hex(Specs[NSPEC_CREATEITEM][5]));
    else
      SetObjProperty(Item, "Creator", me.serial);
      SetObjProperty(Item, "Damage", Specs[NSPEC_CREATEITEM][6]);
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
