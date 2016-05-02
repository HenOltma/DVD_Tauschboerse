# Makefile 'Verteiltes Tauschboerse'. 
# Veranstaltung 'Verteile Systeme', hje, Apr 2016
# 
# Erzeugen des Programms mit 'make all'.
# Starten der Programme (Client bzw. Server) mit 'make runclient' bzw.
# 'make runserver'.
# Aufräumen mit 'make clean' und 'make veryclean'.

all: dvdexclient dvdexserver

runclient: dvdexclient
	./dvdexclient $(shell whoami)@$(shell hostname)

runserver: dvdexserver
	@echo Server './dvdexserver' wird im Hintergrund gestartet.
	./dvdexserver &

DvdEx.hh DvdExSK.cc: DvdEx.idl
	omniidl -bcxx DvdEx.idl

dvdexserver: DvdExSK.o Srv_Main.o DvdExRepository_i.o
	g++ -I. -o dvdexserver DvdExSK.o Srv_Main.o DvdExRepository_i.o -lomnithread -lomniORB4 

dvdexclient: DvdExSK.o Client.o
	g++ -I. -o dvdexclient DvdExSK.o Client.o -lomnithread -lomniORB4

Srv_Main.o: Srv_Main.cc DvdEx.hh
	g++ -g -c -I. Srv_Main.cc

DvdExRepository_i.o: DvdExRepository_i.cc DvdExRepository_i.hh DvdEx.hh
	g++ -g -c -I. DvdExRepository_i.cc

DvdExSK.o: DvdExSK.cc DvdEx.hh
	g++ -g -c -I. DvdExSK.cc

Client.o: Client.cc DvdEx.hh
	g++ -g -c -I. Client.cc

clean:
	rm -f dvdexserver dvdexclient *.o

veryclean: clean
	rm -f DvdExRepository_*@* DvdExSK.cc DvdEx.hh 
	rm -f *.*~ *~
