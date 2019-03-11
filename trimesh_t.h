//
// Created by Administrator on 2019/3/6 0006.
//
#ifndef HALFEDGE_TRIMESH_H
#define HALFEDGE_TRIMESH_H
#include "tramesh_types.h"                                  //提供triangle和edge
#include <utility>
#include <vector>
#include <map>
#include <cassert>

using namespace trimesh_type;
using namespace std;
namespace trimesh {
    /**
     * build()方法需要有edges，如果没有edges的时候就调用这个方法
     * @param numTriangles
     * @param triangles
     * @param edgesOut
     */
    void unorderedEdgesFromTriangles(size_t numTriangles, const triangle_t* triangles, vector<edge_t>& edgesOut);
class trimesh_t {
public:
    typedef long index_t;
    struct halfedge_t {
        index_t toVertex;
        index_t face;
        index_t edge;
        index_t prev;
        index_t oppositeHe;
        index_t nextHe;
        halfedge_t() :toVertex(-1),
                    face(-1),
                    edge(-1),
                    oppositeHe(-1),
                    nextHe(-1) {}
    };
    /**
    * 根据给定的三角面和边构建HalfEdge数据结构
    * Note:通过调用上方的unorderedEdgesFromTriangles()，可以使triangles产生edges，如果调用者没有edge的话可采用这个方法
    * Note：triangles和edges在调用build()以后就没有用了，应该被销毁
    * @param numVertices
    * @param numTriangles
    * @param triangles
    * @param numEdges
    * @param edges
    */
    void build(const vector<point_t >& points, const vector<edge_t>& edges, const vector<triangle_t>& triangles);

    /**
     * 清空
     */
    void clear() {
        mHalfEdges.clear();
        mVertexHalfEdges.clear();
        mFaceHalfEdges.clear();
        mEdgeHalfEdges.clear();
        mDirectedEdge2heIndex.clear();
    }

    const halfedge_t& halfedge(const index_t i) const {
        return mHalfEdges[i];
    }

    /**
     * 给定一个halfEdge的索引，返回相应的有向边
     * @param heIndex
     * @return
     */
    pair<index_t , index_t> heIndex2directdEdge(const index_t heIndex) const {
        const halfedge_t& he = mHalfEdges[heIndex];
        return make_pair(mHalfEdges[he.oppositeHe].toVertex, he.toVertex);
    }

    index_t directedEdge2heIndex(const index_t i, const index_t j) const  {
        auto result = mDirectedEdge2heIndex.find((make_pair(i, j)));
        return result == mDirectedEdge2heIndex.end() ? -1 : result->second;
    }

    /**
     * 在result中返回vertexIndex的邻居
     * @param vertexIndex
     * @param result
     */
    void vvNeighbors(const index_t vertexIndex, vector<index_t >& result) const {
        result.clear();
        const index_t startHei = mVertexHalfEdges[vertexIndex];
        index_t hei = startHei;
        while (true) {
            const halfedge_t& he = mHalfEdges[hei];
            result.push_back(he.toVertex);
            hei = he.prev;
            if(hei == startHei) break;
        }
    }

    vector<index_t > vvNeighbors(const index_t vertexIndex) const {
        vector<index_t > result;
        vvNeighbors(vertexIndex, result);
        return result;
    }

    /**
     * @param vertexIndex
     * @return 返回vertexIndex的邻居数量
     */
    int vertexValence(const index_t vertexIndex) const {
        return static_cast<int>(vvNeighbors(vertexIndex).size());
    }

    /**
     *
     * @param vertexIndex
     * @param 在result中以索引形式返回面邻居
     */
    void vfNeighbors(const index_t vertexIndex, vector<index_t >& result) const {
        result.clear();
        const index_t startHei = mVertexHalfEdges[vertexIndex];
        index_t hei = startHei;
        while (true) {
            const halfedge_t& he = mHalfEdges[hei];
            if(he.face != -1) result.push_back(he.face);
            hei = he.prev;
            if (hei == startHei) break;
        }
    }

    vector<index_t > vfNeighbors(const index_t vertexIndex) const {
        vector<index_t > result;
        vfNeighbors(vertexIndex, result);
        return result;
    }

    void efNeighbors(const index_t vi, const index_t vj, vector<index_t >& result) {
        index_t v = -1;
        if(mDirectedEdge2heIndex.find(make_pair(vi, vj)) != mDirectedEdge2heIndex.end()) {
            v = mHalfEdges[mHalfEdges[mDirectedEdge2heIndex[make_pair(vi, vj)]].oppositeHe].toVertex;
        } else if (mDirectedEdge2heIndex.find(make_pair(vj, vi)) != mDirectedEdge2heIndex.end()) {
            v = mHalfEdges[mHalfEdges[mDirectedEdge2heIndex[make_pair(vj, vi)]].oppositeHe].toVertex;
        }
        assert(v != -1);
        result.clear();
        vfNeighbors(v, result);
    }

    vector<index_t > efNeighbors(const index_t vi, const index_t vj) {
        vector<index_t > result;
        efNeighbors(vi, vj, result);
        return result;
    }

     int boundaryVertices(vector<index_t >& v) const;

    int boundaryEdges(vector<pair<index_t , index_t > >& result);

private:
    vector<point_t> mPoints;
    vector<halfedge_t> mHalfEdges;
    vector<index_t > mVertexHalfEdges;
    vector<index_t > mFaceHalfEdges;
    vector<index_t > mEdgeHalfEdges;
    typedef map<pair<index_t, index_t>, index_t > directedEdge2indexMap_t;
    directedEdge2indexMap_t mDirectedEdge2heIndex;
};
}




#endif //HALFEDGE_TRIMESH_H
