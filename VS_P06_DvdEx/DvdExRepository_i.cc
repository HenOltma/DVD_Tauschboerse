//*************************************************************************/
//
// NAME :   DvdExRepository_i.cc 
// VERSION: omniORB 4 / Mico 2.3
//
// Class implementing interface as POA object.
//
//***************************************************************************/

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "DvdEx.hh"

#include "DvdExRepository_i.hh"

using namespace std;

static ofstream* GLOBMediaOutFile = 0;
static ofstream* GLOBProviderOutFile = 0;
static ifstream* GLOBMediaInFile = 0;
static ifstream* GLOBProviderInFile = 0;

static ostream& initMediaOutFile() {
    if (GLOBMediaOutFile == 0) {
        char filename[50];
        filename[0] = '\0';
        //strcat(filename, getenv("HOME"));
        strcat(filename, ".");
        strcat(filename, "/DvdExMedia.dmp");

        cout << "Ausgabe auf '" << filename << "'." << endl;

        GLOBMediaOutFile = new ofstream(filename);
    }
    return *GLOBMediaOutFile;
}

static ostream& initProviderOutFile() {
    if (GLOBProviderOutFile == 0) {
        char filename[50];
        filename[0] = '\0';
        //strcat(filename, getenv("HOME"));
        strcat(filename, ".");
        strcat(filename, "/DvdExProvider.dmp");

        cout << "Ausgabe auf '" << filename << "'." << endl;

        GLOBProviderOutFile = new ofstream(filename);
    }
    return *GLOBProviderOutFile;
}

static istream& initMediaInFile() {
    if (GLOBMediaInFile == 0) {
        char filename[50];
        filename[0] = '\0';
        //strcat(filename, getenv("HOME"));
        strcat(filename, ".");
        strcat(filename, "/DvdExMedia.dmp");

        cout << "Eingabe von '" << filename << "'." << endl;

        GLOBMediaInFile = new ifstream(filename);
    }
    return *GLOBMediaInFile;
}

static istream& initProviderInFile() {
    if (GLOBProviderInFile == 0) {
        char filename[50];
        filename[0] = '\0';
        //strcat(filename, getenv("HOME"));
        strcat(filename, ".");
        strcat(filename, "/DvdExProvider.dmp");

        cout << "Eingabe von '" << filename << "'." << endl;

        GLOBProviderInFile = new ifstream(filename);
    }
    return *GLOBProviderInFile;
}

static void closeProviderInFile() {
    if (GLOBProviderInFile != 0) {
        GLOBProviderInFile->close();
        GLOBProviderInFile = 0;
    }
}

static void closeProviderOutFile() {
    if (GLOBProviderOutFile != 0) {
        GLOBProviderOutFile->close();
        GLOBProviderOutFile = 0;
    }
}

static void closeMediaInFile() {
    if (GLOBMediaInFile != 0) {
        GLOBMediaInFile->close();
        GLOBMediaInFile = 0;
    }
}

static void closeMediaOutFile() {
    if (GLOBMediaOutFile != 0) {
        GLOBMediaOutFile->close();
        GLOBMediaOutFile = 0;
    }
}

static void backup(DvdEx::DvdExMedia** media, CORBA::Short maxMediaNo,
        DvdEx::DvdExProvider** provider, CORBA::Short maxProviderNo) {
    int i;

    ostream& om = initMediaOutFile();
    if (!om) {
        cerr << "Repository kann nicht gesichert werden!\n";
        return;
    }

    for (i = 0; i < maxMediaNo; i++) {
        if (media[i] != 0) {
            om << media[i]->ObjectId << endl;
            om << media[i]->ProviderId << endl;
            om << media[i]->Type << endl;
            om << media[i]->Title << endl;
            om << media[i]->ProductionCountry << endl;
            om << media[i]->ProductionYear << endl;
            om << media[i]->Length << endl;
            om << media[i]->Status << endl;
        }
    }
    closeMediaOutFile();

    ostream& op = initProviderOutFile();
    if (!op) {
        cerr << "Repository kann nicht gesichert werden!\n";
        return;
    }

    for (i = 0; i < maxProviderNo; i++) {
        if (provider[i] != 0) {
            op << provider[i]->ObjectId << endl;
            op << provider[i]->Name << endl;
            op << provider[i]->FirstName << endl;
            op << provider[i]->ZIPCode << endl;
            op << provider[i]->Address << endl;
            op.flush();
        }
    }
    closeProviderOutFile();
}

