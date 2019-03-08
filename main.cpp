#include <iostream>
#include "trimesh_t.h"

using namespace trimesh;
using namespace std;

int main() {
    vector<triangle_t> triangles;

    triangles.resize(2);
    triangles[0].v[0] = 0;
    triangles[0].v[1] = 1;
    triangles[0].v[2] = 2;
    triangles[1].v[0] = 2;
    triangles[1].v[1] = 1;
    triangles[1].v[2] = 3;
    const int kNumVertices = 4;
    vector<edge_t> edges;
    unorderedEdgesFromTriangles(triangles.size(), &triangles[0], edges);
    trimesh_t mesh;
    mesh.build(kNumVertices, triangles.size(), &triangles[0], edges.size(), &edges[0]);
    vector<index_t> neighs;
    for (int vi = 0; vi < kNumVertices; ++vi) {
        mesh.vvNeighbors(vi, neighs);
        std::cout << "neighbors of vertex " << vi << ": ";
        for (int i = 0; i < neighs.size(); ++i) {
            std::cout << ' ' << neighs.at(i);
        }
        std::cout << '\n';
    }
    vector<index_t > boundary = mesh.boundaryVertices();
    cout << "boundary:";
    for (int i = 0; i < boundary.size(); ++i) {
        cout << " " << boundary[i] ;
    }
    cout << "\n";
    vector<index_t > faceNeighbors = mesh.vfNeighbors(0);
    cout << "face neighbors:";
    for (int i = 0; i < faceNeighbors.size(); ++i) {
        cout << " " << faceNeighbors[i] ;
    }
    cout << "\n";
    cout << "edge neighbors:";
    vector<index_t > edgeNeighbors = mesh.efNeighbors(1, 2);
    for (int i = 0; i < edgeNeighbors.size(); ++i) {
        cout << " " << edgeNeighbors[i] ;
    }
    cout << "\n";
    return 0;
}