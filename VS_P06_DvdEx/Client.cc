//**************************************************************************/
//
// NAME :   Client.cc 
// VERSION: omniORB 4 / Mico 2.3
//
// Einfache interaktive Shell zum Zugriff auf DvdEx server.
//
//***************************************************************************/

#include<stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "DvdEx.hh"

#ifdef MICO_EXPORT
#include <coss/CosNaming.h>
#endif

using namespace std;

void printHelp() {
    cout << "DvdEx commands:\n";
    cout << "\t e(nd)       : Shell verlassen\n";
    cout << "\t q(uit)      : Shell verlassen\n";
    cout << "\t save        : Zustand sichern\n";
    cout << "\t h(elp)      : Hilfe\n";
    cout << "\t lp          : Anbieter auflisten\n";
    cout << "\t np          : Neuen Anbieter anlegen\n";
    cout << "\t dp          : Anbieter loeschen\n";
    cout << "\t gp          : Daten zu Anbieter ausgeben\n";
    cout << "\t nm          : Neue Medium / DVD anlegen\n";
    cout << "\t dm          : Medium loeschen\n";
    cout << "\t gm          : Daten zu Medium ausgeben\n";
    cout << "\t ot          : Daten zu allen Medien vom Typ ausgeben\n";
    cout << "\t yt          : Daten zu allen Medien ab Jahr ausgeben\n";
    cout << "\t ma          : Daten zu allen verfügbaren Medien ausgeben\n";
    cout.flush();
}

void save(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
#ifdef MICO_EXPORT
    CORBA::Request_var req = DvdExRepositoryVar->_request("save");
    req->set_return_type(CORBA::_tc_void);
    //*********** BEGIN FIRST ALTERNATIVE
    req->invoke(); // send_oneway()
    //*********** END FIRST ALTERNATIVE

    //*********** BEGIN SECOND ALTERNATIVE
    //req->send_oneway();
    //*********** END SECOND ALTERNATIVE

    //*********** BEGIN THIRD ALTERNATIVE
    //req->send_deferred();
    //cerr << "Sending deferred request: ";
    //while( !req->poll_response() )
    //  cerr << '#';
    //cerr << endl << "Response received." << endl;
    //*********** END THIRD ALTERNATIVE

    if (req->env()->exception()) {
        cout << "save (DII): An exception was thrown!" << endl;
    }
#else
    DvdExRepositoryVar->save();
#endif 

    return;
}

void newProvider(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    DvdEx::DvdExProvider p;

    char buffer [100];

    cout << "Name? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    p.Name = strdup(buffer);

    cout << "Vorname? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    p.FirstName = strdup(buffer);

    cout << "PLZ? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream zcstr(buffer);
    int zip;
    zcstr >> zip;
    p.ZIPCode = (CORBA::Long) zip;
    cout << "PLZ ist: " << p.ZIPCode << endl;

    cout << "Adresse? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    p.Address = strdup(buffer);

    /* add provider to repository */
    DvdExRepositoryVar->addProvider(p);

    cout << DvdExRepositoryVar->currentMaxProviderId() - 1
            << " ist die ID des neuen Anbieters.\n";
}

DvdEx::Genre convertGenre (char* genre_st)
{
    DvdEx::Genre g;
    stringstream type_str (genre_st);
    unsigned short int type;
    type_str >> type;
    switch (type) {
        case 1:
            g = DvdEx::SciFi;
            break;
        case 2:
            g = DvdEx::Comedy;
            break;
        case 3:
            g = DvdEx::Action;
            break;
        case 4:
            g = DvdEx::Horror;
            break;
        case 5:
            g = DvdEx::Docu;
            break;
        default:
            g = DvdEx::g_undefined;
    }
    return g;
}

