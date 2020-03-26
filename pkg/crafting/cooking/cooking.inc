//-----------------------------------------------------------------------------

include "include/player/nutrition";
include "include/checks/locchecks";
include "include/objtypes/skills";

//-----------------------------------------------------------------------------

const COOKTYPES_CFG          := ":cooking:cooktypes";
const COOKGROUPS_CFG         := ":cooking:cookgroups";

//-----------------------------------------------------------------------------

const CCTF_SUB               := 0x01;

//-----------------------------------------------------------------------------

function CanCookType(who, Type, CookElem, Flags := 0)
  
  var CookTypeCfg := ReadConfigFile(COOKTYPES_CFG);

  if(!CookTypeCfg[Type])
    return 0;
  endif

  foreach Item in GetConfigStringArray(CookTypeCfg[Type], "ReqItem")
    Item := SplitWords(Item);
    if(!CheckCookTypeItem(who, Item, CookElem, Flags))
      SendReqItemMsg(who, Item);
      return 0;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetCookBookMultiple(Book)

  var MultiDict := GetObjProperty(Book, "MultiDict");

  if(!MultiDict)
    MultiDict := dictionary;
  endif

  return MultiDict;

endfunction

//-----------------------------------------------------------------------------

function CheckCookTypeItem(who, Item, CookElem, Flags)

  case(Item[1])
    "_Cauldron":      return IsNearCauldron(who);
    "_Oven":          return IsNearOven(who);
    "_Chimsey":       return (IsNearFireChimsey(who) or IsNearCampFire(who));

    default:          if(!CInt(Item[1]))
                        Item[1] := GetObjTypeByName(Item[1]);
                      else
                        Item[1] := CInt(Item[1]);
                      endif
    
                      Item[2] := CInt(Item[2]);

                      foreach cItem in EnumerateItemsInContainer(who.backpack)
                        if(cItem.objtype == Item[1])
                          if(ReserveItem(cItem))
                            if(Item[2])
                              if(Flags & CCTF_SUB)
                                if(SubItemUses(cItem, Item[2]))
                                  return 1;
                                endif
                              else
                                if(GetItemUses(cItem) >= Item[2])
                                  return 1;
                                endif
                              endif
                            else
                              if(Flags & CCTF_SUB and CookElem.SubCtItem)
                                if(GetItemObjType(CookElem.SubCtItem) == cItem.objtype)
                                  if(!DestroyItem(cItem))
                                    return 0;
                                  endif
                                endif
                              endif
                              return 1;
                            endif
                          endif
                        endif

                        Sleepms(1);
                      endforeach
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendReqItemMsg(who, Item)

  case(Item[1])
    "_Cauldron":      SendSysMessage(who, "Musisz stac niedaleko kotla.", FONT_NORMAL, COLOR_RED);
    "_Oven":          SendSysMessage(who, "Musisz stac niedaleko pieca.", FONT_NORMAL, COLOR_RED);
    "_Chimsey":       SendSysMessage(who, "Musisz stac niedaleko kominka lub ogniska.", FONT_NORMAL, COLOR_RED);

    default:          var iName := GetDescForItem(Item[1]);
                      var Amt   := CInt(Item[2]);
                      if(iName)
                        if(!Amt)
                          SendSysMessage(who, "Musisz miec przedmiot "+iName+" przy sobie.", FONT_NORMAL, COLOR_RED);
                        else
                          if(IsStackableObj(GetItemObjType(Item[1])))
                            SendSysMessage(who, "Potrzebujesz "+Amt+" "+iName+".", FONT_NORMAL, COLOR_RED);
                          else
                            SendSysMessage(who, "Potrzebujesz "+Amt+" porcji "+iName+".", FONT_NORMAL, COLOR_RED);
                          endif
                        endif
                      else
                        SendSysMessage(who, "Brakuje Ci ciagle jakiegos przedmiotu...", FONT_NORMAL, COLOR_RED);
                      endif
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetCookGroupObjTypes(Name)

  var CGrpCfg := ReadConfigFile(COOKGROUPS_CFG);

  var ObjTypes := GetConfigStringArray(CGrpCfg[Name], "Item");

  foreach Obj in ObjTypes
    Obj := GetItemObjType(Obj);
  endforeach

  return ObjTypes;

endfunction

//-----------------------------------------------------------------------------

function GetCookCfgFromBook(Book)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(!ItemCfg[Book.objtype].CookCfg)
    LogError("[Cookbook] Book: ["+Book.objtype+"] Brak CookCfg", LOGF_SCRIPTS);
    return 0;
  endif

  if(!ItemCfg[Book.objtype].CookCfg)
    LogError("[Cookbook] Book: ["+Book.objtype+"] CookCfg: ["+ItemCfg[Book.objtype].CookCfg+"] nie istnieje.", LOGF_SCRIPTS);
    return 0;
  endif

  var CookCfg := ReadConfigFile(ItemCfg[Book.objtype].CookCfg);

  if(!CookCfg)
    LogError("[Cookbook] Book: ["+Book.objtype+"] CookCfg: ["+ItemCfg[Book.objtype].CookCfg+"] nie istnieje.", LOGF_SCRIPTS);
    return 0;
  endif

  return CookCfg;

endfunction

//-----------------------------------------------------------------------------
