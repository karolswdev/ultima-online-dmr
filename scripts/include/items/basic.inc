//-----------------------------------------------------------------------------

include "include/log";
include "include/objtypes/misc";

//-----------------------------------------------------------------------------

function CanUseItem(who, Item)

  if(who and !who.cmdlevel and item.movable)

    var Created := CInt(GetObjProperty(Item, PROP_CREATED));

    if(Created)
      var Owner := __IfElse(IsTamed(who), GetMaster(who), 0);

      if(Created != __IfElse(Owner, Owner.serial, who.serial))
        SendSysMessage(who, "Nie mozesz uzyc tego przedmiotu!", FONT_NORMAL, COLOR_ORANGE);
        LogMsg(FBITEMS_LOG,who,item);
        return 0;
      endif
    endif

    if(!CheckBound(who,Item))
      SendSysMessage(who, "Ten przedmiot nie nalezy do Ciebie!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
