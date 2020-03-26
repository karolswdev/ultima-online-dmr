//-----------------------------------------------------------------------------

const SCRF_STAFF_ACCESS      := 0x01;
const SCRF_TEACH_ACCESS      := 0x02;
const SCRF_TEACH_LEVEL3      := 0x04;
const SCRF_TEACH_LEVEL4      := 0x08;
const SCRF_SHOW_ITEMSCROLL   := 0x10;

//-----------------------------------------------------------------------------

function ShowCraftReceptures(who, byWho, SkillID, Flags := 0)

  var Res;
  var AvailReceps := GetAvailCraftReceptures(SkillID, (Flags & SCRF_SHOW_ITEMSCROLL) == SCRF_SHOW_ITEMSCROLL);
  var List := GetCraftReceptures(who, SkillID);
  var SkElem := GetSkillConfigElem(SkillID);

  // dostep do zmian maja ekipanci z cmdlevel mniejszym od wybranego ekipanta
  // dostep do zmian maja testerzy, tylko na sobie
  // dostep do zmian maja ekipanci na testerach (bez cmdlevel), ale tylko na testowym ofc. 
  // na glownym gmi nie moga zmieniac graczom receptur. Proste (no chyba, ze admin! ;)

  if(IsAdmin(bywho))
    SetBitOn(Flags, SCRF_STAFF_ACCESS);
  elseif(IsStaff(bywho))
    if(IsStaff(who) and byWho.cmdlevel >= who.cmdlevel)
      SetBitOn(Flags, SCRF_STAFF_ACCESS);
    elseif(IsOnDMRTest() and !IsStaff(who))
      SetBitOn(Flags, SCRF_STAFF_ACCESS);
    endif
  endif

  if(IsOnDMRTest() and who == bywho)
    SetBitOn(Flags, SCRF_STAFF_ACCESS);
  endif

  if(!bywho)
    bywho := who;
  endif

  while(IsLoggedIn(bywho))

    BuildCraftRecepGump(AvailReceps, List, SkElem, Flags);
    Res := GSend(bywho)[0];

    if(!Res)
      break;
    elseif(IsBitOn(Flags, SCRF_STAFF_ACCESS))
      if(Res == 2)
        AddAllCraftReceptures(who, SkillID);
      elseif(Res == 3)
        DelAllCraftReceptures(who, SkillID);
      else
        var Idx := RetIndex(List, Res);

        if(!Idx)
          AddCraftRecepture(who, SkillID, Res);
        else
          DelCraftRecepture(who, SkillID, Res);
        endif
      endif
      List := GetCraftReceptures(who, SkillID);
    else
      return Res;
    endif

  endwhile

  return 0;

endfunction

//-----------------------------------------------------------------------------

function BuildCraftRecepGump(byref AvailReceps, byref List, SkElem, Flags)

  GInit(0,0);
  GResPic(0,  20, 2520, 900, 500);
  GResPic(28, 54, 3600, 844, 430);
  GAddPageData(865,55,2650,2651,865,463,2648,2647);
  GPage();
  
  GTextLine(200, 30, 999, "Umiejetnosc:");
  GTextLine(280, 30, 590, SkElem.Name);
  
  if(Flags & SCRF_STAFF_ACCESS)
    GTextLine(520, 30, 999, "Dodaj wszystkie");
    GButton(500, 33, 2117, 2118, 2);

    GTextLine(680, 30, 999, "Usun wszystkie");
    GButton(660, 33, 2117, 2118, 3);
  endif

  GTextLine(80, 490, 999, "Posiadanych:");
  GTextLine(155, 490, 590, List.size());
  
  GTextLine(280, 490, 999, "Dostepnych:");
  GTextLine(350, 490, 590, AvailReceps.size());

  GButton(  800,490,2453,2454,0);

  var i, w := 0;

  for(i:=1; i<=AvailReceps.size(); i+=1)
    if(!w)
      GCheckNextPage(8);
    endif
    BuildCraftRecepItem(i, w+60, 70 + GNPCount()*50, AvailReceps[i], List, Flags);
    w := __IfElse(w, 0, 400);
  endfor

endfunction

//-----------------------------------------------------------------------------

function BuildCraftRecepItem(Nr, x, y, byref Recep, byref List, Flags)

  var IsIn := (Recep.objtype in List);

  GTextLine(x+140, y, __IfElse(IsIn, 1320, 999), Nr+".");
  GTextLine(x+165, y, __IfElse(IsIn, 560, 999), Recep.desc);

  if(!IsIn)
    GTextLine(x+165, y+20, 1420, Recep.cost + " gp");
  endif

  if(IsBitOn(Flags, SCRF_STAFF_ACCESS))
    GTextLine(x+285, y+20, 580, hex(Recep.objtype));
  endif

  GResPic(x+60, y, 0xBB8, 70, 46);
  GTilePicHue(x+70+Recep.gX, y+10+Recep.gY, Recep.graphic, __IfElse(IsIn, Recep.color, 999));

  if(IsBitOn(Flags, SCRF_STAFF_ACCESS))
    GTextLine(x, y, 999, __IfElse(IsIn, "Usun", "Dodaj"));
    GButton(x+40, y+3, 2117, 2118, Recep.objtype);
  elseif(!IsIn and IsBitOn(Flags, SCRF_TEACH_ACCESS))
    if((IsBitOn(Flags, SCRF_TEACH_LEVEL3) and Recep.level == 3) or (IsBitOn(Flags, SCRF_TEACH_LEVEL4) and Recep.level == 4))
      GTextLine(x, y, 999, "Naucz");
      GButton(x+40, y+3, 2117, 2118, Recep.objtype);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
