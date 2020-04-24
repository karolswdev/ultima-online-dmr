use uo;
use os;
use cfgfile;

// Warrior

// major

// 100 - Glyph Of Critical Strike +2% additional critical strike chance
// 101 - Glyph of Improved Healing +5% faster healing
// 102 - Glyph of life leech -> 2% chance of hit's worth
// 103 - Glyph of mana leech -> 2% chance of leeching target mana for stamina 

// minor

// 110 - Glyph of increased health +3 health 
// 111 - Glyph of increased healing +3 total healthpoints healed
// 112 - Glyph of minor striking +1 damage
// 113 - Glyph of increased anatomy 0.5 health per 20% of anatomy
// 115 - Glyph of beastslaying 10% full hp regen chance after monsterkill
// 116 - Glyph of longer running .bieg lasts 5 seconds longer
// 117 - Glyph of improved bless bless spell gives 3 additional points per stat

// Mage

// major

// 200 - Glyph of spell critical strike +2% spell critical strike chance
// 201 - Glyph of improved healing  Greater heal heals 10% better
// 202 - Glyph of return mana 2% chance to return spell worth's mana
// 203 - Glyph of physical toughness redued physical damage

// minor

// 210 - Glyph of increased mana +3 total mana
// 211 - Glyph of increased meditation +3 total starting mana 
// 212 - Glyph of improved magic +1 dmg on every spell
// 213 - Glyph of increased evalint 0.5 mana per 20% of evalint
// 214 - Glyph of beastslaying 10% chance of full mana regen
// 215 - Glyph of improved wrestling 10% more chance to

// artisan

// 500 - glif mistrza rzemieslnika - dodaje szanse na exp
// 501 - glif podwojnego wyrobu - szansa na wykonanie duplikatu
// 600 - glif zmiany barwy - przy wyrobie expa szansa na zmiane koloru
// 601 - 
// 602
// 603

// new

// 300 - major glyph of stealth - adds 8 steps for stealth
// 400 - minor glyph of impressive shot

// 04/23/2020

// new all mage glyphs
// 800 - major glyph of stronger electric bolt
// 801 - major glyph of faster electric bolt

function MajorGlyphs()

	return { 100, 101, 102, 103, 200, 201, 202, 203, 300, 500, 501, 800, 801 };

endfunction

function MinorGlyphs()

	return { 110, 111, 112, 113, 115, 116, 117, 118, 210, 211, 212, 213, 214, 215, 216, 217, 400, 600, 601, 602, 603 };

endfunction

function FindGlyph(who, glyphID, noWear) 

    var book := FindGlyphBook(who);
    if(!book)
	    return 0;
    endif
    if(book.container != who.backpack)
     return 0;
    endif

    if(!GetObjProperty(book, "GlyphBookCreatedTime"))
	    SetObjProperty(book, "GlyphBookCreatedTime", ReadGameClock());
    endif

    var difference := ReadGameClock() - CInt(GetObjProperty(book, "GlyphBookCreatedTime"));

    if(difference > 2419200) // 28 days
	    SendSysMessage(who, "Ksiega glifow magicznie ulatnia sie.", FONT_NORMAL, COLOR_RED);
	    DestroyItem(book);
	    return 0;
    endif

    if(difference > 2000000) // 20 days
	    if(RandomInt(350) == 3)
	        SendSysMessage(who, "Twoja ksiega glifow niedlugo sie rozpadnie ...", FONT_NORMAL, COLOR_RED);
	    endif
    endif


    var storedIDs := GetObjProperty(FindGlyphBook(who), "StoredGlyphs");
    
    foreach glyph in storedIDs
        if(glyph == glyphID)
			return 1;
        endif
    endforeach
	
    return 0;
    
endfunction

function FindGlyphBook( who )

    if(!GetObjProperty(who, "GlyphBookId"))
	    return 0;
    else
	    return SystemFindObjectBySerial(GetObjProperty(who, "GlyphBookId"));
    endif
    
endfunction

function GetMajorGlyphs( book )

    var retArray := { };

    foreach rune in MajorGlyphs()
		foreach runeId in GetObjProperty(book, "StoredGlyphs")
			if(runeId == rune)
				retArray.append(runeId);
			endif
		endforeach
    endforeach
    
    return retArray;
    
endfunction

function GetMinorGlyphs(book)

    var retArray := { } ;
    
    foreach rune in MinorGlyphs()
		foreach runeId in GetObjProperty(book, "StoredGlyphs")
			if(runeId == rune)
				retArray.append(runeId);
			endif
		endforeach
    endforeach
    
    return retArray;
    
