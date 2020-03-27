//--------------------------------------------------------------------------------

include "include/cmds/cancmd";

//--------------------------------------------------------------------------------

function SearchItemBy(Type, Srch)

  Srch := lower(Srch);

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var Items := array;
  var Val;

  foreach Item in GetConfigStringKeys(ItemCfg)
    Val := lower(GetConfigString(ItemCfg[Item],Type));
    if(Val == Srch)
      return array(Item);
    endif

    if(Val[Srch])
      Items.append(Item);
    endif
  endforeach

  if(!Items.size())
    Items := 0;
  endif

  return Items;

endfunction

//--------------------------------------------------------------------------------

const SIF_SHOW_CRAFT := 0x01;

//--------------------------------------------------------------------------------

function ShowItems(who,byref Items, Flags := 0)

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var Width := __IfElse(Flags & SIF_SHOW_CRAFT, 800, 710);

  GInit(30, 30);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, Width, 460);
  GAddPageData(Width-32,16,2650,2651,Width-32,428,2648,2647);
  
  if(Flags & SIF_SHOW_CRAFT)
    GTextLine(630, 12, 999, "Rzemioslo");
    GTextLine(730, 12, 999, "Diff");
    GTextLine(760, 12, 999, "Gain");
  endif
  
  GMultiPage();

  var i := 1;

  foreach Item in Items
    GCheckNextPage(20);
    GTextLine(20, 30+GNPCount()*20, 550, i);
    GButton  (50, 30+GNPCount()*20, 2714, 2715, CInt(Item));
    GTextLine(80, 30+GNPCount()*20, 550, hex(Item));
    GTextLine(140, 30+GNPCount()*20, 670, CInt(Item));
    GTextLine(200, 30+GNPCount()*20, 999, CLStr(GetConfigString(ItemCfg[Item],"Name")));
    GTextLine(400, 30+GNPCount()*20, 995, CLStr(GetConfigString(ItemCfg[Item],"Desc")));

    if(Flags & SIF_SHOW_CRAFT)
      var cData := GetCraftDataForItem(Item);

      if(cData)
        GTextLine(630, 30+GNPCount()*20, 1153, cData.SkElem.Name);
        GTextLine(730, 30+GNPCount()*20, 1320, cData.Diff);
        GTextLine(760, 30+GNPCount()*20, 1420, cData.Gain);
      endif
    endif

    i := i + 1;
  endforeach

  SendSysMessage(who,"Znaleziona liczba przedmiotow: "+Items.size(),FONT_NORMAL,COLOR_BLUE);

  var Res := GSend(who);
  return CInt(Res[0]);

endfunction

//-----------------------------------------------------------------------------

function SelectTemplate(who, text, Type)

  var AllTemps := GetAllNpcTemplates();
  var Temps    := AllTemps;
  var sName, sTemp, cfgElem;

  if(text)
    Temps := {};
    foreach Temp in AllTemps
      
      if(Temp == text)
        return Temp;
      endif

      cfgElem := GetNpcConfig(Temp);

      case(Type)
        1:   sName := lower(cfgElem.Name);
             if(sName[text])
               Temps.append(Temp);
             endif

        2:   sTemp := lower(Temp);
             if(sTemp[text])
               Temps.append(Temp);
             endif
      endcase
    endforeach
  endif

  if(!Temps.size())
    SendSysMessage(who,"Nie znaleziono zadnego Templata z textem ["+text+"]",FONT_NORMAL,COLOR_RED);
    return 0;
  elseif(Temps.size() == 1)
    return Temps[1];
  else
    return SelectNpcsGump(who, Temps);
  endif
  
endfunction


//-----------------------------------------------------------------------------

function CreateSelectedNpc(Template, who, text := 0)

  var cfgElem := GetNpcConfig(Template);

  if(!cfgElem)
    SendSysMessage(who,"Template ["+Template+"] nie istnieje.",FONT_NORMAL,COLOR_RED);
    return;
  endif
    
  var Split := array;

  if(text)

    Split := SplitWords(text);

    if("q" in Split)
      if(!CanUseCmd(who, "npcs qq"))
        SendSysMessage(who, "Nie mozesz tworzyc npecow questowych!", FONT_NORMAL, COLOR_RED);
        return;
      endif
    endif

  endif

  Detach();

  var Loc := LocTarget(who);

  if(Loc)
    text[Template] := "";

    var Flags := 0;
    if("nl" in Split)
      Flags := NPCF_NO_LOOT;
    endif
    
    var Npc;
    var amount := 1;
    if(CInt(split[split.size()]) > 1)
      amount := CInt(split[split.size()]);
    endif
    for i:= 1 to amount
      Npc := CreateNpc(Template,Loc.x+(i-1),Loc.y+(i-1),Loc.z,who.realm,DEF_PARMS,Flags);
      
      if(!Npc)
        SendSysMessage(who,"Blad: "+Npc,FONT_NORMAL,COLOR_RED);
      else
        if("pe" in Split)
          SendChEvent(Npc,Npc,EVID_ENGAGED);
        endif

        if("q" in Split)
          LogMsg(QSITEMS_LOG, who, Npc);
        elseif(who.cmdlevel)
          SetPropCreated(who, Npc);
          LogMsg(CREATE_LOG, who, Npc);
        else
          SetObjProperty(Npc,PROP_OWNER,who.serial);
          LogMsg(CREATE_LOG, who, Npc);
        endif
      endif
    endfor
  endif

