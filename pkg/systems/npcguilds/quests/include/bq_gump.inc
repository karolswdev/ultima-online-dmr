//-----------------------------------------------------------------------------
// Modified: 2005-05-24
//-----------------------------------------------------------------------------

include "include/gumps";
include "include/clock";
include "include/gump_utils";

include ":npcguilds:shared";

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Cities gump
//-----------------------------------------------------------------------------
//=============================================================================

function ChooseQuestCities(who, CurCities, Flags := 0)

  GInit(20, 20);
  GResPic(0, 0, 3600, 480, 410);
  GTextLine(30, 20, 999, "Dostepne miasta:");
  GButton(350, 20, 2714, 2715, 2);
  if(Flags & BQG_ADDABLE)
    GTextLine(370, 20, 560, "Wszystkie");
  else
    GTextLine(370, 20, 560, "Wroc");
  endif
  
  GButton(350, 365, 2714, 2715, 1);
  GTextLine(370, 365, 560, "Wybierz");

  var i := 1;
  var w := 0, h := 0;
  var Cities := GetCities();

  foreach City in Cities
    var Align := GetCityAlignment(City);
    GCheckBox(30+w, 53+h*30, 0x867, 0x869, 10+i, City in CurCities);
    GTextLine(65+w, 58+h*30, __IfElse(Align == CITYAL_NEUTRAL, 999, __IfElse(Align == CITYAL_EVIL, 1209, 560)), City);
    i += 1;
    h += 1;

    if(h > 10)
      w += 150;
      h := 0;
    endif

  endforeach

  var Res := GSend(who);
  var RetCities := array;

  if(CInt(Res[0]) != 2)
    for(i:=1; i<=Cities.size(); i+=1)
      if(Res[10+i])
        RetCities.append(Cities[i]);
      endif
    endfor
  endif

  return RetCities;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Build Quest Element Info (Created, Modified, Usage)
//--------------------------------------------------------------------------------
//================================================================================

function BuildQuestElemModInfo(Elem, sX, sY)

  var CrInfo  := Elem.GetProp(QAPROP_CRINFO);
  var UpInfo  := Elem.GetProp(QAPROP_UPINFO);
  var UseInfo := Elem.GetProp(QAPROP_USEINFO);

  BuildQuestElemModSegInfo(CrInfo,  "Stworzone:",    sX, sY);
  BuildQuestElemModSegInfo(UpInfo,  "Modyfikowane:", sX, sY+50);
  BuildQuestElemModSegInfo(UseInfo, "Uzywane:",      sX, sY+100);

endfunction

//--------------------------------------------------------------------------------

function BuildQuestElemModSegInfo(byref Info, Title, sX, sY)

  if(Info)
    var Person := FindPlayerBySerial(Info[3]);
    GTextLine(sX, sY, 999, Title);
    if(Info[5])
      GTextLine(sX+250, sY, 670, Info[5]);
    endif
    GTextLine(sX, sY+20, 1419, CUnixTime(Info[1]));
    if(Person)
      GTextLine(sX+140, sY+20, 670, Person.name);
    endif
    if(Info[2])
      GTextLine(sX+290, sY+20, 670, Info[2]);
    endif
    if(Info[4])
      GTextLine(sX+360, sY+20, 670, Info[4]);
    endif
  endif

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Quest Target Modification
//--------------------------------------------------------------------------------
//================================================================================

function AddQuestTarget(byref Data)

  if(!Data.SubType)
    SendSysMessage(who, "Nie okreslono PodTypu!", FONT_NORMAL, COLOR_RED);
    return;
  endif

  if(Data.Flags & QSTF_TARG_GROUP)
    var NewTarg;
    InitQuestTarget(Data, NewTarg);
    Data.Targ.append(NewTarg);
  else
    InitQuestTarget(Data, Data.Targ);
  endif

endfunction

//-----------------------------------------------------------------------------

