//-----------------------------------------------------------------------------

include "include/logerror";
include ":banking:common";
include "include/items/prices";

include ":crafting:common";
include ":crafting:creation";
include ":craftquests:craftdata";

//-----------------------------------------------------------------------------

const CRAFT_ORDERS_CFG       := ":craftquests:craftorders";
const UOBJ_CRAFT_TICKET      := 0xA601;

//-----------------------------------------------------------------------------

const PROP_CRAFT_DELAY       := "CraftQuestsDelay";
const PROP_CRAFTITEM_GET     := "#CraftItemGet_";

//-----------------------------------------------------------------------------

const CRQS_DELAY             := 1;
const CRQS_NPC_DELAY         := 600;
const CRQS_NPC_MAX_ORDERS    := 10;
const CRQS_IGET_DELAY        := 60;
const CRQS_PRICE_MOD         := 1.03;
const CRQ_MAX_LEVELS         := 5;

const CRQS_MIN_BULKMOD       := 0.5;
const CRQS_MAX_BULKMOD       := 4.0;

const CRQS_MIN_REPGAIN       := 10;
const CRQS_MAX_REPGAIN       := 1000;

const CRQS_EXPIRE_TIME       := 129600;
const CQ_DEF_EXP_TIME        := 86400;

const CRQS_TICKETDONE_CLR    := 999;

const CRQ_SKILL_GAIN_PERC    := 2.4; // bylo 4.2

//-----------------------------------------------------------------------------

function AcceptCraftOrder(who, Npc, Idx, Level, Harvest)
  
  var CraftData;
  GetCraftData(Npc, CraftData);

  if(!Npc or !CraftData or !who or !Idx)
    return;
  endif

  var Order;
  
  if(Harvest)
    Order := CraftData.Main.HarvOrders[Level][Idx];
  else
    Order := CraftData.Main.Orders[Level][Idx];
  endif

  if(!Order)
    return;
  endif

  var Ticket := CreateItemInPack(who, UOBJ_CRAFT_TICKET);
  if(!Ticket)
    return;
  endif

  if(Harvest)
    CraftData.Main.HarvOrders[Level].erase(Idx);
  else
    CraftData.Main.Orders[Level].erase(Idx);
  endif

  CraftData.Main.Given[who.serial] := CInt(CraftData.Main.Given[who.serial]) + 1;

  if(Harvest)
    CraftData.Main.HarvGivenNum += 1;
    CraftData.Stats[Level].HarvGivenNum += 1;
  else
    CraftData.Main.GivenNum += 1;
    CraftData.Stats[Level].GivenNum += 1;
  endif

  Order.+Level   := Level;
  Order.+aTime   := ReadGameClock();
  Order.+Source  := Npc.serial;
  Order.+Player  := who.serial;
  Order.+Price   := 0;
  Order.+Harvest := Harvest;

  foreach Item in (Order.Items)
    Item.+GotAmt := 0;
  endforeach

  SetName(Ticket, "Zamowienie u "+Npc.name);
  SetObjProperty(who, PROP_CRAFT_DELAY, ReadGameClock() + CRQS_DELAY);
  SetObjProperty(Ticket, PROP_CRAFT_DATA, Order);

  TurnCharToward(Npc, who);

  case(RandomInt(3))
    1: PrintTextAbove(Npc, "Dobrze wiec, prosze bardzo o to zamowienie.");
       PrintTextAbove(Npc, "Czekam na Twoj szybki powrot!");
    2: PrintTextAbove(Npc, "Zamowienie znasz, w takim razie pozostalo Ci je wykonac.");
       PrintTextAbove(Npc, "Jesli uda Ci sie wszystko zrobic, to wroc do mnie.");
    default:
       PrintTextAbove(Npc, "Oho! Zamowienie calkiem ciekawe otrzymal"+ggm(who,3)+".");
       PrintTextAbove(Npc, "Jak uda Ci sie wszystko wykonac, wroc i pokaz mi bilet.");
  endcase

  SetCraftData(Npc, CraftData);

endfunction

//-----------------------------------------------------------------------------

