//-----------------------------------------------------------------------------
// Modified: 2005-07-07
//-----------------------------------------------------------------------------

include "mqdata";

//-----------------------------------------------------------------------------
// Quest Types
//-----------------------------------------------------------------------------

const MQTYPE_CARRYITEM       := 1;
const MQTYPE_TAKEITEM        := 2;
const MQTYPE_GOTONPC         := 3;

//-----------------------------------------------------------------------------

function InitMQuestTarg(byref Targ, Type)

  case(Type)
    MQTYPE_CARRYITEM:    
    MQTYPE_TAKEITEM:     InitMQNpcItemTarg(Targ);
    MQTYPE_GOTONPC:      InitMQNpcTarg(Targ);
  endcase

endfunction 

//-----------------------------------------------------------------------------

function AddMiniQuest(byref MQData, who := 0)

  var Err := VerifyMiniQuest(MQData);

  if(!Err)
    return Err;
  endif

  Err := error;

  var NextID := GetNextMQuestID();

  if(!NextID)
    Err.errortext := "Nie udalo sie pobrac nastepnego IDa MiniQuestu.";
    return Err;
  endif
 
  var QuestData := GetMQuestData();
  var NewQuest  := QuestData.CreateElement(MQUEST_TITLE+CStr(NextID));

  if(!NewQuest)
    Err.errortext := "Nie udalo sie stworzyc nowego elementu w MQuestData.";
    return Err;
  endif

  NewQuest.SetProp(MQPROP_NPCTEMP,  MQData.NpcTemp);
  NewQuest.SetProp(MQPROP_TYPE,     MQData.Type);
  NewQuest.SetProp(MQPROP_TARG,     MQData.Targ);
  NewQuest.SetProp(MQPROP_LOC,      MQData.Loc);
  NewQuest.SetProp(MQPROP_TITLE,    MQData.Title);
  NewQuest.SetProp(MQPROP_ENCRTEXT, MQData.EncrText);
  NewQuest.SetProp(MQPROP_MAINTEXT, MQData.MainText);
  NewQuest.SetProp(MQPROP_SUCCTEXT, MQData.SuccText);
  NewQuest.SetProp(MQPROP_DNPCTEXT, MQData.DNpcText);
  NewQuest.SetProp(MQPROP_FLAGS,    MQData.Flags);

  SetMQuestElemCrInfo(NewQuest, who, NextID);

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function SetMiniQuest(QID, byref MQData, who := 0)

  var Err := VerifyMiniQuest(MQData);

  if(!Err)
    return Err;
  endif

  Err := error;

  var QuestData := GetMQuestData();
  var Quest     := QuestData.FindElement(MQUEST_TITLE+CStr(QID));

  if(!Quest)
    Err.errortext := "Nie znaleziono MiniQuesta ID: "+QID+"!";
    return Err;
  endif  
  
  Quest.SetProp(MQPROP_NPCTEMP,  MQData.NpcTemp);
  Quest.SetProp(MQPROP_TYPE,     MQData.Type);
  Quest.SetProp(MQPROP_TARG,     MQData.Targ);
  Quest.SetProp(MQPROP_LOC,      MQData.Loc);
  Quest.SetProp(MQPROP_TITLE,    MQData.Title);
  Quest.SetProp(MQPROP_ENCRTEXT, MQData.EncrText);
  Quest.SetProp(MQPROP_MAINTEXT, MQData.MainText);
  Quest.SetProp(MQPROP_SUCCTEXT, MQData.SuccText);
  Quest.SetProp(MQPROP_DNPCTEXT, MQData.DNpcText);
  Quest.SetProp(MQPROP_FLAGS,    MQData.Flags);

  SetMQuestElemUpInfo(Quest, who, QID);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteMiniQuest(QID)

  var QuestData := GetMQuestData();

  if(QuestData)
    var Res := QuestData.DeleteElement(MQUEST_TITLE+CStr(QID));
    if(!Res)
      return Res;
    else
      return 1;
    endif
  else
    return QuestData;
  endif

endfunction

//-----------------------------------------------------------------------------