function InitQuestTarget(byref Data, byref Targ)

  case(Data.SubType)
    QSTYPE_NPC:              InitQTargNpc(Targ);
    QSTYPE_EXNPC:            InitQTargExNpc(Targ);
    QSTYPE_ITEM:             InitQTargItem(Targ);
    QSTYPE_PLAYER:           InitQTargPlayer(Targ);
    QSTYPE_LOC:              InitQTargLoc(Targ);
    QSTYPE_FROMNPC:          InitQTargFromNpc(Targ);
    QSTYPE_LOOTNPC:          InitQTargLootNpc(Targ);
    QSTYPE_ALCHEMY:          
    QSTYPE_BLACKSMITH:
    QSTYPE_BOWCRAFTING:
    QSTYPE_CARPENTING:
    QSTYPE_TINKERING:
    QSTYPE_TAILORING:
    QSTYPE_COOKING:
    QSTYPE_INSCRIPTION:
    QSTYPE_MAGICITEMMAKING:
    QSTYPE_CARTOGRAPHY:      InitQTargSkill(Targ);
    QSTYPE_MINE:
    QSTYPE_CHOP:
    QSTYPE_FISH:
    QSTYPE_HERB:
    QSTYPE_FARMING:          InitQTargMine(Targ);
    QSTYPE_HOUSE:            InitQTargHouse(Targ);
    QSTYPE_LOCKTAKE:         InitQTargLockTake(Targ);
  endcase

endfunction

//-----------------------------------------------------------------------------

function BuildQuestTargetTitle(byref Data, sX, sY)

  if(!Data.SubType)
    return;
  endif

  GTextLine(sX, sY, 999, "Nr");

  case(Data.SubType)
    QSTYPE_NPC:              GTextLine(sX+30,  sY, 999, "Template*");
                             GTextLine(sX+150, sY, 999, "Kolor");
                             GTextLine(sX+200, sY, 999, "Imie");
                             
    QSTYPE_EXNPC:            GTextLine(sX+30,  sY, 999, "Template%");
                             GTextLine(sX+150, sY, 999, "Plec");
                             GTextLine(sX+200, sY, 999, "Skrypt%");

    QSTYPE_ITEM:             GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+90,  sY, 999, "ToObj");
                             GTextLine(sX+150, sY, 999, "Kolor");
                             GTextLine(sX+200, sY, 999, "Nazwa");

    QSTYPE_PLAYER:           GTextLine(sX+30,  sY, 999, "SkID");
                             GTextLine(sX+90,  sY, 999, "SkVal");

    QSTYPE_LOC:              GTextLine(sX+30,  sY, 999, "x*");
                             GTextLine(sX+80,  sY, 999, "y*");
                             GTextLine(sX+130, sY, 999, "z*");
                             GTextLine(sX+180, sY, 999, "Realm*");

    QSTYPE_FROMNPC:          GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+70,  sY, 999, "Kolor");
                             GTextLine(sX+110, sY, 999, "Nazwa");
                             GTextLine(sX+170, sY, 999, "Template%");
                             GTextLine(sX+240, sY, 999, "Skrypt%");
                             GTextLine(sX+300, sY, 999, "Plec");

    QSTYPE_LOOTNPC:          GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+70,  sY, 999, "Kolor");
                             GTextLine(sX+110, sY, 999, "Nazwa");
                             GTextLine(sX+170, sY, 999, "Template*");
                             GTextLine(sX+240, sY, 999, "Imie");

    QSTYPE_ALCHEMY:          
    QSTYPE_BLACKSMITH:
    QSTYPE_BOWCRAFTING:
    QSTYPE_CARPENTING:
    QSTYPE_TINKERING:
    QSTYPE_TAILORING:
    QSTYPE_COOKING:
    QSTYPE_INSCRIPTION:
    QSTYPE_CARTOGRAPHY:
    QSTYPE_MAGICITEMMAKING:  GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+90,  sY, 999, "ToObj");
                             GTextLine(sX+150, sY, 999, "SkVal");
                             GTextLine(sX+210, sY, 999, "ExpCh");
                             GTextLine(sX+250, sY, 999, "MatObj");
    QSTYPE_MINE:
    QSTYPE_CHOP:
    QSTYPE_FISH:
    QSTYPE_HERB:
    QSTYPE_FARMING:          GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+90,  sY, 999, "ToObj");
                             GTextLine(sX+150, sY, 999, "SkVal");

    QSTYPE_HOUSE:            ;

    QSTYPE_LOCKTAKE:         GTextLine(sX+30,  sY, 999, "Obj*");
                             GTextLine(sX+90,  sY, 999, "ToObj");
                             GTextLine(sX+150, sY, 999, "Kolor");
                             GTextLine(sX+200, sY, 999, "Nazwa");
                             GTextLine(sX+270, sY, 999, "ContObj*");
  endcase

