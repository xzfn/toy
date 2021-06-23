#include "mesh_data_helper.h"
#include "basic_shapes.h"


MeshData create_mesh_data_unit_cube()
{
    MeshData mesh_data;
    ShapeData shape_data = generate_unit_cube();
    mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
    mesh_data.set_indices(shape_data.indices);
    return mesh_data;
}

MeshData create_mesh_data_unit_quad()
{
    MeshData mesh_data;
    ShapeData shape_data = generate_unit_quad();
    mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
    mesh_data.set_indices(shape_data.indices);
    return mesh_data;
}
