#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary {
// definisco le funzioni che mi vanno a importare le mesh e la lettura dei dati dai vari file
//in particolare se la lettura è andata a buon fine mi restituisce true altrimenti false
bool ImportMesh(const string &filepath,
                PolygonalMesh& mesh);


bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh);


bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh);


bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh);

}



