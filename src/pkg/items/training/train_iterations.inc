//--------------------------------------------------------------------------------

function DefineTrainIterations(who, SkId)

  EraseObjProperty(who, "#StopTrain_"+SkId);

  var Data := struct;

  Data.+curIter := 0;
  Data.+maxIters := 10;
  Data.+SkillId  := SkId;

  return Data;

endfunction

//--------------------------------------------------------------------------------

function TrainIteration(who, byref IterData)

  if(IterData.curIter == IterData.maxIters or IsTrainStop(who, IterData.SkillId))
    FinishTrainIteration(who, IterData);
    return 0;
  endif

  IterData.curIter += 1;
  return 1;

endfunction

//--------------------------------------------------------------------------------

function FinishTrainIteration(who, byref IterData)

  CloseTrainStopGump(who, IterData.SkillId);

endfunction

//--------------------------------------------------------------------------------

function GetTrainIterDesc(byref IterData)

  return "("+IterData.curIter+" z "+IterData.maxIters+")";

endfunction

//--------------------------------------------------------------------------------

function TrainIterationGump(who, byref IterData, Type := 0)  

  if(IterData.maxIters > 1)
    SendTrainStopGump(who, IterData.SkillId, Type);
  endif

endfunction

//-----------------------------------------------------------------------------

function IsTrainStop(who, SkillID)

  if(GetObjProperty(who, "#StopTrain_"+SkillID))
    EraseObjProperty(who, "#StopTrain_"+SkillID);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveTrainStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#TrainStopGump_"+SkillID);

  if(ExPid(CurGump))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendTrainStopGump(who, SkillID, Type := 0)

  start_script(":Training:Train_stop", array(who, SkillID, Type));

endfunction

//-----------------------------------------------------------------------------

function CloseTrainStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#TrainStopGump_"+SkillID);

  if(ExPid(CurGump))
    return CloseGump(who, CurGump);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------
