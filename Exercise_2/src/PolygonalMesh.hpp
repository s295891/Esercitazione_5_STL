#pragma once

#include <iostream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

namespace PolygonalLibrary {

struct PolygonalMesh
{

    // per le celle 0D bisogna memorizzare:
    // il numero di celle
    unsigned int NumberCell0D = 0;
    // un indirizzo id
    std::vector<unsigned int> Cell0DId = {};
    // le coordinate x,y, che vanno inserite in vettori due dimensionali
    std::vector<Vector2d> Cell0DCoordinates = {};
    // una mappa che associ a una chiave una lista di marcatori per le celle
    std::map<unsigned int, list<unsigned int>> Cell0DMarkers = {};


    // per le celle 1D memorizziamo
    // il numero di celle
    unsigned int NumberCell1D = 0;
    //identificativo id
    std::vector<unsigned int> Cell1DId = {};
    //un vettore che contenga i vertici di origine e fine
    std::vector<Vector2i> Cell1DVertices = {};
    // i soliti marker dentro una mappa
    std::map<unsigned int, list<unsigned int>> Cell1DMarkers = {};

    //per le celle 2D:
    // il numero di celle 2D
    unsigned int NumberCell2D = 0;
    // sempre l'id
    std::vector<unsigned int> Cell2DId = {};

    // siccome nei poligoni il numero di vertici e di lati cambia da cella a cella memorizziamo anche
    // il numero di veritici e di lati dei poligoni
    unsigned int NumVerticesCell2D =0;
    unsigned int NumEdgesCell2D = 0;

    // il discorso sulla memorizzazione dei vertici e dei lati è complesso in quanto il numero di vertici e lati cambia da poligono a poligono
    // per cui devo lavorare con la memoria dinamica e l'utilizzo di puntatori
    // in particolare la mappa è una struttura utile che mi permette di assegnare un id alla memorizzazione del poligono
    std::map<unsigned int, std::vector<unsigned int>> Cell2DVertices = {};
    std::map<unsigned int, std::vector<unsigned int>> Cell2DEdges = {};


};




}
