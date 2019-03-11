/*=============================================================================*\
 *                                                                             *
 *	            	      Zhejiang University                              *
 *                        http://www.cad.zju.edu.cn/                           *
 *                                                                             *
 *-----------------------------------------------------------------------------*
 *  Created by shizeyun@zjucadcg.cn                                            *
 *                                                                             *
 \*=============================================================================*/

#ifndef SZY_IO_VTK_H
#define SZY_IO_VTK_H

//== INCLUDES ===================================================================

//STANDARD
#include<iostream>
#include<vector>
#include<algorithm>
#include<assert.h>

// #include<Eigen/Core>

#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)

#define REPORT_LOCATION                         \
  __FILE__ << " " << __LINE__ << " "

//== NAMESPACES =================================================================

  
//=== IMPLEMENTATION ============================================================

class Endianness{
 public:
  static bool is_little_endian(void) {
    unsigned char endian_test[2] = {1,0};
    short x = *(short*) endian_test;
    return (x == 1);
  }

  template<typename IDX_TYPE>
  static void swap_each_word(unsigned char * p, IDX_TYPE char_in_word) {
    assert(p);
    std::reverse(p,p + char_in_word);
  }

  template<typename VAL_TYPE, typename IDX_TYPE>
  static void swap_endian(VAL_TYPE *array, IDX_TYPE size) {
    assert(array);
    IDX_TYPE num = sizeof(VAL_TYPE);
    unsigned char *p = reinterpret_cast<unsigned char *>(array);
    for(IDX_TYPE t = 0; t < size; ++t){
      swap_each_word(p,num);
      p += num;
    }
  }
};

//-------------------------------------------------------------------------------

template<typename OS, typename FLOAT, typename INT>
void line2vtk(OS &os,
              const FLOAT *node, size_t node_num,
              const INT *line, size_t line_num)
{
  os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

  os<< "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << line_num << " " << line_num*3 << "\n";
  for(size_t i = 0; i < line_num; ++i)
    os << 2 << " " << line[i*2+0] << " " << line[i*2+1] << "\n";

  os << "CELL_TYPES " << line_num << "\n";
  for(size_t i = 0; i < line_num; ++i)
    os << 3 << "\n";
}

//-------------------------------------------------------------------------------

template<typename OS, typename FLOAT, typename INT>
void line2cylinder(OS &os,
                   const FLOAT *node, size_t node_num,
                   const INT *line, size_t line_num) {
  
  return ;
}

//-------------------------------------------------------------------------------

template <typename OS,typename FLOAT, typename INT>
void point2vtk(OS &os,
               const FLOAT *node, size_t node_num,
               const INT *points, size_t points_num)
{
  os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

  os<< "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << points_num << " " << points_num*2 << "\n";
  for(size_t i = 0; i < points_num; ++i)
    os << 1 << " " << points[i] << "\n";

  os << "CELL_TYPES " << points_num << "\n";
  for(size_t i = 0; i < points_num; ++i)
    os << 1 << "\n";
}

//-------------------------------------------------------------------------------

template <typename OS,typename FLOAT>
void point2vtk(OS &os,
               const FLOAT *node, size_t node_num)
{
  os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

  os<< "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << node_num << " " << node_num*2 << "\n";
  for(size_t i = 0; i < node_num; ++i)
    os << 1 << " " << i << "\n";

  os << "CELL_TYPES " << node_num << "\n";
  for(size_t i = 0; i < node_num; ++i)
    os << 1 << "\n";
}


//-------------------------------------------------------------------------------

template <typename OS, typename FLOAT, typename INT>
int tri2vtk(OS &os,
            const FLOAT *node, size_t node_num,
            const INT *tri, size_t tri_num) {
  assert(os);

  os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

  os << "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << tri_num << " " << tri_num*4 << "\n";
  for(size_t i = 0; i < tri_num; ++i)
    os << 3 << "  " << tri[i*3+0] << " " << tri[i*3+1] << " " << tri[i*3+2] << "\n";
  os << "CELL_TYPES " << tri_num << "\n";
  for(size_t i = 0; i < tri_num; ++i)
    os << 5 << "\n";
  return 0;
}

//-------------------------------------------------------------------------------

template <typename OS, typename FLOAT, typename INT>
void quad2vtk(
    OS &os,
    const FLOAT *node, size_t node_num,
    const INT *quad, size_t quad_num)
{
  os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";

  os<< "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << quad_num << " " << quad_num*5 << "\n";
  for(size_t i = 0; i < quad_num; ++i)
    os << 4 << "  " << quad[i*4+0] << " " << quad[i*4+1] << " " << quad[i*4+2] << " " << quad[i*4+3] << "\n";
  os << "CELL_TYPES " << quad_num << "\n";
  for(size_t i = 0; i < quad_num; ++i)
    os << 9 << "\n";
}

//-------------------------------------------------------------------------------

template <typename OS, typename FLOAT, typename INT>
void tet2vtk(
    OS &os,
    const FLOAT *node, size_t node_num,
    const INT *tet, size_t tet_num)
{
  os << "# vtk DataFile Version 2.0\nTET\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";
  os << "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << tet_num << " " << tet_num*5 << "\n";
  for(size_t i = 0; i < tet_num; ++i)
    os << 4 << "  "
       << tet[i*4+0] << " " << tet[i*4+1] << " "
       << tet[i*4+2] << " " << tet[i*4+3] << "\n";
  os << "CELL_TYPES " << tet_num << "\n";
  for(size_t i = 0; i < tet_num; ++i)
    os << 10 << "\n";
}

//-------------------------------------------------------------------------------

