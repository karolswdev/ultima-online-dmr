//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Step 1
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

function BuildBackGump(byref Data)

  GInit(100,100);
  GMode(GMODE_NORMAL, GSMODE_PACKET);

  if(Data.Flags & CRF_CANT_QUIT)
    GNoMove();
    GNoDispose();
    GNoClose();
  endif


  GGumpPic(0, 0, 0xCE);
  GGumpPic(40, 0, 0xC9);
  GGumpPic(120, 0, 0xC9);
  GGumpPic(520, 0, 0xCF);

  GGumpPic(0,  40, 0xCA);
  GGumpPic(0,  100, 0xCA);

  var x,y;

  for(x:=0; x<4; x+=1)
    for(y:=0; y<3; y+=1)
      GGumpPic(40+(x*128), 40+(y*128), 0xC8);
    endfor
  endfor

  GGumpPic(520, 40, 0xCB);
  GGumpPic(520, 100, 0xCB);

  GGumpPic(0, 400, 0xCC);
  GGumpPic(40, 400, 0xE9);
  GGumpPic(120, 400, 0xE9);
  GGumpPic(520, 400, 0xCD);

  GTextLine(55, 50, 1310, "Krok "+Data.curStage+" z "+Data.maxStages);
  GGumpPic(50, 70, 0xBBF);
  GGumpPic(150, 70, 0xBBF);

  if(Data.curStage > 1)
    GButton(80, 380, 0x15A1, 0x15A3, 101);
    GTextLine(100, 380, 1150, "Wstecz");
  endif

  if(Data.curStage < Data.maxStages)
    GButton(245, 380, 0x15A4, 0x15A6, 100);
    GTextLine(200, 380, 1150, "Dalej");
  else
    GButton(245, 380, 0x15A4, 0x15A6, 100);
    GTextLine(195, 380, 1150, "Zakoncz");
  endif
  
  if(!(Data.Flags & CRF_CANT_QUIT))
    GButton(430, 376, 0x867, 0x869, 99);
    GTextLine(460, 380, 999, "Anuluj");
  endif

endfunction

//-----------------------------------------------------------------------------

function BuildCreationGump(byref Data)

  BuildBackGump(Data);

  GTextLine(170, 50, 1410, "Imie oraz klasa postaci");
  
  if(Data.Flags & CRF_NAME)
    GTextLine(50, 85, 1000, "Przemysl jakie imie chcesz miec.");
    GTextLine(50, 105, 1000, "Jest to bardzo wazny etap, poniewaz nie bedzie pozniej");
    GTextLine(50, 125, 1000, "mozliwosci zmienic imienia. Dozwolone sa tylko imiona fantasy.");
  else
    GTextLine(50, 85, 1000, "System klas oraz ras ulegl zmianie.");
    GTextLine(50, 105, 1000, "W zwiazku z tym klasa zostaje przypisana Tobie automatycznie.");
    GTextLine(50, 125, 1000, "Natomiast rase oraz kolor skory masz szanse wybrac ponownie.");
  endif

  var ClassColor := 0;

  case(Data.ClassId)
    CLASS_WARRIOR: ClassColor := 1210;
    CLASS_MAGE: ClassColor := 1319;
    CLASS_ARCHER: ClassColor := 1420;
  endcase
  
  GTextLine(92, 160, 1000, "Klasa:");
  BQGAddTextBox(140, 160, 150, 20, ClassColor, ClStr(Data.cfgClass.Title), 0);
  if(Data.Flags & CRF_CLASS)
    GButton(285, 158, 0x867, 0x869, 11);
  endif

  GTextLine(95, 190, 1000, "Imie:");
  BQGAddTextBox(140, 190, 150, 20, 0, Data.CharName, __IfElse(Data.Flags & CRF_NAME, BQG_EDITABLE, 0), 1, 5);

  GTextLine(92, 220, 1000, "Plec:");
  BQGAddTextBox(140, 220, 150, 20, 0, Data.GenderTitle, 0);
  if(Data.Flags & CRF_GENDER)
    GButton(285, 218, 0x867, 0x869, 10);
  endif

  GTextLine(95, 250, 1000, "Rasa:");
  BQGAddTextBox(140, 250, 150, 20, 0, ClStr(Data.cfgRace.Name), 0);
  if(Data.Flags & CRF_RACE)
    GButton(285, 248, 0x867, 0x869, 12);
  endif

  GTextLine(95, 280, 1000, "Kolor:");
  if(Data.Color)
    BQGAddTextBox(140, 280, 150, 20, Data.Color - CHAR_COLOR_BASE, "Aktualny", 0);
  else
    BQGAddTextBox(140, 280, 150, 20, 550, "Nie wybrany", 0);
  endif

  if(Data.Flags & CRF_COLOR)
    GButton(285, 278, 0x867, 0x869, 13);
  endif

  GGumpPic(430, 0, 0x708);

  if(Data.RaceId and Data.Gender and Data.Color)
    var CharPic := 0;
    var ClothPic := 0;

    case(Data.Gender)
      1: CharPic := CInt(Data.cfgFileRace[Data.RaceId].MaleGumpId);
         ClothPic := 0xEC35;
      2: CharPic := CInt(Data.cfgFileRace[Data.RaceId].FemaleGumpId);
         ClothPic := 0xEC1F;
    endcase

    GGumpPic(450, 20, CharPic, Data.Color - CHAR_COLOR_BASE);
    GGumpPic(450, 20, ClothPic);
  endif

  if(Data.cfgClass)
    GGumpPic(330, 155, Data.cfgClass.GumpIcon);
  else
    GGumpPic(330, 155, 0x15A9);
  endif

  if(Data.cfgRace)
    GResPic(330, 230, 0xBB8, 60, 60);
    GTilePic(340, 245, Data.cfgRace.SymbolTile);
  endif

