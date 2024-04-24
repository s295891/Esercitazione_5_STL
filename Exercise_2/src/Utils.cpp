
#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;
namespace PolygonalLibrary {


// con questa funzione verifico la correttezza delle importazioni, e facendo test sulla lunghezza dei lati e le aree dei poligoni

bool ImportMesh(const string& filepath,
                PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    // i marker per 0D e 1D sono stati importartati correttamente

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    // infine svolgo i test assicurandomi che le celle 2D non contengano lati nulli o poligoni con area nulla
    {

        //cout<<"per le celle2D tutto e' stato importato correttamente, si procede ora con il fare dei test"<< endl;

        // Test:
        // Lunghezza dei lati

        for (const auto& pair : mesh.Cell2DEdges)
        {
            unsigned int cellId = pair.first;  //estrae il primo elemento dalla mappa che è l'id
            const auto& edges = pair.second; //estrae il secondo elemento dalla mappa che è il vettore

            // Verifica la lunghezza di ogni edge nella cella 2D
            for (unsigned int edgeId : edges)
            {
                // vertici dell'edge
                const unsigned int fromId = mesh.Cell1DVertices[edgeId][0];
                const unsigned int toId = mesh.Cell1DVertices[edgeId][1];

                // Calcola la lunghezza dell'edge
                const Vector2d& fromVertex = mesh.Cell0DCoordinates[fromId];
                const Vector2d& toVertex = mesh.Cell0DCoordinates[toId];
                const double length = (toVertex - fromVertex).norm();

                //proviamo a vedere la lunghezza dei lati se ha senso:

                //cout<< "la lunghezza che va dal vertice"<<fromVertex<< " al vertice " << toVertex<< "vale " << length << endl;

                // Verifica se la lunghezza è nulla o troppo piccola
                if (length < 1.0e-15) {
                    cerr << "Edge " << edgeId << " della cella 2D " << cellId << " ha lunghezza nulla " << endl;

                }
                // else{
                //     cout<<"i lati hanno la lunghezza maggiore della tolleranza"<<endl;
                // }
            }
        }



        // Area poligoni, calcolata con il metodo che permettere di ricondurre poligoni a aree di triangoli

        for (const auto& pair : mesh.Cell2DVertices)
        {
            unsigned int cellId = pair.first;
            const auto& vertices = pair.second;

            // Controlla che ci siano almeno tre vertici per formare un poligono, condizione necessaria per il calcolo dell'area
            if (vertices.size() < 3) {
                cerr << "La cella 2D " << cellId << " ha meno di tre vertici " << endl;
                continue; // Passa alla prossima cella 2D
            }

            // Calcola l'area del poligono utilizzando il metodo del doppio dell'area del poligono
            double area = 0.0;
            for (unsigned int i = 0; i < vertices.size(); ++i) {
                unsigned int j = (i + 1) % vertices.size();
                const Vector2d& vertex1 = mesh.Cell0DCoordinates[vertices[i]];
                const Vector2d& vertex2 = mesh.Cell0DCoordinates[vertices[j]];
                area += vertex1.x() * vertex2.y() - vertex1.y() * vertex2.x();
            }
            area /= 2.0;
            //cout<<area<<endl;

            // Verifica se l'area è nulla o troppo piccola
            if (area < 1.0e-15) {
                cerr << "La cella 2D " << cellId << " ha area nulla " << endl;

            }
            // else{
            //     cout<<"le aree sono tutte maggiori della tolleranza"<<endl;
            // }
        }





    }
    cout<<"tutti i test sono stati eseguiti correttamente"<<endl;


    return true;

}






// *************************************************************************************************************************************************

bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    // apro il file e verifico che si sia aperto correttamente
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    // la funzione legge le linee dal file e le memorizza in una lista
    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    //la prima riga contiene le intestazioni, perciò viene scartata
    listLines.pop_front();

