//-----------------------------------------------------------------------------

include "include/objtypes/misc";

//-----------------------------------------------------------------------------

const ARTMOD_CUSTOM_START := 100;

const ARTMOD_SLAYER := 100;
const ARTMOD_LEECH  := 101;
const ARTMOD_POISON := 102;

//-----------------------------------------------------------------------------

function IsArtifact(Item)

  if(Item.objtype >= UOBJ_ART_START and Item.objtype <= UOBJ_ART_END)
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ArtCtl(Item)

  start_script(":artifacts:art_ctl",Item);

endfunction

//-----------------------------------------------------------------------------

function ApplyArtMods(who,Mods,Item)

  foreach Mod in Mods
    
    if(_Mod_iter >= 200)
      DoMagicSkillMod(who,item,_Mod_iter - 200,Mod);
    elseif(_Mod_iter < ARTMOD_CUSTOM_START)
      DoMagicMod(who,item,_Mod_iter,Mod);
    endif

  endforeach

endfunction

//-----------------------------------------------------------------------------

function UndoArtMods(who,Mods,ItemSer)

  foreach Mod in Mods
    if(_Mod_iter >= 200)
      UndoMagicSkillMod(who,ItemSer,_Mod_iter - 200, Mod);
    elseif(_Mod_iter < ARTMOD_CUSTOM_START)
      UnMagicMod(who,ItemSer,_Mod_iter,Mod);
    endif
  endforeach
  
endfunction

//-----------------------------------------------------------------------------

function GetArtMods(Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var ArtMods := GetConfigStringDictionary(ItemCfg[Item.objtype], "ArtMod");
  var List := dictionary;

  foreach ArtMod in ArtMods
    case(CInt(_ArtMod_iter))
      ARTMOD_SLAYER: 
        List[CInt(_ArtMod_iter)] := ArtMod;
      default:
        List[CInt(_ArtMod_iter)] := CInt(ArtMod);
    endcase
  endforeach

  if(!GetObjProperty(Item, "IsSet"))
    SetArtMods(Item, List);
  endif

  return List;

endfunction

//-----------------------------------------------------------------------------

function SetArtMods(Item, ArtMods)

  if(IsArtifact(Item))
    
    SetObjProperty(Item,"IsSet", ReadGameClock());
    EraseObjProperty(Item, "ArtMods");

    foreach Mod in ArtMods
      case(_Mod_iter)
        ARTMOD_SLAYER:
          AddNewOnHitScript(Item,SLAYER_HIT_SCRIPT);
          SetObjProperty(Item, "SlayerType", Mod);
        ARTMOD_LEECH:
          AddNewOnHitScript(Item,LEECH_HIT_SCRIPT);
          SetObjProperty(Item, "LeechType", CInt(Mod));
        ARTMOD_POISON:
          var User := FindPlayerBySerial(GetObjProperty(Item,"equipped"));
          if(!IsItemPoisoned(Item))
            SetItemPoisoned(User, Item, CInt(Mod), 10000 + RandomInt(1000));
          endif
      endcase

    endforeach

    if(!GetObjProperty(Item, "MaxArtCharges"))
      var ItemCfg := ReadConfigFile(ITEMS_CFG);
      var charges := CInt(ItemCfg[Item.objtype].ArtCharges) + RandomInt(CInt(CInt(ItemCfg[Item.objtype].ArtCharges) / 4));
      SetArtCharges(Item, charges);
      SetObjProperty(Item,"MaxArtCharges",charges);
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function GetArtCharges(Item)

  return CInt(GetObjProperty(Item,"ArtCharges"));

endfunction

//-----------------------------------------------------------------------------

function SetArtCharges(Item,Charges)

  SetObjProperty(Item,"ArtCharges",Charges);

endfunction

//-----------------------------------------------------------------------------

function GetMaxArtCharges(Item)

  return CInt(GetObjProperty(Item,"MaxArtCharges"));

endfunction

//-----------------------------------------------------------------------------

function GetArtChargesDesc(item)

  var charges := GetArtCharges(Item);
   
  if (charges == 0)
    return "Artefakt "+ item.desc+" nie byl uzywany.";;
  endif

  var Percent := CInt(charges * 100 /  GetMaxArtCharges(Item));

  case(CInt(Percent / 10))
    0:
    1:  return item.desc+" jest prawie bez mocy.";
    2:  return item.desc+" ma niewiele mocy.";
    3:  return item.desc+" ma czesc mocy.";
    4:  return item.desc+" ma troche mocy.";
    5:  return item.desc+" ma polowe mocy";
    6:  return item.desc+" ma troche ponad polowe mocy.";
    7:  return item.desc+" ma spora czesc mocy.";
    8:  return item.desc+" jest lekko wyczerpany.";
    9:  return item.desc+" jest niemal calkowicie wypelniony moca.";
    10: return item.desc+" jest calkowicie wypelniony moca.";
  endcase

 endfunction

//-----------------------------------------------------------------------------
