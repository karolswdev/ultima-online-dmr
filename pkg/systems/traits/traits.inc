//-----------------------------------------------------------------------------

include "common";
include "npcbonus";
include "include/unicode";

//-----------------------------------------------------------------------------

function GetPlayerTraits(who)

  var Traits := GetObjProperty(who, PROP_TRAITS);

  if(!Traits)
    Traits := dictionary;
  endif

  return Traits;

endfunction

//-----------------------------------------------------------------------------

function GetNpcGroupForCorpse(NpcCorpse)

  var NpcTemp    := GetObjProperty(NpcCorpse, PROP_NPCTEMP);
  return GetNpcConfig(NpcTemp).NpcGroup;

endfunction

//-----------------------------------------------------------------------------

function GetTraitGroupForNpcCorpse(NpcCorpse, NpcGroup)

  if(!NpcGroup)
    return 0;
  endif

  var NpcTraits  := ReadConfigFile(CFG_NPCTRAITS);
  var TraitGroup := NpcTraits[NpcGroup].Trait;

  if(!TraitGroup)
    return 0;
  endif

  var Cfg := ReadConfigFile(CFG_TRAITS);

  if(!Cfg[TraitGroup])
    LogError("[Traits] Istnieje zdefiniowana grupa '"+TraitGroup+"' dla npeca '"+GetObjProperty(NpcCorpse, PROP_NPCTEMP)+"', ale nie istnieje w cfg!", LOGF_SCRIPTS);
  endif

  return Cfg[TraitGroup];

endfunction

//-----------------------------------------------------------------------------

function CheckTraitNpcKilled(Killer, NpcCorpse)

  var NpcGroup := GetNpcGroupForCorpse(NpcCorpse);
  var Trait := GetTraitGroupForNpcCorpse(NpcCorpse, NpcGroup);

  if(!Trait)
    return;
  endif

  if(!IsPlayer(Killer))
    if(!IsPlayerTamed(Killer))
      return;
    else
      Killer := GetMaster(Killer);
    endif
  endif

  IncTraitGroupForPlayer(Killer, Trait, NpcGroup);

  var Party := GetNearbyPartyMembers(Killer);

  if(Party.size())
    foreach Memb in Party
      IncTraitGroupForPlayer(Memb, Trait, NpcGroup);
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function UpdateTraitTitle(Killer, Trait, CurLevel)

  var TraitTitleGroup := GetObjProperty(Killer, PROP_TRAITTITLEGROUP);

  if(TraitTitleGroup != Trait.Name)
    return;
  endif

  var Title := GetConfigString(Trait, "Title"+CurLevel);

  if(Title)
    SetTooltipProp(Killer, "Cecha", Title);
  else
    EraseTooltipProp(Killer, "Cecha");
  endif

endfunction

//-----------------------------------------------------------------------------

function ApplyTraitBonus(Killer, Trait, Level)

  var Bonus := SplitWords( GetConfigString(Trait, "Bonus"+Level) );

  if(Bonus[1] == "Damage")
    SetTraitNpcBonus(Killer, Trait.Name, TNB_DAMAGE, CDbl(Bonus[2])/100.0);
  elseif(Bonus[1] == "Armor")
    SetTraitNpcBonus(Killer, Trait.Name, TNB_ARMOR, CDbl(Bonus[2])/100.0);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetTraitGroupCount(Killer, Trait, Count)

  var Traits := GetPlayerTraits(Killer);

  EraseTraitNpcBonus(Killer, Trait.Name, TNB_DAMAGE);
  EraseTraitNpcBonus(Killer, Trait.Name, TNB_ARMOR);

  if(Count == 0)
    Traits.erase(Trait.Name);
  else
    var Data := struct;
    Data.+Count := Count;
    Data.+LastKill := ReadGameClock();
    Data.+Level := 0;

    while(Data.Level < Trait.MaxLevel)
      var ThreshCount := GetConfigInt(Trait, "HeadCount"+(Data.Level+1));

      if(ThreshCount > Data.Count)
        break;
      endif

      Data.Level += 1;
      ApplyTraitBonus(Killer, Trait, Data.Level);
    endwhile
    
    Traits[Trait.Name] := Data;
    UpdateTraitTitle(Killer, Trait, Data.Level);
    SetObjProperty(Killer, PROP_TRAITS, Traits);
  endif

endfunction

//-----------------------------------------------------------------------------

function IncTraitGroupForPlayer(Killer, Trait, NpcGroup)

  var Traits := GetPlayerTraits(Killer);
  var Data;

  if(!Traits.exists(Trait.Name))
    Data := struct;
    Data.+Count := 0;
    Data.+Level := 0;
    Data.+LastKill;
  else
    Data := Traits[Trait.Name];
  endif

  var LevelNpcGroup := GetConfigString(Trait, "Group"+(Data.Level+1));

  if(LevelNpcGroup != NpcGroup or Data.Level == Trait.MaxLevel)
    return;
  endif
  
  var ThreshCount := GetConfigInt(Trait, "HeadCount"+(Data.Level+1));

  if(!ThreshCount)
    return;
  endif
  
  Data.LastKill := ReadGameClock();
  Data.Count += 1;
  
  if(!Traits.exists(Trait.Name))

    UniSendSysMessage(Killer, "Odkry³"+ggm(Killer,3)+" now¹ cechê '"+Trait.Title+"'!", _DEFAULT_UCFONT, COLOR_WHITE);
    
    if(Trait.Text1)
      UniSendGood(Killer, Trait.Text1);
    endif

  else

    if(Data.Count >= ThreshCount)
      var EndText := GetConfigString(Trait, "EndText"+(Data.Level+1));

      if(EndText)
        UniSendSysMessage(Killer, EndText, _DEFAULT_UCFONT, COLOR_WHITE);
      endif
      
      Data.Level += 1;
      
      var NextText := GetConfigString(Trait, "Text"+(Data.Level+1));

      if(NextText)
        UniSendGood(Killer, NextText);
      endif

      UpdateTraitTitle(Killer, Trait, Data.Level);
      ApplyTraitBonus(Killer, Trait, Data.Level);

    endif

  endif

  Traits[Trait.Name] := Data;
  SetObjProperty(Killer, PROP_TRAITS, Traits);

endfunction

//-----------------------------------------------------------------------------
