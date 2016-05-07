//*************************************************************************/
//
// NAME :   Srv_Main.cc
// VERSION: omniORB 4 / Mico 2.3
//
// Server der Anwendung zum Tausch von DVDs.
//
//***************************************************************************/

#include<stdlib.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include "DvdEx.hh"

#include "DvdExRepository_i.hh"

using namespace std;

/* 
 * for retrieval of IOR by connected clients
 */
void
writeIORToFile(CORBA::ORB_ptr orb,
        char* srvname, DvdExRepository_i* myDvdExRepository) {
    ofstream out(srvname);
    if (out) {
        CORBA::String_var ior = orb->object_to_string(myDvdExRepository->_this());
        out << (char*) ior << endl;
        cout << "<writeIORToFile> Datei mit IOR ist '" << srvname << "'"
                << endl;
    }
    else {
        cerr << "<writeIORToFile> IOR kann nicht geschrieben werden." << endl;
    }
}

int main(int argc, char **argv) {
    // create a DvdExRepository object - using the implementation class DvdExRepository_i

    char srvname[40];
    srvname[0] = '\0';
	char* usrname;
	
    strcat(srvname, "DvdExRepository_");
	if ((usrname = getenv("LOGNAME")) == 0) {
		cerr << "LOGNAME kann nicht gelesen werden." << endl;
		return 1;
	}
			
    strcat(srvname, usrname);
    strcat(srvname, "@");
#ifndef _WIN32
    char hostname [64];
    gethostname(hostname, sizeof (hostname));
    strcat(srvname, hostname);
#else
    strcat(srvname, getenv("COMPUTERNAME"));
#endif  
    cout << "<dvdexserver> Server '" << srvname << "' wird gestartet."
            << endl;

#ifndef MICO_EXPORT
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "omniORB4");
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "mico-local-orb");
#endif

    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

    DvdExRepository_i* myDvdExRepository = new DvdExRepository_i(100, 10);
    try {
        // tell OmniORB that we have completed the server's initialisation:
        cout << "<dvdexserver> Server '" << srvname << "' bereit fuer Anfragen."
                << endl;

        PortableServer::ObjectId_var myechoid = poa->activate_object(myDvdExRepository);

        writeIORToFile(orb, srvname, myDvdExRepository);

        myDvdExRepository->_remove_ref();

        PortableServer::POAManager_var pman = poa->the_POAManager();
        pman->activate();

        orb->run();
        orb->destroy();
    } catch (CORBA::SystemException &sysEx) {
        cerr << "<dvdexserver> Unerwartete System Exception" << endl;
        cerr << &sysEx;
        exit(1);
    } catch (...) {
        // an error occured calling impl_is_ready() - output the error.
        cout << "<dvdexserver> Unerwartete Exception." << endl;
        exit(1);
    }

    cout << "<dvdexserver> Abbruch Server." << endl;

    return 0;
}



