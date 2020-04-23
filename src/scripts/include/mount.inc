//=============================================================================
//-----------------------------------------------------------------------------
// Mounting
//-----------------------------------------------------------------------------
//=============================================================================

use uo;

//-----------------------------------------------------------------------------

include "include/client/misc";
include "include/proc";
include "include/events";
include "include/items";
include "include/npcs";
include "include/bits";
include "include/move";
include "include/objtypes/misc";
include "ai/include/tamed";
include "include/finding";
include "include/objects/location";

//-----------------------------------------------------------------------------

function Dismount(who, mount := 0)

  if(ExPid(GetObjProperty(who,"#Dismounting")))
    return 0;
  endif

  while(GetObjProperty(who, "#Mounting"))
    Sleepms(50);
  endwhile

  SetObjProperty(who,"#Dismounting",GetPid());

  if(!mount)
    mount := GetEquipmentByLayer(who,LAYER_MOUNT);
  endif

  if(!mount)
    EraseObjProperty(who,"#Dismounting");
    return 0;
  endif

  if(IsSetFlag(Mount, ITEMF_NO_LOOT))
    EraseObjProperty(who,"#Dismounting");
    return 0;
  endif

  var Animal := GetAnimalFromMount(mount);

  if(!Animal)
    var Temp := GetObjProperty(mount, PROP_NPCTEMP);
    if(Temp)
      Animal := CreateNPC (Temp, 6096, 1192, 0, _DEFAULT_REALM, DEF_PARMS, NPCF_NO_UPKEEP | NPCF_NO_LOOT);

      if(!Animal)
        LogError("[Dismount]: Nie udalo sie stworzyc npeca do dismounta!!! Template: "+Temp+" osoba: "+GetRealName(who), LOGF_SCRIPTS);
        return error;
      endif
      LoadNpcAttrData(Animal, mount, who);
    else
      LogError("[Dismount]: Nie udalo sie stworzyc npeca do dismounta bo brak template'a na mouncie!!! osoba: "+GetRealName(who), LOGF_SCRIPTS);
      EraseObjProperty(who,"#Dismounting");
      return 0;
    endif
  endif

  if(!Animal.backpack)
    var NewPack := CreateItemAtLoc(6096, 1192, 0, UOBJ_BACKPACK, 1);
    if(!EquipItem(Animal,NewPack))
      DestroyItem(NewPack);
    endif
  endif

  Animal.frozen := 0;
  Animal.facing := who.facing;
  if(!MoveObject(Animal, who.x, who.y, who.z, who.realm, MOVECHAR_FORCELOCATION))
    LogError("[Dismount]: Nie udalo sie przeniesc animala do lokacji gracza: "+GetRealName(who)+" - "+GetLocString(who), LOGF_SCRIPTS);
  endif
  EraseObjProperty(Animal,PROP_MOUNTING);
  SendChEvent(Animal,who,EVID_WAKEUP);

  Animal.hidden := 0;

  if(Animal.concealed != who.concealed)
    Animal.concealed := who.concealed;
  endif

  if(!MoveItemToContainer(mount, Animal.backpack))
    DestroyItem(mount);
  endif

  EraseObjProperty(who, "#Dismounting");
  SetObjProperty(who, "#DismountTime", ReadGameClock());
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetAnimalFromMount(Mount)

  return FindNpcBySerial(GetObjProperty(mount,"AnimalSerial"));

endfunction

//-----------------------------------------------------------------------------

