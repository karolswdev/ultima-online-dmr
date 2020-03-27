//-----------------------------------------------------------------------------

const MAX_W            := 7;
const MAX_H            := 7;

const DEF_STONE        := 0x25F8;
const PUSH_STONE       := 0x09A9;
const PUSHED_STONE     := 0x25FF;
const LINK_UP          := 0x2391;
const LINK_LEFT        := 0x2393;
const PSDMG_M          := 12;

const PS_DISCH_TIME    := 14400;
const PS_CHEST_D_TIME  := 900;

const UOBJ_POWERGEM    := 0xF30;
const MAX_GEMS         := 7;

//-----------------------------------------------------------------------------

function AddGems(who,stone)

  var item := ObjTarget(who);
  if(item.container == who.backpack)
    if(item.objtype == UOBJ_POWERGEM)
      AddGemsToStone(who,stone,item);
    else
      SendSysMessage(who,"To nie diamenty.",FONT_NORMAL,COLOR_RED);
    endif
  else
    SendSysMessage(who,"To nie znajduje sie w twoim plecaku.",FONT_NORMAL,COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function AddGemsToStone(who,stone,item)

  if(!StandardItemCheck(who,stone,NO_MOVABLE_CHECK))
    MoveItemToContainer(item,who.backpack);
    return 0;
  endif

  if(!StandardItemCheck(who,item))
    MoveItemToContainer(item,who.backpack);
    return 0;
  endif

  if(item.objtype == UOBJ_POWERGEM)
    var Amt := GetObjProperty(stone,"GemAmount"),AmtAdd;
    if(!Amt)
      Amt := 0;
    endif

    if(Amt >= MAX_GEMS)
      SendSysMessage(who,"W kamieniu nie zmiesci sie wiecej diamentow.",FONT_NORMAL,COLOR_RED);
      MoveItemToContainer(item,who.backpack);
      return 0;
    endif

    AmtAdd := item.amount;
    if(AmtAdd + Amt > MAX_GEMS)
      AmtAdd := MAX_GEMS - Amt;
    endif
    SubtractAmount(item,AmtAdd);
    SendSysMessage(who,"Wlozyl"+ggm(who,3)+" diamenty do Kamienia Mocy.",FONT_NORMAL,COLOR_GREEN);
    SetObjProperty(stone,"GemAmount",Amt+AmtAdd);
  endif

  if(item)
    MoveItemToContainer(item,who.backpack);
  endif
  ReleaseItem(item);
  return 1;
  
endfunction

//-----------------------------------------------------------------------------
