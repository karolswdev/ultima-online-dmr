//-----------------------------------------------------------------------------

const PROP_CRAFT_DATA        := "CraftQuestsData";
const PROP_CRAFT_STATS       := "CraftQuestsStats";

//-----------------------------------------------------------------------------

function GetCraftData(Npc, byref CraftData)

  CraftData := struct;
  CraftData.+Main := GetObjProperty(Npc, PROP_CRAFT_DATA);
  CraftData.+Stats := GetObjProperty(Npc, PROP_CRAFT_STATS);

  if(!CraftData.Main or !IsVarType(CraftData.Main, VAR_STRUCT))
    CraftData.Main           := struct;
    CraftData.Main.+Orders   := array;
    CraftData.Main.+Delay    := 0;
    CraftData.Main.+GivenNum := 0;
    CraftData.Main.+DoneNum  := 0;
    CraftData.Main.+HarvGivenNum := 0;
    CraftData.Main.+HarvDoneNum  := 0;
    CraftData.Main.+CqDone   := dictionary;
  endif

  if(!CraftData.Main.GivenNum and CraftData.Main.GivenNum != 0)
    CraftData.Main.+GivenNum;
    CraftData.Main.GivenNum := 0;
  endif

  if(!CraftData.Main.DoneNum and CraftData.Main.DoneNum != 0)
    CraftData.Main.+DoneNum;
    CraftData.Main.DoneNum := 0;
  endif

  if(!CraftData.Main.HarvGivenNum and CraftData.Main.HarvGivenNum != 0)
    CraftData.Main.+HarvGivenNum;
    CraftData.Main.HarvGivenNum := 0;
  endif

  if(!CraftData.Main.HarvDoneNum and CraftData.Main.HarvDoneNum != 0)
    CraftData.Main.+HarvDoneNum;
    CraftData.Main.HarvDoneNum := 0;
  endif

  if(!CraftData.Stats or !IsVarType(CraftData.Stats, VAR_ARRAY))
    CraftData.Stats := array;

    var i;
    for(i:=1; i<=CRQ_MAX_LEVELS; i+=1)
      var Data := struct;
      Data.+GivenNum := 0;
      Data.+DoneNum := 0;
      Data.+HarvGivenNum := 0;
      Data.+HarvDoneNum := 0;

      CraftData.Stats[i] := Data;
    endfor
  endif
  
  var i;
  for(i:=1; i<=CRQ_MAX_LEVELS; i+=1)
    if(!CraftData.Stats[i].HarvGivenNum and CraftData.Stats[i].HarvGivenNum != 0)
      CraftData.Stats[i].+HarvGivenNum := 0;
    endif
    if(!CraftData.Stats[i].HarvDoneNum and CraftData.Stats[i].HarvDoneNum != 0)
      CraftData.Stats[i].+HarvDoneNum := 0;
    endif
  endfor

  if(!CraftData.Main.CqDone)
    CraftData.Main.+CqDone;
    CraftData.Main.CqDone := dictionary;
  endif

  if(!IsVarType(CraftData.Main.Orders, VAR_ARRAY))
    CraftData.Main.Orders := array;
  endif
  
  if(!IsVarType(CraftData.Main.HarvOrders, VAR_ARRAY))
    CraftData.Main.HarvOrders := array;
  endif
  
  var n;

  for(n:=1; n<=CRQ_MAX_LEVELS; n+=1)
    if(CraftData.Main.Orders.size() < n)
      CraftData.Main.Orders[n] := array;
    elseif(!IsVarType(CraftData.Main.Orders[n], VAR_ARRAY))
      CraftData.Main.Orders[n] := array;
    endif
    
    if(CraftData.Main.HarvOrders.size() < n)
      CraftData.Main.HarvOrders[n] := array;
    elseif(!IsVarType(CraftData.Main.HarvOrders[n], VAR_ARRAY))
      CraftData.Main.HarvOrders[n] := array;
    endif
  endfor

  while(CraftData.Main.Orders.size() > CRQ_MAX_LEVELS)
    CraftData.Main.Orders.erase(CraftData.Main.Orders.size());
  endwhile
  
  while(CraftData.Main.HarvOrders.size() > CRQ_MAX_LEVELS)
    CraftData.Main.HarvOrders.erase(CraftData.Main.HarvOrders.size());
  endwhile

