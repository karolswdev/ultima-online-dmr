//-----------------------------------------------------------------------------

use uo;

//-----------------------------------------------------------------------------

include "include/const/props";

//=============================================================================
//-----------------------------------------------------------------------------
// Bits
//-----------------------------------------------------------------------------
//=============================================================================

function IsBitOn(Num,Bit)

  if((Num & Bit) > 0)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetBitOn(byref Num,Bit)

  Num := (Num | Bit);

endfunction

//-----------------------------------------------------------------------------

function SetBitOff(byref Num,Bit)

  if(IsBitOn(Num,Bit))
    Num := Num - Bit;
  endif

endfunction

//-----------------------------------------------------------------------------

function SwitchBit(byref Num, Bit, Flag)

  if(Flag)
    SetBitOn(Num, Bit);
  else
    SetBitOff(Num, Bit);
  endif

endfunction

//-----------------------------------------------------------------------------

function Shr(byref Num,Bit)

  while(Bit)
    Num := Num / 2;
    Bit := Bit - 1;
  endwhile

  return Num;

endfunction

//-----------------------------------------------------------------------------

function Shl(byref Num,Bit)

  while(Bit)
    Num := Num * 2;
    Bit := Bit - 1;
  endwhile
  
  return Num;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Flags
//-----------------------------------------------------------------------------
//=============================================================================

function SetFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))

    var Flags := GetObjProperty(Object, PROP_FLAGS);
    if(!Flags)
      Flags := 0;
    endif

    SetBitOn(Flags, Flag);
    SetObjProperty(Object, PROP_FLAGS, Flags);
  endif

endfunction

//-----------------------------------------------------------------------------

function UnSetFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))

    var Flags := GetObjProperty(Object, PROP_FLAGS);
    if(!Flags)
      Flags := 0;
    endif

    SetBitOff(Flags, Flag);
    SetObjProperty(Object, PROP_FLAGS, Flags);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetFlags(Object)

  if(Object.isa(POLCLASS_UOBJECT))
    return CInt(GetObjProperty(Object, PROP_FLAGS));
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetFlags(Object, Flags)

  if(Object.isa(POLCLASS_UOBJECT))
    SetObjProperty(Object, PROP_FLAGS, CInt(Flags));
  endif

endfunction

//-----------------------------------------------------------------------------

function IsSetFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))
    var Flags := CInt(GetObjProperty(Object, PROP_FLAGS));
    return IsBitOn(Flags, Flag);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SwitchFlag(Object, Flag, Val)

  if(Val)
    SetFlag(Object, Flag);
  else
    UnSetFlag(Object, Flag);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Flags
//-----------------------------------------------------------------------------
//=============================================================================

function SetTempFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))

    var Flags := GetObjProperty(Object, PROP_TEMPFLAGS);
    if(!Flags)
      Flags := 0;
    endif

    SetBitOn(Flags, Flag);
    SetObjProperty(Object, PROP_TEMPFLAGS, Flags);
  endif

endfunction

//-----------------------------------------------------------------------------

function UnSetTempFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))

    var Flags := GetObjProperty(Object, PROP_TEMPFLAGS);
    if(!Flags)
      Flags := 0;
    endif

    SetBitOff(Flags, Flag);
    SetObjProperty(Object, PROP_TEMPFLAGS, Flags);
  endif

endfunction

//-----------------------------------------------------------------------------

function IsSetTempFlag(Object, Flag)

  if(Object.isa(POLCLASS_UOBJECT))
    var Flags := CInt(GetObjProperty(Object, PROP_TEMPFLAGS));
    return IsBitOn(Flags, Flag);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
