//-----------------------------------------------------------------------------

function CalcSkillLinear(who, SKillID, Perc)

  var SkVal := GetEffectiveSkill(who, SkillID);

  var Val := MinMax(SkVal, 0, 100) * CDbl(Perc) / 100.0;
  
  var OutVal := MinMax(SkVal - 100, 0, 20);

  // + 50% dla 120 max.
  if(OutVal > 0)
    Val := Val + CDbl(OutVal) * 0.025 * CDbl(Perc); // 0.025 aby zrobiæ 0.0 do 0.50 z zakresu 0-20, wtedy mno¿¹c przez procent otrzymamy liniowy wynik z zakresu 0 do Perc/2.
  endif

  return Val;

endfunction

//-----------------------------------------------------------------------------

function CalcSkillExponential(who, SkillID, Perc)

  var SkVal := GetEffectiveSkill(who, SkillID);

  // od 0 do 100 jest normalny exponential.
  var Val := CalcExponential(SkVal, Perc);
  
  // od 100 do 120 bedzie liniowy wzrost z 50% wartoœci bazowej.

  var OutVal := MinMax(SkVal - 100, 0, 20);

  if(OutVal > 0)
    Val := Val + CDbl(OutVal) * 0.025 * CDbl(Perc); // 0.025 aby zrobiæ 0.0 do 0.50 z zakresu 0-20, wtedy mno¿¹c przez procent otrzymamy liniowy wynik z zakresu 0 do Perc/2.
  endif

  return Val; 
  // ostateczna wartosc zwracana to exponential(0 do Perc) dla (0-100 skilla) + linear(0 do Perc/2) dla (100-120 skilla)
  // przyk³ad? Przekaz 4 (4% z wartosci skilla), otrzymasz wynik: 20% = 0.2, 40% = 0.42, 60% = 0.77, 80 = 1.35, 90% = 2, 100% = 4, 110% = 5, 120% = 6.

endfunction

//-----------------------------------------------------------------------------

function CalcExponential(Val, Perc, maxVal := 100)

  Val := MinMax(CDbl(Val)/CDbl(maxVal)*100.0, 1, 100);
  Val := 1.0 - (CDbl(Log10(101 - Val)) / 2.0); // log 1-100 bedzie mial wartosc 0.0 do 2.0, robimy sobie z tego procent do interpolacji (0.0 - 1.0)

  Perc := CDbl(MinMax(Perc, 0, 100));

  return (Val/100*maxVal) * Perc; // procent przekazywany jest w wartosci 0-100, mnoz¹c go przez nasza wartosc 0.0 do 1.0 otrzymamy po¿adany result, czyli wartosc od 0 do Perc.

endfunction

//-----------------------------------------------------------------------------

function CalcLinear(Val, Perc, maxVal := 100)

  return CDbl(MinMax(Val, 0, maxVal)) * CDbl(Perc) / CDbl(maxVal); // zwracamy wartosc liniowa z zakresu 0 do Perc, zalezna od Val.

endfunction

//-----------------------------------------------------------------------------