    // viene memorizzato il numero di celle 0D sulla base della dimensione della lista
    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "Non ci sono celle 0D" << endl;
        return false;
    }


    //la funzione legge gli id  dal file e li memorizza
    mesh.Cell0DId.reserve(mesh.NumberCell0D);

    // riserva spazio nel vettore delle coordinate per poi riempirlo con la lettura dal file
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);


    //adesso la lettura dal mio file deve considerare che i vari dati sono separati da ;

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;

        string token;

        // Legge id
        if (getline(converter, token, ';'))
            id = stoi(token); // Converte la stringa in intero


        //Legge marker
        if (getline(converter, token, ';'))
            marker = stoi(token); // Converte la stringa in intero

        // Legge le coordinate x e y
        for (int i = 0; i < 2; ++i) {
            if (getline(converter, token, ';'))
                coord(i) = stod(token); // Converte la stringa in double
        }


        // vengono "riempite" e memorizzate le varie informazioni dentro al struttura definita in PolygonalMesh.hpp
        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);


        //lettura e memorizzazione dei marker
        if( marker != 0)
        {
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }

    }
    file.close();
    return true;
}



//**************************************************************************************************************************************************
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    // il procedimento è del tutto analogo a ImportCell0Ds
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "Non ci sono celle 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
        string token;

        // Legge id
        if (getline(converter, token, ';'))
            id = stoi(token); // Converte la stringa in intero


        //Legge marker
        if (getline(converter, token, ';'))
            marker = stoi(token); // Converte la stringa in intero

        for (int i = 0; i < 2; ++i) {
            if (getline(converter, token, ';'))
                vertices(i) = stoi(token); // Converte la stringa in intero
        }



        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    file.close();

    return true;
}


//**************************************************************************************************************************************************



bool ImportCell2Ds(const string &filename, PolygonalMesh& mesh)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        return false;
    }


    // Salta la prima riga dell'intestazione, in quanto viene letta ma non vengono svolte operazioni
    string first_line;
    getline(file, first_line);

    string line;

    // Legge il file riga per riga
    while (getline(file, line))
    {
        istringstream converter(line);
        unsigned int id, numVertices, numEdges, marker;

        // Legge l'id, il numero di vertici e il numero di lati
        string token;
        if (getline(converter, token, ';'))
            id = stoi(token);
        //cout<<id<<endl;


        //per saltare il marker
        if (getline(converter, token, ';'))
            marker = stoi(token);
        //cout<<marker<<endl;

        if (getline(converter, token, ';'))
            numVertices = stoi(token);
        //cout<<numVertices<<endl;

        // Alloca dinamicamente i vettori per i vertici, andando con new a chiedere memoria
        vector<unsigned int> vertexArray(numVertices);


        // Legge i vertici
        for (unsigned int i = 0; i < numVertices; ++i)
        {
            if (getline(converter, token, ';'))
                vertexArray[i] = stoi(token);
            //cout<<vertexArray[i]<<endl;
        }

        //controllo i vettori
        // for (const auto& vertex : vertexArray)
        // {
        //     cout << vertex << " ";
        // }
        // cout << endl;

        // Legge il numero di lati
        if (getline(converter, token, ';'))
            numEdges = stoi(token);



        // Alloca dinamicamente i vettori per i lati, andando con new a chiedere memoria

        vector<unsigned int> edgeArray(numEdges);
        //legge i lati
        for (unsigned int i = 0; i < numEdges; ++i)
        {
            if (getline(converter, token, ';'))
                edgeArray[i] = stoi(token);
        }
        // for (const auto& edges : edgeArray)
        // {
        //     cout << edges << " ";
        // }
        // cout << endl;

        // Aggiunge i vettori di vertici e lati alle mappe

        mesh.Cell2DVertices[id] = std::vector<unsigned int>(vertexArray);
        mesh.Cell2DEdges[id] = std::vector<unsigned int>(edgeArray);



        mesh.NumVerticesCell2D = numVertices;
        mesh.NumEdgesCell2D = numEdges;
    }
    // cout << "Vertex Map:" << endl;
    // for (const auto& pair : mesh.Cell2DVertices)
    // {
    //     cout << "ID: " << pair.first << ", Vertices: ";
    //     for (const auto& vertex : pair.second)
    //     {
    //         cout << vertex << " ";
    //     }
    //     cout << endl;
    // }

    // cout << "Edge Map:" << endl;
    // for (const auto& pair : mesh.Cell2DEdges)
    // {
    //     cout << "ID: " << pair.first << ", Edges: ";
    //     for (const auto& edge : *pair.second)
    //     {
    //         cout << edge << " ";
    //     }
    //     cout << endl;
    // }


    // va a leggere il numero di celle guardando la size degli id
    mesh.NumberCell2D = mesh.Cell2DId.size();

    file.close();




    return true;
}







}
