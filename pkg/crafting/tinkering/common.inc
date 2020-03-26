//-----------------------------------------------------------------------------

function GetTinkerMatObj(Item)

  var MatObj := 0;
  var cfg := ReadConfigFile(":tinkering:tinker");
  var Mat := SplitWords(cfg[Item.objtype].material);

  if(Mat[1] == "ingots")
    MatObj := GetIngObjByColor(Item.color);
  elseif(Mat[1] == "boards")
    var LogType := GetItemMakeMod(Item);
    if(!LogType)
      LogType := 1;
    endif
    MatObj := GetBoardObjTypes()[LogType];
  endif

  return MatObj;

endfunction

//-----------------------------------------------------------------------------

function ResurrectGolem(who, Corpse, param := 0)

  if(GetBaseSkill(who, SKILLID_TINKERING) < 100)
    SendSysMessage(who, "Twe umiejetnosci sa zbyt male by moc zlozyc golema.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  if(FindNpcBySerial(GetObjProperty(who,"MadeGolem")))
    SendSysMessage(who,"Masz juz Golema.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(GetMaster(Corpse) != who)
    SendSysMessage(who,"Ten golem nie byl twoj.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!param)
    if(Distance(who, Corpse) > 1)
      SendSysMessage(who, "Stoisz za daleko.", FONT_NORMAL, COLOR_RED);
      return;
    endif

    var ClayIngot    := FindItemInContainer(who.backpack,UOBJ_CLAY);

    if(!ClayIngot)
      SendSysMessage(who, "Nie masz gliny.", FONT_NORMAL, COLOR_RED);
      return;
    elseif(!SubtractAmount(ClayIngot, 20))
      SendSysMessage(who, "Potrzebujesz 20 gliny.", FONT_NORMAL, COLOR_RED);
      return;
    endif

    if(!CanResurrectNpc(who, Corpse))
      return;
    endif
  endif

  var WhoTinkering := GetEffectiveSkill(who, SKILLID_TINKERING);

  SendSysMessage(who, "Probujesz poskladac golema...", FONT_NORMAL, COLOR_GREEN);

  if(!MakeNoise(who, 0x22C, 2000))
    return;
  endif

  var Golem := ResurrectNpc(who, Corpse, (1.0 - (WhoTinkering * 0.5)));

  if(Golem)
    SendSysMessage(who, "Udalo Ci sie poskladac golema.", FONT_NORMAL, COLOR_BLUE);
    SetObjProperty(who, "MadeGolem", Golem.serial);
    if(param)
      SetHp(Golem, GetMaxHp(Golem));
    endif
  else
    SendSysMessage(who, "Nie udalo sie poskladac golema.", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------