static void restore(DvdEx::DvdExMedia** media, CORBA::Short maxMediaNo,
        CORBA::Long& m_currentMaxMediaId,
        DvdEx::DvdExProvider** provider, CORBA::Short maxProviderNo,
        CORBA::Long& m_currentMaxProviderId) {
    char buffer [100];

    istream& im = initMediaInFile();
    while (im && !im.eof()) {
        DvdEx::DvdExMedia* m = new DvdEx::DvdExMedia();

        if (!im.getline(buffer, 100))
            break;
        stringstream ostr(buffer);
        ostr >> m->ObjectId;

        if (!im.getline(buffer, 100))
            break;
        stringstream pstr(buffer);
        pstr >> m->ProviderId;

        if (!im.getline(buffer, 100))
            break;

        stringstream type_str(buffer);
        unsigned short int type;
        type_str >> type;
        switch (type) {
            case 1:
                m->Type = DvdEx::SciFi;
                break;
            case 2:
                m->Type = DvdEx::Comedy;
                break;
            case 3:
                m->Type = DvdEx::Action;
                break;
            case 4:
                m->Type = DvdEx::Horror;
                break;
            case 5:
                m->Type = DvdEx::Docu;
                break;
            default:
                m->Type = DvdEx::g_undefined;
        }
        
        if (!im.getline(buffer, 100))
            break;
        m->Title = CORBA::string_dup(buffer);

        if (!im.getline(buffer, 100))
            break;
        m->ProductionCountry = CORBA::string_dup(buffer);

        if (!im.getline(buffer, 100))
            break;
        stringstream pystr(buffer);
        pystr >> m->ProductionYear;

        if (!im.getline(buffer, 100))
            break;
        stringstream lstr(buffer);
        lstr >> m->Length;

        if (!im.getline(buffer, 100))
            break;
        stringstream status_str(buffer);
        unsigned short int status;
        status_str >> status;
        switch (status) {
            case 1:
                m->Status = DvdEx::available;
                break;
            case 2:
                m->Status = DvdEx::lent;
                break;
            default:
                m->Status = DvdEx::s_undefined;
        }

        if (m->ObjectId < maxMediaNo) {
            media[m->ObjectId] = m;
            if (m->ObjectId >= m_currentMaxMediaId)
                m_currentMaxMediaId = m->ObjectId + 1;
        } else {
            cerr << "<restore> ObjectId for media out of bound!" << endl;
        }
    }
    closeMediaInFile();

    istream& ip = initProviderInFile();
    while (ip && !ip.eof()) {
        DvdEx::DvdExProvider* p = new DvdEx::DvdExProvider();

        if (!ip.getline(buffer, 100))
            break;
        stringstream ostr(buffer);
        ostr >> p->ObjectId;

        if (!ip.getline(buffer, 100))
            break;
        p->Name = CORBA::string_dup(buffer);

        if (!ip.getline(buffer, 100))
            break;
        p->FirstName = CORBA::string_dup(buffer);

        if (!ip.getline(buffer, 100))
            break;
        stringstream zcstr(buffer);
        zcstr >> p->ZIPCode;

        if (!ip.getline(buffer, 100))
            break;
        p->Address = CORBA::string_dup(buffer);

        if (p->ObjectId < maxProviderNo) {
            provider[p->ObjectId] = p;
            if (p->ObjectId >= m_currentMaxProviderId)
                m_currentMaxProviderId = p->ObjectId + 1;
        } else {
            cerr << "<restore> ObjectId for provider out of bound!" << endl;
        }
    }
    closeProviderInFile();
}

// ctor

DvdExRepository_i::DvdExRepository_i(CORBA::Short maxMediaNo,
        CORBA::Short maxProviderNo) {
    cout << "<DvdExRepository_i> Neues Repository wird erzeugt: "
            << maxMediaNo << " media, "
            << maxProviderNo << " provider." << endl;
    cout.flush();

    defaultEmptyMedia = new DvdEx::DvdExMedia();
    defaultEmptyMedia->ObjectId = -1;
    defaultEmptyMedia->ProviderId = -1;
    defaultEmptyMedia->Type = DvdEx::g_undefined;
    defaultEmptyMedia->Title = CORBA::string_dup("<undefined>");
    defaultEmptyMedia->ProductionCountry = CORBA::string_dup("<undefined>");
    defaultEmptyMedia->ProductionYear = -1;
    defaultEmptyMedia->Length = -1;
    defaultEmptyMedia->Status = DvdEx::s_undefined;

    defaultEmptyProvider = new DvdEx::DvdExProvider();
    defaultEmptyProvider->ObjectId = -1;
    defaultEmptyProvider->Name = CORBA::string_dup("<undefined>");
    defaultEmptyProvider->FirstName = CORBA::string_dup("<undefined>");
    defaultEmptyProvider->ZIPCode = -1;
    defaultEmptyProvider->Address = CORBA::string_dup("<undefined>");

    m_currentMaxProviderId = 0;
    m_currentMaxMediaId = 0;

    m_MaxMediaNo = maxMediaNo;
    media = new DvdEx::DvdExMedia* [m_MaxMediaNo];

    m_MaxProviderNo = maxProviderNo;
    provider = new DvdEx::DvdExProvider* [m_MaxProviderNo];

    /* initialize all with 0 ptr */
    for (int i = 0; i < maxMediaNo; i++)
        media[i] = 0;

    for (int j = 0; j < maxProviderNo; j++)
        provider[j] = 0;

    restore(media, m_MaxMediaNo, m_currentMaxMediaId,
            provider, m_MaxProviderNo, m_currentMaxProviderId);
}

