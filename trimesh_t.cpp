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
        assert(!de2fi.empty());
        auto it = de2fi.find(make_pair(vi, vj));
        //没有对应面的he返回-1
        if(it == de2fi.end()) {
            assert(de2fi.find(make_pair(vj, vi)) != de2fi.end());
            return -1;
        }
        return it->second;
    }
}

namespace trimesh {
    void unorderedEdgesFromTriangles(size_t numTriangles, const triangle_t *triangles, vector<edge_t> &edgesOut) {
        typedef set<pair<index_t , index_t > > edgeSet_t;
        edgeSet_t edges;
        for (int t = 0; t < numTriangles; ++t) {
            edges.insert(make_pair(min(triangles[t].i(), triangles[t].j()), max(triangles[t].i(), triangles[t].j())));
            edges.insert(make_pair(min(triangles[t].j(), triangles[t].k()), max(triangles[t].j(), triangles[t].k())));
            edges.insert(make_pair(min(triangles[t].k(), triangles[t].i()), max(triangles[t].k(), triangles[t].i())));
        }
        edgesOut.resize(edges.size());
        int e = 0;
        for (auto it = edges.begin(); it != edges.end(); it++, e++) {
            edgesOut.at(static_cast<unsigned long long int>(e)).start() = it->first;
            edgesOut.at(static_cast<unsigned long long int>(e)).end() = it->second;
        }
    }
    /**
     * 建立triangle mesh
     * @param numVertices   顶点的个数
     * @param numTriangles  三角形的个数
     * @param triangles     存储三角形的数组
     * @param numEdges      边的个数
     * @param edges         存储边的数组
     */
    void trimesh::trimesh_t::build(size_t numVertices, size_t numTriangles, const triangle_t *triangles, size_t numEdges,
                                   const edge_t* edges) {
        assert(triangles);
        assert(edges);
        //有向边到面的映射
        directedEdge2indexMap_t de2fi;
        for (int fi = 0; fi < numTriangles; ++fi) {
            const triangle_t& tri = triangles[fi];
            de2fi[make_pair(tri.v[0], tri.v[1])] = fi;
            de2fi[make_pair(tri.v[1], tri.v[2])] = fi;
            de2fi[make_pair(tri.v[2], tri.v[0])] = fi;
        }
        clear();
        mVertexHalfEdges.resize(numVertices, -1);
        mFaceHalfEdges.resize(numTriangles, -1);
        mEdgeHalfEdges.resize(numEdges, -1);
        //由于是半边，需要申请两倍的边空间
        mHalfEdges.reserve(numEdges * 2);

        for (int ei = 0; ei < numEdges; ++ei) {
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

            assert(mDirectedEdge2heIndex.find(make_pair(edge.v[0], edge.v[1])) == mDirectedEdge2heIndex.end());
            assert(mDirectedEdge2heIndex.find(make_pair(edge.v[1], edge.v[0])) == mDirectedEdge2heIndex.end());
            //根据有向边可以寻找到对应的halfEdge
            mDirectedEdge2heIndex[make_pair(edge.v[0], edge.v[1])] = he0index;
            mDirectedEdge2heIndex[make_pair(edge.v[1], edge.v[0])] = he1index;
            //如果某条边所指向的顶点的halfEdge为空或者该halfEdge所对的面为空
            if(mVertexHalfEdges[he0.toVertex] == -1 || he1.face == -1) {
                //就把当前halfEdge的对边赋值给他
                mVertexHalfEdges[he0.toVertex] = he0.oppositeHe;
            }
            if(mVertexHalfEdges[he1.toVertex] == -1 || he1.face == -1) {
                mVertexHalfEdges[he1.toVertex] = he1.oppositeHe;
            }
            //面对halfEdge的索引
            if (he0.face != -1 && mFaceHalfEdges[he0.face] == -1) {
                mFaceHalfEdges[he0.face] = he0index;
            }
            if(he1.face != -1 && mFaceHalfEdges[he1.face] == -1) {
                mFaceHalfEdges[he1.face] = he1index;
            }
            assert(mEdgeHalfEdges[ei] == -1);
            mEdgeHalfEdges[ei] = he0index;
        }

        vector<index_t > boundaryHeis;
        for (int hei = 0; hei < mHalfEdges.size(); ++hei) {
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
            if (face.v[0] == i) j = face.v[1];
            else if (face.v[1] == i) j = face.v[2];
            else if (face.v[2] == i) j = face.v[0];
            assert(j != -1);
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
        #ifndef NDEBUG
            for (auto it = vertex2outgoingBoundaryHei.begin(); it != vertex2outgoingBoundaryHei.end(); it++) {
                assert(it->second.empty());
            }
        #endif
    }

    vector<index_t> trimesh::trimesh_t::boundaryVertices() const {
        set<index_t > result;
        for (int hei = 0; hei < mHalfEdges.size(); ++hei) {
            const halfedge_t& he = mHalfEdges[hei];
            if (he.face == -1) {
                result.insert(he.toVertex);
                result.insert(mHalfEdges[he.oppositeHe].toVertex);
            }
        }
        return vector<index_t >(result.begin(), result.end());
    }

    vector<pair<index_t, index_t> > trimesh::trimesh_t::boundaryEdges() const {
        vector<pair<index_t , index_t > > result;
        for (int hei = 0; hei < mHalfEdges.size(); ++hei) {
            const halfedge_t& he = mHalfEdges[hei];
            if (he.face == -1) {
                result.push_back(heIndex2directdEdge(hei));
            }
        }
        return result;
    }
}

