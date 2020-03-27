//-----------------------------------------------------------------------------

include "default_setup";

//-----------------------------------------------------------------------------

var NextWander := ReadGameClock() + WANDER_TIME;
var City       := FindCity(me);
var CityAl     := GetCityAlignment(City);
var PushTimer  := ReadGameClock();

SetUpAr();
SaveAiSerial();
CheckLifeTimers();
drop_anchor();
check_frozen();

if(CityAl)
  if(CityAl == CITYAL_EVIL)
    SetCriminal(me, 1, "citynpcsetup.inc", "Setup miastowego Npca. Ustawianie krima i murda bo status miasta "+City+" = evil.");
    me.setMurderer(1);
  else
    SetCriminal(me, 0);
    me.setMurderer(0);
  endif
else
  check_alignment();
endif

InitWalk();