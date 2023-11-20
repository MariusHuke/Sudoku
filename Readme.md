# Sudoku Löser basierend auf einem genetischen Algorithmus
Ziel dieses Projektes ist es Sudokus beliebiger Form mit Hilfe von genetischen Algorithmen zu lösen.
Das bedeutet die Felder eines vorgegebenden Sudokus so auszufüllen, dass in keiner Reihe, Spalte und keinem Block Zahlen doppelt vorkommen. 
Im Datensatz der hier zu lösenden Sudokus befinden sich hauptsächlich 9x9 Sudokus, aber auch wenige [25x25](data/25x25) Sudokus.

## Erklärung der Bestandteile
<details margain_left="20px;">
  <summary>1. Sudoku-Representation</summary>

In [Sudoku.hpp](Sudoku.hpp) wird die Klasse Sudoku definiert. Diese enthält neben Daten zur Gesamtgröße auch öfter verwendete Teilgrößen wie die Größe eines Blocks oder die Anzahl der Blöcke. Die Zahlen des Sudokus sind für einfacheren Zugriff in 2 verschiedenen Datenstrukturen gespeichert, die beide Zugriff auf 
die gleichen shared_ptr haben. Dabei ist die row_representation ein fortgehender 1D-vector, der die Reihen hinterinander speichert. Die grid_representation
ist ein 2D-vector, der in der ersten Dimension die Blöcke und in der zweiten Dimension die Reihen innerhalb der Blöcke speichert. Neben dem default Konstruktor werden auch ein Copy-Konstruktor, welcher eine tiefe Kopie erstellt und ein Size-Konstruktor, welcher ein mit Nullen gefülltes Sudoku der gegebenen Größe erstellt, implementiert. Bei jedem Konstruktur wird automatisch auch die grid_representation erstellt.

Weiterhin ist die Klasse generisch implementiert, sodass sowohl float, als auch int Datentypen verwendet werden können. Die float Variante ist für das Berechnen von individuellen Fitnessbewertung wichtig.
</details>

<details>
  <summary>2. Generation und Operatoren</summary>

In [Generation.hpp](Generation.hpp) werden alle anderen wichtigen Funktionalitäten definiert. In dieser werden die ursprüngliche Populationsgröße und die momentan vorhandene gespeichert. Die Klasse hat neben dem original Sudoku, also dem ursprünglich gegebendem Input mit leeren Feldern, auch eine Menge von Lösungskandidaten (population). Für die Fitness werden außerdem die Fitnesswerte der gesamten Sudokus (fitness_sums), aber auch die der individuellen Felder (fitness_sudokus) gespeichert. Neben dem default Konstruktor existiert ein Konstruktor, der aus einem gegebenden Sudoku und einer Populationsgröße eine Population erstellt und diese je nach gewählter Methode initialisiert.

<details>
  <summary>2.1. Initialisierung</summary>

Für die Initialisierung können 2 verschiedene Methoden verwendet werden. Beide iterieren über jedes Element der Population.
1. Zufällige Initialisierung: Jedem Feld wird eine zufällige Zahl zugeordnet (1-9). Dabei wird allerdings darauf geachtet, dass in keinem Block eine Zahl doppelt vorkommt.
2. Schlaue Initialisierung: ist eine Erweiterung, die bei der Initialisierung nicht nur auf keine Kollisionen im Block achtet, sondern dazu auch zufällig
horizontal oder vertikal wählt und auch dort versucht die Kollisionen mit Reihen bzw. Spalten zu vermeiden. Das ist natürlich nicht immer mit wenig Rechenaufwand möglich.
Daher werden die wenigen Felder, für die sich kein Kandidaten in linearer Laufzeit findet, der nicht mit Reihe bzw. Spalte kollidiert, zufällig so belegt, dass wenigstens die Kollision im Block verhindert wird.
</details>

<details>
  <summary>2.2 Crossover</summary>

Für die Crossover Operation wird eine Operation definiert, die einen Wert n bekommt. Getauscht werden die n-1 Reihen von Blöcken jedes Eltern mit n-1 Reihen des anderen. Wird also auf ein 9x9-Sudoku Crossover von 3 angewendet, werden auch 3 Eltern benötigt. 
Ein sinnvoller Spezialfall für 9x9-Sudokus ist dabei Crossover(2), bei dem nur die mittlere Reihe des einen Eltern durch die des anderen getauscht wird.
Auch wenn theoretisch mehr Möglichkeiten bestehen bietet die Klasse nur das 2-Punkt-Crossover (Crossover(2)) und das diagonale Crossover(Crossover(3)) nach außen an.

</details>

<details>
  <summary>2.3 Fitness</summary>
Für die Fitness werden die Anzahl der Kollisionen in Reihen und Spalten berechnet. Dabei werden für jedes Feld die Summe der Kollisionen berechnet. Für das gesamte Sudoku werden alle Teilkollisionswerte aufsummiert.

Zu beachten ist das die durch diese Methoden berechnete Fitness möglichst niedrig sein sollte. Eine Fitness von 0 entspricht dabei der perfekten Lösung.

