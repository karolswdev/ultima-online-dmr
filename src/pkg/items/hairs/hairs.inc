//================================================================================
//--------------------------------------------------------------------------------
// Hairs Dye
//--------------------------------------------------------------------------------
//================================================================================

function BarberGump(byref Event)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  var who     := Event.source;
  var npc     := Event.npc;

  var Hairs   := array;

  foreach Hair in SplitWords(GetNpcConfig(npc).HairsList)
    Hair := CInt(Hair);
    if(CanCut(who, Hair))
      Hairs.append(Hair);
    endif
  endforeach

  if(!Hairs.size())
    if(!GetEquipmentByLayer(who, LAYER_HAIR) and !GetEquipmentByLayer(who, LAYER_BEARD))
      PrintTextAbove(npc, "Twojej fryzury nie da sie przyciac.");
      Event   := struct;
      Event.+type   := EVID_NH_BARBER_GUMP;
      Event.+npc    := npc;
      Event.+source := who;
      Event.+res    := 0;
      SendEvent(npc,Event);
      return;
    endif
  endif

  var GrapID, i;

  foreach Hair in Hairs
    Hair := CInt(Hair);
  endforeach
  
  case(who.gender)
    0: GrapID := "GumpGrapM";
    1: GrapID := "GumpGrapF";
  endcase

  GInit(100,50);
  GPage();
  GResPic(0,0,BKGND_BLACKSLAB,480,410);
  GResPic(230,45,BKGND_GOLDTRIM,200,300);
  GTextLine(290,20,560,"Strzyzenie");

  if(GetEquipmentByLayer(who,LAYER_HAIR))
    GTextLine(260,350,550,"Ogolenie glowy, 20 gp");
    GButton(240,350,2714,2715,999999);
  endif

  if(GetEquipmentByLayer(who,LAYER_BEARD))
    GTextLine(260,370,550,"Ogolenie brody, 20 gp");
    GButton(240,370,2714,2715,999998);
  endif

  if(who.graphic == CID_ELF_MALE or who.graphic == CID_ELF_FEMALE)
    case(who.gender)
      0: GGumpPic(220,15,0x766);
         GGumpPic(220,15,0x739);
         GGumpPic(220,15,0x738);
      1: GGumpPic(220,15,0x765);
         GGumpPic(220,15,0x714);
         GGumpPic(220,15,0x764);
    endcase
  else
    case(who.gender)
      0: GGumpPic(220,15,0x761);
         GGumpPic(220,15,0x739);
         GGumpPic(220,15,0x738);
      1: GGumpPic(220,15,0x760);
         GGumpPic(220,15,0x714);
         GGumpPic(220,15,0x764);
    endcase
  endif

  i := 2;
  foreach Hair in Hairs
    if(GetConfigInt(ItemCfg[Hair],GrapID))
      GTextLine(55,12+((i-1)*20),550,ClStr(ItemCfg[Hair].desc));
      GButtonPage(35,12+((i-1)*20),2714,2715,i);
      i := i + 1;
    endif
  endforeach
  GPage();

  foreach Hair in Hairs
    if(GetConfigInt(ItemCfg[Hair],GrapID))
      GPage();
      GGumpPic(220,15,GetConfigInt(ItemCfg[Hair],GrapID));
      GTextLine(310,310,550,CInt(ItemCfg[Hair].VendorSellsFor) + " gp");
      GButton(290,310,2714,2715,Hair);
    endif
  endforeach

  var Res := GSend(who)[0];
  Event   := struct;
  
  Event.+type   := EVID_NH_BARBER_GUMP;
  Event.+npc    := npc;
  Event.+source := who;
  Event.+res    := Res;
  
  SendEvent(npc,Event);
  
endfunction

//-----------------------------------------------------------------------------

function CanCut(who, Obj)

  if(Obj >= 999998)
    return 1;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Hair := GetEquipmentByLayer(who,CInt(ItemCfg[Obj].FaceLoc));

  if(!Hair)
    return 0;
  elseif(Hair.objtype == Obj)
    return 0;
  endif
  
  case(CInt(ItemCfg[Obj].FaceLoc))
    LAYER_HAIR:   if(ItemCfg[Obj].LongHairs)
                    if(ItemCfg[Hair.objtype].LongHairs)
                      return 1;
                    endif
                  elseif(ItemCfg[Obj].ShortHairs)
                    if(ItemCfg[Hair.objtype].LongHairs or ItemCfg[Hair.objtype].ShortHairs)
                      return 1;
                    endif
                  endif
    
    LAYER_BEARD:  foreach FromBeard in GetConfigStringArray(ItemCfg[Obj],"FromBeard")
                    if(CInt(FromBeard) == Hair.objtype)
                      return 1;
                    endif
                  endforeach
  endcase
  
  return 0;
  
endfunction

//--------------------------------------------------------------------------------