endfunction

//-----------------------------------------------------------------------------

function SelectGenderGump(who)

  GInit(250, 250);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GResPic(0, 0, 0xA28, 250, 200);

  BQGAddTextBox(50, 50, 110, 20, 0, "Mezczyzna", 0);
  GButton(150, 45, 0x867, 0x869, 1);

  BQGAddTextBox(50, 120, 110, 20, 0, "Kobieta", 0);
  GButton(150, 115, 0x867, 0x869, 2);

  var Res := GSend(who).butid;

  if(Res)
    return Res;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SelectClassGump(who, byref Data)

  var Classes := GetConfigStringKeys(Data.cfgFileClass);

  GInit(100, 100);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GResPic(0, 0, 0xA28, 250, 300);

  var i := 0;

  foreach Class in Classes
    
    BQGAddTextBox(40, 50+(i*70), 120, 20, 0, ClStr(Data.cfgFileClass[Class].Title), 0);
    GButton(140, 30+(i*70), Data.cfgFileClass[Class].GumpIcon, Data.cfgFileClass[Class].GumpIcon+1, i+1);
    i += 1;

  endforeach

  var Res := GSend(who).butid;

  if(Res)
    return Classes[Res];
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function SelectRaceGump(who, byref Data)

  if(!Data.cfgClass)
    SendSysMessage(who, "Musisz wybrac najpierw klase.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Races := GetConfigStringArray(Data.cfgClass, "Race");

  GInit(100, 100);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();
  GResPic(0, 0, 0xA28, 280, 60+70*Races.size());
  GPage();

  var i := 0;

  foreach Race in Races

    BQGAddTextBox(50, 45+(i*70), 115, 40, 1310, ClStr(Data.cfgFileRace[Race].Name), 0);
    GTextLine(95, 65+(i*70), 1215, ClStr(Data.cfgFileRace[Race].EvilName));
    GResPic(170, 30+(i*70), 0xBB8, 60, 60);
    GTilePic(180, 45+(i*70), Data.cfgFileRace[Race].SymbolTile);
    GButton(150, 35+(i*70), 0x867, 0x869, i+1);
    GButtonPage(232, 45+(i*70), 0x15A4, 0x15A5, i+1);
    i += 1;

  endforeach

  foreach Race in Races
    var DescLines := GetConfigStringArray(Data.cfgFileRace[Race], "Description");
    var n := 0;

    GPage();
    GResPic(290, 0, 0xA28, 500, 390); 
    GTextLine(515, 25, 999, "Rasa: ");
    GTextLine(550, 25, 1420, ClStr(Data.cfgFileRace[Race].Name));

    foreach Line in DescLines
      GTextLine(320, 55+(n*22), 1000, ToUnicode(Line));
      n += 1;
    endforeach

  endforeach

  var Res := GSend(who).butid;

  if(Res)
    return Races[Res];
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function SelectColorGump(who, byref Data)

  if(!Data.RaceId or !Data.Gender)
    SendSysMessage(who, "Musisz wybrac najpierw rase i plec.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Colors := SplitWords(Data.cfgFileRace[Data.RaceId].Colors);
  var CharPic := 0;
  var ClothPic := 0;
  
  case(Data.Gender)
    1: CharPic := CInt(Data.cfgFileRace[Data.RaceId].MaleGumpId);
       ClothPic := 0xEC35;
    2: CharPic := CInt(Data.cfgFileRace[Data.RaceId].FemaleGumpId);
       ClothPic := 0xEC1F;
  endcase

  MakeIntArray(Colors);

  var curIdx := RetIndex(Colors, Data.Color);

  if(!curIdx)
    curIdx := 1;
  endif

  while(IsLoggedIn(who))
    GInit(100, 100);
    GMode(GMODE_NORMAL, GSMODE_PACKET);
    GResPic(0, 0, 0xA28, 350, 500);

    GTextLine(75, 35, 1000, "Dostepne kolory dla rasy "+Data.cfgFileRace[Data.RaceId].Name);
    GGumpPic(60, 60, 0x708);

    if(curIdx > 1)
      GButton(40, 250, 0x15A1, 0x15A3, curIdx-1);
    endif

    if(curIdx < Colors.size())
      GButton(300, 250, 0x15A4, 0x15A6, curIdx+1);
    endif

    GGumpPic(80, 80, CharPic, Colors[curIdx] - CHAR_COLOR_BASE);
    GGumpPic(80, 80, ClothPic);

    GTextLine(40, 440, 1420, "Kolor nr "+curIdx);

    GButton(220, 435, 0x867, 0x869, 1000);
    GTextLine(250, 440, 1000, "Wybierz!");

    var Res := CInt(GSend(who).butid);

    if(Res == 1000)
      break;
    elseif(!Res)
      return 0;
    else
      curIdx := Res;
    endif

  endwhile

  Data.Color := Colors[curIdx];

endfunction

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Step 2
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

function BuildSkillsGump(who, byref Data)

  var MaxStats := GetCreationMaxStats(who.acct);
  var MaxSkills := GetCreationMaxSkills(who.acct);

  BuildBackGump(Data);

  GTextLine(170, 50, 1410, "Umiejetnosci Twojej postaci");
  
  GTextLine(50, 85, 1000, "Kolejny etap to wybor umiejetnosci i statystyk.");
  GTextLine(50, 105, 1000, "Zastanow sie kim chcesz byc i co chcesz robic.");
  GTextLine(50, 125, 1000, "Pamietaj rowniez, ze umiejetnosci mozna zapomniec i nauczyc sie.");
  
  GTextLine(55, 160, 1000, "Statystyki:");
  
  GTextLine(155, 160, 999, "Suma:");
  GTextLine(195, 160, 1420, GetArraySum(Data.Stats) + "/" + MaxStats );
  GGumpPic(40, 180, 0xBB9);
  
  GTextLine(90, 208, 1320, "Sila:");
  GButton(120, 210, 0x15E7, 0x15E3, 11);
  GButton(160, 190, 0x15E4, 0x15E0, 13);
  GResPic(140, 205, 0xBB8, 55, 25);
  GTextLine(160, 208, 0, Data.Stats[1]);
  GButton(160, 231, 0x15E6, 0x15E2, 12);
  GButton(197, 210, 0x15E5, 0x15E1, 14);
  
  GTextLine(55, 268, 1320, "Zrecznosc:");
  GButton(122, 270, 0x15E7, 0x15E3, 21);
  GButton(160, 250, 0x15E4, 0x15E0, 23);
  GResPic(140, 265, 0xBB8, 55, 25);
  GTextLine(160, 268, 0, Data.Stats[2]);
  GButton(160, 291, 0x15E6, 0x15E2, 22);
  GButton(197, 270, 0x15E5, 0x15E1, 24);
  
  GTextLine(50, 328, 1320, "Inteligencja:");
  GButton(122, 330, 0x15E7, 0x15E3, 31);
  GButton(160, 310, 0x15E4, 0x15E0, 33);
  GResPic(140, 325, 0xBB8, 55, 25);
  GTextLine(160, 328, 0, Data.Stats[3]);
  GButton(160, 351, 0x15E6, 0x15E2, 32);
  GButton(197, 330, 0x15E5, 0x15E1, 34);

  GTextLine(265, 160, 1000, "Umiejetnosci:");
  GResPic(260, 180, 0xBB8, 240, 190);
  GTextLine(405, 160, 999, "Suma:");
  GTextLine(445, 160, 1420, GetArraySum(Data.Skills) + "/" + MaxSkills);

  if(Data.Skills.size() < CR_MAX_SKILLS)
    GButton(505, 185, 0xD2, 0xD3, 50);
    GGumpPic(507, 187, 0x151A);
  endif

  var i := 0;

  foreach SkillVal in (Data.Skills)
    
    GButton(270, 197+i*40, 0x1519, 0x1519, 200+_SkillVal_iter);
    GButton(292, 197+i*40, 0x15E7, 0x15E3, 300+_SkillVal_iter);
    GResPic(310, 192+i*40, 0xBB8, 35, 25);
    GTextLine(320, 195+i*40, 0, SkillVal);
    GButton(346, 197+i*40, 0x15E5, 0x15E1, 400+_SkillVal_iter);
    GTextLine(370, 195+i*40, 560, GetSkillConfigElem(_SkillVal_iter).Name);
    i += 1;

  endforeach

endfunction

//-----------------------------------------------------------------------------

function SelectClassSkillGump(who, byref Data, limitSkills := 0, bAll := 0)

  GInit();
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GStartPos(100, 100);
  GResPic(0, 0, 0xA28, 300, 500);
  GPage();
  GAddPageData(30, 450, 0x15A1, 0x15A3, 260, 450, 0x15A4, 0x15A6);

  GTextLine(40, 25, 1000, "Dostepne umiejetnosci dla klasy:");
  GTextLine(120, 45, 1310, Data.cfgClass.Title);

  var Idx := 1;
  var Groups := GetCharGroupedSkills(Data.ClassId, limitSkills, bAll);

  foreach Skills in Groups 
    
    var GroupCfg := GetAttributeGroupConfig(_Skills_iter);

    GCheckNextPage(12);
    GTextLine(50, 77+GNPCount()*30, 1000, "Grupa: "+GroupCfg.Name);
    var i := 0;

    foreach SkillId in Skills

      if(GNpCount() == 11 and i != 0)
        GCheckNextPage(12);
        GTextLine(50, 77+GNPCount()*30, 1000, "Grupa: "+GroupCfg.Name);
      endif

      GCheckNextPage(12);
      GButton(50, 77+GNPCount()*30, 0x867, 0x869, SkillId+1);
      GTextLine(80, 80+GNPCount()*30, 550, GetSkillConfigElem(SkillId).Name);

      Idx += 1;
      i += 1;

    endforeach
    
    if(GNPCount() == 10)
      GCheckNextPage(12);
    endif

  endforeach

  var Res := GSend(who).butid;

  if(Res)
    return Res-1;
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------
