//
// Created by Administrator on 2019/3/11 0011.
//
/**
 * 管理mesh的io
 */
#pragma once
#ifndef HALFEDGE_DATA_LOADER_H
#define HALFEDGE_DATA_LOADER_H
#include "trimesh_t.h"
#include <iostream>     // std::cin, std::cout
#include <fstream>      // std::ifstream
#include <vector>
#endif
using namespace trimesh;
using namespace std;

int readTri(ifstream& is, trimesh_t& mesh) {
    vector<point_t> points;
    vector<triangle_t> triangles;
    vector<edge_t> edges;
    int pcount = 0, tricount = 0;
    string s;
    char c;
    while (is.get(c)) {
        switch (c) {
            case '#':
                getline(is, s);
                if (s.find("Vertices") != string::npos) {
                    points.resize(size_t (stoll(s.substr(11))));
                }
                if (s.find("Faces") != string::npos) {
                    triangles.resize(size_t (stoll(s.substr(8))));
                }
                break;
            case 'v':
                point_t p;
                p.index = pcount;
                is >> p.x >> p.y >> p.z;
                is.get();
                //cout << "points" << p.x << " " << p.y << " " << p.z << endl;
                points[pcount++] = p;
                break;
            case 'f':
                triangle_t triangle;
                int x, y, z;
                is >> x >> y >> z;
                is.get();
                //cout << "face:" << x << " " << y << " " << z << endl;
                if (!(x <= points.size() && y <= points.size() && z <= points.size())) {
                    cout << "数组越界：x=" << x << "，y=" << y << "，z=" << z << endl;
                    cout << "maxSize=" << points.size() << endl;
                    exit(-1);
                }
                if (tricount == 18429) {
                    cout << endl;
                }
                triangle.v[0] = points[x - 1];
                triangle.v[1] = points[y - 1];
                triangle.v[2] = points[z - 1];
                triangles[tricount++] = triangle;
                break;
            default:
                //getline(is, s);
                break;
        }
    }
    //cout << "points:" << pcount << " faces:" << tricount << endl;
    unorderedEdgesFromTriangles(triangles, edges);
    mesh.build(points, edges, triangles);
    return 0;
}

int writeVtk(string path, vector<point_t> points, vector<triangle_t> triangles) {
    vector<float > nodes;
    vector<int> faces;
    for (index_t i = 0; i < points.size(); i++) {
        nodes.push_back(points[i].x);
        nodes.push_back(points[i].y);
        nodes.push_back(points[i].z);
        //cout << "points:" << points[i].x << " " << points[i].y << " " << points[i].z << endl;
    }
    for (index_t i = 0; i < triangles.size(); i++) {
        faces.push_back(triangles[i].i().index);
        faces.push_back(triangles[i].j().index);
        faces.push_back(triangles[i].k().index);
        //cout << "faces:" << triangles[i].i().index << " " << triangles[i].j().index << " " << triangles[i].k().index << endl;
    }
    ofstream os(path.c_str());
    tri2vtk(os, &nodes[0], points.size(), &faces[0], triangles.size());
    return 0;
}

