//----------------------------------------------------------------------

include ":waypoints:include/waypoints";

//----------------------------------------------------------------------

function SendTravelGump(who, Locations)

  GInit(20,50);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();
  GResPic(0,0,3600,530,380);

  GPage();
  GTilePic(28,148,3948);
  GButton(60,230,999,211, 9);

  GTextLine(115,30,999, "Lokacja");
  GTextLine(275,30,999, "Koszt");
  GTextLine(345,30,999, "Powrót");
  GTextLine(415,30,999, "Odleg³oœæ");

  GAddPageData(495,18,2650,2651,495,345,2648,2647);

  var i := 1;

  foreach Data in Locations
    GCheckNextPage(10);
    GButton(90,(50+(GNPCount()*30)),210,211,i);
    GTextLine(115,(50+(GNPCount()*30)), __IfElse((Data.Level == "Hard"), 1209, 600), Data.Desc);
    GTextLine(275,(50+(GNPCount()*30)),1117, Data.Price+"gp");
    GTextLine(345,(50+(GNPCount()*30)),1119, CInt(Data.Price*WP_RETURN_COST_MULT)+"gp");
    GTextLine(415,(50+(GNPCount()*30)),1424, CDbl(Data.PointDist/10.0)+"km" );

    i:=i+1;
  endforeach

  return GSend( who ).butid;

endfunction

//----------------------------------------------------------------------
