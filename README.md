# DVD_Tauschboerse

## Aufgabe 1

Das binden an den Server wird über die Methode `bindToServer()` realisiert. Dort
wird zunächst die IOR-Datei ausgelesen und dann mithilfe von
`orb->string_to_object` in ein Proxy-Objekt konvertiert.

## Aufgabe 2

Wir haben zunächst die Methode `getMediaAvailable()` in der DvdExRepository_i.hh
und der DvdEx.idl Datei hinzugefügt, dann die entsprechenden Schnittstellen beim
Client vorbereitet (bei den prompts).
Den Code im Client konnte man, in leicht abgeleiteter Form, von der Methode `getMediaofType()` übernehmen, den Code vom Server ebenfalls.

