# DVD_Tauschboerse

## Aufgabe 1

Das binden an den Server wird über die Methode `bindToServer()` realisiert. Dort
wird zunächst die IOR-Datei ausgelesen und dann mithilfe von
`orb->string_to_object` in ein Proxy-Objekt konvertiert.