endfunction

//-----------------------------------------------------------------------------

function BuildQuestTarget(byref Data, Nr, byref Targ, Flags, sX, sY, Id)

  if(!Targ)
    return;
  endif

  GTextLine(sX, sY, 1419, Nr);
  if(Flags & BQG_EDITABLE)
    GButton(sX+325, sY+3, 2117, 2118, Id);
  endif

  case(Data.SubType)
    QSTYPE_NPC:              if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 100, 20, FCLR_TEBOX, Id, Targ.Template);
                               GTextEntry(sX+150, sY, 35,  20, FCLR_TEBOX, Id+20, Targ.Color);
                               GTextEntry(sX+200, sY, 120, 20, FCLR_TEBOX, Id+40, Targ.Name);
                             else
                               GTextLine(sX+30,  sY, FCLR_TEBOX, Targ.Template);
                               GTextLine(sX+150, sY, FCLR_TEBOX, Targ.Color);
                               GTextLine(sX+200, sY, FCLR_TEBOX, Targ.Name);
                             endif
                             
    QSTYPE_EXNPC:            if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 100, 20, FCLR_TEBOX, Id,    Targ.Template);
                               GTextEntry(sX+150, sY, 35,  20, FCLR_TEBOX, Id+20, Targ.Gender);
                               GTextEntry(sX+200, sY, 100, 20, FCLR_TEBOX, Id+40, Targ.Script);
                             else
                               GTextLine(sX+30,  sY, FCLR_TEBOX, Targ.Template);
                               GTextLine(sX+150, sY, FCLR_TEBOX, Targ.Gender);
                               GTextLine(sX+200, sY, FCLR_TEBOX, Targ.Script);
                             endif

    QSTYPE_ITEM:             if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 45, 20, FCLR_TEBOX, Id, Targ.ObjType);
                               GTextEntry(sX+90,  sY, 45, 20, FCLR_TEBOX, Id+20, Targ.ToObjType);
                               GTextEntry(sX+150, sY, 45, 20, FCLR_TEBOX, Id+40, Targ.Color);
                               GTextEntry(sX+200, sY, 100,20, FCLR_TEBOX, Id+60, Targ.Name);
                             else
                               GTextLine(sX+30,   sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+90,   sY, FCLR_TEBOX, Targ.ToObjType);
                               GTextLine(sX+150,  sY, FCLR_TEBOX, Targ.Color);
                               GTextLine(sX+200,  sY, FCLR_TEBOX, Targ.Name);
                             endif

    QSTYPE_PLAYER:           if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 35, 20, FCLR_TEBOX, Id,    Targ.SkID);
                               GTextEntry(sX+90,  sY, 35, 20, FCLR_TEBOX, Id+20, Targ.SkVal);
                             else
                               GTextLine(sX+30,  sY, FCLR_TEBOX, Targ.SkID);
                               GTextLine(sX+90,  sY, FCLR_TEBOX, Targ.SkVal);
                             endif

    QSTYPE_LOC:              if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 45,  20, FCLR_TEBOX, Id,    Targ.x);
                               GTextEntry(sX+80,  sY, 45,  20, FCLR_TEBOX, Id+20, Targ.y);
                               GTextEntry(sX+130, sY, 45,  20, FCLR_TEBOX, Id+40, Targ.z);
                               GTextEntry(sX+180, sY, 80,  20, FCLR_TEBOX, Id+60, Targ.Realm);
                             else
                               GTextLine(sX+30,  sY, FCLR_TEBOX, Targ.x);
                               GTextLine(sX+80,  sY, FCLR_TEBOX, Targ.y);
                               GTextLine(sX+130, sY, FCLR_TEBOX, Targ.z);
                               GTextLine(sX+180, sY, FCLR_TEBOX, Targ.Realm);
                             endif

    QSTYPE_FROMNPC:          if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+20,  sY, 35,  20, FCLR_TEBOX, Id,    Targ.ObjType);
                               GTextEntry(sX+65,  sY, 35,  20, FCLR_TEBOX, Id+20, Targ.Color);
                               GTextEntry(sX+105, sY, 60,  20, FCLR_TEBOX, Id+40, Targ.Name);
                               GTextEntry(sX+170, sY, 60,  20, FCLR_TEBOX, Id+60, Targ.Template);
                               GTextEntry(sX+240, sY, 80,  20, FCLR_TEBOX, Id+80, Targ.Script);
                               GTextEntry(sX+300, sY, 80,  20, FCLR_TEBOX, Id+100,Targ.Gender);
                             else
                               GTextLine(sX+20,  sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+65,  sY, FCLR_TEBOX, Targ.Color);
                               GTextLine(sX+105, sY, FCLR_TEBOX, Targ.Name);
                               GTextLine(sX+170, sY, FCLR_TEBOX, Targ.Template);
                               GTextLine(sX+240, sY, FCLR_TEBOX, Targ.Script);
                               GTextLine(sX+300, sY, FCLR_TEBOX, Targ.Gender);
                             endif

    QSTYPE_LOOTNPC:          if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+20,  sY, 35,  20, FCLR_TEBOX, Id,    Targ.ObjType);
                               GTextEntry(sX+65,  sY, 35,  20, FCLR_TEBOX, Id+20, Targ.Color);
                               GTextEntry(sX+105, sY, 60,  20, FCLR_TEBOX, Id+40, Targ.Name);
                               GTextEntry(sX+170, sY, 60,  20, FCLR_TEBOX, Id+60, Targ.Template);
                               GTextEntry(sX+240, sY, 80,  20, FCLR_TEBOX, Id+80, Targ.NpcName);
                             else
                               GTextLine(sX+20,  sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+65,  sY, FCLR_TEBOX, Targ.Color);
                               GTextCrop(sX+105, sY, 60, 20, FCLR_TEBOX, Targ.Name);
                               GTextLine(sX+170, sY, FCLR_TEBOX, Targ.Template);
                               GTextCrop(sX+240, sY, 80, 20, FCLR_TEBOX, Targ.NpcName);
                             endif

    QSTYPE_ALCHEMY:          
    QSTYPE_BLACKSMITH:
    QSTYPE_BOWCRAFTING:
    QSTYPE_CARPENTING:
    QSTYPE_TINKERING:
    QSTYPE_TAILORING:
    QSTYPE_COOKING:
    QSTYPE_INSCRIPTION:
    QSTYPE_CARTOGRAPHY:
    QSTYPE_MAGICITEMMAKING:  if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 45, 20, FCLR_TEBOX, Id,    Targ.ObjType);
                               GTextEntry(sX+90,  sY, 45, 20, FCLR_TEBOX, Id+20, Targ.ToObjType);
                               GTextEntry(sX+150, sY, 45,  20, FCLR_TEBOX, Id+40, CStr(Targ.SkVal));
                               GTextEntry(sX+210, sY, 45,  20, FCLR_TEBOX, Id+60, CStr(Targ.ExpChance));
                               GTextEntry(sX+250, sY, 45,  20, FCLR_TEBOX, Id+80, CStr(Targ.MatObjType));
                             else
                               GTextLine(sX+30,   sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+90,   sY, FCLR_TEBOX, Targ.ToObjType);
                               GTextLine(sX+150,  sY, FCLR_TEBOX, Targ.SkVal);
                               GTextLine(sX+210,  sY, FCLR_TEBOX, Targ.ExpChance);
                               GTextLine(sX+250,  sY, FCLR_TEBOX, Targ.MatObjType);
                             endif
    QSTYPE_MINE:
    QSTYPE_CHOP:
    QSTYPE_FISH:
    QSTYPE_HERB:
    QSTYPE_FARMING:          if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 45,  20, FCLR_TEBOX, Id, Targ.ObjType);
                               GTextEntry(sX+90,  sY, 45,  20, FCLR_TEBOX, Id+20, Targ.ToObjType);
                               GTextEntry(sX+150, sY, 45,  20, FCLR_TEBOX, Id+40, CStr(Targ.SkVal));
                             else
                               GTextLine(sX+30,   sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+90,   sY, FCLR_TEBOX, Targ.ToObjType);
                               GTextLine(sX+150,  sY, FCLR_TEBOX, Targ.SkVal);
                             endif
    QSTYPE_HOUSE:            
    

    QSTYPE_LOCKTAKE:         if(Flags & BQG_EDITABLE)
                               GTextEntry(sX+30,  sY, 45, 20, FCLR_TEBOX, Id, Targ.ObjType);
                               GTextEntry(sX+90,  sY, 45, 20, FCLR_TEBOX, Id+20, Targ.ToObjType);
                               GTextEntry(sX+150, sY, 45, 20, FCLR_TEBOX, Id+40, Targ.Color);
                               GTextEntry(sX+200, sY, 100,20, FCLR_TEBOX, Id+60, Targ.Name);
                               GTextEntry(sX+270, sY, 100,20, FCLR_TEBOX, Id+80, Targ.ContObjType);
                             else
                               GTextLine(sX+30,   sY, FCLR_TEBOX, Targ.ObjType);
                               GTextLine(sX+90,   sY, FCLR_TEBOX, Targ.ToObjType);
                               GTextLine(sX+150,  sY, FCLR_TEBOX, Targ.Color);
                               GTextLine(sX+200,  sY, FCLR_TEBOX, Targ.Name);
                               GTextLine(sX+270,  sY, FCLR_TEBOX, Targ.ContObjType);
                             endif
  endcase

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Quest Price Modification
//--------------------------------------------------------------------------------
//================================================================================

