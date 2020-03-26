//-----------------------------------------------------------------------------

const EVENT_STATUS_STARTED := 1;
const EVENT_STATUS_STOPPED := 2;
const EVENT_STATUS_STOPING := 3;

//-----------------------------------------------------------------------------

function GetTimeLeftForEvent(Event)

  var Flags := CInt(Event.GetProp("Flags"));

  var Time := PolCore().systime;
  var Comps := ExtractTimeComponents(Time);
  var Count := 0, j := Comps.WeekDay;
  var EventHourTime := Event.GetProp("Hour")*60;
  var LeftTime := 0;
  var Day := 0, Hour := GetStringFromTime(Event.GetProp("Hour"));

  while(Count <= 7)
    var Mask := Shl(1, j);

    if(IsBitOn(Flags, Mask) and Comps.daytime < EventHourTime)
      LeftTime += EventHourTime - Comps.daytime;
      Day := j;
      break;
    endif

    if(Comps.daytime > 0)
      LeftTime += DAY_LENGTH - Comps.daytime;
      Comps.daytime := 0;
    else
      LeftTime += DAY_LENGTH;
    endif

    Count += 1;
    j += 1;

    if(j > 7)
      j := 1;
    endif
  endwhile

  if(Count == 8)
    return error;
  endif

  return array(LeftTime, Day, Hour);

endfunction

//--------------------------------------------------------------------------------

function IsAutoEventItem(Item)

  return GetObjProperty(Item, PROP_AUTOEVENT_ITEM) != error;

endfunction

//-----------------------------------------------------------------------------

function IsOnAutoEvent(who)

  return CInt(GetObjProperty(who, "AutoEvent")) > 0;

endfunction

//-----------------------------------------------------------------------------

