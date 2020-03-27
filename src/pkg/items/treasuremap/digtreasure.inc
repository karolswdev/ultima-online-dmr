//-----------------------------------------------------------------------------

use util;

//-----------------------------------------------------------------------------

include "include/loots";
include "include/checks/objchecks";
include "include/key";
include "include/tools";

//-----------------------------------------------------------------------------

function DigTreasure( who, tool, map )

  var chestx := CInt(GetObjProperty( map , "chestx" ));
  var chesty := CInt(GetObjProperty( map , "chesty" ));
  var chestz := GetObjProperty( map , "chestz" );
  var Rlm  := GetObjProperty( map , "chestRlm" );

  if( (!chestx) or (!chesty) or chestz  == error)
     SendSysmessage( who , "To nie mapa skarbu.",FONT_NORMAL,COLOR_RED);
     return;
  endif

  chestz := CInt(chestz);

  if(!Rlm)
    Rlm := _DEFAULT_REALM;
  endif

  SendSysmessage( who , "Wybierz miejsce do kopania.",FONT_NORMAL,COLOR_GREEN);
  var dig := LocTarget( who );

  if(!StandardLocCheck(who, dig))
    return;
  endif

  var Quality := CheckItemQuality(tool); // 1-24

  if(coordist(who.x,who.y,dig.x,dig.y) > ToMin(CInt(Quality / 8), 1))
    SendSysMessage(who,"Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(who.realm != Rlm)
    SendSysMessage(who,"Wydaje sie jakbys szukal"+ggm(who,4)+" bardzo daleko od celu...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  TurnCharTowardLocation(who, dig);

  if(!MakeNoise(who, 0x126, 1500, ANIM_ATTACK_1HAND_DOWN, 1))
    return;
  endif

  CheckToolWear(who,tool,SKILLID_MINING);
  Quality := Quality / 6;
  
  var range := GetRange( who ) + Quality;
  
  if(range > 10)
    range := 10;
  endif
  
  var distn := coordist( dig.x , dig.y , chestx , chesty );
  var dirns := "";
  var direw := "";

  if( distn > range )

    if( dig.y > chesty )
       dirns := "polnoc";
    endif

    if( dig.y < chesty )
      dirns := "poludnie";
    endif

    if( dig.x < chestx )
      direw := "wschod";
    endif

    if( dig.x > chestx )
      direw := "zachod";
    endif

    if(direw == "")
      SendSysmessage( who , "Kopal"+ggm(who,3)+" przez chwile i czujesz ze musisz powedrowac na " + dirns + ".",FONT_NORMAL,COLOR_GREEN );
    else
      SendSysmessage( who , "Kopal"+ggm(who,3)+" przez chwile i czujesz ze musisz powedrowac na " + dirns + ", " + direw + "." ,FONT_NORMAL,COLOR_GREEN);
    endif

  else
     var lvl := GetTreasureLvl( map );

     if(!SubtractAmount(map, 1))
       return;
     endif

     var chest := CreateTreasureChest( who, lvl , chestx , chesty , chestz, Rlm );
     if(chest)
       SendSysmessage( who , "Znalazl"+ggm(who,3)+" skarb!",FONT_NORMAL,COLOR_BLUE );
     endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GetRange( who );

  var skill := GetEffectiveSkill( who , SKILLID_MINING );

  if( skill >= 100 )
     return 10;
  elseif( skill >= 90 )
     return 9;
  elseif( skill >= 80 )
     return 8;
  elseif( skill >= 70 )
     return 7;
  elseif( skill >= 60 )
     return 6;
  elseif( skill >= 50 )
     return 5;
  elseif( skill >= 40 )
     return 4;
  elseif( skill >= 30 )
     return 3;
  elseif( skill >= 20 )
     return 2;
  else
     return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function CreateTreasureChest( who, lvl, x, y, z, Realm)

   foreach item in ListItemsNearLocationOfType(CInt(x) , CInt(y) , CInt(z) , 0 , UOBJ_TREASURE_CHEST, Realm)
     ReleaseItem(item);
     DestroyItem(item);
   endforeach

   var chest := CreateItemAtLoc( CInt(x) , CInt(y) , CInt(z) , UOBJ_TREASURE_CHEST , 1, Realm);
   chest.invisible := 1;
   var lootgroup := 4+lvl;

   SetTreasureLvl( chest, lvl );
   chest.movable := 0;

   chest.usescript := ":treasuremap:treasurechest";
   SendSysMessage(who,"Zacz"+ggm(who,5)+" kopac...",FONT_NORMAL,COLOR_GREEN);

   CreateLoot(chest, lootgroup);
   chest.invisible := 0;
   chest.locked    := 1;

   return chest;

endfunction

//-----------------------------------------------------------------------------
