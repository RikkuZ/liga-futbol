#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

// --- ESTRUCTURAS ---
struct Equipo {
    string nombre;
    int pj = 0, pg = 0, pe = 0, pp = 0;
    int gf = 0, gc = 0, dg = 0, pts = 0;
};

struct Partido {
    string fecha, local, visitante;
    int golesL, golesV;
};

struct Config {
    string nombreLiga;
    int pGanado, pEmpatado, pPerdido;
    vector<string> nombresEquipos;
};

// --- PROTOTIPOS ---
bool leerConfig(Config &c);
void registrarPartido(const Config &c);
void mostrarTabla(const Config &c);
void actualizarEstadisticas(Equipo* e, int favor, int contra, const Config &c);
void verHistorialJornadas();

// --- FUNCIÓN PRINCIPAL ---
int main() {
    Config config;
    if (!leerConfig(config)) return 1;

    int opcion;
    do {
        cout << "\n--- " << config.nombreLiga << " ---" << endl;
        cout << "1. Ver tabla de posiciones\n2. Registrar resultado\n3. Ver historial de jornadas\n4. Salir\nSeleccion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: mostrarTabla(config); break;
            case 2: registrarPartido(config); break;
            case 3: verHistorialJornadas(); break;
        }
    } while (opcion != 4);

    return 0;
}

// --- IMPLEMENTACIONES ---

bool leerConfig(Config &c) {
    ifstream file("data/config.txt");
    if (!file.is_open()) {
        cout << "Error: No se encuentra data/config.txt" << endl;
        return false;
    }
    string linea;
    while (getline(file, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        size_t pos = linea.find('=');
        string clave = linea.substr(0, pos);
        string valor = linea.substr(pos + 1);

        if (clave == "nombre") c.nombreLiga = valor;
        else if (clave == "p_ganado") c.pGanado = stoi(valor);
        else if (clave == "p_empatado") c.pEmpatado = stoi(valor);
        else if (clave == "p_perdido") c.pPerdido = stoi(valor);
        else if (clave == "equipo") c.nombresEquipos.push_back(valor);
    }
    return true;
}

void actualizarEstadisticas(Equipo* e, int favor, int contra, const Config &c) {
    e->pj++;
    e->gf += favor;
    e->gc += contra;
    e->dg = e->gf - e->gc;
    if (favor > contra) { e->pg++; e->pts += c.pGanado; }
    else if (favor == contra) { e->pe++; e->pts += c.pEmpatado; }
    else { e->pp++; e->pts += c.pPerdido; }
}

void mostrarTabla(const Config &c) {
    vector<Equipo> tabla;
    for (const string& n : c.nombresEquipos) {
        Equipo eq; eq.nombre = n;
        tabla.push_back(eq);
    }

    ifstream file("data/partidos.txt");
    string f, el, ev; int gl, gv;
    while (file >> f >> el >> ev >> gl >> gv) {
        for (auto &eq : tabla) {
            if (eq.nombre == el) actualizarEstadisticas(&eq, gl, gv, c);
            if (eq.nombre == ev) actualizarEstadisticas(&eq, gv, gl, c);
        }
    }

    sort(tabla.begin(), tabla.end(), [](const Equipo &a, const Equipo &b) {
        return a.pts > b.pts;
    });

    cout << "\n# | Equipo | PJ | PG | PE | PP | GF | GC | DG | PTS" << endl;
    for (int i = 0; i < tabla.size(); i++) {
        cout << i + 1 << " | " << setw(10) << tabla[i].nombre << " | " << tabla[i].pj << " | " 
             << tabla[i].pts << endl;
    }
}

void registrarPartido(const Config &c) {
    string fecha, local, visit;
    int gl, gv;
    cout << "Fecha (DD/MM): "; cin >> fecha;
    cout << "Local: "; cin >> local;
    cout << "Visitante: "; cin >> visit;
    cout << "Goles Local: "; cin >> gl;
    cout << "Goles Visitante: "; cin >> gv;

    ofstream pFile("data/partidos.txt", ios::app);
    pFile << fecha << " " << local << " " << visit << " " << gl << " " << gv << endl;
    
    ofstream fFile("data/fechas.txt", ios::app);
    fFile << "JORNADA_N\n" << local << " " << gl << " - " << gv << " " << visit << "\nFIN_JORNADA" << endl;
}

void verHistorialJornadas() {
    ifstream file("data/fechas.txt");
    string linea;
    while (getline(file, linea)) cout << linea << endl;
}
