#define _USE_MATH_DEFINES
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>
#include <iostream>

int main()
{
    std::vector<double> vals{1, 2, 3, 4};

    Eigen::Tensor<double, 3, Eigen::RowMajor> rmj(1, vals.size(), 1);
    rmj.setValues({{{1, 2, 3, 4}}});

    Eigen::Tensor<double, 3, Eigen::RowMajor> rma(vals.size(), 1, 1);
    rma.setValues({{{2, 3, 1, 1}}});

    Eigen::array<Eigen::Index, 3> dims = rmj.dimensions();
    Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
    Eigen::array<Eigen::Index, 3> extents = {1, dims[1], 1};

    rmj(0, 0, 0) = 42;
    //Eigen::Tensor<double, 3> foo = rmj.slice(offsets, extents);
    Eigen::array<Eigen::Index, 1> onedim{{dims[1]}};

    //Eigen::Tensor<double, 3> foo = rmj * rma;
    Eigen::array<int, 3> bcast({2, 1, 1});
    std::cout << rmj.broadcast(bcast);
    return 0;
}
