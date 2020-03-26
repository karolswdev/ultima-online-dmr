//-----------------------------------------------------------------------------

function process_combat_event()

  do_damage();
  if((GetHp(me) * 100/GetMaxHp(me)) < GetFleePoint())
    flee();
  endif
  
endfunction

//-----------------------------------------------------------------------------

function post_combat()

  DisableCombatEvents();
  EnableMainEvents();
  sleep(1);
  look_around();
  
endfunction

//-----------------------------------------------------------------------------

function prepare_for_fight()

  DisableMainEvents();
  EnableCombatEvents();
  
endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  if(((GetHp(me) * 100) / GetMaxHp(me)) >= GetFleePoint())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function do_damage()

  if(IsPlayer(opponent) and RandomInt(10) == 5 and Distance(me, opponent) < 3 and CheckLineOfSight(me, opponent))
    PrintTextAbovePrivate(me, "*"+me.name+" pluje na Ciebie!*", opponent);
    if(!HasShield())
      var dmg    := RandomInt(2);
      if(dmg)
        var things := ListEquippedItems( opponent );
        var thing  := things[ CInt(RandomInt(len(things))+1) ];
        if ( CanEat(thing) )
          SendSysMessage(opponent, "zracy plyn niszczy "+ thing.desc, FONT_NORMAL,COLOR_RED);
          CheckToolWear(opponent, thing, -1, 1000);
          if ((GetItemHp(thing) <= 0))
            SendSysMessage(opponent, GetName(thing) + " zostal zniszczony!", FONT_NORMAL,COLOR_RED);
            DestroyItem(thing);
          endif
        endif
      endif
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function CanEat(item)

  if ( ( item.objtype >= 0x2030 ) && (item.objtype <= 0x2060) )
    return 0;
  endif
  if ( (item.objtype == 0x0e75) || (item.objtype == UOBJ_MOUNT ) )
    return 0;
  endif
  if (!item.maxhp)
    return 0;
  endif
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function HasShield()

  var shield := GetEquipmentByLayer( opponent, LAYER_HAND2 );
  if ( (shield.objtype == 0x1bc3) or (shield.objtype == 0x1bc4) )
    SendSysMessage(opponent,"Tarcza zablokowala zracy plyn!", FONT_NORMAL, COLOR_GREEN);
    return 1;
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------
