#include <iostream>
#include <string>
#include <fstream>
#include "trimesh_t.h"
#include "vtk.h"
#include "data_io.h"

using namespace trimesh;
using namespace std;

int main() {
    string path = "mesh.vtk";
    ifstream is("torus.obj");
    if (!is.is_open()) {
        cout << "fail to open the file" << endl;
        return -1;
    }
    trimesh_t mesh;
    readTri(is, mesh);
    writeVtk(path, mesh.getMPoints(), mesh.getMTriangles());
    vector<index_t > neighs;
    for (index_t vi = 0; vi < mesh.getPointSize(); ++vi) {
        mesh.vvNeighbors(vi, neighs);
        cout << "neighbors of vertex " << vi << ": ";
        for (index_t i = 0; i < neighs.size(); ++i) {
            cout << ' ' << neighs.at(i);
        }
        cout << '\n';
    }
    vector<pair<index_t, index_t> > boundary;
    mesh.boundaryEdges(boundary);
    cout << "boundary:";
    for (index_t i = 0; i < boundary.size(); ++i) {
        cout << boundary[i].first << "->" << boundary[i].second << endl;
    }
    cout << "\n";
    vector<index_t > faceNeighbors = mesh.vfNeighbors(0);
    cout << "face neighbors:";
    for (index_t i = 0; i < faceNeighbors.size(); ++i) {
        cout << " " << faceNeighbors[i] ;
    }
    cout << "\n";
    cout << "edge neighbors:";
    vector<index_t > edgeNeighbors = mesh.efNeighbors(1, 2);
    for (index_t i = 0; i < edgeNeighbors.size(); ++i) {
        cout << " " << edgeNeighbors[i] ;
    }
    cout << "\n";
    return 0;
}