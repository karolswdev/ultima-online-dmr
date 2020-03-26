//--------------------------------------------------------------------------------

include "include/tempmods";
include "include/checks/locchecks";
include ":weather:weather";

//--------------------------------------------------------------------------------

const RAIN_MAX_CYCLES        := 5;
const RAIN_MAX_INTENSITY     := 0x46;

//--------------------------------------------------------------------------------

function SendLogonWPackets(who)

  SendSeasonPacket(who);
  UpdateLight(who);

endfunction

//--------------------------------------------------------------------------------

function UpdateLight(who)

  if(who.cmdlevel)
    who.setlightlevel(31, 1);
    who.setlightlevel(0,3600000);
  elseif(CanMagicMod(who, MOD_LIGHT) == 0)
    who.setlightlevel(31, 1);
    who.setlightlevel(0, 186400);
  elseif(CanMod(who, MOD_LIGHT) == 0)
    who.setlightlevel(31, 1);
    who.setlightlevel(CInt(GetModAmount(who, MOD_LIGHT)), GetModLeftTime(who, MOD_LIGHT));
  else
    RestoreLightLevel(who);
  endif

endfunction

//--------------------------------------------------------------------------------

function RestoreLightLevel(who)

  var LightLvl := CInt(GetGlobalProperty(PROP_WEATHERSTATE)[1][2]);

  if(LightLvl)
    who.setlightlevel( 31, 1);
    who.setlightlevel( LightLvl, 1);
  endif

endfunction

//--------------------------------------------------------------------------------

function SendWeatherUpdatePacket(who)

  var State := GetGlobalProperty(PROP_WEATHERSTATE);

  if(State[4][1])
    if(!IsDungeon(who.x, who.y, who.realm))
      var MidState  := ((RAIN_MAX_CYCLES+1)/2);
      var Intensity := CInt( (MidState - Abs(MidState - State[4][1])) * RAIN_MAX_INTENSITY / MidState);
      var Packet    := CreatePacket(0x65, 0x04);
      Packet.SetInt8(1, State[4][4]);
      Packet.SetInt8(2, Intensity);
      Packet.SendPacket(who);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------
