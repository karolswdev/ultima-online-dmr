//-----------------------------------------------------------------------------

function GetEconomyData()

  var Data := OpenDataFile(":crafting:economy");

  if(!Data)
    Data := CreateDataFile(":crafting:economy");
  endif

  return Data;

endfunction 

//-----------------------------------------------------------------------------

function GetEconomyStats(Date)

  var EconData := GetEconomyData();

  if(!EconData.FindElement(Date))
    CreateEconomyStats();
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckCont(byref EconStats, Cont)

  ProcessItem(EconStats, Cont);

  foreach Item in EnumerateItemsInContainer(Cont)
    if(Item.container == Cont)
      if(Item.isa(POLCLASS_CONTAINER))
        CheckCont(EconStats, Item);
      else
        ProcessItem(EconStats, Item);
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
// global scan
//-----------------------------------------------------------------------------

function CreateEconomyStats(Date)

  var Prc := 0, lPrc := 25;
  var tox, toy, x, y;
  var Rlms := Realms();

  var EconStats;
  PrepareEconomyStats(EconStats);

  foreach Realm in Rlms
    Prc := 0;
    lPrc := 0;
    AlertAdmins("[EconStats] Realm: "+_Realm_iter+" 0%", FONT_NORMAL, COLOR_GREEN_LIGHT);

    for(y:=0;y<Realm.height;y:=y+64)
      if(y+63 > Realm.height)
        toy := Realm.height;
      else
        toy := y + 63;
      endif

      for(x:=0;x<Realm.width;x:=x+64)
        if(x+63 > Realm.width)
          tox := Realm.width;
        else
          tox := x + 63;
        endif

        foreach Item in ListObjectsInBox(x, y, -127, tox, toy,127, _Realm_iter)
          if(Item.isa(POLCLASS_CONTAINER))
            CheckCont(EconStats, Item);
          elseif(Item.isa(POLCLASS_ITEM))
            ProcessItem(EconStats, Item);
          endif
        endforeach
      endfor

      Prc := CInt(((y+32) * 100) / Realm.height);
      if(abs(Prc - lPrc) >= 25 or Prc == 100)
        AlertAdmins("[EconStats] Realm: "+_Realm_iter+" "+Prc+"%", FONT_NORMAL, COLOR_STONE);
        lPrc := Prc;
      endif

      sleepms(10);

    endfor

    AlertAdmins("[EconStats] Realm: "+_Realm_iter+" 100%, Items: ["+EconStats.CountItems+"]",FONT_NORMAL,COLOR_GREEN);
  endforeach

  var Chr;

  var Accs := ListAccounts();
  var c    := 1, i;

  EconStats.CountAccs[3] := Accs.size();

  foreach Acc in Accs
    Acc := FindAccount(Acc);

    if(IsActiveAccount(Acc))
      EconStats.CountAccs[1] += 1;
    endif
    
    if(IsNewAccount(Acc))
      EconStats.CountAccs[2] += 1;
    endif

    for(x:=1;x<=5;x:=x+1)
      Chr   := Acc.GetCharacter(x);
      if(Chr and !IsStaff(Chr))
        sleepms(10);
        CheckCont(EconStats, Chr.backpack);
        CheckCont(EconStats, FindBankBox(Chr));
        CheckCont(EconStats, FindStoreBox(Chr));

        for(i:=0;i<=25;i:=i+1)
          if(i != LAYER_PACK)
            var Eqp := GetEquipmentByLayer(Chr,i);
            if(Eqp)
              ProcessItem(EconStats, Eqp);
            endif
          endif
        endfor

        ProcessChar(EconStats, Chr);
        sleepms(10);
      endif
    endfor

    Prc := CInt((c * 100) / Accs.size());
    if(abs(Prc - lPrc) >= 20 or Prc == 100)
      AlertAdmins("[EconStats] Chars: "+Prc+"%", FONT_NORMAL, COLOR_STONE);
      lPrc := Prc;
    endif
    c := c + 1;

  endforeach

  var Storage := StorageAreas()[ VENDOR_STORAGE ];
  if(Storage)
    AlertAdmins("[EconStats] VendorStorage...", FONT_NORMAL, COLOR_STONE);
    foreach Item in Storage
      CheckCont(EconStats, Item);
      sleepms(10);
    endforeach
  endif
  
  Storage := StorageAreas()[ ESCROW_STORAGE ];

  if(Storage)
    AlertAdmins("[EconStats] EscrowStorage...", FONT_NORMAL, COLOR_STONE);
    foreach Item in Storage
      CheckCont(EconStats, Item);
      sleepms(10);
    endforeach
  endif

  SaveEconomyStats(Date, EconStats);
  AlertAdmins("[EconStats] Finished", FONT_NORMAL, COLOR_STONE);

