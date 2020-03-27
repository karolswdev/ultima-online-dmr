//-----------------------------------------------------------------------------

include "include/mobile/basic";

//-----------------------------------------------------------------------------
// Age Flags
//-----------------------------------------------------------------------------

const AGEF_REAL   := 0x01;

//=============================================================================
//-----------------------------------------------------------------------------
// Age / Names
//-----------------------------------------------------------------------------
//=============================================================================

function GetAge(who, Flags := 0)

  // Jesli nie chcemy pobrac prawdziwego wieku, to sprawdzamy wartosc z moda MOD_INCO

  if((Flags & AGEF_REAL) == 0)
    if(CInt(GetObjProperty(who, "#TempAge")))
      return CInt(GetObjProperty(who, "#TempAge"));
    endif
  endif

  var Class := GetMobileClass(who);
  var Age   := 1;

  case(Class)

    POLCLASS_NPC:     Age := NPC_START_AGE;

                      if(CInt(GetObjProperty(who, PROP_CREATEDAT)))
                        Age := Age + CInt((ReadGameClock() - CInt(GetObjProperty(who, PROP_CREATEDAT))) / NPC_AGE_COUNT);
                      endif

                      Age := ToMax(Age, NPC_MAX_AGE);


    POLCLASS_PLAYER:  Age := START_AGE;

                      if(GetObjProperty(who, PROP_ONLINETIMER))
                        Age := Age + CInt(CInt(GetObjProperty(who, PROP_ONLINETIMER)) / AGE_COUNT);
                      endif

  endcase

  return Age;

endfunction

//-----------------------------------------------------------------------------

function GetRealAge(who)

  return GetAge(who, AGEF_REAL);

endfunction

//-----------------------------------------------------------------------------

function GetAgeMonth(who, Flags := 0)

  // jesli nie chcemy pobrac prawdziwego wieku - sprawdzamy czy jest ustawiony MOD_INCO, jesli tak to zwracamy 0 miesiecy.

  if((Flags & AGEF_REAL) == 0)
    if(CInt(GetObjProperty(who, "#TempAge")))
      return 0;
    endif
  endif

  var Months := 0;
  var Class  := GetMobileClass(who);

  case(Class)

    POLCLASS_NPC:     if(CInt(GetObjProperty(who, PROP_CREATEDAT)))
                        var AgeTime := ReadGameClock() - CInt(GetObjProperty(who, PROP_CREATEDAT));
                        Months := (AgeTime % NPC_AGE_COUNT) / (NPC_AGE_COUNT / 12);      
                      endif

    POLCLASS_PLAYER:  if(GetObjProperty(who, PROP_ONLINETIMER))
                        var AgeTime := CInt(GetObjProperty(who, PROP_ONLINETIMER));
                        Months := (AgeTime % AGE_COUNT) / (AGE_COUNT / 12);
                      endif

  endcase

  return Months;

endfunction

//-----------------------------------------------------------------------------

function GetAgeDesc(who, Flags := 0)

  var Age    := GetAge(who, Flags);
  var Months := GetAgeMonth(who, Flags);

  var Desc := Age + " " + Gam(Age, GAM_LAT);

  if(Months)
    Desc := Desc + ", "+Months +" miesi"+gam(Months, GAM_ECY);
  endif

  return Desc;

endfunction

//-----------------------------------------------------------------------------