<sub>[1]: https://www.researchgate.net/profile/Kim-Viljanen/publication/228840763_New_Developments_in_Artificial_Intelligence_and_the_Semantic_Web/links/09e4150a2d2cbb80ff000000/New-Developments-in-Artificial-Intelligence-and-the-Semantic-Web.pdf#page=95</sub>
</details>

<details>
  <summary>2.3 Selektion</summary>

Für die Selektion existieren 2 Möglichkeiten.
1. Ordnung der Individuen nach ihrer Fitness und auswählen dann der besten n (abhängig von der gewählten keeping_percentage) Individuen aus.
2. Stochastic-Universal-Sampling: Normalisiert alle Fitnesswerte durch MinMax-Normalisierung und zieht diese vom Maximum ab, sodass der niedrigste Wert der schlechtesten Fitness entspricht. Bildet dann ein Roulette-Rad, wobei der Wert jedes Elementes dem Anteil auf dem Rad entspricht. Dann wird ein zufälliger Startpunkt zwischen 0 und 1 gewählt und von diesem aus werden in Schritten, deren Größe von der keeping_percentage abhängt, die Individuen solange suagewählt, bis man wieder am Startpunkt ist und somit die gewünscht Populationsgrößer erreich hat.
</details>

<details>
  <summary>2.4 Mutation</summary>

Die Mutation nutzt den fitness_sudoku vector. Dabei wird für jedes Feld geschaut ob dieses einen Fitnesswert > 1 hat. Ist dies der Fall wird die Position in eine Liste swaps eingefügt. Weiterhin werden auch Felder mit Fitnesswert 0 mit einer Wahrscheinlichkeit von 1/9 eingefügt. Diese Liste wird zufällig gemischt und Felder werden in Paaren vertauscht. Sollte ein Feld übrig bleiben wird es einfach wieder an seinen Platz zurückgeschrieben.
</details>

<details>
  <summary>2.5 Abbruchkriterium</summary>

Die Funktion bricht ab, wenn in 25 aufeinanderfolgenden Generationen keine Verbesserung des Fitnesswertes erreicht wurde.
</details>

<details>
  <summary>2.6 Sonstiges</summary>

Weiterhin stehen Funktionen zum Ausgeben der Population und Fitnesswerte bereit. Eine weitere bereitstehende Methode punish_same kann genutzt werden um die Fitness von Individuen zu erhöhen (also zu verschlechtern), wenn diese in vielen Feldern mit anderen übereinstimmen.
</details>
</details>

<details>
  <summary>3. Einlesen von Daten</summary>

In main.cpp werden 2 Funktionen zum Einlesen von Daten, wie Sie in data vorliegen, bereitgestellt. Dabei werden mit # beginnende Zeilen ignoriert und alle anderen Reihenweise eingelesen, wobei 0 für ein leeres Feld und [1-9] für den jeweiligen Wert steht. Das einlesen von 25x25 unterscheidet sich leicht, da dort auch zweistellige Zahlen vorkommen.
</details>

<details>
  <summary>4. Auswertung der Ergebnisse</summary>

Um das Programm zu testen wird die Datei [testdata](data/testdata) verwendet. In dieser sind zu jeder der Schwierigkeiten (easy,medium,hard,expert) 10 Sudokus hinterlegt. 
Die Initialisierung schien außer auf den Startwert der 
Als beste Methode konnte die Kombination aus diagonalem Crossover und simpler Selektion der besten 20% empirisch bestimmt werden.
Die Auswirkungen der unterschiedlichen Populationsgrößen ist dabei wie folgt:

#### 1. Populationgröße 100

|Schwierigkeit|gelöst (%)|durschnittliche Lösungszeit(ms)|durschnittliche Generationen|durschnittliche Abbruchszeit(ms)|
|:-:|:-:|:-:|:-:|:-:|
|Leicht|100|1102|7.2|-|
|Mittel|60|2721|18.8|6381|
|Schwer|0|0|9412|
|Experte|0|0|9583|

#### 2. Populationgröße 500
|Schwierigkeit|gelöst (%)|durschnittliche Zeit|durschnittliche Generationen|durschnittliche Zeit(Abbruch)|
|:-:|:-:|:-:|:-:|:-:|
|Leicht|100|4662|5.6|-|
|Mittel|90|13222|11|35048|
|Schwer|50|25979|32,4|53385|
|Experte|0|0|0|51216|

#### 3. Populationgröße 1000
|Schwierigkeit|gelöst (%)|durschnittliche Zeit|durschnittliche Generationen|durschnittliche Zeit(Abbruch)
|:-:|:-:|:-:|:-:|:-:|
|Leicht|100|9208|5.3|-|
|Mittel|100|24384|14.8|-|
|Schwer|60|43039|25.5|87460|
|Experte|20|91698|54,5|123294|

#### 4. Populationgröße 5000

(Genaugenommen sind Populationsgrößen so angepasst, dass sie für das Crossover durch 3 teilbar sind, also 102,501,1002)
</details>

<details>
  <summary>5. GUI</summary>
  
  Die Gui ist eine angepasste Version der [hier](https://github.com/robovirmani/sudoku-solver) vorhandenen. Sie funktioniert nur für 9x9 Sudokus.
</details>
