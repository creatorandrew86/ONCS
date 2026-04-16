#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>


// Proprietatile camerei
struct Camera {
    // Dimensiunile camerei
    double A_camera, inaltime, V_camera;                                        // (m2) / (m) / (m3)                           

    // "perete" cu referinta la peretele spre exterior, cu fereastra
    double A_perete, A_fereastra;                                               // (m2) / (m2)


    // Rezistentele termice
    double R_interior, R_exterior;                                              // (m2K/W)
    double R_perete, R_fereastra;                                               // (m2K/W)

    // Grosimea peretelui
    double t_perete;                                                            // (m)

    // "Air Changes per Hour" - infiltratia aerului (1/h)       ||         Solar Heat Gain Coefficient (SGHC)
    double ACH, SGHC, FMT;                                       

    // Puterea de incalzire/racire (W)
    double P_radiator;                                                      
};


// Proprietatile de stare
struct Stare {
    // Temperatura exterioara
    double T_exterior;

    // Proprietatile aerului   (kg/m3) || (J/kgK)
    double rho_aer, cp_aer;          
};


// Configuratia programului
struct configSolver {
    // Temperatura dorita
    double T_target;
};


// Preset pentru tipul de bloc
struct PresetBloc {
    double t_perete, R_perete, ACH;
};

// Preset pentru tipul de fereastra
struct PresetFereastra {
    double R_fereastra, SHGC;
};


// Baza de date pentru tipul de bloc
// Returneaza { grosimea peretelui || rezistenta termica a peretelui || Air Change per Hour }
inline PresetBloc get_preset_bloc(const std::string& tip) {
    if (tip == "BLOC_VECHI")                return {0.20, 0.56, 1.0};
    if (tip == "BLOC_RENOVAT_PARTIAL")      return {0.20, 0.56, 0.5};
    if (tip == "BLOC_IZOLAT")               return {0.30, 2.80, 0.5};
    if (tip == "BLOC_NOU")                  return {0.35, 3.50, 0.35};

    throw std::invalid_argument("Input invalid: tip_bloc = " + tip);
}


// Baza de date pentru tipul de fereastra
// Returneaza { rezistenta termica a ferestrei || Solar Heat Gain Coefficient }
inline PresetFereastra get_preset_fereastra(const std::string& tip) {
    if (tip == "VITRAJ_SIMPLU")             return {0.17, 0.86};
    if (tip == "DUBLU_VITRAJ")              return {0.35, 0.76};
    if (tip == "TERMOPAN_2FOI")             return {0.55, 0.70};
    if (tip == "TERMOPAN_3FOI")             return {0.88, 0.50};

    throw std::invalid_argument("Input invalid: tip_fereastra = " + tip);
}


// Analizarea fisierului de configuratie (sistem cheie - valoare || key value)

// Tabela cheie - valoare
using Config = std::unordered_map<std::string, std::string>;

// Functia de deschidere si interpretare a fisierului de configuratie (fisier_config)
inline Config upload_config(const std::string& fisier_config) {

    std::ifstream f(fisier_config);

    
    if (!f.is_open())
        throw std::runtime_error("Input invalid: Fisierul nu poate fi deschis " + fisier_config);

    Config cfg;
    std::string line;

    while (std::getline(f, line)) {
        // Ignorarea liniilor goale si a comentariilor
        if (line.empty() || line[0] == '#') continue;

        auto sep = line.find('=');

        if (sep == std::string::npos)
            throw std::invalid_argument("Input invalid: " + line);


        auto trim = [](std::string s) {
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
        };

        std::string key = trim(line.substr(0, sep));
        std::string value = trim(line.substr(sep + 1));

        
        if (key.empty())
            throw std::invalid_argument("Input invalid: " + line);
        if (value.empty())
            throw std::invalid_argument("Input invalid: " + key);

        cfg[key] = value;
    }
    return cfg;
}


inline double get_double(const Config& cfg, const std::string& key) {
    auto item = cfg.find(key);

    if (item == cfg.end())
        throw std::invalid_argument("Input invalid: " + key);
    try {
        return std::stod(item->second);
    } catch (...) {
        throw std::invalid_argument("Input invalid: " + key);
    }
}




// Initializarea valorilor numerice
Camera make_camera(const Config& cfg) {
    Camera c;

    // Obtinerea tipului de bloc / fereastra din fisier
    auto bloc = get_preset_bloc(cfg.at("TIP_BLOC"));
    auto fereastra = get_preset_fereastra(cfg.at("TIP_FEREASTRA"));

    // Factorul de masa termica
    c.FMT = get_double(cfg, "FACTOR_MASA_TERMICA");


    // Dimensiunile camerei (m2 / m / m3)
    c.A_camera = get_double(cfg, "ARIE_CAMERA");                                                            
    c.inaltime = get_double(cfg, "INALTIME_CAMERA");
    c.V_camera = c.A_camera * c.inaltime;

    // Dimensiunile peretelui spre exterior
    c.A_fereastra = get_double(cfg, "ARIE_FEREASTRA");                                                          
    c.A_perete = get_double(cfg, "ARIE_PERETE") - c.A_fereastra;                                                             



    // Rezistentele termice la convectie (ISO 6946) (m2K/W)
    c.R_exterior = 0.04;                                                        
    c.R_interior = 0.13;     
    
    // Rezistentele termice la conductie (m2K/W)
    c.R_perete = bloc.R_perete;
    c.R_fereastra = fereastra.R_fereastra;

    // Grosimea peretelui (m)
    c.t_perete = bloc.t_perete;


    // ACH (1/h)
    c.ACH = bloc.ACH;                                                                

    // Puterea de incalzire/racire (W)
    c.P_radiator = get_double(cfg, "PUTERE_TERMOSTAT");                                                       


    return c;
}

Stare make_stare(const Config& cfg) {
    Stare s;

    // Temperaturi (deg C)
    s.T_exterior = get_double(cfg, "TEMPERATURA_EXTERIOR");

    // Proprietatile aerului
    s.cp_aer = 1004;                                                            // (J/kgK)
    s.rho_aer = 1.225;                                                          // (kg/m3)

    return s;
}

configSolver make_configSolver(const Config& cfg) {
    configSolver cSolver;

    cSolver.T_target = get_double(cfg, "TEMPERATURA_TARGET");

    return cSolver;
}
