//-----------------------------------------------------------------------------

function IsTownGuard(Npc)

  if(Npc.script["townguard"])
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SeekInformTownGuard(who, Text, Gmr := 0)

  __ParamUnused(Text);
  var Inf := 0;

  if(!who)
    return 0;
  endif

  if(Gmr)
    Gmr := {"Gmr", Gmr};
  endif

  foreach Npc in ListMobilesNearLocation(who.x,who.y,who.z,16,who.realm)
    if(Npc.isa(POLCLASS_NPC))
      if(IsTownGuard(Npc))
        SendChEvent(Npc,who,EVID_ENGAGED,Gmr);
        Inf := Npc;
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
      GMR_STEALING:       GmrTxt := "Kradziez";
      GMR_ATTACKED:       GmrTxt := "Atak na jakiegos Npca";
      GMR_PLAYER_ATTACK:  GmrTxt := "Atak na gracza";
      GMR_INMEMORY:       GmrTxt := "Pamietany";
      
      GMR_CITYCHASING:    var GChase := GetObjProperty(who, PROP_GUARD_CHASE);
                          if(GChase.Flags & GDCF_ALLCITIES)
                            GmrTxt := "Scigany we wszystkich miastach";
                          else
                            GmrTxt := "Scigany w "+FindCity(who);
                          endif

      GMR_NPCATTACKED:    GmrTxt := "Atak na Npca";
      GMR_MAGIC_CAST:     GmrTxt := "Czarowanie w miescie";
      default:            GmrTxt := "Brak";
    endcase

    SetCriminal(who, 1, "SeekInformTownGuard", "Wezwano straznikow z powodu: "+GmrTxt+", ale straznikow nie bylo.");
  endif

  return Inf;

endfunction

//-----------------------------------------------------------------------------