endfunction

//-----------------------------------------------------------------------------

function SelectNpcsGump(who, byref Temps)

  if(!Temps.size())
    SendSysMessage(who, "Nie znaleziono zadnych Templatow.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  Temps.sort();

  GInit(30, 30);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, 750, 460);
  GAddPageData(718,16,2650,2651,718,428,2648,2647);
  GMultiPage();

  var i := 1;
  var cfgElem;

  foreach Temp in Temps

    cfgElem := GetNpcConfig(Temp);

    GCheckNextPage(20);
    GTextLine(20, 30+GNPCount()*20, 550, i);
    GButton  (50, 30+GNPCount()*20, 2714, 2715, i);
    GTextLine(80, 30+GNPCount()*20, 550, hex(cfgElem.ObjType));
    GTextLine(140, 30+GNPCount()*20, 670, CInt(cfgElem.ObjType));
    GTextLine(200, 30+GNPCount()*20, 999, Temp);
    GTextLine(400, 30+GNPCount()*20, 995, CLStr(cfgElem.Name));
    i := i + 1;
  endforeach
  
  SendSysMessage(who,"Znaleziona liczba npecow: "+Temps.size(),FONT_NORMAL,COLOR_BLUE);

  var Res := GSend(who);
  return Temps[CInt(Res[0])];

endfunction

//-----------------------------------------------------------------------------

function BuildGoLocGump(who)

  var AllElems := GetConfigStringKeys(GoLocCfg);

  GInit(50,50);
  GResPic(0,0,3600,350,440);
  GTextLine(100,20,550,"Wybierz kategorie");

  GButton(30,72,2714,2715,1);
  GTextLine(60,72,560,"Miasta");

  GButton(30,92,2714,2715,2);
  GTextLine(60,92,560,"Lochy");

  GButton(30,112,2714,2715,3);
  GTextLine(60,112,560,"Grobowce");

  GButton(30,132,2714,2715,4);
  GTextLine(60,132,560,"Cmentarze");

  GButton(30,152,2714,2715,5);
  GTextLine(60,152,560,"Inne");

  GButton(30,172,2714,2715,6);
  GTextLine(60,172,560,"Strefy Wojny");

  GButton(30,192,2714,2715,7);
  GTextLine(60,192,560,"Swiatynie");

  GButton(30,212,2714,2715,8);
  GTextLine(60,212,560,"Eventy");
  
  GButton(30,232,2714,2715,9);
  GTextLine(60,232,560,"Nowa mapa");

  GButton(30,252,2714,2715,10);
  GTextLine(60,252,560,"Nowa mapa - miasta");

  GButton(30,272,2714,2715,11);
  GTextLine(60,272,560,"eremen");

  GButton(30,292,2714,2715,12);
  GTextLine(60,292,560,"goqsane");

  GButton(30,312,2714,2715,13);
  GTextLine(60,312,560,"instancje");

  var Res := GSend(who)[0];

  var SType := 0,i := 1,b := 1,Elem;

  case(Res)
    1: SType   := "City";
    2: SType   := "Dungeon";
    3: SType   := "Tombs";
    4: SType   := "Cementary";
    5: SType   := "Other";
    6: SType   := "WarZone";
    7: SType   := "Temples";
    8: SType   := "Events";
    9: SType   := "NewMap";
    10: SType := "NewMapCities";
    11: SType := "Eremen";
    12: SType := "goqsane";
    13: SType := "Instances";
	
  endcase

  if(!SType)
    return 0;
  endif

  GInit(50,50);
  GPage();
  GResPic(0,0,3600,420,440);
  GTextLine(100,20,550,"Wybierz lokacje");
  GResPic(40,50,BKGND_GOLDTRIM,340,350);
  GAddPageData(385,50,2650,2651,385,379,2648,2647);

  foreach ElemName in AllElems
    Elem := GoLocCfg[ElemName];
    
    if(Elem.Type == SType)
      GCheckNextPage(15);
      GButton(60,72+(GNPCount()*20),2714,2715,b);
      GTextLine(90,72+(GNPCount()*20),550,i+".");
      GTextLine(120,72+(GNPCount()*20),560,ConvName(ElemName));
      GTextLine(250,72+(GNPCount()*20),570,Elem.Coords);
      i := i + 1;
    endif
    b := b + 1;

  endforeach

  Res := GSend(who)[0];
  
  if(Res)
    return AllElems[Res];
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GoToLoc(who,LocN)

  var Coords := SplitWords(GoLocCfg[LocN].Coords);
  var Realm  := GoLocCfg[LocN].Realm;
  
  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif

  if(Coords)
    SendSysMessage(who,"Przenoszenie do "+ConvName(LocN),FONT_NORMAL,COLOR_GREEN);
    LogMsg(GOTO_LOG,who,{0,CInt(Coords[1]),CInt(Coords[2]),CInt(Coords[3]), Realm});
    MoveObject(who,CInt(Coords[1]),CInt(Coords[2]),CInt(Coords[3]),Realm,MOVECHAR_FORCELOCATION, MOVETYPE_CMD);
  endif

