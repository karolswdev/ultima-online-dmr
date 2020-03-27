//-----------------------------------------------------------------------------

include "include/const/props";
include "include/finding";

//-----------------------------------------------------------------------------

const COFFIN_CLOSE_TIME      := 300;

//-----------------------------------------------------------------------------

function IsCoffin(Item)

  var Serials := GetObjProperty(Item, PROP_PARTS_SERS);

  if(!Serials.size() or !Serials)
    return 0;
  endif

  foreach cfItem in Serials

    cfItem := FindItemBySerial(cfItem);
    if(cfItem and cfItem.objtype == 0xA80E)
      return cfItem;
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCoffinParts(Blood)

  var Serials := GetObjProperty(Blood, PROP_PARTS_SERS);

  if(!Serials)
    return 0;
  endif

  var Part, Parts := struct;
  Parts.+coffin := array;
  Parts.+blood  := Blood;

  foreach Serial in Serials

    if(Blood.serial != Serial)
      Part := FindItemBySerial(Serial);
      if(!Part)
        return 0;
      endif

      Parts.coffin.append(Part);
    endif

  endforeach

  return Parts;

endfunction

//-----------------------------------------------------------------------------

function ChangeCoffin(byref Parts, Closed)

  foreach Part in (Parts.coffin)
    if(Closed)
      Part.graphic := Part.objtype;
    else
      case(Part.objtype)
        0x1C44:  Part.graphic := 0x1C40;
        0x1C45:  Part.graphic := 0x1C3F;
        0x1C46:  Part.graphic := 0x1C3E;
        0x21A2:  Part.graphic := 0x1C3C;
      endcase
    endif
  endforeach
  
endfunction

//-----------------------------------------------------------------------------
