//-----------------------------------------------------------------------------

include ":eventcore:finding";
include ":tourney:tourney";

//-----------------------------------------------------------------------------

const PVPAREA_LEFT_DELAY     := 900; // czas potrzebny na ochloniecie po wyjsciu z pvp area.
const PVPACCEPT_DELAY        := 1800; // pol godziny czasu, na walke do pierwszej smierci

//-----------------------------------------------------------------------------

function HaveAcceptedFight(who, Attacker)

  var Data := CInt(GetObjProperty(who, "#AcceptedFight_"+Attacker.serial));
  var AttackerData := CInt(GetObjProperty(Attacker, "#AcceptedFight_"+who.serial));

  if(Data and AttackerData)
    if(((Data + PVPACCEPT_DELAY) > ReadGameClock()) and (AttackerData + PVPACCEPT_DELAY) > ReadGameClock())
      return 1;
    endif
  endif

  if(IsInEnemyGuild(who, Attacker))
    return 2;
  elseif(IsInEnemyGuildHelped(who, Attacker))
    return 3;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInEnemyGuildHelped(who, Attacker)

  foreach PropName in GetObjPropertyNames(who)
    if(substr(PropName, 1, 12) == "#AttackHelp_")
      if(GetObjProperty(who, PropName) + PVPACCEPT_DELAY > ReadGameClock())
        var Serial := substr(PropName, 13, len(PropName)-12);
        var Player := FindPlayerBySerial(Serial);

        if(Player and IsInEnemyGuild(Player, Attacker))
          return 1;
        endif
      else
        EraseObjProperty(who, PropName);
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetFightHelper(who, Helped)

  if(IsPlayer(who) and IsPlayer(Helped) and who != Helped)
    SetObjProperty(who, "#AttackHelp_"+Helped.serial, ReadGameClock());
  endif

endfunction

//-----------------------------------------------------------------------------

function IsOrderGuarded(Victim, Attacker)

  if(Victim.murderer or Victim.criminal or IsInEnemyGuild(Victim, Attacker))
    return 0;
  endif

  var EvID := FindEventAreaByLoc(Victim);

  if(EvID)
    return 0;
  elseif(Victim.realm == REALM_BRITANNIA and IsInTourneyIsland(Victim.x, Victim.y))
    return 0;
  endif

  var Delay := CInt(GetObjProperty(Victim,"#WasStealing"));
  if(Delay and Delay+900 > ReadGameClock())
    return 0;
  endif

  var Time := CInt(GetObjProperty(Victim, "#PvpAreaLeft"));

  if(Time and Time + PVPAREA_LEFT_DELAY > ReadGameClock())
    return 0;
  endif

  if(!IsPersonInPvpArea(Victim))
    if(!HaveAcceptedFight(Victim, Attacker))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function RecheckAttackFlag(Attacker, Victim)

  if(HaveAcceptedFight(Victim, Attacker))
    return;
  endif

  if(!(Attacker.murderer or Attacker.criminal) and (Victim.murderer or Victim.criminal)) 
   // jesli atakujacy jest blue a ofiara murd/crim, to wtedy sprawdzamy czy zaatakowal murd pierwszy - jak nie to ustawiamy flage .pvp
    if(!GetObjProperty(Victim, "#AcceptedFight_"+Attacker.serial)) 
      SetObjProperty(Victim, "#AcceptedFight_"+Attacker.serial, ReadGameClock());
      SetObjProperty(Attacker, "#AcceptedFight_"+Victim.serial, ReadGameClock());
    endif
  else
    // ten ktos zaatakowal pierwszy, wiec ustawiamy flage tylko na nim, aby dostal klatwe i mozna bylo zglosic chama!
    SetObjProperty(Attacker, "#AcceptedFight_"+Victim.serial, ReadGameClock());
  endif

endfunction 

//-----------------------------------------------------------------------------

function IsPersonInPvpArea(who)

  return CInt(GetObjProperty(who, "PvpArea"));

endfunction

//-----------------------------------------------------------------------------
// Handling
//-----------------------------------------------------------------------------

function HandleEnterPvpArea(who)

  SetObjProperty(who, "PvpArea", ReadGameClock());
  EraseObjProperty(who, "#PvpAreaLeft");

endfunction

//-----------------------------------------------------------------------------

function HandleLeavePvpArea(who)

  if(GetObjProperty(who, "PvpArea"))
    EraseObjProperty(who, "PvpArea");
    SetObjProperty(who, "#PvpAreaLeft", ReadGameClock());
  endif

endfunction

//-----------------------------------------------------------------------------

function HandleFightAcceptance(Victim, Attacker)

  SetObjProperty(Victim, "#AcceptedFight_"+Attacker.serial, ReadGameClock());
  SetObjProperty(Attacker, "#AcceptedFight_"+Victim.serial, ReadGameClock());

endfunction

//-----------------------------------------------------------------------------

function CheckDeathForPvp(Victim)

  foreach PropName in GetObjPropertyNames(Victim)
    if(substr(PropName, 1, 15) == "#AcceptedFight_")
      var Serial := substr(PropName, 16, len(PropName)-15);
      var Player := FindPlayerBySerial(Serial);

      if(Player)
        EraseObjProperty(Player, "#AcceptedFight_"+Victim.serial);
      endif

      EraseObjProperty(Victim, PropName);
    elseif(substr(PropName, 1, 12) == "#AttackHelp_")
      EraseObjProperty(Victim, PropName);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GetAcceptedFights(Victim)

  var Fights := array;

  foreach PropName in GetObjPropertyNames(Victim)
    if(substr(PropName, 1, 15) == "#AcceptedFight_")
      var Serial := substr(PropName, 16, len(PropName)-15);
      var Player := FindPlayerBySerial(Serial);

      if(Player and HaveAcceptedFight(Victim, Player))
        Fights.Append(Player);
      endif

    endif
  endforeach

  return Fights;

endfunction

//-----------------------------------------------------------------------------