function VerifyMiniQuest(byref MQData)

  var Err := error;

  if(!MQData.Type)
    Err.errortext := "Brak Typu.";
    return Err;
  elseif(!MQData.Title)
    Err.errortext := "Brak Tytulu.";
    return Err;
  elseif(!MQData.Loc)
    Err.errortext := "Brak lokacji.";
    return Err;
  elseif(!MQData.Targ)
    Err.errortext := "Brak celu.";
    return Err;
  endif

  Err := VerifyMQuestText(MQData.EncrText);
  if(!Err)
    Err.errortext := "Zacheta: "+Err.errortext;
    return Err;
  endif

  Err := VerifyMQuestText(MQData.MainText);
  if(!Err)
    Err.errortext := "Glowny: "+Err.errortext;
    return Err;
  endif

  Err := VerifyMQuestText(MQData.SuccText);
  if(!Err)
    Err.errortext := "Powodzenie: "+Err.errortext;
    return Err;
  endif

  Err := VerifyMQuestText(MQData.DNpcText);
  if(!Err)
    Err.errortext := "Porazka: "+Err.errortext;
    return Err;
  endif

  Err := error;

  if(!MQData.EncrText.size())
    Err.errortext := "Brak tekstu zachety.";
    return Err;
  elseif(!MQData.MainText.size())
    Err.errortext := "Brak glownego tekstu.";
    return Err;
  elseif(!MQData.SuccText.size())
    Err.errortext := "Brak tekstu powodzenia.";
    return Err;
  elseif(!MQData.DNpcText.size())
    Err.errortext := "Brak tekstu docelowego npeca.";
    return Err;
  endif

  if(MQData.NpcTemp)

    var cfgElem := GetNpcConfig(MQData.NpcTemp);

    if(!cfgElem)
      Err.errortext := "Bledny Template ["+MQData.NpcTemp+"] nie istnieje.";
      return Err;
    elseif(!cfgElem.CityNpc)
      Err.errortext := "Bledny Template ["+MQData.NpcTemp+"] Npc nie jest miastowy.";
      return Err;
    endif
  endif

  case(MQData.Type)
    MQTYPE_CARRYITEM:   
    MQTYPE_TAKEITEM:    Err := VerifyMQuestNpcTarg(MQData.Targ);
                        if(Err == 1)
                          Err := VerifyMQuestItemTarg(MQData.Targ);
                        endif

    MQTYPE_GOTONPC:     Err := VerifyMQuestNpcTarg(MQData.Targ);
  endcase

  if(!Err)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyMQuestText(byref Text)

  var tIdx := Text.size();
  var Vars := GetMQuestTextVars();

  while(tIdx > 0)
    if(!Text[tIdx])
      Text.erase(tIdx);
    else
      break;
    endif
    tIdx := tIdx - 1;
  endwhile

  while(tIdx > 0)
    if(!Text[tIdx])
      Text[tIdx] := "";
    endif
    tIdx := tIdx - 1;
  endwhile

  var sText := Text;

  foreach Line in sText

    var Keys := Vars.keys();

    foreach Key in Keys
      while(Line[Key])
        Line[Key] := "";
      endwhile
    endforeach
  endforeach
  
  var Pos;
  var i := 1;

  foreach Line in sText
    Pos  := find(Line,"%", 1);
    if(Pos)
      var lLen := find(Line," ",Pos);
      if(!lLen)
        lLen := len(Line);
      else
        lLen := lLen - 1;
      endif
      return RetError("Linia "+i+" zawiera bledna zmienna ["+Line[Pos,lLen+1-Pos]+"]");
    endif
    i    := i + 1;
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyMQuestNpcTarg(byref Targ)

  var Err    := error;

  if(!Targ.Template and !Targ.Script)
    Err.errortext := "Cel: Brak Template lub Skryptu.";
    return Err;
  elseif(Targ.Template)
    Targ.Script := "";
    if(!GetNpcConfig(Targ.Template))
      Err.errortext := "Cel: Bledny Template ["+Targ.Template+"] nie istnieje.";
      return Err;
    elseif(!GetNpcConfig(Targ.Template).CityNpc)
      Err.errortext := "Cel: Bledny Template ["+Targ.Template+"] Npc nie jest miastowy.";
      return Err;
    endif
  elseif(Targ.Script)
    Targ.Template := "";
    var Template  := 0;

    foreach NpcTemp in GetAllNpcTemplates()
      if(GetNpcConfig(NpcTemp).script == Targ.Script)
        Template := NpcTemp;
        break;
      endif
    endforeach
    
    if(!Template)
      Err.errortext := "Cel: Bledny Skrypt ["+Targ.Script+"] nie istnieje zaden Npc z takim skryptem.";
      return Err;
    elseif(!GetNpcConfig(Template).CityNpc)
      Err.errortext := "Cel: Bledny Skrypt ["+Targ.Script+"] Npc posiadajacy ten skrypt: Temp ["+Template+"] nie jest miastowy.";
      return Err;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyMQuestItemTarg(byref Targ)

  var Err     := error;
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(!Targ.ObjType)
    Err.errortext := "Cel: Brak ObjType przedmiotu.";
    return Err;
  elseif(!ItemCfg[Targ.ObjType])
    Err.errortext := "Cel: Bledny ObjType ["+Targ.ObjType+"] nie istnieje.";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