endfunction

//-----------------------------------------------------------------------------

function SetCraftData(Npc, byref CraftData)

  SetObjProperty(Npc, PROP_CRAFT_DATA, CraftData.Main);
  SetObjProperty(Npc, PROP_CRAFT_STATS, CraftData.Stats);

endfunction

//-----------------------------------------------------------------------------

function CheckCraftData(Npc, byref CraftData, ForceChange := 0)

  var i, n, fullCount, harvCount;
  var bChanged := ForceChange;
  var bError := 0;

  while(CraftData.Main.Delay < ReadGameClock())
    if(!CraftData.Main.Delay)
      CraftData.Main.Delay := ReadGameClock() + CRQS_NPC_DELAY + (RandomInt(CInt(CRQS_NPC_DELAY/2)));
    else
      CraftData.Main.Delay := ReadGameClock() + CRQS_NPC_DELAY + (RandomInt(CInt(CRQS_NPC_DELAY/2))) - (ReadGameClock() - CraftData.Main.Delay);
    endif

    fullCount := 0;
    harvCount := 0;

    for(i:=1; i<=CRQ_MAX_LEVELS; i+=1)

      if(CraftData.Main.Orders[i].size() < CRQS_NPC_MAX_ORDERS)
        var NewOrder := GenerateCraftQuest(Npc, i, 0);
        if(!NewOrder)
          bError := 1;
        else
          CraftData.Main.Orders[i].append(NewOrder);
          bChanged := 1;
        endif
      else
        fullCount += 1;
      endif

      if(CraftData.Main.HarvOrders[i].size() < CRQS_NPC_MAX_ORDERS)
        var NewOrder := GenerateCraftQuest(Npc, i, 1);
        if(!NewOrder)
          bError := 1;
        else
          CraftData.Main.HarvOrders[i].append(NewOrder);
          bChanged := 1;
        endif
      else
        harvCount += 1;
      endif

    endfor

    if((fullCount == CRQ_MAX_LEVELS and harvCount == CRQ_MAX_LEVELS) or bError)
      CraftData.Main.Delay := ReadGameClock() + CRQS_NPC_DELAY + (RandomInt(CInt(CRQS_NPC_DELAY/2)));
    endif

  endwhile

  for(n:=1; n<=CRQ_MAX_LEVELS; n+=1)
  
    for(i:=1; i<=CraftData.Main.Orders[n].size(); i+=1)
      if(CraftData.Main.Orders[n][i].sTime + CRQS_EXPIRE_TIME < ReadGameClock())
        var NewOrder := GenerateCraftQuest(Npc, n, 0);
        if(NewOrder)
          CraftData.Main.Orders[n][i] := NewOrder;
        else
          CraftData.Main.Orders[n].erase(i);
          i := i - 1;
        endif
      endif
    endfor

    for(i:=1; i<=CraftData.Main.HarvOrders[n].size(); i+=1)
      if(CraftData.Main.HarvOrders[n][i].sTime + CRQS_EXPIRE_TIME < ReadGameClock())
        var NewOrder := GenerateCraftQuest(Npc, n, 1);
        if(NewOrder)
          CraftData.Main.HarvOrders[n][i] := NewOrder;
        else
          CraftData.Main.HarvOrders[n].erase(i);
          i := i - 1;
        endif
      endif
    endfor

  endfor

  if(bChanged)
    SetCraftData(Npc, CraftData);
  endif

endfunction

//-----------------------------------------------------------------------------
