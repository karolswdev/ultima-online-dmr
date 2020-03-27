//-----------------------------------------------------------------------------

include ":ai_speech:const";

//-----------------------------------------------------------------------------

function NpcAskYesNo(Npc, Source, Question)

  // przejsciowka ze zdefiniowanymi odpowiedziami

  var Answers := dictionary;

  Answers["tak"] := 1;
  Answers["nie"] := 2;

  // zwracamy true, jesli odpowiedz byla zgodna, czyli na tak

  var Res := NpcAskQuestion(Npc, Source, Question, Answers);

  if(Res == error)
    return Res;
  endif

  return (Res == 1);

endfunction

//-----------------------------------------------------------------------------

function NpcAskQuestion(Npc, Source, Question, Answers)

  // weryfikacja poprawnosci danych

  if(!Npc.isa(POLCLASS_NPC) or !Source.isa(POLCLASS_MOBILE))
    return error{"errortext" := "Blednie podany obiekt. Brak NPC."};
  endif

  if(!Source.isa(POLCLASS_MOBILE))
    return error{"errortext" := "Blednie podany obiekt. Brak zrodla."};
  endif

  if(lower(TypeOf(Answers)) != "dictionary")
    return error{"errortext" := "Bledne odpowiedzi."};
  endif

  if(!Question)
    return error{"errortext" := "Brak pytania!"};
  endif

  var Clock := ReadGameClock();
  var Event := struct;

  // budujemy odpowiedni event i wysylamy go npecowi
  // on jak dostanie odpowiedz z dicta Answers, zwroci nam result 
  // lub EVRES_FAILED, jesli zrodlo nie odpowiedzialo w ogole w okreslonym czasie

  Event.+type     := EVID_QUESTION;
  Event.+source   := Source;
  Event.+question := Question;
  Event.+answers  := Answers;
  Event.+script   := GetProcess(GetPid());

  if(!SendEvent(Npc, Event))
    LogError("Nie udalo sie wyslac zdarzenia NpcQuestion!");
  endif

  // czekamy na event zwrotny od npeca

  while(Clock + AI_SPEECH_QUESTION_WAIT_TIME*2 > ReadGameClock())

    Event := wait_for_event(5);

    if(Event.type == EVID_QUESTION)
      return Event.result;
    endif

  endwhile

  return error{"errortext" := "Czas oczekiwania na odpowiedz minal."};

endfunction

//-----------------------------------------------------------------------------