endfunction

//-----------------------------------------------------------------------------
// Process functions
//-----------------------------------------------------------------------------

function ProcessItem(byref Data, Item)

  if(IsGmCreated(Item))
    return;
  endif

  if(Item.objtype == UOBJ_GOLDCOIN)
    Data.CountGold += 1;
    Data.SumGold += Item.amount;
  elseif(Item.objtype == UOBJ_CHEQUE)
    Data.CountCheque += 1;
    Data.SumCheque += GetChequeVal(Item);
  endif

  if(IsMagicItem(Item))
    var Mods := GetMagicMods(item);
    var Group := MinMax(CInt(Mods[3]/4)+1, 1, 4);

    if(Mods[4] or Mods[5])
      Data.MagicItems[Group] += 1;
    endif
    
  endif

  if(IsItemMade(Item))

    var SkID := error;
    foreach ObjTypes in (Data.Cache.Craft)
      if(Item.objtype in ObjTypes)
        SkID := _ObjTypes_iter;
        break;
      endif
    endforeach

    if(SkID != error and Data.CraftItems.exists(SkID))
      if(Item.isa(POLCLASS_ARMOR))
        Data.CraftItems[SkID][1] += 1;
      elseif(Item.isa(POLCLASS_WEAPON))
        Data.CraftItems[SkID][2] += 1;
      elseif(Item.isa(POLCLASS_EQUIPMENT))
        Data.CraftItems[SkID][3] += 1;
      else
        Data.CraftItems[SkID][4] += 1;
      endif
    endif

  else
    var SkID := error;
    foreach ObjTypes in (Data.Cache.Harv)
      if(Item.objtype in ObjTypes)
        SkID := _ObjTypes_iter;
        break;
      endif
    endforeach

    if(SkID != error and Data.HarvItems.exists(SkID))
      if(!Data.HarvItems[SkID].exists(Item.objtype))
        Data.HarvItems[SkID][Item.objtype] := array(0, 0);
      endif
      Data.HarvItems[SkID][Item.objtype][1] += 1;
      Data.HarvItems[SkID][Item.objtype][2] += Item.amount;
    endif

  endif

  Data.CountItems += 1;

endfunction

//-----------------------------------------------------------------------------

function ProcessChar(byref Data, Char)

  var AccAmount := GetBankAccountAmount(Char);

  if(AccAmount > 0)
    Data.CountBankAccount += 1;
    Data.SumBankAccount += AccAmount;
  endif

  Data.CountChars += 1;

  foreach SkData in (Data.CraftSkills)
    var BaseSkill := GetBaseSkill(Char, _SkData_iter);
    if(BaseSkill > 10)
      BaseSkill := CInt(BaseSkill / 25)+1;
      if(BaseSkill > 100)
        BaseSkill += 1;
      endif

      BaseSkill := MinMax(BaseSkill, 1, 6);

      SkData[BaseSkill] += 1;
    endif
  endforeach
  
  foreach SkData in (Data.HarvSkills)
    var BaseSkill := GetBaseSkill(Char, _SkData_iter);
    if(BaseSkill > 10)
      BaseSkill := CInt(BaseSkill / 25)+1;
      if(BaseSkill > 100)
        BaseSkill += 1;
      endif
      
      BaseSkill := MinMax(BaseSkill, 1, 6);

      SkData[BaseSkill] += 1;
    endif
  endforeach

  var SubClass := GetCharSubClassTitle(Char);
  var Class := GetCharClass(Char);
  var Race := GetCharRace(Char);

  Data.CountClasses[Class] += 1;
  Data.CountRaces[Race] += 1;

  if(!Data.CountSubClasses.exists(SubClass))
    Data.CountSubClasses[SubClass] := 0;
  endif

  Data.CountSubClasses[SubClass] += 1;

  if(IsRealMurderer(Char))
    Data.CountStatus[2] += 1;
  else
    Data.CountStatus[1] += 1;
  endif