void newMedia(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    DvdEx::DvdExMedia m;

    char buffer [100];

    cout << "ID des Anbieters? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream pcstr(buffer);
    int id;
    pcstr >> id;
    m.ProviderId = (CORBA::Long) id;

    cout << "Typ [1: SciFi, 2: Comedy, 3: Action, 4: Horror, 5: Docu]? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    m.Type = convertGenre (buffer);
    
    cout << "Titel? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    m.Title = strdup(buffer);

    cout << "Produktionsland? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    m.ProductionCountry = strdup(buffer);

    cout << "Productionsjahr [z.B. 1995]? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream ycstr(buffer);
    unsigned short int year;
    ycstr >> year;
    m.ProductionYear = (CORBA::Short) year;

    cout << "Laenge [min]? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream lstr(buffer);
    unsigned short int length;
    lstr >> length;
    m.Length = (CORBA::Short) length;

    cout << "Status [1: verfuegbar, 2: entliehen]? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream status_str(buffer);
    unsigned short int status;
    status_str >> status;
    switch (status) {
        case 1:
            m.Status = DvdEx::available;
            break;
        case 2:
            m.Status = DvdEx::lent;
            break;
        default:
            m.Status = DvdEx::s_undefined;
    }

    /* add media to repository */
    DvdExRepositoryVar->addMedia(m);

    cout << DvdExRepositoryVar->currentMaxMediaId() - 1
            << " ist die ID des neuen Mediums.\n";
}

void getAndDumpMedia(DvdEx::DvdExRepository_var DvdExRepositoryVar, CORBA::Long id) {
    /* retrieve media */
    try {
        DvdEx::DvdExMedia* m = DvdExRepositoryVar->getMedia(id);
        cout << m->ObjectId << endl;
        cout << m->ProviderId << endl;
        cout << m->Type << endl;
        switch (m->Type) {
            case 1:
                cout << "SciFi";
                break;
            case 2:
                cout << "Comedy";
                break;
            case 3:
                cout << "Action";
                break;
            case 4:
                cout << "Horror";
                break;
            case 5:
                cout << "Docu";
                break;
            default:
                cout << "undefined";
        }
        cout << endl;
        cout << m->Title << endl;
        cout << m->ProductionCountry << endl;
        cout << m->ProductionYear << endl;
        cout << m->Length << endl;
        switch (m->Status) {
            case DvdEx::available:
                cout << "verfuegbar";
                break;
            case DvdEx::lent:
                cout << "entliehen";
                break;
            default:
                cout << "nicht definiert";
        }
        cout << endl;
        cout.flush();
    } catch (...) {
        cerr << "Exception in 'getAndDumpMedia()'\n";
    }
}

void getAndDumpProvider(DvdEx::DvdExRepository_var DvdExRepositoryVar, CORBA::Long id) {
    /* retrieve Provider */
    try {
        DvdEx::DvdExProvider* p = DvdExRepositoryVar->getProvider(id);
        cout << "ID:        \t" << p->ObjectId << endl;
        cout << "Name:      \t" << p->Name << endl;
        cout << "Vorname:   \t" << p->FirstName << endl;
        cout << "PLZ:       \t" << p->ZIPCode << endl;
        cout << "Adresse:   \t" << p->Address << endl;
        cout.flush();
    } catch (...) {
        cerr << "Exception in 'getAndDumpProvider()'\n";
    }
}

void getMedia(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "Id? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream idstr(buffer);
    CORBA::Long id;
    idstr >> id;

    getAndDumpMedia(DvdExRepositoryVar, id);
}

void delMedia(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "Id? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream idstr(buffer);
    CORBA::Long id;
    idstr >> id;

    /* retrieve media */
    try {
        DvdExRepositoryVar->delMedia(id);
    } catch (...) {
        cerr << "Exception in 'delMedia()'\n";
    }
    cout << "Id " << id << " geloescht." << endl;
    cout.flush();
}

void getMediaYoungerThan(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "ab Jahr? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream yearstr(buffer);
    CORBA::Long year;
    yearstr >> year;

    /* retrieve media */
    try {
        DvdEx::DvdExMediaSeq* s = DvdExRepositoryVar->getMediaYoungerThan(year);
        for (unsigned int i = 0; i < s->length(); i++) {
            const DvdEx::DvdExMedia& p = (*s)[i];
            getAndDumpMedia(DvdExRepositoryVar, p.ObjectId);
            cout << "--------\n";
            cout.flush();
        }
    } catch (...) {
        cerr << "Exception in 'getMediaYoungerThan()'\n";
    }
    cout.flush();
}