endfunction

//-----------------------------------------------------------------------------

function ConvName(Name)

  var NewName := "";
  
  while(Name["_"])
    Name["_"] := " ";
  endwhile
  
  foreach Sp in SplitWords(Name)
    Sp[1] := Upper(Sp[1]);
    if(NewName)
      NewName := NewName + " " + Sp;
    else
      NewName := Sp;
    endif
  endforeach
  
  return NewName;

endfunction

//-----------------------------------------------------------------------------

function ShowSets(who,text)

  var SCfg   := ReadConfigFile(ISETS_CFG);
  var Items  := GetConfigStringKeys(SCfg);

  var SetItem;

  foreach Item in Items
    if(lower(Item) == lower(text[2]))
      SetItem := Item;
      break;
    endif
  endforeach

  if(!SetItem)
    foreach Item in Items
      if(lower(Item)[lower(text[2])])
        SetItem := Item;
        break;
      endif
    endforeach
  endif

  var Amt := 50;

  if(CInt(text[2]))
    Amt := CInt(text[2]);
  elseif(CInt(text[3]))
    Amt := CInt(text[3]);
  endif

  if(!SetItem)
    GInit(30, 30);
    GMultiHeaderInit();
    GResPic(0, 0, 3600, 700, 460);
    GAddPageData(668,16,2650,2651,668,428,2648,2647);
    GMultiPage();

    var i := 1;

    foreach Item in Items
      GCheckNextPage(20);
      GTextLine(20, 30+GNPCount()*20, 1419, i);
      GButton(50, 30+GNPCount()*20, 2714, 2715, i);
      GTextLine(80, 30+GNPCount()*20, 550, Item);
      i := i + 1;
    endforeach

    var Res := CInt(GSend(who)[0]);
    if(Res)
      SetItem := Items[Res];
    endif
  endif

  if(!SetItem)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  elseif(!SCfg[SetItem])
    SendSysMessage(who,"Bledna nazwa grupy.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Cont, Loc;

  if(CombineWords(text)["trg"])
    Loc := LocTarget(who);
    if(!Loc)
      SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
      return;
    endif
    Cont := CreateItemAtLoc(Loc.x,Loc.y,Loc.z,CInt(SCfg[SetItem].Cont),who.realm);
  elseif(SCfg[SetItem].ItemStack)
    Cont := CreateItemInPack(who,CInt(SCfg[SetItem].Cont), Amt);
  else
    Cont := CreateItemInPack(who,CInt(SCfg[SetItem].Cont));
  endif

  if(!Cont)
    SendSysMessage(who,"error : "+Cont,FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(Cont.isa(POLCLASS_CONTAINER))
    SetName(Cont,SetItem);
  endif

  var CAmt,Item,i;

  if(Cont)
    var Objs := GetConfigStringArray(SCfg[SetItem],"Obj");
    foreach Obj in Objs
      Obj := SplitWords(Obj);
      if(!CInt(Obj[3]))
        CAmt := 1;
      else
        CAmt := Amt;
      endif

      for(i:=CInt(Obj[1]);i<=CInt(Obj[2]);i:=i+1)
        Item := CreateItemInCont(Cont,i,CAmt);
        if(!Item)
          Item := CreateItemInPack(who,i,CAmt);
          MoveItemToContainer(Item,Cont);
        endif
      endfor
    endforeach
    var Props := GetConfigStringArray(SCfg[SetItem],"Prop");
    if(Props.size() and Props)
      foreach Prop in Props
        Prop := SplitWords(Prop);
        if(CInt(Prop[2]))
          Prop[2] := CInt(Prop[2]);
        endif
        SetObjProperty(Cont,Prop[1],Prop[2]);
      endforeach
    endif
  endif

  SendSysMessage(who,"Stworzono SetItema: "+SetItem,FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------
