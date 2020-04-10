# Mini-questy

![Miniquesty](img/miniquests-welcome.png)

System miniquestów pozwala ekipie shardu na szybkie tworzenie małych questów, które umilą grę.

## Komendy

* **.mqt** - Otwiera główne menu systemu miniquestów
* **.mqn** - Użyty na NPCa, który bierze udział w miniqueście pokazuje informacje dotyczące questa
* **.mqp** - Użyty na graczu, pokazuje informacje dotyczące obecnego questa

### mqt
|Parametr|Opis|
|---|---|
|vars|Wyświetla informacje odnośnie specjalnych znaków do użycia w tekstach.|
|stat|Wyświetla informacje dotyczące statystyk wszystkich miniquestów.|

## Dodawanie questa

Aby dodać questa należy nacisnąć na guzik odpowiadający opcji 'Dodawanie questa'

![pierwszy krok](img/miniquests-add-s1.png)

Przykładowe okno wypełnionego questa

![przyklad](img/miniquests-details.png)

### Opis elementów gumpa

|Element|Opis
|---|---|
|![tytuł](img/miniquests-legend-title.png)|Tytuł questu.|
|![zacheta](img/miniquests-legend-incentive.png)|Tekst zachęty, który ukaże się graczowi przed wybraniem questa. Tekst może korzystać ze zmiennych, aby wygenerować prawidłowe koncówki. Przeczytaj o tym [tu](#zmienne).|
|![powodzenie](img/miniquests-legend-success.png)|Tekst, który wyświetli się graczowi przy skutecznym wykonaniu questa. Tekst może korzystać ze zmiennych, aby wygenerować prawidłowe koncówki. Przeczytaj o tym [tu](#zmienne).|
|![glowny](img/miniquests-legend-main.png)|Główny tekst, który wyświetli się graczowi przed przystąpieniem do questa. Tekst może korzystać ze zmiennych, aby wygenerować prawidłowe koncówki. Przeczytaj o tym [tu](#zmienne).|
|![docelowy npc](img/miniquests-legend-targetnpc.png)|Tekst, który wyświeli się w kontakcie z docelowym NPC. Tekst może korzystać ze zmiennych, aby wygenerować prawidłowe koncówki. Przeczytaj o tym [tu](#zmienne).|
|![NpcTemp](img/miniquests-legend-npctemp.png)|Template NPC'a, który będzie dawał quest.|
|![typ](img/miniquests-legend-type.png)|Typ questu. Sprawdź opis typu questów poniżej.|
|![lokacja](img/miniquests-legend-location.png)|W jakich miejscach będzie można napotkać to zadanie.|
|![wgraj](img/miniquests-legend-load.png)|Załadowuje informacje z instniejącego już questa.|
|![wyczyść](img/miniquests-legend-clear.png)|Wyczyszcza wszystkie wpisane informacje z gumpów.|
|![sprawdź](img/miniquests-legend-verify.png)|Sprawdza, czy wpisane dane mogą zostać zapisane.|
|![zapisz](img/miniquests-legend-save.png)|Zapisuje wpisane dane jako szablon (na styl CTRL-C w Windowsie).
|![dodaj](img/miniquests-legend-add.png)|Dodaje wpisane dane jako miniquest w systemie.

### Wartości do wpisania przy tworzeniu questa

|Nazwa|Opis|
|---|---|
|![płeć](img/miniquests-legend-targetgender.png)|**0** - kobieta, **1** - mężczyzna - płeć NPC, który zostanie stworzony jako druga odnoga questa.|
|![imie](img/miniquests-legend-targetname.png)|Nazwa przedmiotu, który zostanie stworzony do przekazania albo odebrania.|
|![kolor](img/miniquests-legend-targetcolor.png)|Kolor przedmiotu, który zostanie zdany do przekazania albo odebrania.|
|![objtype](img/miniquests-legend-targetobjtype.png)|ObjType przedmiotu, który zostanie stworzony do przekazania albo odebrania. Sprawdź w itemdesc.cfg każdej paczki.|
|![template](img/miniquests-legend-targettemplate.png)|Template NPC, który zostanie stworzony jako druga odnoga questa.|
|![skrypt](img/miniquests-legend-targetscript.png)|Skrypt NPC, który zostanie stworzony jako druga odnoga questa. Można zostawić puste.|

### Typy questów

![typy](img/miniquests-typers.png)

### Zmienne

Zmienne wykorzystujemy po to, aby wygenerować tekst, który będzie odmienialny. 

|Tekst wpisany w gump|Wynik w grze|
|---|---|
|**%pn**, proszę, jeśli mogl **%gp1**, zabierz ten **%in**. W **%loc** czekać na Ciebie będzie **%nn**. Je **%gd19%** rodzina coś o tym przedmiocie wie.|**Raziel**, proszę, jeśli mogl**bys**, zabierz ten **magiczny miecz**. W **Sakkath** czekać na Ciebie będzie **Sabrina - Mieszkanka Sakkath**. Je**j** rodzina coś o tym przedmiocie wie.


![zmienne](img/miniquests-vars.png)



