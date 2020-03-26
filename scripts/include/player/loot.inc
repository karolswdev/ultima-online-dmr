//-----------------------------------------------------------------------------

function CanPlayerLoot(who, Owner)

  if(Owner == who)
    return 1;
  elseif(IsInAllyGuild(who, Owner))
    return 2;
  elseif(IsFriend(Owner, who, NO_PARTY_CHECK))
    return 2;
  elseif(IsInSameParty(Owner, who) and Owner.partycanloot)
    return 2;
  elseif(IsInEnemyGuild(who, Owner))
    return 3;
  endif

  return 0;

endfunction
    
//-----------------------------------------------------------------------------

function IsAllowedToLootNpc(who, Corpse)

  var Table := GetNpcAggroTable(Corpse);
  var bCan := 1; // domyslnie mozna grabic, no chyba ze znajdzie ludka co ma wieksze aggro!

  if(Table and Table.size())
    foreach Elem in Table
      if(Elem[2] >= 1 and (Elem[1] - ReadGameClock()) < 3600) // godzina czasu wystarczy
        var Player := FindPlayerBySerial(_Elem_iter);
        
        if(IsTamed(Player))
          Player := GetMaster(Player);
        endif
        
        if(IsPlayer(Player))
          if(IsInSameParty(Player, who) or Player == who)
            return 1;
          else
            bCan := 0; // jak znalazl jakiegos ludka co nie pasuje, to znaczy ze nie moze grabic!
          endif
        endif
      endif
    endforeach
  endif

  return bCan;

endfunction
    
//-----------------------------------------------------------------------------
