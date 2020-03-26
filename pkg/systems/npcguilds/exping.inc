//-----------------------------------------------------------------------------

include "include/mobile/titles";

//-----------------------------------------------------------------------------

function LoseGuildExpInCity(who, City, Value)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var GuildData  := GetNGdData();
  var Lost       := 0;
  
  foreach GdData in (PlayerData.Guilds)
    if(GdData.Flags & NGPF_MEMBER)
      var Guild := GuildData.FindElement(NGUILD_TITLE+_GdData_iter);
      if(Guild)
        foreach GdCity in (Guild.GetProp(GDPROP_CITIES))
          if(GdCity == City)
            if(LoseGuildExp(who, PlayerData, _GdData_iter, Guild, Value))
              if(!Lost)
                Lost := 1;
              endif
            endif
          endif
        endforeach
      endif
    endif
  endforeach

  if(Lost)
    SetObjProperty(who, PROP_NGDDATA, PlayerData);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GainGuildExpInCity(who, City, Value)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var GuildData  := GetNGdData();
  var Gain       := 0;
  var Guild;
  
  foreach GdData in (PlayerData.Guilds)
    if(GdData.Flags & NGPF_MEMBER)
      Guild := GuildData.FindElement(NGUILD_TITLE+_GdData_iter);
      if(Guild)
        foreach GdCity in (Guild.GetProp(GDPROP_CITIES))
          if(GdCity == City)
            if(GainGuildExp(who, PlayerData, _GdData_iter, Guild, Value))
              if(!Gain)
                Gain := 1;
              endif
            endif
          endif
        endforeach
      endif
    endif
  endforeach

  if(Gain)
    SetObjProperty(who, PROP_NGDDATA, PlayerData);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function LoseGuildExp(who, byref PlayerData, GdId, byref Guild, Value, bMsg := 1)

  if(!Value)
    return 1;
  endif

  if(PlayerData.Guilds[GdId].Exp <= Value)
    return 0;
  endif

  PlayerData.Guilds[GdId].Exp := PlayerData.Guilds[GdId].Exp - Value;
  if(bMsg)
    SendSysMessage(who, "Twoja reputacja w Gildii "+Guild.GetProp(GDPROP_NAME)+" spadla.", FONT_NORMAL, COLOR_RED+1);
  endif

  if(PlayerData.Guilds[GdId].Level > 1)
    if(RawToBaseSkill(PlayerData.Guilds[GdId].Exp) < ((PlayerData.Guilds[GdId].Level)*200)-50)
      PlayerData.Guilds[GdId].Level := PlayerData.Guilds[GdId].Level - 1;
      if((PlayerData.Guilds[GdId].Flags & NGPF_MAXSK_CHOOSEN) and (PlayerData.Guilds[GdId].Flags & NGPF_MEMBER))
        SetMaxSkillValue(who, PlayerData.MaxSkID, DEF_SK_MAX_VALUE + PlayerData.Guilds[GdId].Level*10, MSKT_NPCGUILD);
      endif

      var Title  := GetNpcGuildTitle(Guild.GetProp(GDPROP_TYPE), PlayerData.Guilds[GdId].Level);
      var sTitle := GetPlayerNGDTitleByData(PlayerData);
      SetTitleSuffix(who, sTitle);
      if(bMsg)
        SendSysMessage(who, "Twoje miejsce w Gildii "+Guild.GetProp(GDPROP_NAME)+" spadlo, jestes teraz znan"+ggm(who,6)+" jako "+Title+".", FONT_NORMAL, COLOR_RED);
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GainGuildExp(who, byref PlayerData, GdId, byref Guild, Value)

  if(!Value)
    return 1;
  endif

  var MaxVal := BaseToRawSkill((PlayerData.Guilds[GdId].Level+1)*200);
  
  if(PlayerData.Guilds[GdId].Exp + Value > MaxVal)
    if(GetNpcGuildAgeReq(PlayerData.Guilds[GdId].Level+1) > GetRealAge(who))
      Value := MaxVal - PlayerData.Guilds[GdId].Exp;
    else
      PlayerData.Guilds[GdId].Level := PlayerData.Guilds[GdId].Level + 1;
      if(PlayerData.Guilds[GdId].Flags & NGPF_MAXSK_CHOOSEN)
        SetMaxSkillValue(who, PlayerData.MaxSkID, DEF_SK_MAX_VALUE + PlayerData.Guilds[GdId].Level*10, MSKT_NPCGUILD);
      endif
      var Title  := GetNpcGuildTitle(Guild.GetProp(GDPROP_TYPE), PlayerData.Guilds[GdId].Level);
      var sTitle := GetPlayerNGDTitleByData(PlayerData);
      SetTitleSuffix(who, sTitle);
      SendSysMessage(who, "Awansowal"+ggm(who,3)+" w Gildii "+Guild.GetProp(GDPROP_NAME)+"! Jestes teraz znan"+ggm(who,6)+" jako "+Title+".", FONT_NORMAL, COLOR_BLUE);
    endif
  endif

  if(RawToBaseSkill(PlayerData.Guilds[GdId].Exp + Value) > 1000)
    PlayerData.Guilds[GdId].Exp := BaseToRawSkill(1000);
  elseif(Value)
    PlayerData.Guilds[GdId].Exp := PlayerData.Guilds[GdId].Exp + Value;
    SendSysMessage(who, "Twoja reputacja w Gildii "+Guild.GetProp(GDPROP_NAME)+" wzrosla.", FONT_NORMAL, COLOR_BLUE+1);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetPlayerNGDTitle(who)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  return GetPlayerNGDTitleByData(PlayerData);

endfunction

//-----------------------------------------------------------------------------

function GetPlayerNGDTitleByData(byref PlayerData)

  if(!PlayerData)
    return "";
  endif
  
  var GuildData := GetNGdData();
  var GdId      := 0;
  var MaxLvl    := -1;
  var MaxExp    := 0;

  foreach GdData in (PlayerData.Guilds)
    if(GdData.Flags & NGPF_MEMBER)
      if(PlayerData.Guilds[_GdData_iter].Exp > MaxExp)
        GdId   := _GdData_iter;
        MaxExp := PlayerData.Guilds[_GdData_iter].Exp;
        MaxLvl := PlayerData.Guilds[_GdData_iter].Level;
      endif
    endif
  endforeach

  if(GdId)
    var Guild := GuildData.FindElement(NGUILD_TITLE+GdId);
    return GetNpcGuildTitle(Guild.GetProp(GDPROP_TYPE), MaxLvl);
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetNpcGuildTitle(Type, Lvl)

  var GdCfg := ReadConfigFile(NGD_CFG);
  
  if(!GdCfg)
    LogError("GetNpcGuildTitle: Nie udalo sie odczytac NpcGuildsCfg: "+NGD_CFG);
  else
    if(!GdCfg[NGDCFG_TITLES])
      LogError("GetNpcGuildTitle: Brak CfgElem z tytulami graczy NpcGildii ["+NGD_CFG+"]");
    else
      var Name := GetConfigString(GdCfg[NGDCFG_TITLES], NGDCFG_TYPE_TITLE+CStr(CInt(Type))+"_"+CStr(CInt(Lvl)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------