function ShowCraftQuests(who, Npc)

  if(!GetNpcConfig(Npc).CanGiveCraftQuest)
    return 0;
  endif

  var CraftData;
  GetCraftData(Npc, CraftData);

  var Delay := GetQuestCraftDelay(who);
  var Res;

  CheckCraftData(Npc, CraftData);

  if(!IsStaff(who))
    if(Delay > 0)
      PrintTextAbove(Npc, "Przyjdz pozniej, a znajdzie sie cos dla Ciebie.");
      return 0;
    endif
  endif

  var curLevel := 1;
  var OrderIdx := 0;
  var Harv := CInt(GetObjProperty(who, "HarvOrders"));

  if(Harv)
    BuildMainHarvestGump(Npc, who, CraftData, curLevel);
  else
    BuildMainCraftGump(Npc, who, CraftData, curLevel);  
  endif

  while(IsLoggedIn(who))
    Res := CInt(GSend(who)[0]);
    if(!Res)
      break;
    endif

    if(Res >= 1 and Res <= CRQS_NPC_MAX_ORDERS)
      if(SendCraftOrderGump(who, Npc, CraftData, Res, curLevel, Harv))
        OrderIdx := Res;
      endif
    elseif(Res > 200 and Res <= 200+CRQ_MAX_LEVELS)
      curLevel := Res-200;
    elseif(Res > 2000 and Res <= 2000+CraftData.Main.Orders[curLevel].size() and IsStaff(who))
      CraftData.Main.Orders[curLevel].erase(Res-2000);
      SendSysMessage(who, "Skasowano zamowienie Nr: "+(Res-2000)+".", FONT_NORMAL, COLOR_GREEN);
      SetCraftData(Npc, CraftData);
    elseif(Res > 2100 and Res <= 2100+CraftData.Main.HarvOrders[curLevel].size() and IsStaff(who))
      CraftData.Main.HarvOrders[curLevel].erase(Res-2100);
      SendSysMessage(who, "Skasowano zamowienie wydobywcze Nr: "+(Res-2000)+".", FONT_NORMAL, COLOR_GREEN);
      SetCraftData(Npc, CraftData);
    else
      case(Res)
        1010:    if(IsStaff(who) or IsOnDMRTest())
                   CraftData.Main.Delay := 0;
                   CheckCraftData(Npc, CraftData, 1);
                 endif

        1011:    if(IsStaff(who))
                   BuildDoneQuestGump(Npc, CraftData);
                   continue;
                 endif

        1012:    Harv := !Harv;
                 SetObjProperty(who, "HarvOrders", Harv);
                 
        1013:    ShowBulks(who, Npc);

      endcase
    endif

    if(OrderIdx)
      var curCraftData;
      GetCraftData(Npc, curCraftData);

      if(Harv)
        if(curCraftData.Main.HarvOrders[curLevel].exists(OrderIdx))
          var curOrder := curCraftData.Main.HarvOrders[curLevel][OrderIdx];
          var oldOrder := CraftData.Main.HarvOrders[curLevel][OrderIdx];

          if(!CompareCraftOrders(curOrder, oldOrder))
            OrderIdx := 0;
          endif
        else
          OrderIdx := 0;
        endif
      else
        if(curCraftData.Main.Orders[curLevel].exists(OrderIdx))
          var curOrder := curCraftData.Main.Orders[curLevel][OrderIdx];
          var oldOrder := CraftData.Main.Orders[curLevel][OrderIdx];

          if(!CompareCraftOrders(curOrder, oldOrder))
            OrderIdx := 0;
          endif
        else
          OrderIdx := 0;
        endif
      endif

      if(!OrderIdx)
        PrintTextAbove(Npc, "Ops.. przepraszam, ale zbyt dlugo zastanawial"+ggm(who,3)+" sie.");
        PrintTextAbove(Npc, "To zamowienie jest juz nieaktualne.");
        PrintTextAbove(Npc, "Spojrz jeszcze raz na dostepne zamowienia.");
        CraftData := curCraftData;
      else
        break;
      endif
    endif

    if(Harv)
      BuildMainHarvestGump(Npc, who, CraftData, curLevel);
    else
      BuildMainCraftGump(Npc, who, CraftData, curLevel);
    endif
  endwhile

  if(OrderIdx)
    var Ret := struct;
    Ret.+Idx := OrderIdx;
    Ret.+Level := curLevel;
    Ret.+Harvest := Harv;
    return Ret;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ShowBulks(who, Npc)

  if(!IsAdmin(who))
    return;
  endif

  var Res := run_script(":bulks:bulk_gump", array(who, Npc));

  if(Res == error)
    SendSysMessage(who, "Blad: "+Res.errortext, FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function CompareCraftOrders(byref oldOrder, byref curOrder)

  if(!oldOrder or !curOrder)
    return 0;
  elseif(GetOrderSkillId(oldOrder) != GetOrderSkillId(curOrder))
    return 0;
  endif

  if(oldOrder.Items.size() != curOrder.Items.size())
    return 0;
  endif

  var i;

  for(i:=1; i<=curOrder.Items.size(); i+=1)
    if(curOrder.Items[i].ObjType != oldOrder.Items[i].ObjType)
      return 0;
    elseif(curOrder.Items[i].Amount != oldOrder.Items[i].Amount)
      return 0;
    elseif(curOrder.Items[i].MatObjType != oldOrder.Items[i].MatObjType)
      return 0;
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function BuildMainCraftGump(Npc, who, byref CraftData, curLevel)

  var gWidth := CRQ_MAX_LEVELS*100+60;
  var i;

  GInit(80, 100);
  GResPic(0,0,3600,gWidth,150+CRQS_NPC_MAX_ORDERS*40);
  GCheckerTrans(15,15,gWidth-30,120+CRQS_NPC_MAX_ORDERS*40);
  GTextLine(30, 17, 1420, "Zamowienia u "+Npc.name);
  
  GButton(gWidth-200, 13, 0x9A8, 0x9AA, 1012);
  GTextLine(gWidth-170, 17, 1320, "Zamowienia wydobywcze");

  for(i:=0; i<=CRQ_MAX_LEVELS-2; i+=1)
    GGumpPic(15+i*97, 40, 0xBB9);
    GGumpPic(15+i*97, 65, 0xBB9);
  endfor

  for(i:=0; i<CRQ_MAX_LEVELS; i+=1)
    if(i+1 == curLevel)
      GButton(25+i*100, 40, 0x9A8, 0x9AA, 200+i+1);
      GTextLine(55+i*100, 45, 1320, "Poziom "+(i+1));
    else
      GButton(25+i*100, 40, 0x25F8, 0x25F9, 200+i+1);
      GTextLine(55+i*100, 45, 1000, "Poziom "+(i+1));
    endif
  endfor

  var Desc;
  i := 0;

  if(IsStaff(who))
    GTextLine(gWidth-50, 65, 999, "Kasuj");
  endif

  foreach Order in (CraftData.Main.Orders[curLevel])
    Desc := "";
    foreach Item in (Order.Items)
      if(!Desc)
        Desc := GetMatDescForItem(Item.ObjType, Item.MatObjType);
      else
        Desc := Desc +", "+GetMatDescForItem(Item.ObjType, Item.MatObjType);
      endif
    endforeach

    Desc := CutWord(Desc, 60);

    GButton(25, 83+i*40, 2117, 2118, i+1);
    GTextLine(45, 80+i*40, 670, (i+1)+".");
    GTextLine(60, 80+i*40, 550, Desc);

    var SkId := GetOrderSkillId(Order);

    if(SkId != error)
      GTextLine(60, 100+i*40, 999, "Umiejetnosc:");
      GTextLine(140, 100+i*40, 1420, GetSkillConfigElem(SkId).Name);
    endif

    if(IsStaff(who))
      GButton(gWidth-30, 83+i*40, 2117, 2118, 2000+i+1);
    endif

    i := i + 1;
  endforeach

  if(IsStaff(who) or IsOnDMRTest())
    GButton(25, 103+CRQS_NPC_MAX_ORDERS*40, 2117, 2118, 1010);
    GTextLine(45, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Opoznienie: ");
    GTextLine(120, 100+CRQS_NPC_MAX_ORDERS*40, 1420, RealTime(CraftData.Main.Delay - ReadGameClock()));

    GTextLine(205, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Wzietych: ");
    GTextLine(265, 100+CRQS_NPC_MAX_ORDERS*40, 1420, CraftData.Main.GivenNum );

    GButton(305, 103+CRQS_NPC_MAX_ORDERS*40, 2117, 2118, 1011);
    GTextLine(325, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Wykonanych: ");
    GTextLine(400, 100+CRQS_NPC_MAX_ORDERS*40, 1420, CraftData.Main.DoneNum );
    
    if(isAdmin(who))
      GButton(435, 103+CRQS_NPC_MAX_ORDERS*40, 2117, 2118, 1013);
      GTextLine(455, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Bulks");
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function BuildMainHarvestGump(Npc, who, byref CraftData, curLevel)

  var gWidth := CRQ_MAX_LEVELS*100+60;
  var i;

  GInit(80, 100);
  GResPic(0,0,3600,gWidth,150+CRQS_NPC_MAX_ORDERS*40);
  GCheckerTrans(15,15,gWidth-30,120+CRQS_NPC_MAX_ORDERS*40);
  GTextLine(30, 17, 1420, "Zamowienia wydobywcze u "+Npc.name);
  
  GButton(gWidth-200, 13, 0x9A8, 0x9AA, 1012);
  GTextLine(gWidth-170, 17, 1320, "Zamowienia na produkty");

  for(i:=0; i<=CRQ_MAX_LEVELS-2; i+=1)
    GGumpPic(15+i*97, 40, 0xBB9);
    GGumpPic(15+i*97, 65, 0xBB9);
  endfor

  for(i:=0; i<CRQ_MAX_LEVELS; i+=1)
    if(i+1 == curLevel)
      GButton(25+i*100, 40, 0x9A8, 0x9AA, 200+i+1);
      GTextLine(55+i*100, 45, 1320, "Poziom "+(i+1));
    else
      GButton(25+i*100, 40, 0x25F8, 0x25F9, 200+i+1);
      GTextLine(55+i*100, 45, 1000, "Poziom "+(i+1));
    endif
  endfor

  var Desc;
  i := 0;

  if(who.cmdlevel)
    GTextLine(gWidth-50, 65, 999, "Kasuj");
  endif

  foreach Order in (CraftData.Main.HarvOrders[curLevel])
    Desc := "";
    foreach Item in (Order.Items)
      if(!Desc)
        Desc := GetMatDescForItem(Item.ObjType, Item.MatObjType);
      else
        Desc := Desc +", "+GetMatDescForItem(Item.ObjType, Item.MatObjType);
      endif
    endforeach

    Desc := CutWord(Desc, 60);

    GButton(25, 83+i*40, 2117, 2118, i+1);
    GTextLine(45, 80+i*40, 670, (i+1)+".");
    GTextLine(60, 80+i*40, 550, Desc);

    var SkId := GetOrderSkillId(Order);

    if(SkId != error)
      GTextLine(60, 100+i*40, 999, "Umiejetnosc:");
      GTextLine(140, 100+i*40, 1420, GetSkillConfigElem(SkId).Name);
    endif

    if(who.cmdlevel)
      GButton(gWidth-30, 83+i*40, 2117, 2118, 2100+i+1);
    endif

    i := i + 1;
  endforeach

  if(who.cmdlevel or IsOnDMRTest())
    GButton(25, 103+CRQS_NPC_MAX_ORDERS*40, 2117, 2118, 1010);
    GTextLine(45, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Opoznienie: ");
    GTextLine(120, 100+CRQS_NPC_MAX_ORDERS*40, 1420, RealTime(CraftData.Main.Delay - ReadGameClock()));

    GTextLine(205, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Wzietych: ");
    GTextLine(265, 100+CRQS_NPC_MAX_ORDERS*40, 1420, CraftData.Main.HarvGivenNum );

    GButton(305, 103+CRQS_NPC_MAX_ORDERS*40, 2117, 2118, 1011);
    GTextLine(325, 100+CRQS_NPC_MAX_ORDERS*40, 550, "Wykonanych: ");
    GTextLine(400, 100+CRQS_NPC_MAX_ORDERS*40, 1420, CraftData.Main.HarvDoneNum );
  endif

endfunction

//-----------------------------------------------------------------------------

function SendCraftOrderGump(who, Npc, byref CraftData, Idx, Level, Harv)

  var Order;

  if(Harv)
    Order := CraftData.Main.HarvOrders[Level][Idx];
  else
    Order := CraftData.Main.Orders[Level][Idx];
  endif

  var MaxHeight := Order.Items.size()*50;

  GInit(80, 100);
  GResPic(20,20,3600,460,150+MaxHeight);
  GCheckerTrans(35,35,430,120+MaxHeight);
  GTextLine(50, 40, 600, "Zamowienie u "+Npc.name);

  var SkId := GetOrderSkillId(Order);
  
  if(SkId != error)
    GTextLine(300, 40, 999, "Umiejetnosc:");
    GTextLine(380, 40, 1420, ClStr(GetSkillConfigElem(SkId).Name));
  endif

  GGumpPic(35, 60, 0xBB9);
  GGumpPic(135, 60, 0xBB9);
  GGumpPic(225, 60, 0xBB9);

  //if(IsStaff(who)) TODO: USUNAC ZA TYDZIEN !!! 2013-05-27!
    GTextLine(350, 120+MaxHeight, 999, "ID: "+Order.Id);
  //endif

  var Desc, i := 1;

  GTextLine(140, 70, 999, "Ilosc");
  GTextLine(170, 70, 999, "Nazwa");

  foreach Item in (Order.Items)
    Desc := GetMatDescForItem(Item.ObjType, Item.MatObjType);
    Desc := CutWord(Desc, 40);

    var craftCfg := GetCraftConfigForItem(SkId, Item.ObjType);

    GResPic(65, 50+i*50, 0xBB8, 70, 46);
    GTilePicHue(75+CInt(craftCfg.GumpX), 60+i*50+CInt(craftCfg.GumpY), GetGraphicForItem(Item.ObjType), GetColorForItem(__IfElse(Item.MatObjType, Item.MatObjType, Item.ObjType)));

    GTextLine(47, 60+i*50,  670, i+".");
    GTextLine(140, 60+i*50, 1024,Item.Amount);
    GTextLine(170, 60+i*50,  550, Desc);

    i := i + 1;
  endforeach

  GButton(50, 123+MaxHeight, 2117, 2118, 1);
  GTextLine(70, 120+MaxHeight, 1420, "Akceptuj");
  GButton(150, 123+MaxHeight, 2117, 2118, 0);
  GTextLine(170, 120+MaxHeight, 1204, "Anuluj");
  
  var Res := GSend(who)[0];

  if(!Res)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function BuildDoneQuestGump(Npc, byref CraftData)

  GInit(80, 100);
  GResPic(20,20,3600,500,400);
  GCheckerTrans(35,35,470,370);
  GTextLine(50, 40, 600, "Wykonane zamowienia u "+Npc.name);
  GAddPageData(486,40,2650,2651,485,383,2648,2647);

  var Player, i := 1;

  foreach Entry in (CraftData.Main.CqDone)
    Player := FindPlayerBySerial(_Entry_iter);
    GCheckNextPage(15);
    GTextLine(50, 70+GNPCount()*20, 999,  i);
    GTextLine(70, 70+GNPCount()*20, 550,  GetRealName(Player));
    GTextLine(260,70+GNPCount()*20, 1320, Entry[1]);
    GTextLine(320,70+GNPCount()*20, 1000, Entry[2]);
    GTextLine(390,70+GNPCount()*20, 670,  ShortRealTime(ReadGameClock() - Entry[3]));
    i := i + 1;
  endforeach

  GButton(50, 83+15*20, 2117, 2118, 1001);
  GTextLine(70, 80+15*20, 1424, "Wroc");

endfunction

//-----------------------------------------------------------------------------

function GetQuestCraftDelay(who)
  
  var Delay := CInt(GetObjProperty(who, PROP_CRAFT_DELAY));
  if(Delay - ReadGameClock() < 0)
    return 0;
  else
    return Delay - ReadGameClock();
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckOrderComplete(Npc, who, byref Order)

  foreach Item in (Order.Items)
    if(Item.GotAmt != Item.Amount)
      return 0;
    endif
  endforeach

  var RepGain := CInt(Order.Price / (NGD_EXP_MODIFIER * 0.3));
  var City    := FindCity(Npc);

  if(!CreateGold(who.backpack, Order.Price, who))
    if(!CreateGold(FindBankBox(who), Order.Price, who))
      PrintTextAbove(Npc, "Przyjdz, jak bedziesz miec troche wiecej miejsca w plecaku.");
      return 1;
    else
      PrintTextAbove(Npc, "Brak Ci miejsca w plecaku na zloto, wiec przekazal"+ggm(Npc,8)+" Twoja zaplate bankierowi.");
    endif
  endif
  
   if(Order.Level >= 5)
	var glyph := CreateItemInPack(who, 0x3391, 1);
	if(glyph)
		SendSysMessage(who, "Za Twoje trudy glif pojawil sie w Twoim plecaku.", FONT_NORMAL, COLOR_BLUE);
	endif
  endif

  case(RandomInt(5))
    1:       PrintTextAbove(Npc, "Dziekuje, tego mi trzeba bylo.");
    2:       PrintTextAbove(Npc, "Dziekuje za skompletowanie zamowienia.");
    3:       PrintTextAbove(Npc, "Tak, to wszystkie przedmioty z zamowienia, dziekuje!");
    4:       PrintTextAbove(Npc, "Wlasnie! Tego mi brakowalo, dzieki!");
    default: PrintTextAbove(Npc, "Bardzo milo. W koncu bede mogl"+ggm(Npc, GGM_A)+" dokonczyc prace.");
  endcase

  PrintTextAbovePrivate(Npc, "Prosze, to zaplata w wysokosci "+Order.Price+" gp za zamowienie.", who);

  var SkId := GetOrderSkillId(Order);
  
  var brokenItem := 0;

  if(SkId != error)

    var SkElem := GetSkillConfigElem(SkId);
    var SkGain := 0;

    if(SkElem.AttrGroup != "crafting")
      var Gain := Max(CInt(GetHarvestPoints(SkId, GetBaseSkill(who, SkId)) * 0.1), 1);

      foreach Item in (Order.Items)
        SkGain += Max(1, CInt(Item.amount*4)) * Gain;
      endforeach

    else
      var ItemGain;

      foreach Item in (Order.Items)
	  
	  if(Item.graphic == 0x97B)
		brokenItem := 1;
	  endif
	  
        var cData := GetCraftDataForItem(Item.ObjType, SkId);

        if(cData)
          var Gain := GetCraftPoints(cData.SkillId, GetBaseSkill(who, cData.SkillId)); 
          ItemGain := GetRestrictedPoints(who, cData.SkillId, cData.Diff, Gain, GRPF_FOR_BASE);
          SkGain += ItemGain * Item.Amount;
        endif
      endforeach
    endif

    if(SkGain)
      SkGain := CInt(SkGain * CRQ_SKILL_GAIN_PERC);
      
      var StGain := STAT_ALWAYS_GAIN;
      var MinSk := 10 + Order.Level*25;

      if(StGain)
        if(GetEffectiveSkill(who, SkId) > MinSk)
          StGain := NO_STAT_AWARD;
        endif
      endif

      if(StGain == STAT_ALWAYS_GAIN)
		if(!brokenItem)
			AwardPoints(who, SkId, SkGain, STAT_GAIN_ENHANCED | STAT_ALWAYS_GAIN, MinMax(CInt(SkGain*0.001), 5, 21));
		endif
      else
		if(!brokenItem)
			AwardPoints(who, SkId, SkGain, NO_STAT_AWARD);
		endif
      endif

      SendSysMessage(who, "Twoja umiejetnosc "+SkElem.Name+" wzrosla.", FONT_NORMAL, COLOR_WHITE);

      RepGain := CInt(SkGain / 10);
    endif
  endif

  RepGain := MinMax(RepGain, CRQS_MIN_REPGAIN, CRQS_MAX_REPGAIN);

  GainGuildExpInCity(who, City, RepGain);
  
  var evData := struct;
  evData.+npc := Npc;
  evData.+source := who;
  evData.+price := Order.Price;
  start_script(":bulks:bulk_check", evData);

  var CraftData;
  GetCraftData(Npc, CraftData);
  CheckCraftData(Npc, CraftData);

  if(Order.Harvest)
    CraftData.Main.HarvDoneNum += 1;
  else
    CraftData.Main.DoneNum += 1;
  endif

  if(Order.Level)
    if(Order.Harvest)
      CraftData.Stats[Order.Level].HarvDoneNum += 1;
    else
      CraftData.Stats[Order.Level].DoneNum += 1;
    endif
  endif

  if(!CraftData.Main.CqDone.exists(who.serial))
    CraftData.Main.CqDone[who.serial] := array(1, Order.Price, ReadGameClock());
  else
    CraftData.Main.CqDone[who.serial][1] := CInt(CraftData.Main.CqDone[who.serial][1]) + 1;
    CraftData.Main.CqDone[who.serial][2] := CInt(CraftData.Main.CqDone[who.serial][2]) + Order.Price;
    CraftData.Main.CqDone[who.serial][3] := ReadGameClock();
  endif

  SetCraftData(Npc, CraftData);

  Order.+Comp := 1;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SubtractCraftItems(who, Item, byref Table, byref ItemList, RepLvl, byref Price)

  if(Item.isa(POLCLASS_CONTAINER) and Item.item_count-1 > 0)
    foreach cItem in ListRootItemsInContainer(Item)
      if(cItem.isa(POLCLASS_CONTAINER) and cItem.item_count-1 > 0)
        SubtractCraftItems(who, cItem, Table, ItemList, RepLvl, Price);
      else
        SubtractCraftItem(who, cItem, Table, ItemList, RepLvl, Price);
      endif
    endforeach
  else
    SubtractCraftItem(who, Item, Table, ItemList, RepLvl, Price);
  endif

endfunction

//-----------------------------------------------------------------------------

function SubtractCraftItem(who, Item, byref Table, byref ItemList, RepLvl, byref Price)

  var Idx := Table[Item.objtype];

  if(Idx)
    if(GetItemMakerSerial(Item) != who.serial and GetHarvesterSerial(Item) != who.serial)
      return;
    endif

    if(ItemList[Idx].MatObjType)
      if(GetItemMakeMod(Item) != GetMaterialMod(ItemList[Idx].MatObjType))
        return;
      endif
    endif
  endif

  if(ItemList[Idx].GotAmt < ItemList[Idx].Amount)
    var sAmt := Item.amount;
    if(ItemList[Idx].GotAmt + sAmt > ItemList[Idx].Amount)
      sAmt := ItemList[Idx].Amount - ItemList[Idx].GotAmt;
    endif
    var ItemPrice := GetItemBuyPrice(Item, RepLvl);
    if(SubtractAmount(Item, sAmt))
      ItemList[Idx].GotAmt := ItemList[Idx].GotAmt + sAmt;
      Price := Price + CInt((CDbl(ItemPrice) * CRQS_PRICE_MOD) * sAmt);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckCraftItemCont(Item, byref Table)

  var Res;
 
  foreach cItem in ListRootItemsInContainer(Item)
    if(cItem.isa(POLCLASS_CONTAINER) and cItem.item_count-1 > 0)
      Res := CheckCraftItemCont(cItem, Table);
    elseif(Table[cItem.objtype])
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GenerateCraftQuest(Npc, Level, Harvest)

  var cfgOrders := ReadConfigFile(CRAFT_ORDERS_CFG);
  var Order    := struct;
  var Ids      := GetConfigIntKeys(cfgOrders);
  var OrderIds := array;

  Order.+Id    := 0;
  Order.+Items := array;
  Order.+sTime := ReadGameClock();

  var NpcTemp := Npc.npctemplate;

  if(NpcTemp[1, 3] == "elf")
    NpcTemp[1, 3] := "";
  endif
  
  if(NpcTemp[1, 6] == "tokuno")
    NpcTemp[1, 6] := "";
  endif

  var Templates;
  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);
  var attrGroup := __IfElse(Harvest, array("misc", "harvest"), array("crafting"));

  foreach Id in Ids
    var cfgElem := cfgOrders[Id];
    if(cfgElem.Level == Level)
      if(AttrCfg[cfgElem.Attribute].AttrGroup in attrGroup)
        Templates := GetConfigStringArray(cfgElem, "Template");
        if(NpcTemp in Templates or Npc.npctemplate in Templates)
          OrderIds.append(Id);
        endif
      endif
    endif
  endforeach

  Order.Id := OrderIds[RandomInt(OrderIds.size())+1];

  if(!Order.Id or !OrderIds.size())
    LogError("GenerateCraftQuest: Nie udalo sie okreslic ID CQ na Level ["+Level+"] AttrGrupa ["+attrGroup+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"/"+NpcTemp+"]");
    return 0;
  endif
  
  Order.+SkillId := GetSkillIdByAttributeId(cfgOrders[Order.Id].Attribute);
  Order.+ExpTime := cfgOrders[Order.Id].ExpTime;

  var cItems  := GetConfigStringArray(cfgOrders[Order.Id], "Item");
  var cGroups := GetConfigStringArray(cfgOrders[Order.Id], "Group");

  if((!cItems or !cItems.size()) and (!cGroups or !cGroups.size()))
    LogError("GenerateCraftQuest: Nie znaleziono przedmiotow dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return 0;
  endif

  var OrdItem         := struct;
  var aRes;
  OrdItem.+ObjType    := 0;
  OrdItem.+Amount     := 0;
  OrdItem.+MatObjType := 0;

  foreach cItem in cItems
    cItem := SplitWords(cItem);
    aRes := FillCraftQuestItem(Npc, Order, OrdItem, cItem);
    if(aRes == error)
      return 0;
    elseif(aRes)
      Order.Items.append(OrdItem);
    endif
  endforeach

  foreach cGrp in cGroups
    cGrp := SplitWords(cGrp);
    var ObjType := GetRandomItemFromGroup(cGrp[1]);

    if(!ObjType)
      LogError("GenerateCraftQuest: "+ObjType.errortext+" CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
      return 0;
    endif

    cGrp[1] := ObjType;

    aRes := FillCraftQuestItem(Npc, Order, OrdItem, cGrp);
    if(aRes == error)
      return 0;
    elseif(aRes)
      Order.Items.append(OrdItem);
    endif
  endforeach

  if(!Order.Items.size())
    LogError("GenerateCraftQuest: Nie udalo sie wygenerowac zadnego przedmiotu w CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return 0;
  endif

  return Order;

endfunction

//-----------------------------------------------------------------------------

function FillCraftQuestItem(Npc, byref Order, byref OrdItem, byref GrpItem)

  var ItemCfg  := ReadConfigFile(ITEMS_CFG);

  if(GrpItem[1] == error or GrpItem[2] == error)
    LogError("FillCraftQuestItem: Bledny wpis przedmiotu ["+GrpItem+"] dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return error;
  endif

  GrpItem[3] := CInt(GrpItem[3]);

  if(!GrpItem[3])
    LogError("FillCraftQuestItem: Brak szansy we wpisie ["+GrpItem[1]+"] dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return error;
  endif

  if(RandomInt(100) > GrpItem[3])
    return 0;
  endif
    
  OrdItem.ObjType := GetItemObjType(GrpItem[1]);

  if(!ItemCfg[OrdItem.ObjType])
    LogError("FillCraftQuestItem: Bledny ObjType ["+GrpItem[1]+"] dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return error;
  endif

  foreach oItem in (Order.Items)
    if(oItem.ObjType == OrdItem.ObjType)
      return 0;
    endif
  endforeach

  OrdItem.Amount := RandomDiceRoll(GrpItem[2]);

  if(!GrpItem[2])
    LogError("FillCraftQuestItem: Brak ilosci przedmiotu w ["+GrpItem[1]+"] dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
    return error;
  endif

  if(IsItemGroupName(GrpItem[4]))
    OrdItem.MatObjType := GetRandomItemFromGroup(GrpItem[4]);

    if(!OrdItem.MatObjType)
      LogError("FillCraftQuestItem: Definicja materialu: "+OrdItem.errortext+" ["+GrpItem[1]+"] dla CQ ID ["+Order.Id+"], Npc ["+Npc.serial+"/"+Npc.npctemplate+"]");
      return OrdItem.MatObjType;
    endif
  else
    OrdItem.MatObjType := GetItemObjType(GrpItem[4]);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetOrderSkillId(byref Order)

  // funkcja dla kompatybilnosci wstecz - aby stare zamowienia nie zesraly sie od razu i mialy szanse na uzupelnienie skid.
  var SkId := Order.SkillId;

  if(SkId == error and Order.Id)
    var Cfg := ReadConfigFile(CRAFT_ORDERS_CFG);

    if(Cfg[Order.Id].Attribute)
      SkId := GetSkillIdByAttributeId(Cfg[Order.Id].Attribute);
    endif
  endif

  return SkId;

endfunction

//-----------------------------------------------------------------------------


