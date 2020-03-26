//-----------------------------------------------------------------------------

include "include/log";
include "include/npcs";
include "include/attributes/main";

//-----------------------------------------------------------------------------

function Possess( Npc, who )

  if(GetObjProperty(Npc,"Possessed"))
    return;
  endif

  LogMsg(POSSESS_LOG,who,Npc);

  if(IsPossessing(who))
    UnPossess(who);
  endif

  var Skills := {};
  var i;
  
  who.concealed := 1;
  var maxSkId := GetMaxSkillId();
  
  for(i:=0;i<=maxSkId;i:=i+1)
    Skills[i+1] := GetBaseSkillBaseValue(who,i);
    SetBaseSkillBaseValue(who,i,GetBaseSkillBaseValue(Npc,i));
  endfor

  var Sets  := {};
  Sets[1]   := who.name;
  Sets[2]   := who.color;
  Sets[3]   := who.graphic;
  Sets[4]   := who.title_prefix;
  Sets[5]   := who.title_suffix;
  Sets[6]   := who.title_guild;
  Sets[7]   := who.title_race;
  Sets[12]  := who.gender;
  Sets[13]  := who.criminal;
  Sets[14]  := who.murderer;

  var EqItems   := {};
  var Vitals    := {};

  Sleepms(1);

  who.color        := Npc.color;
  who.graphic      := Npc.graphic;
  who.name         := Npc.name;
  who.facing       := Npc.facing;
  who.title_prefix := Npc.title_prefix;
  who.title_suffix := Npc.title_suffix;
  who.title_guild  := Npc.title_guild;
  who.title_race   := Npc.title_race;
  
  who.gender       := Npc.gender;

  if(Npc.criminal)
    SetCriminal(who, 1, "Possess", "Possessowany Npc "+Npc.name+" ["+Npc.serial+"] mial krima.");
  else
    SetCriminal(who, 0);
  endif
  
  if(Npc.murderer)
    who.SetMurderer(1);
  else
    who.SetMurderer(0);
  endif

  Vitals[1]     := GetBaseStrength(who);
  Vitals[2]     := GetBaseDexterity(who);
  Vitals[3]     := GetBaseIntelligence(who);

  SetBaseStrength(who,    GetBaseStrength(Npc));
  SetBaseDexterity(who,   GetBaseDexterity(Npc));
  SetBaseIntelligence(who,GetBaseIntelligence(Npc));

  SetHp(who,GetStrength(who) * (CDbl(GetHp(Npc)) / GetMaxHp(Npc)));
  SetMana(who,GetMana(Npc));
  SetStamina(who,GetStamina(Npc));
  
  var Item;
  for(i:=1;i<=25;i:=i+1)
    if(i != LAYER_PACK)
      Item := GetEquipmentByLayer(who,i);
      if(Item)
        EqItems.append(item.serial);
        item.movable := 1;
        MoveToPack(item, who);
      endif
    endif
  endfor

  Sleepms(1);
  
  for(i:=1;i<=25;i:=i+1)
    if(i != LAYER_PACK)
      Item := GetEquipmentByLayer(Npc,i);
      if(Item)
        item.movable := 1;
        EquipItem(who,Item);
      endif
    endif
  endfor
  
  Sets[8]  := Skills;
  Sets[9]  := EqItems;
  Sets[10] := Vitals;
  Sets[11] := Npc.npctemplate;

  SetObjProperty(who,"Possess",Npc.serial);
  SetObjProperty(who,"PossessSets",Sets);
  SetObjProperty(Npc,"Possessed",who.serial);
  
  MoveObject(who,Npc.x,Npc.y,Npc.z,Npc.realm,MOVECHAR_FORCELOCATION);
  MoveObject(Npc,6104,1183,0,BRITB_REALM,MOVECHAR_FORCELOCATION);
  who.concealed := 0;
  who.hidden    := Npc.hidden;
  Npc.frozen    := 1;
  Npc.concealed := 1;

endfunction

//-----------------------------------------------------------------------------

function IsPossessing(who)

  if(GetObjProperty(who,"Possess"))
    return SystemFindObjectBySerial(GetObjProperty(who,"Possess"));
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function UnPossess(who)

  var Npc := IsPossessing(who);
  
  if(GetObjProperty(who,"Possess"))
  
    var Sets         := GetObjProperty(who,"PossessSets"),i;
    if(!Sets)
      return;
    endif

    who.concealed := 1;

    if(!Npc)
      Npc := CreateNpc(Sets[11],who.x,who.y,who.z,who.realm);
      foreach Item in ListEquippedItems(who)
        DestroyItem(Item);
      endforeach
    else
      var Item;
      for(i:=1;i<=25;i:=i+1)
        if(i != LAYER_PACK)
          Item := GetEquipmentByLayer(who,i);
          if(Item)
            item.movable := 1;
            EquipItem(Npc,Item);
          endif
        endif
      endfor
    endif
    
    MoveObject(Npc,who.x,who.y,who.z,who.realm,MOVECHAR_FORCELOCATION);
    Npc.concealed := 0;
    Npc.frozen := 0;

    var Skills       := Sets[8];
    var EqItems      := Sets[9];
    var Vitals       := Sets[10];

    who.name         := Sets[1];
    who.color        := Sets[2];
    who.graphic      := Sets[3];
    who.title_prefix := Sets[4];
    who.title_suffix := Sets[5];
    who.title_guild  := Sets[6];
    who.title_race   := Sets[7];
    who.gender       := Sets[12];

    SetCriminal(who, Sets[13], "UnPossess", "Krim ustawiony po unpossessie, bo przed possessem byl krim. Npc ["+Npc.serial+"].");
    who.SetMurderer(Sets[14]);

    foreach Itm in EqItems
      Itm := SystemFindObjectBySerial(Itm);
      if(Itm)
        Itm.movable := 1;
        EquipItem(who,Itm);
      endif
    endforeach

    for(i:=1;i<=Skills.size();i:=i+1)
      SetBaseSkillBaseValue(who,i-1,Skills[i]);
    endfor
    
    SetBaseStrength(who,Vitals[1]);
    SetBaseDexterity(who,Vitals[2]);
    SetBaseIntelligence(who,Vitals[3]);

    SetHp(who, GetMaxHp(who));
    SetMana(who, GetMaxMana(who));
    SetStamina(who, GetMaxStamina(who));
    
    EraseObjProperty(who,"Possess");
    EraseObjProperty(who,"PossessSets");
    EraseObjProperty(Npc,"Possessed");

  endif

endfunction

//-----------------------------------------------------------------------------
