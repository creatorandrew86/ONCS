#include <iostream>
#include "input.h"


double qTransfer(Camera& camera, Stare& stare, double T_camera);

void control();
void simulare(Camera& camera, Stare& stare, double T_init);


int main() {
    // Fisierul de configuratie
    std::string fisier_config;

    std::cout << "Nota: Fisierul de configuratie trebuie sa fie in acelasi folder cu main.cpp!" << std::endl;

    std::cout << "Numele fisierului de configuratie: ";
    std::cin >> fisier_config;

    // Obtinerea fisierului de configuratie, notat: cfg
    Config cfg = upload_config(fisier_config);

    // Obtinerea obiectelor camera si stare
    Camera camera = make_camera(cfg);
    Stare stare = make_stare(cfg);

    // Temperatura camerei (deg C)
    double T_camera = 20;

    return 0;
}



// Functia de calculare a pierderii totale de caldura in functie de temperatura camerei
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



// Functia de control a termostatului
void control(){

}


// Functia de simulare a apartamentului
void simulare(Camera& camera, Stare& stare, double T_init){
    // Initializarea variabilelor
    double Q, T_camera, dT;

    T_camera = T_init;

    // Ora inceperii/finalizarii simularii (hh:00:00)
    double t = 8;
    double t_final = 18;

    // Pasul temporal (6 min)
    double dt = 0.1;

    
    // Time loop
    while (t <= t_final) {
        // Transferul de caldura
        Q = qTransfer(camera, stare, T_camera);

        // Diferenta de temperatura
        dT = (Q * (dt * 3600)) / (stare.cp_aer * (stare.rho_aer * camera.V_camera * camera.FMT));


        T_camera += dT;
        t += dt;
    }

    
}