//-----------------------------------------------------------------------------

function GetRandomStoneShard()

  case(RandomInt(3))
    1: return 0x1362;
    default: return 0x1363;
  endcase

endfunction

//-----------------------------------------------------------------------------

function PlayStoneFallEffect(who)

  PlayMovEffXYZ(who.x, who.y, Min(who.z+80, 127), who.x, who.y, who.z+10, 4534, 0x6, 4, 0, who.realm);
  sleepms(600);

  if(who.isa(POLCLASS_MOBILE))
    PlayPcEffect(who, FX_EXPLODE_1, 0, 0x10, 0x10);
  else
    PlayPcStEffect(who.x, who.y, who.z, who.realm, FX_EXPLODE_1, 0, 0x10, 0x10);
  endif

  var Roll := "2d3+9";
  var Locs := array;
  Locs.append(array(who.x+RandomDiceRoll(Roll), who.y+RandomDiceRoll(Roll)));
  Locs.append(array(who.x-RandomDiceRoll(Roll), who.y+RandomDiceRoll(Roll)));
  Locs.append(array(who.x-RandomDiceRoll(Roll), who.y-RandomDiceRoll(Roll)));
  Locs.append(array(who.x+RandomDiceRoll(Roll), who.y-RandomDiceRoll(Roll)));

  PlayMovEffXYZ(who.x, who.y, who.z+5, Locs[1][1], Locs[1][2], who.z, GetRandomStoneShard() , 0xA, 4, 0, who.realm);
  PlayMovEffXYZ(who.x, who.y, who.z+5, Locs[2][1], Locs[2][2], who.z, GetRandomStoneShard() , 0xA, 4, 0, who.realm);
  PlayMovEffXYZ(who.x, who.y, who.z+5, Locs[3][1], Locs[3][2], who.z, GetRandomStoneShard() , 0xA, 4, 0, who.realm);
  PlayMovEffXYZ(who.x, who.y, who.z+5, Locs[4][1], Locs[4][2], who.z, GetRandomStoneShard() , 0xA, 4, 0, who.realm);
  
  return Locs;

endfunction

//-----------------------------------------------------------------------------
