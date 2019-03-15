//
// Created by Administrator on 2019/3/6 0006.
//

#include "trimesh_t.h"
#include <cassert>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;
namespace {
    typedef map<pair<index_t, index_t >, index_t > directedEdge2indexMap_t;
    index_t directedEdge2faceIndex(const directedEdge2indexMap_t& de2fi, index_t vi, index_t vj) {
        if (de2fi.empty()) {
            cout << "fail to build mesh: de2fi == empty()" << endl;
            return -1;
        }
        auto it = de2fi.find(make_pair(vi, vj));
        //没有对应面的he返回-1
        if(it == de2fi.end()) {
            if (de2fi.find(make_pair(vj, vi)) == de2fi.end()) {
                cout << "fail to build mesh: de2fi.find(make_pair(vj, vi)) == de2fi.end()" << endl;
                return -1;
            }
            return -1;
        }
        return it->second;
    }
}

namespace trimesh {
    /**
     * 根据三角形网格的信息输出边的集合
     * 没有准备边集信息时调用此函数
     * @param triangles         三角形网格信息
     * @param edgesOut          输出的边集
     * @return
     */
    int unorderedEdgesFromTriangles(const vector<triangle_t> &triangles, vector<edge_t> &edgesOut) {
        typedef set<pair<index_t , index_t > > edgeSet_t;       //边集，防止重复建边
        edgeSet_t edges;
        for (index_t t = 0; t < triangles.size(); ++t) {
            edges.insert(make_pair(min(triangles[t].i().index, triangles[t].j().index), max(triangles[t].i().index, triangles[t].j().index)));
            edges.insert(make_pair(min(triangles[t].j().index, triangles[t].k().index), max(triangles[t].j().index, triangles[t].k().index)));
            edges.insert(make_pair(min(triangles[t].k().index, triangles[t].i().index), max(triangles[t].k().index, triangles[t].i().index)));
        }
        edgesOut.resize(edges.size());
        int e = 0;
        for (auto it = edges.begin(); it != edges.end(); it++, e++) {
            edgesOut.at(static_cast<unsigned long long int>(e)).start() = it->first;
            edgesOut.at(static_cast<unsigned long long int>(e)).end() = it->second;
        }
        return 0;
    }
    /**
     * 建立mesh
     * @param points    点集
     * @param edges     边集
     * @param triangles 三角形网格
     * @return
     */
    int trimesh::trimesh_t::build(const vector<point_t >& points, const vector<edge_t>& edges, const vector<triangle_t>& triangles) {
        //有向边到面的映射
        directedEdge2indexMap_t de2fi;
        for (int fi = 0; fi < triangles.size(); ++fi) {
            const triangle_t& tri = triangles[fi];
            de2fi[make_pair(tri.v[0].index, tri.v[1].index)] = fi;
            de2fi[make_pair(tri.v[1].index, tri.v[2].index)] = fi;
            de2fi[make_pair(tri.v[2].index, tri.v[0].index)] = fi;
        }
        clear();
        mTriangles = triangles;
        mPoints = points;
        mVertexHalfEdges.resize(points.size(), -1);
        mFaceHalfEdges.resize(triangles.size(), -1);
        mEdgeHalfEdges.resize(edges.size(), -1);
        //由于是半边，需要申请两倍的边空间
        mHalfEdges.reserve(edges.size() * 2);

        for (int ei = 0; ei < edges.size(); ++ei) {
            const edge_t& edge = edges[ei];
            //把一条边分为两个有向的半边
            const index_t he0index = mHalfEdges.size();
            mHalfEdges.push_back(halfedge_t());
            halfedge_t& he0 = mHalfEdges.back();

            const index_t he1index = mHalfEdges.size();
            mHalfEdges.push_back(halfedge_t());
            halfedge_t& he1 = mHalfEdges.back();
            //存储面
            he0.face = directedEdge2faceIndex(de2fi, edge.v[0], edge.v[1]);
            //存储边 halfEdge（0->1）则对应的toVertex为1
            he0.toVertex = edge.v[1];
            //存储对应的边
            he0.edge = ei;

            he1.face = directedEdge2faceIndex(de2fi, edge.v[1], edge.v[0]);
            he1.toVertex = edge.v[0];
            he1.edge = ei;
            //存储对边
            he0.oppositeHe = he1index;
            he1.oppositeHe = he0index;

            if (mDirectedEdge2heIndex.find(make_pair(edge.v[0], edge.v[1])) != mDirectedEdge2heIndex.end()) {
                cout << "fail to build mesh: mDirectedEdge2heIndex.find(make_pair(edge.v[0], edge.v[1])) != mDirectedEdge2heIndex.end()" << endl;
                return -1;
            }
            if (mDirectedEdge2heIndex.find(make_pair(edge.v[1], edge.v[0])) != mDirectedEdge2heIndex.end()) {
                cout << "fail to build mesh: mDirectedEdge2heIndex.find(make_pair(edge.v[1], edge.v[0])) != mDirectedEdge2heIndex.end()" << endl;
                return -1;
            }
            //根据有向边可以寻找到对应的halfEdge
            mDirectedEdge2heIndex[make_pair(edge.v[0], edge.v[1])] = he0index;
            mDirectedEdge2heIndex[make_pair(edge.v[1], edge.v[0])] = he1index;
            //如果某条边所指向的顶点的halfEdge为空或者该halfEdge所对的面为空
            if(mVertexHalfEdges[he0.toVertex] == -1 || he1.face == -1) {
                //就把当前halfEdge的对边赋值给他
                mVertexHalfEdges[he0.toVertex] = he0.oppositeHe;
            }
            if(mVertexHalfEdges[he1.toVertex] == -1 || he0.face == -1) {
                mVertexHalfEdges[he1.toVertex] = he1.oppositeHe;
            }
            //面对halfEdge的索引
            if (he0.face != -1 && mFaceHalfEdges[he0.face] == -1) {
                mFaceHalfEdges[he0.face] = he0index;
            }
            if(he1.face != -1 && mFaceHalfEdges[he1.face] == -1) {
                mFaceHalfEdges[he1.face] = he1index;
            }
            if(mEdgeHalfEdges[ei] != -1) {
                cout << "fail to build mesh: mEdgeHalfEdges[ei] != -1" << endl;
                return -1;
            }
            mEdgeHalfEdges[ei] = he0index;
        }

        vector<index_t > boundaryHeis;
        for (index_t hei = 0; hei < mHalfEdges.size(); ++hei) {
            //标记边界
            halfedge_t& he = mHalfEdges[hei];
            if(he.face == -1) {
                boundaryHeis.push_back(hei);
                continue;
            }
            //不是边界就标记next
            const triangle_t& face = triangles[he.face];
            const index_t i = he.toVertex;
            index_t j = -1;
            if (face.v[0].index == i) j = face.v[1].index;
            else if (face.v[1].index == i) j = face.v[2].index;
            else if (face.v[2].index == i) j = face.v[0].index;
            if(j == -1) {
                cout << "fail to build mesh: Caused by next halfEdge not found exception" << endl;
                return -1;
            }
            he.nextHe = mDirectedEdge2heIndex[make_pair(i, j)];
        }
        //对于边界的he，将整个边界路径找到
        map<index_t , set<index_t > > vertex2outgoingBoundaryHei;
        for (auto hei = boundaryHeis.begin(); hei != boundaryHeis.end(); hei++) {
            //找到结点的对应边界，一个结点可能会对应多个边界he，用set存储
            const index_t originatingVertex = mHalfEdges[mHalfEdges[*hei].oppositeHe].toVertex;
            vertex2outgoingBoundaryHei[originatingVertex].insert(*hei);
        }
        //外边界逐个添加next
        for (auto hei = boundaryHeis.begin(); hei != boundaryHeis.end(); hei++) {
            halfedge_t& he = mHalfEdges[*hei];
            set<index_t >& outgoing = vertex2outgoingBoundaryHei[he.toVertex];
            if (!outgoing.empty()) {
                auto it = outgoing.begin();
                he.nextHe = *it;
                outgoing.erase(it);
            }
        }
        //添加prev
        for (auto it = mHalfEdges.begin(); it != mHalfEdges.end(); it++) {
            halfedge_t& he = *it;
            he.prev = mHalfEdges[he.oppositeHe].nextHe;
        }

        for (auto it = vertex2outgoingBoundaryHei.begin(); it != vertex2outgoingBoundaryHei.end(); it++) {
            if(!it->second.empty()) {
                cout << "fail to build mesh: Caused by border redundant" << endl;
                return -1;
            }
        }
        cout << "successfully built the mesh" << endl;
        return 0;
    }
    /**
     * 找出边界上的所有顶点
     * @return 返回边界点集
     */
     int trimesh::trimesh_t::boundaryVertices(vector<index_t>& v) const {
        set<index_t > result;
        v.clear();
        for (index_t hei = 0; hei < mHalfEdges.size(); ++hei) {
            const halfedge_t& he = mHalfEdges[hei];
            if (he.face == -1) {
                result.insert(he.toVertex);
                result.insert(mHalfEdges[he.oppositeHe].toVertex);
            }
        }
        v = vector<index_t >(result.begin(), result.end());
        return 0;
    }
    /**
     * 找出边界
     * @param result
     * @return
     */
    int trimesh::trimesh_t::boundaryEdges(vector<pair<index_t , index_t > >& result) {
        result.clear();
        index_t flag = 0, temp;
        while (mHalfEdges[flag].face != -1 && flag < mHalfEdges.size()){
            flag++;
        }
        if (flag >= mHalfEdges.size()) {
            cout << "no boundary have been found" << endl;
            return -1;
        }
        result.push_back(heIndex2directdEdge(flag));
        temp = mHalfEdges[flag].nextHe;
        while (temp != flag) {
            //找到一条边界以后就不断地next获取其余边界
            result.push_back(heIndex2directdEdge(temp));
            temp = mHalfEdges[temp].nextHe;
        }
        return 0;
    }

    const vector<point_t> &trimesh_t::getMPoints() const {
        return mPoints;
    }

    const vector<trimesh_t::halfedge_t> &trimesh_t::getMHalfEdges() const {
        return mHalfEdges;
    }

    const vector<index_t> &trimesh_t::getMVertexHalfEdges() const {
        return mVertexHalfEdges;
    }

    const vector<index_t> &trimesh_t::getMFaceHalfEdges() const {
        return mFaceHalfEdges;
    }

    const vector<index_t> &trimesh_t::getMEdgeHalfEdges() const {
        return mEdgeHalfEdges;
    }

    const vector<triangle_t> &trimesh_t::getMTriangles() const {
        return mTriangles;
    }
}

