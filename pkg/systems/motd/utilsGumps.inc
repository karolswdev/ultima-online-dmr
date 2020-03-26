use uo;

function SendStringAsHtmlWindow(who, text, title := "Wiadomosci...")
  var data := array(
  "page 0",
  "nodispose",
  "resizepic 240 140 3600 300 400",
  "gumppic 190 120 10440",
  "gumppic 508 120 10410",
  "gumppic 508 280 10411",
  "gumppic 508 440 10412",
  "gumppic 300 95 10452",
  "tilepic 252 155 5357",
  "tilepic 262 151 4032",
  "text 340 153 55 0",
  "htmlgump 252 179 276 348 1 1 1"
  );
  var lines := array(
  title,
  text
  );
  return SendDialogGump( who, data, lines );
endfunction
