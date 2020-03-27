//-----------------------------------------------------------------------------

include "traits";
include "include/misc";

//-----------------------------------------------------------------------------

function ShowTraitsGump(who, bywho := 0)

  var Res := 1;
  var Viewer := __IfElse(bywho, bywho, who);
  
  while(Res and IsLoggedIn(Viewer))
    BuildTraitsGump(who, bywho);

    Res := GSend(Viewer).butid;

    if(Res > 100)
      SetTraitTitle(who, bywho, Res-100);
    elseif(Res > 10)
      EditTraitCount(who, bywho, Res-10);
    endif

  endwhile

endfunction

//-----------------------------------------------------------------------------

function BuildTraitsGump(who, bywho)

  var Traits := GetPlayerTraits(who);
  StandSkLayout(17, 18, 1);

  GTextLine(50, 30, 610,"Odswiez");
  GButton(  30, 34, 2117,2118,1);
  GButton(  130,34, 2117,2118,0);
  GTextLine(150,30, 610,"Umiejetnosci");

  if(!Traits.size())
    GTextLine(60, 72, 999, "Nie odkryto jeszcze zadnej cechy.");
    return;
  endif

  GAddPageData(515,55, 2650,2651,515,403,2648,2647);
  var Cfg := ReadConfigFile(CFG_TRAITS);
  var lCanEdit := (bywho and (bywho.cmdlevel >= who.cmdlevel) or bywho == who);
  
  GTextLine(50, 72, 999, "Lp");
  GTextLine(70, 72, 999, "Nazwa");
  GTextLine(210,72, 999, "Iloœæ");
  GTextLine(300,72, 999, "Ostatni");
  GTextLine(400,72, 999, "Ustaw tytu³");

  GPage();

  var i := 0;
  var lDict := dictionary;
  
  foreach Trait in Traits
    Trait.+Name := _Trait_iter;
    lDict[ZeroFill(ReadGameClock() - Trait.LastKill, 6)+"_"+_Trait_iter] := Trait;
  endforeach

  foreach Data in lDict

    i += 1;

    var Trait := Cfg[Data.Name];

    if(!Trait)
      continue;
    endif

    var ThreshCount := GetConfigInt(Trait, "HeadCount"+(Data.Level+1));
    
    GCheckNextPage(16);

    GTextLine(50,90+(20*GNpCount()), 999, i+".");
    GTextLine(70, 90+(20*GNpCount()), 550, Trait.Title);

    GTextLine(260, 90+(20*GNpCount()), 999, "Obra¿enia:");
    GTextLine(320, 90+(20*GNpCount()), 1152, CInt(100*GetTraitNpcBonus(who, Trait.Name, TNB_DAMAGE))+"%");
    GTextLine(375, 90+(20*GNpCount()), 999, "Obrona:");
    GTextLine(420, 90+(20*GNpCount()), 1152, CInt(100*GetTraitNpcBonus(who, Trait.Name, TNB_ARMOR))+"%");

    GButton(500, 95+(20*GNpCount()), 2117,2118, 100+i);
    
    GCheckNextPage(16);
    
    if(lCanEdit)
      GButton(50, 95+(20*GNpCount()), 2117,2118, 10+i);
    endif
    
    GTextLine(70,90+(20*GNpCount()), 1319, ClStr(GetConfigString(Trait, "Title"+Data.Level)));
    
    GTextLine(260,90+(20*GNpCount()), 1414, __IfElse(ThreshCount, Data.Count+"/"+ThreshCount, Data.Count) );
    GTextLine(350,90+(20*GNpCount()), 650, DescGameTime(ReadGameClock() - Data.LastKill));

  endforeach

endfunction

//-----------------------------------------------------------------------------

function SetTraitTitle(who, bywho, pIdx)

  var Traits := GetPlayerTraits(who);
  var Keys := Traits.keys();
  var TraitName := Keys[pIdx];
  var Cfg := ReadConfigFile(CFG_TRAITS);
  var Trait := Cfg[TraitName];

  if(!Trait)
    return;
  endif

  var Data := Traits[TraitName];
  
  SetObjProperty(who, PROP_TRAITTITLEGROUP, Trait.Name);
  UpdateTraitTitle(who, Trait, Data.Level);

  UniSendGood(__IfElse(bywho, bywho, who), "Ustawiono tytu³ cechy '"+Trait.Title+"'");

endfunction

//-----------------------------------------------------------------------------

function EditTraitCount(who, bywho, pIdx)

  var Traits := GetPlayerTraits(who);
  var Keys := Traits.keys();
  var TraitName := Keys[pIdx];
  var Cfg := ReadConfigFile(CFG_TRAITS);
  var Trait := Cfg[TraitName];

  if(!Trait)
    SendBad(bywho, "Cecha '"+TraitName+"' nie istnieje.");
    return;
  endif

  var NewCount := SendTextEntryGump(bywho, "Jaki ustawic licznik dla '"+Trait.Name+"'?",TE_CANCEL_ENABLE,TE_STYLE_NUMERICAL,GetConfigInt(Trait, "HeadCount"+Trait.MaxLevel));

  if(NewCount == error)
    return;
  endif

  NewCount := CInt(NewCount);
  SetTraitGroupCount(who, Trait, NewCount);

  UniSendGood(bywho, "Ustawiono count "+NewCount+" na ceche '"+Trait.Title+"' osobie "+GetRealName(who));

endfunction

//-----------------------------------------------------------------------------
