//-----------------------------------------------------------------------------

include "include/move";

//-----------------------------------------------------------------------------

function ProcessCharacterCreation(Char)

  var cfgElem := GetCreationConfig();
  
  Char.truecolor := 32768 + CInt(cfgElem.Color);
  Char.color     := Char.truecolor;
  SetName(Char, cfgElem.StartName);

  var Location := SplitWords(cfgElem.Location);

  MoveObject(Char, Location[1], Location[2], Location[3], Location[4]);

  SetBaseStrength(Char, cfgElem.Str);
  SetBaseDexterity(Char, cfgElem.Dex);
  SetBaseIntelligence(Char, cfgElem.Int);
  SetStamina(Char, GetMaxStamina(Char));
  SetMana(Char, GetMaxMana(Char));
  SetHp(Char, GetMaxHp(Char));

  start_script(":character:creation", Char);

endfunction

//-----------------------------------------------------------------------------
