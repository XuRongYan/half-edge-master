//
// Created by Administrator on 2019/3/6 0006.
//
/**
 * 基本数据结构
 */
#include <vector>

#ifndef HALFEDGE_TRAMESH_H
#define HALFEDGE_TRAMESH_H

#endif //HALFEDGE_TRAMESH_H

namespace trimesh_type {                //命名空间三角网格
    typedef long index_t;

    struct point_t {                    //顶点
        index_t index;                  //顶点的索引
        float x, y, z;                  //顶点的坐标
    };

    struct edge_t {                     //三角网格中的边
        index_t v[2];                   //存储两个顶点
        index_t& start() {
            return v[0];
        }

        index_t& end() {
            return v[1];
        }

        edge_t() {
            v[0] = v[1] = -1;           //初始化两个顶点的索引
        }
    };

    struct triangle_t {                 //三角形
        point_t v[3];                   //三角形三个顶点
        const point_t& i() const {
            return v[0];
        }
        const point_t& j() const {
            return v[1];
        }
        const point_t& k() const {
            return v[2];
        }
    };
}