endfunction

function GetGlyphDesc(glyphId)

    case(glyphId)
    100: return "Glif krytycznego ciosu";
    101: return "Glif ulepszonego leczenia";
    102: return "Glif pijawki zycia";
    103: return "Glif pijawki many";
    200: return "Glif krytycznego czaru";
    201: return "Glif ulepszonego magicznego leczenia";
    202: return "Glif powrotu many";
    203: return "Glif twardosci fizycznej";
    110: return "Glif zwiekszonego zdrowia";
    111: return "Glif wiekszego leczenia";
    112: return "Glif pomniejszego uderzenia";
    113: return "Glif mistrza anatomii";
    115: return "Glif zabojcy bestii";
    116: return "Glif dluzszego biegu";
    117: return "Glif zwiekszonego blogoslawienstwa";
	118: return "Glif ognistego ciosu";
    210: return "Glif zwiekszonej many";
    211: return "Glif zwiekszonej medytacji";
    212: return "Glif silniejszej magii";
    213: return "Glif mistrza madrosci";
    214: return "Glif magicznego zabojcy bestii";
    215: return "Glif ulepszonych zapasow";
    216: return "Glif ulepszonego rozbrojenia";
	217: return "Glif mistrza zywiolaka ognia";
	300: return "Glif mistrza zakradania";
	500: return "Glif mistrza rzemieslnika";
	400: return "Glif imponujacego ciosu";
	501: return "Glif podwojnego wyrobu";
	600: return "Glif zmiany barwy przedmiotu";
	601: return "Glif cudnego rzemieslnika";
	602: return "Glif grawera";
	603: return "Glif wytrwalego gornika";
    800: return "Glif mocnego magicznego ciosu";
    801: return "Glif szybkiego magicznego ciosu";
    endcase
    
endfunction

function IsMajorGlyph(glyphId)

    if(glyphId in MajorGlyphs())
		return 1;
	endif
    
    return 0;
    
endfunction

function IsMinorGlyph(glyphId)
    
    if(glyphId in MinorGlyphs())
		return 1;
    endif
    
    return 0;
    
endfunction

function AddGlyph(book, glyphId)
    if(!GetObjProperty(book,"StoredGlyphs"))
     SetObjProperty(book,"StoredGlyphs",{});
    endif
    var glyphs := GetObjProperty(book, "StoredGlyphs");
	
	if(glyphId in glyphs)
		PrintTextAbove(book, "Ksiega ta posiada juz te runo.", FONT_NORMAL, COLOR_RED);
		return 0;
		return;
	endif
    
    glyphs.append(glyphId);
    
    SetObjProperty(book, "StoredGlyphs", glyphs);
	
	return 1;
    
endfunction

function WriteGlyphBook(who, oldBook)

    SendSysMessage(who, "Zaczynasz naprawiac ksiege...", FONT_NORMAL, GREEN_COLOR);
    if(RandomInt(100) < 5)
		SendSysMessage(who, "Ksiega zostala zniszczona.", FONT_NORMAL, RED_COLOR);
		DestroyItem(oldBook);
    else
		if(DestroyItem(oldBook))
			CreateItemInPack(who, "glyphbook", 1);
		endif
    endif
    
endfunction

function GetGlyphMod(who,glyphName, noWear := 0) //returns 0 if character doesn't have the glyph or mod if does

	var glyphCfg := ReadConfigFile(":glyphs:glyphs");
	var glyph := FindConfigElem(glyphCfg, glyphName);
    var gId := GetConfigInt(glyph,"id");
	var gmod := GetConfigInt(glyph,"mod");
		if(!FindGlyph(who, gId, noWear))
			return 0;
		endif
	return gmod;
	
endfunction

function GetGlyphCustomScript(who, glyphName)

	var glyphCfg := ReadConfigFile(":glyphs:glyphs");
	var glyph := FindConfigElem(glyphCfg, glyphName);
	var scriptId := GetConfigString(glyph, "CustomScript");
	
	if(!scriptId)
		return "";
	endif
	
	return scriptId;

endfunction

function glyphWarrior(who)
if(GetEffectiveSkill(who,SKILLID_TACTICS)>70)
 return 1;
endif
return 0;
endfunction

function glyphMage(who)
if(GetEffectiveSkill(who,SKILLID_MAGERY)>70)
 return 1;
endif
return 0;
endfunction