//-----------------------------------------------------------------------------

function AI_Log(SubSys, Log)

  syslog("[AI::"+SubSys+"] Npc: ["+Self().serial+"] "+Log);

endfunction

//-----------------------------------------------------------------------------

function AI_LogError(SubSys, Log)

  LogError("[AI::"+SubSys+"] Npc: ["+Self().serial+"/"+Self().npctemplate+"]: "+Log, LOGF_WORLD);

endfunction

//-----------------------------------------------------------------------------
