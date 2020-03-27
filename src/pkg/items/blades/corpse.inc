//-----------------------------------------------------------------------------

include "include/noto";
include "include/gumpboxes";
include "include/mobile/age";
include "include/getobj";
include "include/checks/objchecks";
include "include/traps";

//-----------------------------------------------------------------------------

function CarveCorpse(who, cadaver)

  if(!StandardItemCheck(who,cadaver,NO_MOVABLE_CHECK))
    return;
  endif

  if(Distance(who,cadaver) > 1)
    SendSysMessage( who, "Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  var Owner   := FindPlayerBySerial(GetObjProperty(cadaver, PROP_CORPSE_OWNER));
  var Crimtxt := "";

  if(!Owner)
    Owner   := FindPlayerBySerial(GetObjProperty(cadaver, PROP_MASTER));
    if(Owner)
      Crimtxt := ", gdy wlasciciel zwierzaka "+Owner.name+" ["+Owner.serial+"] byl blue";
    endif
  else
    Crimtxt := ", gdy wlasciciel ["+Owner.serial+"] byl blue";
  endif

/*   if (IsPlayer(Owner) and IsYoung(Owner))
     SendSysMessage(who, "Nie mozesz ciac zwlok mlodych.", FONT_NORMAL, COLOR_RED); 
     return;
  endif*/
  
  /* __NONPVP__
  var PvPItemOwner;
  foreach item in EnumerateItemsInContainer(cadaver)
    PvPItemOwner := GetItemPvPStatus(item)[1];
    if (PvPItemOwner and PvPItemOwner != who)
      SendSysMessage(who, "W tych zwlokach sa przedmioty, ktore nie naleza do Ciebie!", FONT_NORMAL, COLOR_RED);
      return;
    endif
  endforeach*/

  if(IsPlayer(Owner) and Owner != who)
    if(!GuildAcp(who,Owner))
      if(GetObjProperty(cadaver, "DeathStatus") == "Order" and !who.murderer)
        SetCriminal(who, 1, "CarveCorpse", "Pocial "+cadaver.desc+Crimtxt+".");
        FameLost(who);
        SendBad(who, "Tniesz zwloki prawej osoby! Za ten niecny wystepek zostaniesz kryminalista!");
      endif
    endif
  endif

  var c_type := cadaver.corpsetype;
  var Temp   := GetObjProperty(cadaver, PROP_NPCTEMP);
  var Master := FindPlayerBySerial(GetObjProperty(cadaver, PROP_MASTER));

  if(Temp and IsPlayer(Master))
    if(GetObjProperty(cadaver, "EndurProtected") == error)
      var Chance := 100;
      Chance := Chance - (GetBaseSpecialAbility(cadaver, NPC_ABLID_ENDURANCE) * 0.2);

      if(RandomInt(100) >= Chance)
        SetObjProperty(cadaver, "EndurProtected", 1);
      else
        SetObjProperty(cadaver, "EndurProtected", 0);
      endif
    endif

    if(CInt(GetObjProperty(cadaver, "EndurProtected")))
      SendSysMessage(who, "Zwloki wydaja sie zbyt twarde, by je pociac.", FONT_NORMAL, COLOR_RED);
      sleep(2);
      return;
    endif
  endif

  if(GetObjProperty(cadaver, "GuardedBy") and Owner != who and !IsInAllyGuild(who, Owner))
    var Npc := FindNpcBySerial(GetObjProperty(cadaver, "GuardedBy"));
    if(Npc)
      SendSysMessage(who, "Nie uda Ci sie pociac zwlok, poki je ktos pilnuje!", FONT_NORMAL, COLOR_RED);
      if(IsInEnemyGuild(who, Owner) or who.criminal or who.murderer or Owner.criminal or Owner.murderer)
        SendCorpseLootEvent(cadaver, who, Owner);
      endif
      sleep(2);
      return;
    else
      EraseObjProperty(cadaver, "GuardedBy");
    endif
  endif

  Detach();
  NoLongerBusy(who);
  
  var cfgElem  := GetNpcConfig(Temp);

  if(GetObjProperty(cadaver,"cut"))

    if(GetBaseSkill(who,SKILLID_TRACKING) >= 90 and cfgElem.CampItem)
      var Item := SplitWords(cfgElem.CampItem);
      if(Item.size() > 1)
        Item := SelectItemGump(who,Item);
      else
        Item := CInt(Item[1]);
      endif

      if(Item)
        var CampCfg := ReadConfigFile(":tracking:camping");
        var Diff    := CInt(CampCfg[Item].Diff);
        if(Diff)
          if(CheckSkill(who,SKILLID_TRACKING,Diff,0))
            if(CreateItemAtLoc(cadaver.x,cadaver.y,cadaver.z,Item,1,cadaver.realm))
              SendSysMessage( who, "Udalo Ci sie wyciac przedmiot!",FONT_NORMAL, COLOR_BLUE);
            endif
          else
            SendSysMessage( who, "Nie udalo Ci sie pociac zwlok na cos uzytecznego.",FONT_NORMAL,COLOR_RED);
          endif
        else
          SendSysMessage( who, "Wiecej nie wytniesz z tych zwlok.",FONT_NORMAL,COLOR_RED);
        endif
      else
        SendSysMessage( who, "Wiecej nie wytniesz z tych zwlok.",FONT_NORMAL,COLOR_RED);
      endif
    else
      SendSysMessage( who, "Wiecej nie wytniesz z tych zwlok.",FONT_NORMAL,COLOR_RED);
    endif

    foreach thing in ListRootItemsInContainer(cadaver)
      if(IsSetFlag(thing, ITEMF_NO_LOOT))
        DestroyItem(thing);
      else
        SetObjProperty(thing, "#CorpseMoveGround", ReadGameClock());
        MoveObject(thing, cadaver.x, cadaver.y, cadaver.z, cadaver.realm, MOVEITEM_FORCELOCATION);
      endif
    endforeach
    DestroyItem(cadaver);

  elseif(c_type == CID_HUMAN_MALE or c_type == CID_HUMAN_FEMALE or c_type == CID_ELF_MALE or c_type == CID_ELF_FEMALE)

    foreach thing in ListRootItemsInContainer(cadaver)
      if(IsSetFlag(thing, ITEMF_NO_LOOT))
        DestroyItem(thing);
      else
        SetObjProperty(thing, "#CorpseMoveGround", ReadGameClock());
        MoveObject(thing, cadaver.x, cadaver.y, cadaver.z, cadaver.realm, MOVEITEM_FORCELOCATION);
      endif
    endforeach

    var partsarray := {};
    var part;
    part := CreateItemAtLoc(cadaver.x-1, cadaver.y, cadaver.z, UOBJ_HEAD, 1, cadaver.realm);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    SetObjProperty(part, "serial", GetObjProperty(cadaver, PROP_CORPSE_OWNER));
    SetObjProperty(part, "c",who.name);
    var aname := 0;
    var Ghost := FindPlayerBySerial(GetObjProperty(cadaver, PROP_CORPSE_OWNER));

    if(Ghost)
      aName := GetRealName(Ghost);
    endif

    if(!aname)
      aname := cadaver.name;
      aname["Zwloki "] := "";
    endif

    var Name := "Glowa " + aname;
    var Age := GetRealAge(cadaver);

    if(Age)
      case(CInt(GetObjProperty(cadaver,"Gender")))
        0: Name := Name + " zyl "+GetRealAge(cadaver)+" "+Gam(Age, GAM_LAT);
        1: Name := Name + " zyla "+GetRealAge(cadaver)+" "+Gam(Age, GAM_LAT);
      endcase
    endif

    part.name := Name;
    SaveLastHit(part, GetLastHitter(cadaver));

    if(RandomInt(100) < 10)
      case(c_type)
        CID_HUMAN_MALE:
        CID_HUMAN_FEMALE:  part := CreateItemAtLoc(cadaver.x-1, cadaver.y, cadaver.z, UOBJ_HUMAN_EARS, 1, cadaver.realm);
        CID_ELF_MALE:
        CID_ELF_FEMALE:    part := CreateItemAtLoc(cadaver.x-1, cadaver.y, cadaver.z, UOBJ_ELF_EARS, 1, cadaver.realm);
      endcase

      if(part)
        part.name := "Uszy "+ aname;
        partsarray.append({part, part.x, part.y, part.z, part.realm});
      endif
    endif

    part := CreateItemAtLoc(cadaver.x-1, cadaver.y, cadaver.z, 0x1da1, 1, cadaver.realm);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    part := CreateItemAtLoc(cadaver.x-1, cadaver.y+1, cadaver.z, 0x1da2, 1, cadaver.realm);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    part := CreateItemAtLoc(cadaver.x+1, cadaver.y, cadaver.z, 0x1da3, 1, cadaver.realm);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    part := CreateItemAtLoc(cadaver.x+1, cadaver.y+1, cadaver.z, 0x1da4, 1, cadaver.realm);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    part := CreateItemAtLoc(cadaver.x, cadaver.y, cadaver.z, 0x1d9f, 1, cadaver.realm);
    SetObjProperty(part, "c",who.name);
    partsarray.append({part, part.x, part.y, part.z, part.realm});
    CreateBlood(cadaver.x, cadaver.y, cadaver.z, cadaver.realm, CB_ALL);
    DestroyItem(cadaver);
    sleep(600);

    foreach thing in partsarray
      if(!thing[1].container and thing[1])
        if(thing[2] == thing[1].x and thing[3] == thing[1].y and thing[4] == thing[1].z and thing[5] == thing[1].realm)
          DestroyItem(thing[1]);
        endif
      endif
    endforeach

  else

    if(!Temp)
      SendSysMessage(who, "Poci"+ggm(who,5)+" zwloki ale nie udalo Ci sie znalezc czegos uzytecznego.", FONT_NORMAL,COLOR_RED);
    elseif(!IsSetFlag(cadaver,NPCF_NO_LOOT) and !GetObjProperty(cadaver, PROP_CREATED))
      if(cfgElem.Mechanic)
        SendSysMessage(who, "Nie zdolasz pociac metalu!", FONT_NORMAL, COLOR_RED);
        return;
      endif

      var corpseitm := cfgElem.corpseitm;
      var corpseamt := cfgElem.corpseamt;

      if(corpseitm)
        var Hides  := GetHidesObjTypes();
        var CampSk := GetEffectiveSkill(who,SKILLID_TRACKING);
        var Amt    := 0;

        corpseitm := SplitWords(corpseitm);
        corpseamt := SplitWords(corpseamt);

        foreach thing in corpseitm
          Amt   := CInt(corpseamt[_thing_iter]);
          thing := GetItemObjType(thing);

          
          if(thing == UOBJ_RAWRIBS)
            Amt := Amt + CInt(CampSk / 10);
          elseif(thing == UOBJ_SPIDER_SILK)
            Amt := Amt + CInt(CampSk / 20);
          elseif(thing in Hides)
            Amt := GetCampingCut(who, Amt);
          endif

          var itmDesc := GetHarvestDescriptor(thing, who);

          if(IsStackableObj(thing))
            CreateItemInCont(cadaver, itmDesc, Amt);
          else
            for i := 1 to Amt
              CreateItemInCont(cadaver, itmDesc, 1);
            endfor
          endif
        endforeach

        SendSysMessage(who, "Wycial"+ggm(who,3)+" cos ze zwlok.",FONT_NORMAL,COLOR_GREEN);

        if(GetBaseSkill(who, SKILLID_TRACKING) >= 10)
          AwardPoints(who, SKILLID_TRACKING, CInt(GetDefaultPointsValue(SKILLID_TRACKING)*5));
        endif
      endif
    else
      SendSysMessage(who, "Z tych zwlok nic nie wytniesz.", FONT_NORMAL, COLOR_RED_LIGHT);
    endif

    SetObjProperty(cadaver, "cut", 1);
    if(!IsNpcElemental(Temp, NPCELEM_UNDEAD) and !IsNpcElemental(Temp, NPCELEM_ELEMENTAL))
      CreateBlood(cadaver.x, cadaver.y, cadaver.z, cadaver.realm, CB_ALL);
    endif
    ReleaseItem(cadaver);
    sleep(600);
    if(cadaver)
      DestroyItem(cadaver);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function EatCorpse(cadaver)

  foreach thing in ListRootItemsInContainer(cadaver)
    if(IsSetFlag(thing, ITEMF_NO_LOOT))
      DestroyItem(thing);
    else
      SetObjProperty(thing, "#CorpseMoveGround", ReadGameClock());
      MoveObject(thing, cadaver.x, cadaver.y, cadaver.z, cadaver.realm, MOVEITEM_FORCELOCATION);
    endif
  endforeach

  CreateBlood(cadaver.x, cadaver.y, cadaver.z, cadaver.realm, CB_ALL);
  DestroyItem(cadaver);

endfunction

//-----------------------------------------------------------------------------

function GetCampingCut(who, amt)
  
  var SkCamp := GetEffectiveSkill(who,SKILLID_TRACKING);
  var index;
  
  if(SkCamp < 20)
    index := 1;
  elseif(SkCamp >= 20 and SkCamp < 30)
    index := 1.04;
  elseif(SkCamp >= 30 and SkCamp < 40)
    index := 1.08;
  elseif(SkCamp >= 40 and SkCamp < 50)
    index := 1.12;
  elseif(SkCamp >= 50 and SkCamp < 60)
    index := 1.16;
  elseif(SkCamp >= 60 and SkCamp < 70)
    index := 1.2;
  elseif(SkCamp >= 70 and SkCamp < 80)
    index := 1.24;
  elseif(SkCamp >= 80 and SkCamp < 90)
    index := 1.28;
  elseif(SkCamp >= 90 and SkCamp < 100)
    index := 1.32;
  elseif(SkCamp >= 100 and SkCamp < 110)
    index := 1.36;
  elseif(SkCamp >= 110 and SkCamp < 120)
    index := 1.40;
  elseif(SkCamp >= 120)
    index := 1.44;
  endif
  
  amt := Cint(amt * index);
  
  return amt;


endfunction

//-----------------------------------------------------------------------------