// dtor

DvdExRepository_i::~DvdExRepository_i() {
    cout << "<DvdExRepository_i> Deleting repository...\n";
    cout.flush();

    backup(media, m_MaxMediaNo, provider, m_MaxProviderNo);

    /* delete all non-null entries */
    for (int i = 0; i < m_MaxMediaNo; i++)
        if (media[i] != 0)
            delete (media[i]);

    for (int j = 0; j < m_MaxProviderNo; j++)
        if (provider[j] != 0)
            delete (provider[j]);
}

CORBA::Long
DvdExRepository_i::currentMaxProviderId() {
    return m_currentMaxProviderId;
}

CORBA::Long
DvdExRepository_i::currentMaxMediaId() {
    return m_currentMaxMediaId;
}

DvdEx::DvdExMediaSeq *
DvdExRepository_i::getMediaYoungerThan(CORBA::Short y) {
    int cnt = 0;
    int i = 0;

    /* determine sequence length */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            cout << i << ": " << media[i]->ProductionYear << endl;
            if (media[i]->ProductionYear >= y)
                cnt++;
        }
    }

    cout << "<getMediaYoungerThan> " << cnt << " Medien gefunden.\n";
    cout.flush();

    DvdEx::DvdExMediaSeq* seq = new DvdEx::DvdExMediaSeq(cnt);
    seq->length(cnt);

    cout << "<getMediaYoungerThan> " << seq->length() << " in der Sequenz.\n";
    cout.flush();

    cnt = 0;

    /* copy result */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            if (media[i]->ProductionYear >= y) {
                (*seq)[cnt] = *(new DvdEx::DvdExMedia(*(media[i])));
                cnt++;
            }
        }
    }

    return seq;
}

DvdEx::DvdExMediaSeq *
DvdExRepository_i::getMediaOfType(DvdEx::Genre g) {
    int cnt = 0;
    int i = 0;

    /* determine sequence length */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            if (media[i]->Type == g)
                cnt++;
        }
    }

    cout << "<getMediaOfType> " << cnt << " Medien gefunden.\n";
    cout.flush();

    DvdEx::DvdExMediaSeq* seq = new DvdEx::DvdExMediaSeq(cnt);
    seq->length(cnt);

    cout << "<getMediaOfType> " << seq->length() << " in der Sequenz.\n";
    cout.flush();

    cnt = 0;

    /* copy result */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            if (media[i]->Type == g) {
                (*seq)[cnt] = *(new DvdEx::DvdExMedia(*(media[i])));
                cnt++;
            }
        }
    }

    return seq;
}

DvdEx::DvdExMedia *
DvdExRepository_i::getMedia(CORBA::Long id) {
    DvdEx::DvdExMedia* res;
    if ((id >= m_MaxMediaNo - 1) || (id < 0) ||
            (media[id] == 0)) {
        cerr << "<getMedia> " << id << " existiert nicht!"
                << endl;
        cerr.flush();
        // one cannot return NULL pointers since references to
        // CORBA struct are automatically freed
        res = defaultEmptyMedia;
    } else {
        res = media[id];
    }
    return new DvdEx::DvdExMedia(*res);
}

DvdEx::DvdExMediaSeq *
DvdExRepository_i::getMediaAvailable() {
    int cnt = 0;
    int i = 0;

    /* determine sequence length */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            if (media[i]->Status == 1)
                cnt++;
        }
    }

    cout << "<getMediaAvailable> " << cnt << " Medien gefunden.\n";
    cout.flush();

    DvdEx::DvdExMediaSeq* seq = new DvdEx::DvdExMediaSeq(cnt);
    seq->length(cnt);

    cout << "<getMediaAvailable> " << seq->length() << " in der Sequenz.\n";
    cout.flush();

    cnt = 0;

    /* copy result */
    for (i = 0; i < m_MaxMediaNo; i++) {
        if (media[i] != 0) {
            if (media[i]->Status == 1) {
                (*seq)[cnt] = *(new DvdEx::DvdExMedia(*(media[i])));
                cnt++;
            }
        }
    }

    return seq;
}

