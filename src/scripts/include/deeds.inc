//--------------------------------------------------------------------------------

include "include/const/cfgs";
include "include/items";

//--------------------------------------------------------------------------------

function BuildFromTemplate(who,Temp,x,y,z,Realm := 0)

  UnloadConfigFile(MDEEDS_CFG);
  var DCfg     := ReadConfigFile(MDEEDS_CFG);

  if(!DCfg)
    LogError("BuildFromTemplate: Nie mozna bylo odnalezc konfiga deedow!");
    return 0;
  endif

  if(!DCfg[Temp])
    SendSysMessage(who,"Template nie istnieje.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  var Num := 0,Item,Multis := 0;
  var Items  := GetConfigStringArray(DCfg[Temp],"Item");
  var Multi  := GetConfigStringArray(DCfg[Temp],"Multi");
  
  if(!Realm)
    Realm := who.realm;
    if(!Realm)
      Realm := _DEFAULT_REALM;
    endif
  endif
  
  if(Items.size() > 50)
    if(!YesNo(who,"Na pewno?"))
      return 0;
    endif
  endif
  
  if(Multi.size() and !IsAdmin(who))
    SendSysMessage(who,"Nie mozesz tego postawic.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  foreach ItemSpec in Items
    ItemSpec := SplitWords(ItemSpec);
    Item     := CreateItemAtLoc(x+CInt(ItemSpec[5]),y+CInt(ItemSpec[6]),z+CInt(ItemSpec[7]),CInt(ItemSpec[1]),1,Realm);
    if(Item)
      if(CInt(ItemSpec[2]))
        Item.color := CInt(ItemSpec[2]);
      endif
      if(CInt(ItemSpec[3]) != CInt(ItemSpec[1]))
        Item.graphic := CInt(ItemSpec[3]);
      endif
      if(ItemSpec[4])
        while(ItemSpec[4]["_"])
          ItemSpec[4]["_"] := " ";
        endwhile
        SetName(Item,ItemSpec[4]);
      endif
      Item.movable := 0;
      Num := Num + 1;
    endif
  endforeach
  
  foreach ItemSpec in Multi
    ItemSpec := SplitWords(ItemSpec); 
    Item     := CreateMultiAtLocation(x+CInt(ItemSpec[2]),y+CInt(ItemSpec[3]),z+CInt(ItemSpec[4]),CInt(ItemSpec[1]),CRMULTI_IGNORE_ALL,Realm);
    Multis := Multis + 1;
  endforeach

  SendSysMessage(who,"Stworzono "+Num+" przedmiotow "+Multis+", multi. Template - "+Temp,FONT_NORMAL,COLOR_GREEN);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetTemplateItems(Temp)

  UnloadConfigFile(MDEEDS_CFG);
  var DCfg  := ReadConfigFile(MDEEDS_CFG);
  var Items := GetConfigStringArray(DCfg[Temp],"Item");
  
  foreach Item in Items
    Item := CInt(SplitWords(Item)[1]);
  endforeach
  
  return Items;

endfunction

//--------------------------------------------------------------------------------

function SelectTemplate(who)

  var DCfg  := ReadConfigFile(MDEEDS_CFG);
  var Temps := GetConfigStringKeys(DCfg);
  var i     := 1;
  var Realm;

  GInit(30, 30);
  GMode(GMODE_MULTIHD, GSMODE_NORMAL);
  GResPic(0, 0, 3600, 450,450);
  GMultiPage();
  GAddPageData(415,20,2650,2651,415,413,2648,2647);

  foreach Temp in Temps
    GCheckNextPage(20);
    GButton(16, GNPCount()*20+20, 2714, 2715, i);
    GTextLine(38, GNPCount()*20+20, 1212, i);
    GTextLine(90 , GNPCount()*20+20, 670, Temp);
    GTextLine(240, GNPCount()*20+20, 490, GetConfigStringArray(DCfg[Temp], "Item").size());
    Realm := DCfg[Temp].Realm;
    if(!Realm)
      Realm := _DEFAULT_REALM;
    endif
    GTextLine(290, GNPCount()*20+20, 550, Realm);
    i := i + 1;
  endforeach

  var Res := GSend(who);

  if(Res[0])
    return Temps[Res[0]];
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

