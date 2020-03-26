//--------------------------------------------------------------------------

function MovePlayerToSafeLoc(who)

  // gracz jest juz w domu lub w miescie? to jest juz w safeLoc, powiedzmy..
  if(who.multi)
    return 1;
  endif

  var Houses := GetHouseOwnerList(who);
  var Res := 0;

  foreach House in Houses
    House := FindItemBySerial(House);

    if(House)
      Res := MoveObject(who, House.x, House.y, GetStandZ(House.x, House.y, House.realm), House.realm);

      if(Res)
        SendSysMessage(who, "Zostal"+ggm(who,3)+" przeniesion"+ggm(who, 6)+" w bezpieczna lokacje.", FONT_NORMAL, COLOR_WHITE);
        break;
      endif
    endif

  endforeach

  if(!Res)
    var Al := __IfElse(who.murderer or who.criminal, CITYAL_NEUTRAL, CITYAL_GOOD);
    var cityRes := FindNearestCity(who.x, who.y, who.realm, al);
    var DropLoc := GetCityDropLoc(cityRes.city);

    if(DropLoc)
      Res := MoveObject(who, DropLoc.x, DropLoc.y, DropLoc.z, DropLoc.realm, MOVECHAR_FORCELOCATION);

      if(Res)
        SendSysMessage(who, "Zostal"+ggm(who,3)+" przeniesion"+ggm(who, 6)+" w bezpieczna lokacje.", FONT_NORMAL, COLOR_WHITE);
      endif
    endif
  endif

  return Res;

endfunction

//--------------------------------------------------------------------------
