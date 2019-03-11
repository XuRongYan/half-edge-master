#include <iostream>
#include <string>
#include <fstream>
#include "trimesh_t.h"
#include "vtk.h"
/**
       TODO 1.减少复杂数据结构的返回


 */
using namespace trimesh;
using namespace std;

int main() {
    vector<triangle_t> triangles;
    vector<point_t> points(4);
    points[0] = {0, 1 ,2, 3};
    points[1] = {1, 0, 0, 1};
    points[2] = {2, 0, 1, 0};
    points[3] = {3, 1, 0, 0};
    triangles.resize(2);
    triangles[0].v[0] = points[0];
    triangles[0].v[1] = points[1];
    triangles[0].v[2] = points[2];
    triangles[1].v[0] = points[2];
    triangles[1].v[1] = points[1];
    triangles[1].v[2] = points[3];
    const int kNumVertices = 4;
    vector<edge_t> edges;
    ofstream os("test.vtk");
    float nodes[12] = {1, 2, 3, 0, 0, 1, 0, 1, 0, 1, 0, 0};
    int tries[6] = {0, 1, 2, 2, 1, 3};
    tri2vtk(os, nodes, 4, tries, 2);
    unorderedEdgesFromTriangles(triangles.size(), &triangles[0], edges);
    trimesh_t mesh;
    mesh.build(points, edges, triangles);
    vector<index_t> neighs;
    for (int vi = 0; vi < kNumVertices; ++vi) {
        mesh.vvNeighbors(vi, neighs);
        cout << "neighbors of vertex " << vi << ": ";
        for (int i = 0; i < neighs.size(); ++i) {
            cout << ' ' << neighs.at(i);
        }
        cout << '\n';
    }
    vector<pair<index_t, index_t> > boundary;
    mesh.boundaryEdges(boundary);
    cout << "boundary:";
    for (int i = 0; i < boundary.size(); ++i) {
        cout << boundary[i].first << "->" << boundary[i].second << endl;
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