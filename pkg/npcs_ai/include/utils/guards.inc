//-----------------------------------------------------------------------------

function IsTownGuard(Npc)

  if(Npc.script["townguard"])
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function InformGuards(Npc, who, City, Text, Gmr := 0)

  var Inf := 0;

  if(!who)
    return 0;
  endif

  if(Gmr)
    Gmr := {"Gmr", Gmr};
  endif

  foreach Mob in ListMobilesNearLocation(who.x,who.y,who.z,16,who.realm)
    if(Mob.isa(POLCLASS_NPC))
      if(IsTownGuard(Mob))
        SendChEvent(Mob,who,EVID_ENGAGED,Gmr);
        Inf := 1;
      endif
    endif
  endforeach

  if(!Inf)
    var GmrTxt;
    case(Gmr[2])
      GMR_CHASING:        GmrTxt := "Scigany przez innego straznika";
      GMR_ANIMAL_INB:     GmrTxt := "Zwierzak w budynku";
      GMR_NOT_MURD:       GmrTxt := "Nie morderca!?";
      GMR_TO_WEAK_MURD:   GmrTxt := "Za slaby morderca!?";
      GMR_MURD:           GmrTxt := "Morderca";
      GMR_CRIMINAL:       GmrTxt := "Kryminalista";
      GMR_PLAYER_ATTACK:  GmrTxt := "Atak na gracza";
      GMR_STEALING:       GmrTxt := "Kradziez";
      GMR_ATTACKED:       GmrTxt := "Atak na jakiegos Npca";
      GMR_INMEMORY:       GmrTxt := "Pamietany";
      
      GMR_CITYCHASING:    var GChase := GetObjProperty(who, PROP_GUARD_CHASE);
                          if(GChase.Flags & GDCF_ALLCITIES)
                            GmrTxt := "Scigany we wszystkich miastach";
                          else
                            GmrTxt := "Scigany w "+City;
                          endif

      GMR_NPCATTACKED:    GmrTxt := "Atak na Npca";
      default:            GmrTxt := "Brak";
    endcase

    SetCriminal(who, 1, "InformGuards", "Npc "+Npc.name+" ["+Npc.serial+"] chcial wezwac straznikow z powodu: "+GmrTxt+", ale straznikow nie bylo.");
  elseif(Npc.isa(POLCLASS_NPC))
    PrintTextAbove(Npc, Text);
  endif

  return Inf;

endfunction

//-----------------------------------------------------------------------------

function SetGuardChase(who, City, Flags := 0)

  var GuardChase := GetObjProperty(who, PROP_GUARD_CHASE);
  if(!GuardChase.Cities)
    GuardChase         := struct;
    GuardChase.+Cities := dictionary;
    GuardChase.+Flags  := 0;
    GuardChase.+aTime  := 0;
  endif

  if(Flags & GDCF_ALLCITIES)
    SetBitOn(GuardChase.Flags, GDCF_ALLCITIES);
    GuardChase.aTime := ReadGameClock();
  endif

  GuardChase.Cities[City] := ReadGameClock();
  SetObjProperty(who, PROP_GUARD_CHASE, GuardChase);

endfunction

//-----------------------------------------------------------------------------

function IsGuardChasing(who, City)

  if(who.dead or who.hidden or who.concealed)
    return 0;
  endif

  var Chase := GetObjProperty(who, PROP_GUARD_CHASE);

  if(Chase.Flags & GDCF_ALLCITIES)
    if(Chase.aTime + GUARD_REMEMBER_TIME > ReadGameClock())
      return 1;
    else
      Chase.aTime := 0;
      SetBitOff(Chase.Flags, GDCF_ALLCITIES);
      SetObjProperty(who, PROP_GUARD_CHASE, Chase);
    endif
  elseif(Chase.Cities.exists(City))
    if(Chase.Cities[City] + GUARD_REMEMBER_TIME > ReadGameClock())
      return 1;
    else
      Chase.Cities.erase(City);
      SetObjProperty(who, PROP_GUARD_CHASE, Chase);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
