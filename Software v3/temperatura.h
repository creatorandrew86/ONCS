#pragma once
#include <string>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <cstdlib>
#include "input.h"
 
 
// Baza de date pentru coordonatele oraselor disponibile
struct Coordonate { 
    double lat, lon; 
};
 
inline Coordonate get_coordonate(const std::string& oras) {
    static const std::unordered_map<std::string, Coordonate> orase = {
        { "BUCURESTI",   { 44.4268, 26.1025 } },
        { "CLUJ_NAPOCA", { 46.7712, 23.6236 } },
        { "CONSTANTA",   { 44.1733, 28.6383 } },
        { "IASI",        { 47.1585, 27.6014 } },
        { "TIMISOARA",   { 45.7489, 21.2087 } },
        { "ARAD",        { 46.1866, 21.3123 } },
        { "ORADEA",      { 47.0458, 21.9189 } },
        { "BRAILA",      { 45.2692, 27.9574 } },
        { "SUCEAVA",     { 47.6514, 26.2556 } },
        { "GALATI",      { 45.4353, 28.0080 } },
        { "BRASOV",      { 45.6427, 25.5887 } },
        { "SIBIU",       { 45.7983, 24.1256 } },
        { "ALBA_IULIA",  { 46.0674, 23.5799 } },
        { "TULCEA",      { 45.1787, 28.8047 } },
    };
    auto it = orase.find(oras);
    if (it == orase.end())
        throw std::invalid_argument("Input invalid: ORAS = " + oras);
    return it->second;
}
 
 

//  Functia de extragere a temperaturii din Python
//
//  Format "data"     : "YYYY-MM-DD"  ex: "2024-01-15"
//  Formar "prognoza" : false = date istorice | true = prognoza viitoare

inline double get_temperatura(const std::string& oras, const std::string& data, int ora, bool prognoza = true) {
    if (ora < 0 || ora > 23)
        throw std::invalid_argument("Input invalid: ora = " + std::to_string(ora));
 
    auto [lat, lon] = get_coordonate(oras);
 
    // Extragerea temperaturii din fisierul python
    std::string cmd = "python getTemp.py "
        + std::to_string(lat) + " "
        + std::to_string(lon) + " "
        + data + " "
        + std::to_string(ora) + " "
        + (prognoza ? "1" : "0")
        + " > temp.txt";
 
    if (system(cmd.c_str()) != 0)
        throw std::runtime_error("Eroare: Python");
 
    // Citirea rezultatului din fisier
    std::ifstream f("temp.txt");
    if (!f.is_open())
        throw std::runtime_error("Eroare: temp.txt nu a putut fi deschis");
 
    double T_exterior;
    if (!(f >> T_exterior))
        throw std::runtime_error("Eroare: temperatura invalida");
 
    return T_exterior;
}