void getMediaOfType(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "Typ [1: SciFi, 2: Comedy, 3: Action, 4: Horror, 5: Docu]? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    DvdEx::Genre g = convertGenre (buffer);
    
    /* retrieve media */
    try {
        DvdEx::DvdExMediaSeq* s = DvdExRepositoryVar->getMediaOfType(g);
        for (unsigned int i = 0; i < s->length(); i++) {
            const DvdEx::DvdExMedia& p = (*s)[i];
            getAndDumpMedia(DvdExRepositoryVar, p.ObjectId);
            cout << "--------\n";
            cout.flush();
        }
    } catch (...) {
        cerr << "Exception in 'getMediaYoungerThan()'\n";
    }
    cout.flush();
}

void getMediaAvailable (DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    /* retrieve media */
    try {
        DvdEx::DvdExMediaSeq* s = DvdExRepositoryVar->getMediaAvailable();
        for (unsigned int i = 0; i < s->length(); i++) {
            const DvdEx::DvdExMedia& p = (*s)[i];
            getAndDumpMedia(DvdExRepositoryVar, p.ObjectId);
            cout << "--------\n";
            cout.flush();
        }
    } catch (...) {
        cerr << "Exception in 'getMediaAvailable()'\n";
    }
    cout.flush();
}

void listProviders(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    cout << "Anbieter:" << endl;
    try {
        DvdEx::DvdExProviderSeq* s = DvdExRepositoryVar->getProviders();
        for (unsigned int i = 0; i < s->length(); i++) {
            const DvdEx::DvdExProvider& p = (*s)[i];
            getAndDumpProvider(DvdExRepositoryVar, p.ObjectId);
            cout << "--------\n";
            cout.flush();
        }
    } catch (...) {
        cerr << "Exception in 'getProviders()'\n";
    }
}

void getProvider(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "Id? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream idstr(buffer);
    CORBA::Long id;
    idstr >> id;

    /* retrieve provider */
    try {
        DvdEx::DvdExProvider* p = DvdExRepositoryVar->getProvider(id);
        cout << "ID:        \t" << p->ObjectId << endl;
        cout << "Name:      \t" << p->Name << endl;
        cout << "Vorname:   \t" << p->FirstName << endl;
        cout << "PLZ:       \t" << p->ZIPCode << endl;
        cout << "Adresse:   \t" << p->Address << endl;
        cout.flush();
    } catch (...) {
        cerr << "Exception in 'getProvider()'\n";
    }
}

void delProvider(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    cout << "Id? ";
    cout.flush();
    cin.getline(buffer, 100, '\n');
    stringstream idstr(buffer);
    CORBA::Long id;
    idstr >> id;

    /* retrieve provider */
    try {
        DvdExRepositoryVar->delProvider(id);
    } catch (...) {
        cerr << "Exception in 'delProvider()'\n";
    }
    cout << "Id " << id << " geloescht." << endl;
    cout.flush();
}

void dispatchCmd(DvdEx::DvdExRepository_var DvdExRepositoryVar,
        char* s) {
    if (strncmp(s, "end", 1) == 0) {
        exit(0);
    }
    if (strncmp(s, "quit", 1) == 0) {
        exit(0);
    }
    if (strncmp(s, "help", 1) == 0) {
        printHelp();
        return;
    }
    if (strncmp(s, "save", 4) == 0) {
        save(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "lp", 2) == 0) {
        listProviders(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "np", 2) == 0) {
        newProvider(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "gp", 2) == 0) {
        getProvider(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "dp", 2) == 0) {
        return;
    }
    if (strncmp(s, "nm", 2) == 0) {
        newMedia(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "gm", 2) == 0) {
        getMedia(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "dm", 2) == 0) {
        delMedia(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "ot", 2) == 0) {
        getMediaOfType(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "yt", 2) == 0) {
        getMediaYoungerThan(DvdExRepositoryVar);
        return;
    }
    if (strncmp(s, "ma", 2) == 0) {
        getMediaAvailable(DvdExRepositoryVar);
        return;
    }
    cout << "Unbekannter Befehl. 'h' fuer Hilfe." << endl;
}

/*
 * Main loop: read shell commands until 'quit'.
 */