void
DvdExRepository_i::delMedia(CORBA::Long id) {
    if ((id >= m_MaxMediaNo - 1) || (id < 0) ||
            (media[id] == 0)) {
        cerr << "<delMedia> ID existiert nicht!" << endl;
        cerr.flush();
    }

    DvdEx::DvdExMedia* nm = media[id];
    cout << "<delMedia> Loeschen von Medium '"
            << nm->Title << "' (" << nm->ObjectId << ")."
            << endl;
    cout.flush();
    media[id] = 0;
    delete nm;
}

void
DvdExRepository_i::addMedia(const DvdEx::DvdExMedia & m) {
    cout << "Wartet 30 Sekunden vorm hinzufügen eines neuen Mediums"<<endl;
    sleep(30);
    if (m_currentMaxMediaId < m_MaxMediaNo - 1) {
        DvdEx::DvdExMedia* nm = new DvdEx::DvdExMedia();
        nm->ObjectId = m_currentMaxMediaId;
        nm->ProviderId = m.ProviderId;

        nm->Type = m.Type;
        nm->Title = CORBA::string_dup(m.Title);
        nm->ProductionCountry = CORBA::string_dup(m.ProductionCountry);
        nm->ProductionYear = m.ProductionYear;
        nm->Length = m.Length;
        nm->Status = m.Status;

        media[m_currentMaxMediaId] = nm;

        cout << "<addMedia> neues Medium '"
                << nm->Title << "' (" << nm->ObjectId << ")."
                << endl;
        cout.flush();
        m_currentMaxMediaId++;
    } else {
        cerr << "<addMedia> max. Anzahl erreicht!" << endl;
        cerr.flush();
    }
}

DvdEx::DvdExProvider *
DvdExRepository_i::getProvider(CORBA::Long id) {
    DvdEx::DvdExProvider* res;
    if ((id >= m_MaxProviderNo - 1) || (id < 0) ||
            (provider[id] == 0)) {
        cerr << "<getProvider> " << id << " existiert nicht!" << endl;
        cerr.flush();
        // one cannot return NULL pointers since references to
        // CORBA struct are automatically freed
        res = defaultEmptyProvider;
    } else {
        res = provider[id];
    }
    return new DvdEx::DvdExProvider(*res);
}

DvdEx::DvdExProviderSeq *
DvdExRepository_i::getProviders() {
    int cnt = 0;
    int i = 0;

    /* determine sequence length */
    for (i = 0; i < m_MaxProviderNo; i++) {
        if (provider[i] != 0) {
            cnt++;
        }
    }

    cout << "<getProviders> " << cnt << " Anbieter gefunden.\n";
    cout.flush();

    DvdEx::DvdExProviderSeq* seq = new DvdEx::DvdExProviderSeq(cnt);
    seq->length(cnt);

    cout << "<getProviders> " << seq->length() << " in der Sequenz.\n";
    cout.flush();

    cnt = 0;

    /* copy result */
    for (i = 0; i < m_MaxProviderNo; i++) {
        if (provider[i] != 0) {
            (*seq)[cnt] = *(new DvdEx::DvdExProvider(*(provider[i])));
            cnt++;
        }
    }

    return seq;
}

void
DvdExRepository_i::delProvider(CORBA::Long id) {
    if ((id >= m_MaxProviderNo - 1) || (id < 0) ||
            (provider[id] == 0)) {
        cerr << "<delProvider> ID existiert nicht!" << endl;
        cerr.flush();
    }

    DvdEx::DvdExProvider* np = provider[id];
    cout << "<delProvider> Anbieter loeschen'"
            << np->Name << "' (" << np->ObjectId << ")."
            << endl;
    cout.flush();
    delete np;
    provider[id] = 0;
}

void
DvdExRepository_i::addProvider(const DvdEx::DvdExProvider & p) {
    cout << "Wartet 30 Sekunden vorm hinzufügen eines neuen Mediums"<<endl;
    sleep(30);
    if (m_currentMaxProviderId < m_MaxProviderNo - 1) {
        DvdEx::DvdExProvider* np = new DvdEx::DvdExProvider();
        np->ObjectId = m_currentMaxProviderId;
        np->Name = CORBA::string_dup(p.Name);
        np->FirstName = CORBA::string_dup(p.FirstName);
        np->ZIPCode = p.ZIPCode;
        np->Address = CORBA::string_dup(p.Address);

        provider[m_currentMaxProviderId] = np;

        cout << "<addProvider> neuer Anbieter '"
                << np->Name << "' (" << np->ObjectId << ")."
                << endl;
        cout.flush();
        m_currentMaxProviderId++;
    } else {
        cerr << "<addProvider> max. Anzahl erreicht!" << endl;
        cerr.flush();
    }
}

// save

void
DvdExRepository_i::save() {
    cout << "<DvdExRepository_i> Sichern des Repositorys...\n";
    cout.flush();

    backup(media, m_MaxMediaNo, provider, m_MaxProviderNo);
}