template <typename OS, typename FLOAT, typename INT>
void hex2vtk(
    OS &os,
    const FLOAT *node, size_t node_num,
    const INT *hex, size_t hex_num)
{
  os << "# vtk DataFile Version 2.0\nTET\nASCII\n\nDATASET UNSTRUCTURED_GRID\n";
  os << "POINTS " << node_num << " float\n";
  for(size_t i = 0; i < node_num; ++i)
    os << node[i*3+0] << " " << node[i*3+1] << " " << node[i*3+2] << "\n";

  os << "CELLS " << hex_num << " " << hex_num*9 << "\n";
  for(size_t i = 0; i < hex_num; ++i)
    os << 8 << "  "
       << hex[i*8+7] << " " << hex[i*8+5] << " "
       << hex[i*8+4] << " " << hex[i*8+6] << " "
       << hex[i*8+3] << " " << hex[i*8+1] << " "
       << hex[i*8+0] << " " << hex[i*8+2] << "\n";
  os << "CELL_TYPES " << hex_num << "\n";
  for(size_t i = 0; i < hex_num; ++i)
    os << 12 << "\n";
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void vtk_data(OS &os, Iterator first, INT size, const char *value_name, const char *table_name = "my_table")
{
  os << "SCALARS " << value_name << " float\nLOOKUP_TABLE " << table_name << "\n";
  for(size_t i = 0; i < size; ++i, ++first)
    os << *first << "\n";
}

template <typename OS, typename Iterator, typename INT>
void vtk_data_rgba(OS &os, Iterator first, INT size, char *value_name, char *table_name = "my_table")
{
  os << "COLOR_SCALARS " << value_name << " 4\n";//\nLOOKUP_TABLE " << table_name << "\n";
  for(size_t i = 0; i < size; ++i)
  {
    for(size_t j = 0; j < 4; ++j,++first)
    {
      if(j != 3)
        os << *first << " ";
      else
        os << *first;
    }
    os << "\n";
  }
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void point_data(OS &os, Iterator first, INT size, const char *value_name, const char *table_name = "my_table")
{
  os << "POINT_DATA " << size << "\n";
  vtk_data(os, first, size, value_name, table_name);
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void cell_data(OS &os, Iterator first, INT size, const char *value_name, const char *table_name = "my_table")
{
  os << "CELL_DATA " << size << "\n";
  vtk_data(os, first, size, value_name, table_name);
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void cell_data_rgba(OS &os, Iterator first, INT size, char *value_name, char *table_name = "my_table")
{
  os << "CELL_DATA " << size << "\n";
  vtk_data_rgba(os, first, size, value_name, table_name);
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void cell_data_rgba_and_scalar(OS &os, Iterator rgba_first, Iterator scalar_first, INT size,
                               char *rgba_value_name, char *scalar_value_name,
                               char *table_name = "my_table")
{
  os << "CELL_DATA " << size << "\n";
  vtk_data_rgba(os, rgba_first, size, rgba_value_name, table_name);
  vtk_data(os, scalar_first, size, scalar_value_name, table_name);

}

//-------------------------------------------------------------------------------
//// since the data is so large, binary file becomes necessary
template <typename OS, typename FLOAT, typename INT>
void tet2vtk_b(OS &os, const FLOAT *node, size_t node_num, const INT *tet, size_t tet_num)
{
  std::vector<FLOAT> node_temp(node_num * 3);
  std::copy(node,node + node_num * 3, node_temp.begin());

  os << "# vtk DataFile Version 2.0\nTET\nBINARY\n\nDATASET UNSTRUCTURED_GRID\n";

  os << "POINTS " << node_num << " "<< (sizeof(FLOAT) == sizeof(float)?"float":"double") << "\n";
  //os.write(reinterpret_cast<const char*>(&node[0]),3 * node_num * sizeof(FLOAT));

  if(Endianness::is_little_endian()) {
    Endianness::swap_endian(&(node_temp[0]),node_num * 3);
  }
  os.write((char*)&(node_temp[0]),3 * node_num * sizeof(FLOAT));

  os << "CELLS " << tet_num << " " << tet_num*5 << "\n";

  std::vector<INT> temp(5 * tet_num);

  for(size_t i = 0; i < tet_num; ++i)
  {
    temp[i * 5 + 0] = 4;
    temp[i * 5 + 1] = tet[i*4+0];
    temp[i * 5 + 2] = tet[i*4+1];
    temp[i * 5 + 3] = tet[i*4+2];
    temp[i * 5 + 4] = tet[i*4+3];
  }
  //os.write(reinterpret_cast<const char*>(&temp[0]), 5 * tet_num * sizeof(INT));
  if(Endianness::is_little_endian()) {
    Endianness::swap_endian(&(temp[0]),5 * tet_num);
  }
  os.write((char*)&(temp[0]), 5 * tet_num * sizeof(INT));
  os << "CELL_TYPES " << tet_num << "\n";

  std::vector<INT> TYPE_ARRAY(tet_num,10);
  //os.write(reinterpret_cast<const char*>(&TYPE_ARRAY[0]), tet_num * sizeof(INT));
  if(Endianness::is_little_endian()) {
    Endianness::swap_endian(&(TYPE_ARRAY[0]),tet_num);
  }
  os.write((char*)&(TYPE_ARRAY[0]), tet_num * sizeof(INT));
}

//-------------------------------------------------------------------------------

template <typename OS, typename Iterator, typename INT>
void vtk_data_b(OS &os, Iterator first, INT size, char *value_name, char *table_name = "my_table")
{
  os << "SCALARS " << value_name << (sizeof(*first) == sizeof(float)?" float":" double")<< "\n";
  os << "LOOKUP_TABLE " << table_name << "\n";
  os.write(reinterpret_cast<const char*>(first),size * sizeof(*first));
}

//===============================================================================

#endif //SZY_IO_VTK_H

//===============================================================================


