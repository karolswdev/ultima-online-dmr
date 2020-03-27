//-----------------------------------------------------------------------------

include ":housing:common";

//-----------------------------------------------------------------------------

function DestroyDeedItem(Item,who,Cont := 0,Force := 0)

  var MainCfg := ReadConfigFile(":*:deeds");

  if(!Cont)
    Cont := who.backpack;
  endif

  var CanDes := Force;

  if(!CanDes)
    if(IsGm(who))
      CanDes := 1;
    elseif(IsOwner(who, item))
      CanDes := 1;
    elseif(CanControlHouse(item, who))
      CanDes := 1;
    endif
  endif

  if(!CanDes)
    SendSysMessage(who,"Ten przedmiot nie nalezy do Ciebie i nie mozesz go zniszczyc.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var ObjType := CInt(GetObjProperty(item,"DeedObjType"));

  if(!ObjType or !MainCfg[ObjType] or !GetObjProperty(item,"PartsSerials"))
    if(!Force)
      SendSysMessage(who,"Tego nie uda Ci sie zburzyc.",FONT_NORMAL,COLOR_RED);
    endif
    return;
  endif

  var Parts := GetObjProperty(item,"PartsSerials");

  if(!Force and !who.cmdlevel)
    foreach PartItem in Parts
      PartItem := FindItemBySerial(PartItem);
      if(PartItem)
        if(GetObjProperty(PartItem, "#NoDestroy"))
          SendSysMessage(who, "Teraz nie mozesz zniszczyc tego przedmiotu.", FONT_NORMAL, COLOR_RED);
          return;
        endif
      endif
    endforeach
  endif

  var Deed  := CreateItemInCont(Cont,ObjType,1);
  if(!Deed and !Force)
    SendSysMessage(who,"Masz za malo miejsca w plecaku.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  foreach PartItem in Parts
    if(PartItem)
      ReleaseItem(PartItem);
      DestroyItem(PartItem);
    endif
  endforeach

  if(!Force)
    SendSysMessage(who, "Zniszczyl"+ggm(who,3)+" przedmiot i wlozyl"+ggm(who,3)+" deed do plecaka.",FONT_NORMAL,COLOR_GREEN);
  endif

  return Deed;

endfunction

//-----------------------------------------------------------------------------

function IsItemFromDeed(Item)

  return CInt(GetObjProperty(item,"DeedObjType"));

endfunction

//-----------------------------------------------------------------------------