function CanGetMount(master, me)

  if(GetEquipmentByLayer(master, LAYER_MOUNT) or (me.hidden and !master.cmdlevel) or !master)
    return 0;
  endif
  
  if(GetObjProperty(master, "#DismountTime") + 1 > ReadGameClock() and !master.cmdlevel)
    SendSysMessage(master, "Juz cos robisz.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var mounttype := GetMountType(me.graphic);
  if(!mounttype or GetNecroControlledTime(me) or IsSummoned(me))
    SendSysMessage(master, "Tego stworzenia nie da sie ujezdzac.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(master.hidden)
    SendSysMessage(master, "Nie uda Ci sie schowac wierzchowca.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(!CanMod(master,MOD_GRAPHIC) or IsWearwolfMorphed(master))
    Sendsysmessage(master, "Nie mozesz wsiasc na wierzchowca w tej formie.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif
  
  var cfgElem := GetNpcConfig(me);
  if(CInt(cfgElem.GenderRestrict))
    if(CInt(cfgElem.GenderRestrict) - 1 != master.gender and IsPlayer(master) and !IsGM(master))
      SendSysMessage(master,"Nie mozesz wsiasc na to stworzenie.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  if(CInt(GetObjProperty(me, "#IsStabling")))
    LogNotice("[Mount] CanGetMount: "+GetRealName(master)+" ["+master.serial+"]/"+master.acctname+" chcial wsiasc na mounta "+me.name+" po wskazaniu go do przechowania u tresera ["+master.x+" "+master.y+" "+master.z+" "+master.realm+"].");
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetMount(master, me)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  var Hand1 := GetEquipmentByLayer(master,LAYER_HAND1);
  var Hand2 := GetEquipmentByLayer(master,LAYER_HAND2);


  var hand1Item := ItemCfg[Hand1.objtype];
  
  if(hand1Item)
	if(hand1Item.NoMount)
		return 0;
	endif
  endif
  
  var hand2Item := ItemCfg[Hand2.objtype];
  
  if(hand2Item)
	if(hand2Item.NoMount)
		return 0;
	endif
  endif

  if(GetObjProperty(master, "#Mounting"))
    return 0;
  endif

  while(GetObjProperty(master, "#Dismounting"))
    Sleepms(50);
  endwhile

  SetObjProperty(master, "#Mounting", GetPid());

  if(!CanGetMount(master, me))
    EraseObjProperty(master, "#Mounting");
    return 0;
  endif

  var mounttype := GetMountType(me.graphic);
  
  if(MoveObject(master,me.x,me.y,me.z,me.realm))
    EraseObjProperty(master,"#HealTimer");
    EraseObjProperty(master,"#MedTimer");
    master.facing := me.facing;
    master.hidden := 0;

    var Mount := FindObjTypeInContainer(me.backpack, UOBJ_MOUNT);
    if(!Mount)
      Mount         := CreateItemAtLoc(5228, 1182, 0, UOBJ_MOUNT, 1);
      Mount.color   := me.color;
      Mount.graphic := mounttype;
    else
      if(Mount.graphic != mounttype)
        Mount.graphic := mounttype;
      endif

      if(Mount.color != me.color)
        Mount.color := me.color;
      endif
    endif
    
    //usuniecie wszelkich propow - w tym creatora z uobj_mount - potem i tak zostanie taki prop wrzucony w razie czego.
    //w savenpcattrdata
    foreach prop in GetObjPropertyNames(Mount)
      EraseObjProperty(Mount,prop);
    endforeach

    SaveNpcAttrData(me, Mount);

    SetObjProperty(Mount, "AnimalSerial", me.serial);
    SetName(Mount, me.name);
    SendChEvent(me,master,EVID_MOUNT);
    MoveObject(me, 5228, 1182, 0, _DEFAULT_REALM, MOVECHAR_FORCELOCATION);
    me.hidden := 1;
    SetObjProperty(me, PROP_MOUNTING, ReadGameClock());
    EquipItem(master,Mount);
    RecalcVitals(master);
    EraseObjProperty(master, "#Mounting");
    return 1;
  endif

  EraseObjProperty(master, "#Mounting");
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMountType(Graphic)

  return CInt(GetNpcGrapElem(Graphic).MountGrap);

endfunction

//-----------------------------------------------------------------------------

function GetMountNpcTemplateFromMaster(who)

  var mount := GetEquipmentByLayer(who,LAYER_MOUNT);

  if(mount)
    return GetObjProperty(mount, PROP_NPCTEMP);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetMountNpcSerialFromMaster(who)

  var mount := GetEquipmentByLayer(who,LAYER_MOUNT);

  if(mount)
    return GetObjProperty(mount, "AnimalSerial");
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetMountTemplate(MountId,Color := 0)

  if(Color)
    case(Color)
      1109:   case(MountId)
                0x3e9f: return "nightmare";
              endcase
      1133:   case(MountId)
                0x3e91: return "worg";
              endcase
      1152:   case(MountId)
                0x3ea2: return "icemustang";
              endcase
	    1154:   case(MountId)
                0x3eb8: return "snowridgeback";
              endcase
      1161:   case(MountId)
                0x3ea2: return "firemustang";
                0x3e94: return "feniks";
              endcase
      1301:   case(MountId)
                0x3eb8: return "iceridgeback";
              endcase
      0x4001: case(MountId)
                0x3ebc: return "widmowyzuk";
                0x3eb8: return "widmowygarbacz";
                0x3ec5: return "widmowyniedzwiedz";
                0x3e91: return "widmowyworg";
                0x3ead: return "widmowykirin";
                0x3eb4: return "widmowyjednorozec";
                0x3ebd: return "widmowysmok";
                0x3ebe: return "widmowyopancerzonysmok";
                0x3e94: return "widmowyhiryu";
                0x3e90: return "widmowachimera";
                0x3ebb: return "widmowykoszmar";
              endcase 
		    2964: 
              case(MountId)
					      0x3ea4: return "toxicostard";
				      endcase
		    2063:
             case(MountId)
					      0x3ebe: return "naturaldragon";
				      endcase
		    2106:   
              case(MountId)
					        0x3ebb: return "smiercionosnykoszmar";
              endcase
		    1195:   
              case(MountId)
					      0x3ebc: return "lodowcowyzuk";
				      endcase
    endcase
  endif

  case(MountId)
    0x3ea2: return "horse";
    0x3e9f: return "horse2";
    0x3ea0: return "horse3";
    0x3ea1: return "horse4";
    0x3ea6: return "llama";
    0x3ea3: return "desertostard";
    0x3ea4: return "frenziedostard";
    0x3ea5: return "forestostard";
    0x3e9d:
    0x3eb4: return "unicorn";
    0x3eb8: return "ridgeback";
    0x3eaa: return "widmowykon";
    0x3eab: return "widmowalama";
    0x3eac: return "widmowyostard";
    0x3ead: return "kirin";
    0x3eba: return "savageridgeback";
    0x3ebb: return "skeletalmount";
    0x3ebc: return "giantbeatle";
    0x3ebd: return "swampdragon";
    0x3ebe: return "armoredswampdragon";
    0x3eb3: return "seahorse";
    0x3e94: return "hiryu";
    0x3e90: return "chimera";
    0x3e91: return "cusidhe";
    0x3e92: return "armorednightmare";
    0x3ea7: return "nightmare";
    0x3ec5: return "polar";
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function DismountAndKill(corpse,template := 0)

  var mount;
  foreach item in EnumerateItemsInContainer(corpse)
    if(GetObjProperty(item,"AnimalSerial") and item.objtype == UOBJ_MOUNT)
      DestroyItem(item);
    elseif(item.objtype == UOBJ_MOUNT)
      mount := item;
      break;
    endif
  endforeach

  if(!mount)
    return;
  endif

  if(template != "doppelganger")
    var critter := GetMountTemplate(Mount.graphic,Mount.color);
    if(critter)
      var animal := CreateNpc(critter, corpse.x, corpse.y, corpse.z, corpse.realm, DEF_PARMS, NPCF_NO_UPKEEP | NPCF_NO_LOOT);
      if(animal)
        if(template)
          SetObjProperty(animal,"NpcMount",template);
        endif
        SetNpcColor(animal, mount.color);
        animal.truecolor := mount.color;
        DealDamage(animal,GetMaxHp(animal)+1);
      endif
    endif
  endif

  DestroyItem( mount );

endfunction

//-----------------------------------------------------------------------------

function DismountNpcAfterDeath(corpse)

  var mount;
  foreach item in EnumerateItemsInContainer(corpse)
    if(GetObjProperty(item,"AnimalSerial") and item.objtype == UOBJ_MOUNT)
      DestroyItem(item);
    elseif(item.objtype == UOBJ_MOUNT)
      mount := item;
      break;
    endif
  endforeach

  if(!mount)
    return;
  endif

  var critter := GetMountTemplate(Mount.graphic,Mount.color);
  if(critter)
    var animal := CreateNpc(critter, corpse.x, corpse.y, corpse.z, corpse.realm);
    if(animal)
      SetNpcColor(animal, mount.color);
      animal.truecolor := mount.color;
      animal.facing := corpse.faceing;
      if(IsGmCreated(corpse)) 
        DealDamage(animal,GetMaxHp(animal)+1);
      else
        var ev     := struct;
        ev.+type   := EVID_ENGAGED;
        ev.+source := FindNpcBySerial(GetObjProperty(corpse,"LastHit"));
        SendEvent(Animal, ev);
      endif
    endif
  endif

  DestroyItem( mount );

endfunction

//-----------------------------------------------------------------------------

function CanDismount(who)

  if(CInt(GetObjProperty(who, "#NoDismount")) < ReadGameClock())
    return 1;
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function CombatDismount(who)
  
  if(CanDismount(who))
    if(Dismount(who))
      SetModDelay(who, MOD_PARA, 8);
      SetObjProperty(who,"#NoDismount",ReadGameClock() + 30);
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveMount(who)

  var Mount := GetEquipmentByLayer(who, LAYER_MOUNT);

  if(Mount)
    return 1;
  endif

  if(CanMod(who, MOD_RUN_SPEED) == 0)
    if(GetModAmount(who, MOD_RUN_SPEED) == RUNSPEED_FAST)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
