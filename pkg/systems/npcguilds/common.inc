//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "const";

//-----------------------------------------------------------------------------

function GetNGdData()

  var Data := OpenDataFile(NGD_DATA);

  if(!Data)
    Data := CreateDataFile(NGD_DATA);
  endif

  if(!Data)
    LogError("GetNgdData: Nie udalo sie stworzyc NpcGuildData ["+NGD_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetPlayerNpcGuildCities(Player)

  var PlayerCities := GetObjProperty(Player, PROP_NGDCITIES);

  if(!PlayerCities or PlayerCities.LastCheck < ReadGameClock())
    if(!PlayerCities)
      PlayerCities := struct;
      PlayerCities.+Cities    := dictionary;
      PlayerCities.+LastCheck := 0;
    else
      PlayerCities.Cities     := dictionary;
    endif

    var PlayerData := GetObjProperty(Player, PROP_NGDDATA);
    var GuildData  := GetNGdData();
    var Guild;

    if(PlayerData)
      foreach GdData in (PlayerData.Guilds)
        if(GdData.Flags & NGPF_MEMBER)
          Guild := GuildData.FindElement(NGUILD_TITLE+_GdData_iter);
          if(Guild)
            foreach GdCity in (Guild.GetProp(GDPROP_CITIES))
              if(PlayerCities.Cities.exists(GdCity))
                if(PlayerCities.Cities[GdCity] < GdData.Level)
                  PlayerCities.Cities[GdCity] := GdData.Level;
                endif
              else
                PlayerCities.Cities[GdCity] := GdData.Level;
              endif
            endforeach
          endif
        endif
      endforeach
    endif

    PlayerCities.LastCheck := ReadGameClock() + NGD_CITIES_CHECK_TIME;
    SetObjProperty(Player, PROP_NGDCITIES, PlayerCities);
  endif

  return PlayerCities;

endfunction

//-----------------------------------------------------------------------------

function GetPlayerNpcGuildCityLvl(Player, City)

  if(!Player or !City)
    return 0;
  endif
 
  return CInt(GetPlayerNpcGuildCities(Player).Cities[City]);

endfunction

//-----------------------------------------------------------------------------