void mainLoop(DvdEx::DvdExRepository_var DvdExRepositoryVar) {
    char buffer [100];

    while (1) {
        cout << "DvdEx> ";
        cout.flush();
        cin.getline(buffer, 100, '\n');
        dispatchCmd(DvdExRepositoryVar, buffer);
    }
}

/*
 * Extract IOR out of file 'fname'
 */
char* getIORFromFile(char* fname) {
    static char ior[500];
    cout << "IOR Datei '" << fname << "' oeffnen." << endl;

    istream* is = 0;
    try {
        is = new ifstream(fname);
    } catch (...) {
        cerr << "Datei '" << fname << "' kann nicht geoeffnet werden." << endl;
        return NULL;
    }

    if (!*is || is->eof()) {
        cerr << "Datei '" << fname << "' kann nicht geloescht werden." << endl;
        return NULL;
    }

    is->getline(ior, 500, '\n');

    cout << "IOR ist '" << ior << "'." << endl;
    return ior;
}

/*
 * Bind to server.
 */
    CORBA::Object_ptr
bindToServer(CORBA::ORB_ptr orb, char* srvname) {
    char* ior = getIORFromFile(srvname);
    if (ior == NULL)
        return CORBA::Object::_nil();
    try {
        CORBA::Object_ptr obj = orb->string_to_object(ior);
        return obj;
    } catch (CORBA::COMM_FAILURE& ex) {
        cerr << "Caught system exception COMM_FAILURE, unable to contact the "
                << "object." << endl;
    }
#ifndef MICO_EXPORT
    catch (omniORB::fatalException& ex) {
        cerr << "Caught omniORB fatalException."
                << "The error message is: " << ex.errmsg() << endl;
    }
#endif
    catch (...) {
        cerr << "Caught a system exception." << endl;
    }
    return CORBA::Object::_nil();
}

int main(int argc, char **argv) {
    DvdEx::DvdExRepository_var DvdExRepositoryVar;
    // pointer to the DvdEx::DvdExRepository object that will be used.
    CORBA::Long currentMaxProviderId, currentMaxMediaId;

    if (argc < 2) {
        cout << "Aufruf: " << argv[0] << " <user@host>" << endl;
        exit(-1);
    }

#ifndef MICO_EXPORT
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "omniORB4");
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "mico-local-orb");
#endif

    char srvname[100];
    srvname[0] = '\0';
    strcat(srvname, "DvdExRepository_");
    strcat(srvname, argv[1]);

    cout << "<dvdexclient> Versuche Verbindung zu '" << srvname << "'..."
            << endl;
    cout.flush();

    try {
        // First bind to the DvdEx::DvdExRepository object.
        // argv[1] has to be the hostname (if any) of the target 
        // DvdEx::DvdExRepository object;
        // the default is the local host:
        CORBA::Object_ptr obj = bindToServer(orb, srvname);
        if (CORBA::Object::_nil() == obj) {
           cerr << "<dvdexclient> Server kann nicht gefunden werden" << endl; 
           exit (1);
        }
        DvdExRepositoryVar = DvdEx::DvdExRepository::_narrow(obj);
    } catch (CORBA::SystemException &sysEx) {
        cerr << "<dvdexclient> Unerwartete System Exception" << endl;
        cerr << &sysEx;
        exit(1);
    } catch (...) {
        // an error occurred while trying to bind to the DvdExRepository object.
        cerr << "<dvdexclient> Bindung an Objekt fehlgeschlagen" << endl;
        cerr << "<dvdexclient> Unerwartete Exception " << endl;
        exit(1);
    }

    cout << "<dvdexclient> Verbindung aufgebaut...\n";
    cout.flush();

    try {
        currentMaxProviderId = DvdExRepositoryVar->currentMaxProviderId();
        currentMaxMediaId = DvdExRepositoryVar->currentMaxMediaId();
    } catch (CORBA::SystemException &sysEx) {
        cerr << "<dvdexclient> Unerwartete System Exception" << endl;
        cerr << &sysEx;
        exit(1);
    } catch (...) {
        // an error occurred while calling set or get:
        cerr << "<dvdexclient> Aufruf von get() oder set() fehlgeschlagen" << endl;
        cerr << "<dvdexclient> Unerwartete Exception " << endl;
        exit(1);
    }
    mainLoop(DvdExRepositoryVar);
    return 0;
}



