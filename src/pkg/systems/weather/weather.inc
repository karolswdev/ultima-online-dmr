//--------------------------------------------------------------------------------

include "include/const/props";
include "include/updweather";

//--------------------------------------------------------------------------------

const DAY_STATE              := 1;
const NIGHT_STATE            := 2;

const SEASON_SPRING          := 1;
const SEASON_SUMMER          := 2;
const SEASON_FALL            := 3;
const SEASON_WINTER          := 4;

const WEATHER_CH_BREAK       := 300;

//--------------------------------------------------------------------------------

const WTYPE_RAIN             := 0x00;
const WTYPE_STORM            := 0x01;
const WTYPE_SNOW             := 0x02;
const WTYPE_FIERCESTORM      := 0x03;
const WTYPE_END              := 0xFF;

//--------------------------------------------------------------------------------

function GetWeatherState()

  if(!GetGlobalProperty(PROP_WEATHERSTATE))
    SetGlobalProperty(PROP_WEATHERSTATE, array);
  endif

  return GetGlobalProperty(PROP_WEATHERSTATE);

endfunction

//--------------------------------------------------------------------------------

function SetWeatherState(byref State)

  SetGlobalProperty("WeatherState",State);

endfunction

//--------------------------------------------------------------------------------

function GetDayHS()

  return GetWeatherState()[2][1];

endfunction

//--------------------------------------------------------------------------------

function GetNightHS()

  return GetWeatherState()[2][2];

endfunction

//--------------------------------------------------------------------------------

function GetCurrentSeason()

  return GetWeatherState()[3][1];

endfunction

//--------------------------------------------------------------------------------

function GetCurrentSeasonPacket()

  return GetWeatherState()[3][2];

endfunction

//--------------------------------------------------------------------------------

function GetGlobalLightLevel()

  return CInt(GetWeatherState()[1][2]);

endfunction

//--------------------------------------------------------------------------------

function IsDayLight()

  var DCfg  := ReadConfigFile(DAYS_CFG);
  var DayBr := CInt(DCfg["DayTime"].DayBrightness);

  if(Abs(GetGlobalLightLevel() - DayBr) <= 3)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsNightLight()

  var DCfg    := ReadConfigFile(DAYS_CFG);
  var NightBr := CInt(DCfg["DayTime"].NightBrightness);

  if(Abs(GetGlobalLightLevel() - NightBr) <= 3)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsRaining()

  if(GetWeatherState()[4][1])
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetWeatherChangeSound()

  var Sounds := GetWeatherState()[4][5];
  var Sound;

  if(Sounds.size())
    Sound := Sounds[RandomInt(Sounds.size())+1];
    if(!Sound)
      Sound := Sounds[1];
    endif
    return CInt(Sound);
  elseif(CInt(Sound))
    return CInt(Sound);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function SendSeasonPacket(who := 0)

  if(who)
    SendPacket(who,GetCurrentSeasonPacket());
  else
    var Season := GetCurrentSeasonPacket();
    foreach Char in EnumerateOnlineCharacters()
      SendPacket(Char,Season);
    endforeach
  endif

endfunction

//--------------------------------------------------------------------------------

function SetWeatherRain(byref State)

  if(!State[4][1])
    SetWeatherType(WTYPE_END, 0);
  else
    var MidState  := ((RAIN_MAX_CYCLES+1)/2);
    var Intensity := CInt( (MidState - Abs(MidState - State[4][1])) * RAIN_MAX_INTENSITY / MidState);
    SetWeatherType(State[4][4], Intensity);
  endif

endfunction

//--------------------------------------------------------------------------------

function SendWeatherSound()

  var Sound := GetWeatherChangeSound();

  foreach Char in EnumerateOnlineCharacters()
    if(!Char.cmdlevel)
      if(!IsDungeon(Char.x, Char.y, Char.realm))
        if(Sound)
          PlaySoundEffect(Char, Sound);
        endif
      endif
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------

function SetBackgroundLight(Realm := 0,Lvl := -1)

  if(Lvl == -1)
    Lvl := GetGlobalLightLevel();
  endif

  case(Realm)
    BRITB_REALM:  SetRegionLightLevel ("BritSurface", Lvl);
    BRITA_REALM:  SetRegionLightLevel ("BritAltSurface", Lvl);
    ILSHE_REALM:  SetRegionLightLevel ("IlshSurface", Lvl);
    MALAS_REALM:  SetRegionLightLevel ("MalasSurface", Lvl);
    TOKUNO_REALM: SetRegionLightLevel ("TokunoSurface", Lvl);

    default:
                  SetRegionLightLevel ("BritSurface", Lvl);
                  SetRegionLightLevel ("BritAltSurface", Lvl);
                  SetRegionLightLevel ("IlshSurface", Lvl);
                  SetRegionLightLevel ("MalasSurface", Lvl);
                  SetRegionLightLevel ("TokunoSurface", Lvl);
  endcase

endfunction

//--------------------------------------------------------------------------------

