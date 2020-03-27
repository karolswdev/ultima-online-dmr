//--------------------------------------------------------------------------

const QT_SEEK_EXNPC_TRIES   := 12;

//--------------------------------------------------------------------------

const QGLOCF_MURDER           := 0x00000001;
const QGLOCF_GOOD             := 0x00000002;
const QGLOCF_NEUTRAL          := 0x00000004;

//--------------------------------------------------------------------------

function SeekNpcForQuest(LocGroup, Targ, ExLocs, Except)

  var Npcs      := array;
  var Res       := error;
  var Tries     := 0;
  var Realm     := 0;

  var x, y, tx, ty, LocRange;

  while(Npcs.size() == 0 and (Tries <= QT_SEEK_EXNPC_TRIES))

    LocRange := GenQuestLocFromGroup(LocGroup);

    if(!LocRange)
      Res.errortext := "Nie udalo sie okreslic zasiegu lokacji.";
      return Res;
    endif

    Realm := GetRealmById(LocRange[5]);
    if(!Realm)
      Res.errortext := "Bledny ID krainy w lokacji, RlmID: ["+LocRange[5]+"].";
      return Res;
    endif
    
    for(x:=LocRange[1];x<LocRange[3];x:=x+128)
      for(y:=LocRange[2];y<LocRange[4];y:=y+128)
        tx := x + 128;
        ty := y + 128;
        if(tx > LocRange[3])
          tx := LocRange[3];
        endif
        if(ty > LocRange[4])
          ty := LocRange[4];
        endif

        // nie sprawdzamy dla wykluczonych lokacji!
        if(IsInQuestExLoc(ExLocs, x+CInt((tx-x)/2), y+CInt((ty-y)/2), Realm))
          continue;
        endif

        foreach Npc in ListObjectsInBox(x, y, -127, tx, ty, 127, Realm)
          if(Npc.isa(POLCLASS_NPC))
            if(Targ.Template)
              if(IsQuestSeekNpcTemplate(Npc.npctemplate, Targ.Template))
                if(Targ.Gender and IsHuman(Npc))
                  if(Targ.Gender - 1 == Npc.gender)
                    if(!(Npc.serial in Except) and !IsInQuestExLoc(ExLocs, Npc.x, Npc.y, Realm))
                      Npcs.append(Npc);
                    endif
                  endif
                elseif(!(Npc.serial in Except) and !IsInQuestExLoc(ExLocs, Npc.x, Npc.y, Realm))
                  Npcs.append(Npc);
                endif
              endif
            elseif(Npc.script == Targ.Script)
              if(Targ.Gender and IsHuman(Npc))
                if(Targ.Gender - 1 == Npc.gender)
                  if(!(Npc.serial in Except) and !IsInQuestExLoc(ExLocs, Npc.x, Npc.y, Realm))
                    Npcs.append(Npc);
                  endif
                endif
              elseif(!(Npc.serial in Except) and !IsInQuestExLoc(ExLocs, Npc.x, Npc.y, Realm))
                Npcs.append(Npc);
              endif
            endif
          endif
        endforeach
      endfor
    endfor

    Tries := Tries + 1;
  endwhile

  if(!Npcs.size())
    Res.errortext := "Nie udalo sie znalezc Npeca [G:"+Targ.Gender+" S:"+Targ.Script+" T:"+Targ.Template+"] w LocRange ["+LocRange+"]";
    return Res;
  endif

  var Npc := Npcs[RandomInt(Npcs.size())+1];

  if(!Npc)
    Res.errortext := "Nie udalo sie wylosowac Npeca ["+Npc+"].";
    return Res;
  endif

  return Npc;

endfunction

//-----------------------------------------------------------------------------

function IsInQuestExLoc(byref ExLocs, x, y, byref Realm)

  if(!ExLocs.size() or !ExLocs)
    return 0;
  endif

  foreach ExLoc in ExLocs
    if(ExLoc[5] == Realm)
      if(ExLoc[1] <= x and ExLoc[3] >= x and ExLoc[2] <= y and ExLoc[4] >= y)
        return 1;
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetQuestExLocsInCities(byref ExLocs, Flags, ExCity := 0)

  if(Flags & QGLOCF_MURDER or Flags & QGLOCF_GOOD or Flags & QGLOCF_NEUTRAL)
    var cAlign, ExLoc;
    foreach City in GetCities()
      cAlign := GetCityAlignment(City);
      ExLoc  := 1;

      if(Flags & QGLOCF_MURDER)
        if(cAlign == CITYAL_EVIL)
          ExLoc := 0;
        endif
      endif

      if(Flags & QGLOCF_GOOD)
        if(cAlign == CITYAL_GOOD)
          ExLoc := 0;
        endif
      endif

      if(Flags & QGLOCF_NEUTRAL)
        if(cAlign == CITYAL_NEUTRAL)
          ExLoc := 0;
        endif
      endif

      if(ExLoc)
        ExLocs.append(GetCityCoords(City));
        ExLocs[ExLocs.size()][5] := GetCityRealm(City);
      endif
    endforeach
  endif
  
  if(ExCity)
    ExLocs.append(GetCityCoords(ExCity));
    ExLocs[ExLocs.size()][5] := GetCityRealm(ExCity);
  endif

endfunction

//--------------------------------------------------------------------------

function IsQuestSeekNpcTemplate(NpcTemp, SeekTemp)

  SeekTemp := lower(SeekTemp);
  NpcTemp := lower(NpcTemp);

  if(NpcTemp == SeekTemp)
    return 1;
  // specjalne sprawdzenie dla tokuno. Jak dojdzie wiecej, zrobi sie jakis cfg.
  elseif(NpcTemp == "tokuno"+SeekTemp)
    return 1;
  elseif(NpcTemp == "elf"+SeekTemp)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------