function AddQuestPrice(byref Data)

  var NewTarg;
  InitQuestPrice(NewTarg);
  Data.Price.append(NewTarg);

endfunction

//-----------------------------------------------------------------------------

function BuildQuestPriceTitle(sX, sY)

  GTextLine(sX, sY, 999, "Nr");

  GTextLine(sX+20,  sY, 999, "Obj*");
  GTextLine(sX+70,  sY, 999, "ToObj");
  GTextLine(sX+120, sY, 999, "Ilosc*");
  GTextLine(sX+170, sY, 999, "Sz*");
  GTextLine(sX+200, sY, 999, "Kolor");
  GTextLine(sX+235, sY, 999, "Nazwa");

endfunction

//-----------------------------------------------------------------------------

function BuildQuestPrice(Nr, byref Price, Flags, sX, sY, Id)

  if(!Price)
    return;
  endif

  GTextLine(sX, sY, 1419, Nr);

  if(Flags & BQG_EDITABLE)
    GButton(sX+325, sY+3, 2117, 2118, Id);
    GTextEntry(sX+20,  sY, 45, 20, FCLR_TEBOX, Id,    Price.ObjType);
    GTextEntry(sX+70,  sY, 45, 20, FCLR_TEBOX, Id+20, Price.ToObjType);
    GTextEntry(sX+120, sY, 45, 20, FCLR_TEBOX, Id+40, Price.Amount);
    GTextEntry(sX+170, sY, 32, 20, FCLR_TEBOX, Id+60, Price.Chance);
    GTextEntry(sX+200, sY, 32, 20, FCLR_TEBOX, Id+80, Price.Color);
    GTextEntry(sX+235, sY, 100,20, FCLR_TEBOX, Id+100,Price.Name);
  else
    GTextLine(sX+20,   sY, FCLR_TEBOX, Price.ObjType);
    GTextLine(sX+70,   sY, FCLR_TEBOX, Price.ToObjType);
    GTextLine(sX+120,  sY, FCLR_TEBOX, Price.Amount);
    GTextLine(sX+170,  sY, FCLR_TEBOX, Price.Chance);
    GTextLine(sX+200,  sY, FCLR_TEBOX, Price.Color);
    GTextLine(sX+235,  sY, FCLR_TEBOX, Price.Name);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Types Gumps