function SetWeatherType(Type, Intensity, Realm := 0, LightLvl := -1)

  case(Realm)
    BRITB_REALM:  SetRegionWeatherLevel ("BritSurface", Type, Intensity, 0, LightLvl);
    BRITA_REALM:  SetRegionWeatherLevel ("BritAltSurface", Type, Intensity, 0, LightLvl);
    ILSHE_REALM:  SetRegionWeatherLevel ("IlshSurface", Type, Intensity, 0, LightLvl);
    MALAS_REALM:  SetRegionWeatherLevel ("MalasSurface", Type, Intensity, 0, LightLvl);
    TOKUNO_REALM: SetRegionWeatherLevel ("TokunoSurface", Type, Intensity, 0, LightLvl);

    default:
                  SetRegionWeatherLevel ("BritSurface", Type, Intensity, 0, LightLvl);
                  SetRegionWeatherLevel ("BritAltSurface", Type, Intensity, 0, LightLvl);
                  SetRegionWeatherLevel ("IlshSurface", Type, Intensity, 0, LightLvl);
                  SetRegionWeatherLevel ("MalasSurface", Type, Intensity, 0, LightLvl);
                  SetRegionWeatherLevel ("TokunoSurface", Type, Intensity, 0, LightLvl);
  endcase

endfunction

//--------------------------------------------------------------------------------

function PlayNatureSounds(who)

  if(IsWaterNearby(who.x, who.y, who.realm))
    PlayWaterSound(who);
  endif

  if(RandomInt(100) < 75)
    if(!IsRaining())
      var MapInfo := GetMapInfo(who.x, who.y, who.realm);

      if(IsSwampTile(MapInfo.landtile))
        PlaySwampSound(who);
      elseif(IsTile(MapInfo.landtile, LTILE_JUNGLE))
        PlayJungleSound(who);
      elseif(GetCurrentSeason() != SEASON_WINTER)
        if(IsTile(MapInfo.landtile, LTILE_FOREST))
          PlayForestSound(who);
        elseif(IsTile(MapInfo.landtile, LTILE_GRASS))
          PlayNightSound(who);
        elseif(IsTile(MapInfo.landtile, LTILE_DIRT))
          PlayNightSound(who);
        endif
      elseif(RandomInt(100) < 30)
        if(IsTile(MapInfo.landtile, LTILE_GRASS))
          PlayNightSound(who);
        elseif(IsTile(MapInfo.landtile, LTILE_DIRT))
          PlayNightSound(who);
        endif
      endif
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function PlayWaterSound(who)

  if(RandomInt(100) < 10)
    if(RandomInt(100) < 50)
      PlaySoundEffect(who, 0x13);
    else
      PlaySoundEffect(who, 0x14);
    endif
  elseif(RandomInt(100) < 50)
    PlaySoundEffect(who, 0x11);
  else
    PlaySoundEffect(who, 0x12);
  endif

endfunction

//--------------------------------------------------------------------------------

function PlaySwampSound(who)

  if(IsDayLight())

    case(RandomInt(4))
      1:       PlaySoundEffect(who, 0x7);
      2:       PlaySoundEffect(who, 0x8);
      3:       PlaySoundEffect(who, 0x2B7);
      default: PlaySoundEffect(who, 0x2B8);
    endcase

  elseif(IsNightLight())

    case(RandomInt(2))
      1:       PlaySoundEffect(who, 0xF);
      default: PlaySoundEffect(who, 0x10);
    endcase

  endif

endfunction

//--------------------------------------------------------------------------------

function PlayForestSound(who)

  if(IsDayLight())

    case(RandomInt(3))
      1:       PlaySoundEffect(who, 0x1);
      2:       PlaySoundEffect(who, 0x2);
      default: PlaySoundEffect(who, 0x3);
    endcase

  elseif(IsNightLight())

    case(RandomInt(2))
      1:       PlaySoundEffect(who, 0x9);
      default: PlaySoundEffect(who, 0xA);
    endcase

  endif

endfunction

//--------------------------------------------------------------------------------

function PlayJungleSound(who)

  if(IsDayLight())

    case(RandomInt(6))
      1:       PlaySoundEffect(who, 0x4);
      2:       PlaySoundEffect(who, 0x5);
      3:       PlaySoundEffect(who, 0x2B4);
      4:       PlaySoundEffect(who, 0x2B5);
      5:       PlaySoundEffect(who, 0x2B6);
      default: PlaySoundEffect(who, 0x6);
    endcase

  elseif(IsNightLight())

    case(RandomInt(2))
      1:       PlaySoundEffect(who, 0xD);
      default: PlaySoundEffect(who, 0xE);
    endcase

  endif

endfunction

//--------------------------------------------------------------------------------

function PlayNightSound(who)

  if(IsNightLight())
    case(RandomInt(2))
      1:        PlaySoundEffect(who, 0xB);
      default:  PlaySoundEffect(who, 0xC);
    endcase
  endif

endfunction

//--------------------------------------------------------------------------------
