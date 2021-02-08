#include <igl/marching_cubes.h>
#include <igl/sparse_voxel_grid.h>
#include <igl/opengl/glfw/Viewer.h>

#include <Eigen/Core>
#include <iostream>

#include "tutorial_shared_path.h"

int main(int argc, char * argv[])
{
  // An implicit function which is zero on the surface of a sphere centered at the origin with radius 1
  // This function is negative inside the surface and positive outside the surface
  std::function<double(const Eigen::RowVector3d&)> scalar_func = 
    [](const Eigen::RowVector3d& x) -> double {
    const double R = x.norm();
    const double s = atan2(x.head(2).norm(),x(2));
    const double p = atan2(x(1),x(0));
    return pow(sin(s),2.)*(pow(cos(12.*s),3.)*0.1+pow(sin(6.*p),2)*0.2)+(R-0.5);
    //return R-0.5;
  };

  // We know that the point (0, 0, 0.5) lies on the implicit surface
  Eigen::RowVector3d p0(0., 0., 0.5);

  // Construct a sparse voxel grid whose cubes have edge length eps
  // The cubes will form a thin shell around the implicit surface
  const double eps = 0.01;

  // CS will hold one scalar value at each cube vertex corresponding
  // the value of the implicit at that vertex
  Eigen::VectorXd CS;
  // CV will hold the positions of the corners of the sparse voxel grid
  Eigen::MatrixXd CV;
  // CI is a #cubes x 8 matrix of indices where each row contains the
  // indices into CV of the 8 corners of a cube
  Eigen::MatrixXi CI;
  // Construct the voxel grid, populating CS, CV, and CI
  igl::sparse_voxel_grid(p0, scalar_func, eps, 200000/*expected_number_of_cubes*/, CS, CV, CI);

  // Given the sparse voxel grid, use Marching Cubes to construct a triangle mesh of the surface
  Eigen::MatrixXi F;
  Eigen::MatrixXd V;
  igl::marching_cubes(CS, CV, CI, 0, V, F);

  // Draw the meshed implicit surface
  igl::opengl::glfw::Viewer viewer;
  viewer.data().clear();
  viewer.data().set_mesh(V,F);
  viewer.data().set_face_based(true);
  viewer.launch();
}
