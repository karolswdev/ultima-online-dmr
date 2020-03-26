//-----------------------------------------------------------------------------

function ShowGodInfo(who, Targ)

  GInit(100, 100);
  GResPic(0,0,3600,200,280);
  GTextLine(20,20 ,999,"Gracz:");
  GTextLine(65,20 ,550,GetRealName(Targ));
  GTextLine(20,50 ,580,"Bog");
  GTextLine(110,50 ,580,"BW");
  GTextLine(160,50 ,580,"PW");

  var i;

  for(i:=1;i<=GODS_MAXID;i:=i+1)

    var GodName := GetGodNameById(i);
    var Pts := GetGodPoints(Targ, i);
    
    GTextLine(20,50+(i*20),550, GodName);
    GTextLine(110,50+(i*20),1209, Pts.bw);
    GTextLine(160,50+(i*20),1309, Pts.pw);

  endfor

  GSend(who);

endfunction

//-----------------------------------------------------------------------------
