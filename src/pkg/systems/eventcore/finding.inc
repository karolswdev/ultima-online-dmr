//-----------------------------------------------------------------------------

include "include/gump_utils";
include "include/move";
include "basic";

//-----------------------------------------------------------------------------

function FindEventAreaByLoc(Loc)

  var Data := GetEventsData();
  var Keys := GetDataKeys(Data, EVENT_TITLE, BQG_ZEROFILL);
  var RealmId := GetRealmId(Loc.realm);

  foreach Key in Keys
    var Realkey := UnZeroFill(Key);
    var Event := Data.FindElement(EVENT_TITLE+RealKey);
    var Areas := Event.GetProp("Areas");

    if(Areas)
      foreach Area in Areas
        if(Loc.x >= Area[1] and Loc.x <= Area[3] and Loc.y >= Area[2] and Loc.y <= Area[4] and RealmId == Area[5])
          return CInt(Realkey);
        endif
      endforeach
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