//-----------------------------------------------------------------------------
//=============================================================================

function ShowChooseTypeGump(who, byref Data)

  var Type := ChooseTypeGump(who);

  if(Type > 0)
    Data.Type := Type;
    var AvSubTypes := GetQuestAvSubTypes(Data.Type);

    if(!Data.SubType)
      Data.SubType := AvSubTypes[1];
      if(Data.Targ)
        if(Data.Flags & QSTF_TARG_GROUP)
          Data.Targ := array;
        else
          Data.Targ := 0;
        endif
      endif
    elseif(!(Data.SubType in AvSubTypes))
      Data.SubType := AvSubTypes[1];
      if(Data.Targ)
        if(Data.Flags & QSTF_TARG_GROUP)
          Data.Targ := array;
        else
          Data.Targ := 0;
        endif
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ShowChooseSubTypeGump(who, byref Data)

  if(Data.Type)
    var SubType := ChooseSubTypeGump(who, GetQuestAvSubTypes(Data.Type));

    if(SubType > 0)
      if(Data.SubType != SubType)
        if(Data.Flags & QSTF_TARG_GROUP)
          Data.Targ := array;
        else
          Data.Targ := 0;
        endif
      endif
      Data.SubType := SubType;
    endif
  else
    SendSysMessage(who, "Wybierz najpierw Typ.", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// SubGumps
//-----------------------------------------------------------------------------
//=============================================================================

function ChooseTypeGump(who)

  BQGInit("Typy Questow:", 280, 280);

  var i;

  for(i:=1;i<=QTYPE_MAXID;i:=i+1)
    GButton(30, 43+i*20, 2117, 2118, i);
    GTextLine(50, 40+i*20, 560, GetQuestTypeName(i));
  endfor

  return CInt(GSend(who).butid);

endfunction

//-----------------------------------------------------------------------------

function ChooseSubTypeGump(who, AvSubTypes := 0)

  BQGInit("PodTypy Questow:", 280, 320);

  var i,c := 1;
  var Show;

  for(i:=1;i<=QSTYPE_MAXID;i:=i+1)
    if(AvSubTypes)
      if(i in AvSubTypes)
        Show := 1;
      else
        Show := 0;
      endif
    else
      Show := 1;
    endif

    if(Show)
      GButton(30, 43+c*20, 2117, 2118, i);
      GTextLine(50, 40+c*20, 560, GetQuestSubTypeName(i));
      c := c + 1;
    endif
  endfor

  return CInt(GSend(who).butid);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Group Selection Gump
//-----------------------------------------------------------------------------
//=============================================================================

function BuildQuestGroups(Groups)

  BQGInit("Grupy: "+Groups.size(), 600, 500, 0, 1);
  GAddPageData(565,18,2650,2651,565,467,2648,2647);

  GTextLine(30,  40, 999, "Wybierz");
  GTextLine(100, 40, 999, "Nazwa");
  GMultiPage();

  var i := 1;

  foreach GroupName in Groups
    GCheckNextPage(20);
    GTextLine(30, 60+GNPCount()*20, 1419, i);
    GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    GTextLine(100,60+GNPCount()*20, 550, GroupName);
    i := i + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function SelectQuestGroup(who)

  var qCfg := ReadConfigFile(QUESTS_CFG);
  var Keys := GetConfigStringArray(qCfg["Groups"], "Group");
  var Res;

  BuildQuestGroups(Keys);

  while(IsLoggedIn(who))
    Res := CInt(GSend(who).butid);

    if(Res != 100000)
      if(Res > 0)
        return Keys[Res];
      else
        return 0;
      endif
    endif

    BuildQuestGroups(Keys);
  endwhile

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Selection Gump
//-----------------------------------------------------------------------------
//=============================================================================

function SelectQuest(QuestGroup, who, QSet := 0, QEnd := 0)

  var Data := GetQuestData(QuestGroup);
  var Keys := GetDataKeys(Data, QUEST_TITLE, BQG_ZEROFILL, QSet, QEnd);
  var Res;

  Keys.sort();
  Keys.reverse();

  BuildQuests(Keys, Data);

  while(IsLoggedIn(who))
    Res := CInt(GSend(who).butid);

    if(Res != 100000)
      if(Res > 10000)
        BuildQuestModInfo(Data, CInt(UnZeroFill(Keys[Res-10000])));
        continue;
      elseif(Res > 0)
        return CInt(UnZeroFill(Keys[Res]));
      else
        return 0;
      endif
    endif

    BuildQuests(Keys, Data);
  endwhile

  return 0;

endfunction

//-----------------------------------------------------------------------------

function BuildQuests(byref Keys, byref Data)

  BQGInit("Questy: "+Keys.size(), 600, 500, 0, 1);
  GAddPageData(565,18,2650,2651,565,467,2648,2647);

  GTextLine(30,  40, 999, "Wybierz");
  GTextLine(100, 40, 999, "Tytul");
  GTextLine(350, 40, 999, "Lvl");
  GTextLine(380, 40, 999, "Typ");
  GTextLine(460, 40, 999, "PodTyp");
  GTextLine(558, 40, 999, "Info");
  GMultiPage();

  var Quest, i := 1;

  foreach Key in Keys
    GCheckNextPage(20);
    Quest := Data.FindElement(QUEST_TITLE+UnZeroFill(Key));
    GTextLine(30, 60+GNPCount()*20, 1419, Key);
    GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    GButton(560, 63+GNPCount()*20, 2117, 2118, 10000+i);
    GTextLine(100,60+GNPCount()*20, 550, Quest.GetProp(QPROP_TITLE));
    GTextLine(350,60+GNPCount()*20, 550, Quest.GetProp(QPROP_LEVEL));
    GTextLine(380,60+GNPCount()*20, 550, GetQuestTypeName(Quest.GetProp(QPROP_TYPE)));
    GTextLine(460,60+GNPCount()*20, 550, GetQuestSubTypeName(Quest.GetProp(QPROP_SUBTYPE)));
    i := i + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function BuildQuestModInfo(byref Data, QId)

  var Quest := Data.FindElement(QUEST_TITLE+QId);

  BQGInit("Info: Quest ID: "+QID, 440, 330, 100000);
  GTextLine(30, 40, 999, "Nazwa:");
  GTextLine(80, 40, 550, Quest.GetProp(QPROP_TITLE));

  BuildQuestElemModInfo(Quest, 30, 80);

endfunction

//-----------------------------------------------------------------------------
