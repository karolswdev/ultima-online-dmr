//=============================================================================
//-----------------------------------------------------------------------------
// Spell & Circles Props
//-----------------------------------------------------------------------------
//=============================================================================

const STRING_ID := 1;
const ARRAY_ID  := 2;

//-----------------------------------------------------------------------------

function GetSpellValue(SpellID, vName, vType := 0)

  if(!SpellID)
    return error;
  endif

  var SpellsCfg := ReadConfigFile(":*:spells");
  if(!SpellsCfg or !SpellsCfg[SpellID])
    LogError("GetSpellValue: Nie mozna znalezc configa czarow lub czaru! SID ["+SpellID+"] SCFG ["+SpellsCfg[SpellID]+"] (:*:spells.cfg)");
    return 0;
  endif

  case(vType)
    ARRAY_ID:  return GetConfigStringArray(SpellsCfg[SpellID], vName);
    STRING_ID: return GetConfigString(SpellsCfg[SpellID], vName);
    default:   return GetConfigInt(SpellsCfg[SpellID], vName);
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetCircleValue(circle,type,others := 0)

  var CirclesCfg := ReadConfigFile("::circles");
  if(!CirclesCfg)
   LogError("GetCircleValue: Nie mozna znalezc configa kregow! (circles.cfg)");
   return 0;
  endif

  case(others)
    ARRAY_ID:  return GetConfigStringArray(CirclesCfg[Circle],type);
    STRING_ID: return GetConfigString(CirclesCfg[Circle],type);
    default:   return GetConfigInt(CirclesCfg[Circle],type);
  endcase

endfunction

//-----------------------------------------------------------------------------