endfunction

//-----------------------------------------------------------------------------
// Prepare functions
//-----------------------------------------------------------------------------

function PrepareEconomyStats(byref Data)

  Data := struct;
  Data.+CountGold := 0;
  Data.+CountCheque := 0;
  Data.+SumGold := 0;
  Data.+SumCheque := 0;
  Data.+CountBankAccount := 0;
  Data.+SumBankAccount := 0;
  Data.+CountItems := 0;
  Data.+CountChars := 0;
  Data.+CountRaces := dictionary;
  Data.+CountClasses := dictionary;
  Data.+CountStatus := array(0, 0);
  Data.+CountSubClasses := dictionary;
  Data.+CountAccs := array(0, 0, 0);
  Data.+MagicItems := array(0, 0, 0, 0);

  var Races := ReadConfigFile(RACES_CFG);

  foreach Race in GetConfigStringKeys(Races)
    Data.CountRaces[Race] := 0;
  endforeach
  
  var Classes := ReadConfigFile(CLASSES_CFG);

  foreach Class in GetConfigStringKeys(Classes)
    Data.CountClasses[Class] := 0;
  endforeach

  Data.+CraftSkills := dictionary;
  Data.+HarvSkills := dictionary;
  Data.+CraftItems := dictionary;
  Data.+HarvItems := dictionary;

  var SkID;

  Data.+Cache := struct;
  Data.Cache.+Harv := dictionary;
  Data.Cache.+Craft := dictionary;

  for(SkID:=0; SkID<=SKILLID__HIGHEST; SkID+=1)
    var SkElem := GetSkillConfigElem(SkID);

    if(SkElem.AttrGroup == "crafting")
      Data.CraftSkills[SkID] := array(0, 0, 0, 0, 0, 0); // <25, <50, <75, <100, 100, > 100
      Data.CraftItems[SkID] := array(0, 0, 0, 0); // armor, bronie, equip, itemy

      Data.Cache.Craft[SkID] := GetObjTypesForCraft(SkID);

    elseif(SkElem.AttrGroup == "harvest")
      Data.HarvSkills[SkID] := array(0, 0, 0, 0, 0, 0); // 0, 25, 50, 75, 100, > 100
      Data.HarvItems[SkID] := dictionary; // zbior objtypow z arrayem (ilosc, suma)
  
      Data.Cache.Harv[SkID] := GetObjTypesForHarvest(SkID);
    endif
  endfor

endfunction 

//-----------------------------------------------------------------------------

function SaveEconomyStats(Date, byref EconStats)

  var EconData := GetEconomyData();

  var Elem := EconData.FindElement(Date);

  if(!Elem)
    Elem := EconData.CreateElement(Date);
  endif


  Elem.SetProp("CountBankAccount", EconStats.CountBankAccount);
  Elem.SetProp("CountGold", EconStats.CountGold);
  Elem.SetProp("CountCheque", EconStats.CountCheque);
  Elem.SetProp("SumBankAccount", EconStats.SumBankAccount);
  Elem.SetProp("SumGold", EconStats.SumGold);
  Elem.SetProp("SumCheque", EconStats.SumCheque);
  Elem.SetProp("CountItems", EconStats.CountItems);
  Elem.SetProp("CountChars", EconStats.CountChars);
  Elem.SetProp("CountAccs", EconStats.CountAccs);
  Elem.SetProp("CountRaces", EconStats.CountRaces);
  Elem.SetProp("CountClasses", EconStats.CountClasses);
  Elem.SetProp("CountSubClasses", EconStats.CountSubClasses);
  Elem.SetProp("CountStatus", EconStats.CountStatus);
  Elem.SetProp("CraftSkills", EconStats.CraftSkills);
  Elem.SetProp("HarvSkills", EconStats.HarvSkills);
  Elem.SetProp("CraftItems", EconStats.CraftItems);
  Elem.SetProp("HarvItems", EconStats.HarvItems);
  Elem.SetProp("MagicItems", EconStats.MagicItems);

endfunction

//-----------------------------------------------------------------------------
