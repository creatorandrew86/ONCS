#include <iostream>
#include <cmath>
#include "input.h"
#include "temperatura.h"


double qTransfer(Camera& camera, Stare& stare, double T_camera);

void simulare(Camera& camera, Stare& stare, Locatie& locatie, double T_init, double t_init, double t_final, bool radiator);
void control();

int main() {
    // Fisierul de configuratie
    std::string fisier_config;

    std::cout << "Numele fisierului de configuratie: ";
    std::cin >> fisier_config;

    // Citirea fisierului
    Config cfg = upload_config(fisier_config);

    // Obtinerea obiectelor camera, stare si locatie
    Camera camera = make_camera(cfg);
    Stare stare = make_stare(cfg);
    Locatie locatie = make_locatie(cfg);

    simulare(camera, stare, locatie, 19, 14, 15, false);

    return 0;
}



// Functia de calculare a transferului total de caldura in functie de temperatura camerei
double qTransfer(Camera& camera, Stare& stare, double T_camera){
    // Transferul de caldura prin perete
    double qPerete = (T_camera - stare.T_exterior) / (camera.R_interior + camera.R_exterior + camera.R_perete);
    double QPerete = qPerete * camera.A_perete;

    // Transferul de caldura prin fereastra
    double qFereastra = (T_camera - stare.T_exterior) / (camera.R_interior + camera.R_exterior + camera.R_fereastra);
    double QFereastra = qFereastra * camera.A_fereastra;

    // Pierderea de cadura prin infiltrare
    double QInfiltrare = stare.cp_aer * stare.rho_aer * (camera.ACH * camera.V_camera / 3600) * (T_camera - stare.T_exterior);


    // Transferul total de caldura (W)
    double QTotal = QFereastra + QPerete + QInfiltrare;
    
    return QTotal;
}



// Functia de simulare a apartamentului
void simulare(Camera& camera, Stare& stare, Locatie& locatie, double T_init, double t_init, double t_final, bool radiator){
    // Initializarea variabilelor
    double T_camera = T_init;
    double dT;

    // Pasul temporal (h)
    double dt = 0.05;

    
    // Time loop
    for (int t = t_init; t <= t_final; t += dt) {
        // Obtinerea temperaturii exterioare
        stare.T_exterior = get_temperatura(locatie.oras, locatie.data, floor(t));

        // Diferenta de temperatura
        if (radiator) {
            dT = ( (-qTransfer(camera, stare, T_camera) + camera.P_radiator) * (dt * 3600) ) / (stare.cp_aer * stare.rho_aer * camera.V_camera * camera.FMT);
        } else {
            dT = ( -qTransfer(camera, stare, T_camera) * (dt * 3600) ) / (stare.cp_aer * stare.rho_aer * camera.V_camera * camera.FMT);
        }

        std::cout << t << std::endl << T_camera << "\n\n";
        
        T_camera += dT;
    }
}


// Functia de control a termostatului
void control(){

}