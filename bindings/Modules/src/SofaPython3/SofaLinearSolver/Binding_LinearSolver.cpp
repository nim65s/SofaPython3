/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2021 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <SofaPython3/SofaLinearSolver/Binding_LinearSolver.h>
#include <SofaPython3/SofaLinearSolver/Binding_LinearSolver_doc.h>
#include <pybind11/pybind11.h>

#include <SofaPython3/Sofa/Core/Binding_Base.h>
#include <SofaPython3/PythonFactory.h>

#include <sofa/component/linearsolver/iterative/MatrixLinearSolver.h>
#include <pybind11/eigen.h>

namespace py { using namespace pybind11; }

namespace sofapython3 {

using CRS = sofa::linearalgebra::CompressedRowSparseMatrix<SReal>;
using CRSLinearSolver = sofa::component::linearsolver::MatrixLinearSolver<CRS, sofa::linearalgebra::FullVector<SReal> >;
using EigenSparseMatrix = Eigen::SparseMatrix<SReal, Eigen::RowMajor>;
using EigenMatrixMap = Eigen::Map<Eigen::SparseMatrix<SReal, Eigen::RowMajor> >;
using Vector = Eigen::Matrix<SReal,Eigen::Dynamic, 1>;
using EigenVectorMap = Eigen::Map<Vector>;

void moduleAddLinearSolver(py::module &m)
{
    const std::string type_name = CRSLinearSolver::GetClass()->className;

    py::class_<CRSLinearSolver,
               sofa::core::objectmodel::BaseObject,
               sofapython3::py_shared_ptr<CRSLinearSolver> > c(m, type_name.c_str(), sofapython3::doc::linearsolver::linearSolverClass);

    c.def("A", [](CRSLinearSolver& self) -> EigenSparseMatrix
    {
        if (CRS* matrix = self.getSystemMatrix())
        {
            return EigenMatrixMap(matrix->rows(), matrix->cols(), matrix->getColsValue().size(),
                    (EigenMatrixMap::StorageIndex*)matrix->rowBegin.data(), (EigenMatrixMap::StorageIndex*)matrix->colsIndex.data(), matrix->colsValue.data());
        }
        return {};
    }, sofapython3::doc::linearsolver::linearSolver_A);

    c.def("b", [](CRSLinearSolver& self) -> Vector
    {
        if (auto* vector = self.getSystemRHVector())
        {
            return EigenVectorMap(vector->ptr(), vector->size());
        }
        return {};
    }, sofapython3::doc::linearsolver::linearSolver_b);

    c.def("x", [](CRSLinearSolver& self) -> Vector
    {
        if (auto* vector = self.getSystemLHVector())
        {
            return EigenVectorMap(vector->ptr(), vector->size());
        }
        return {};
    }, sofapython3::doc::linearsolver::linearSolver_x);


    /// register the binding in the downcasting subsystem
    PythonFactory::registerType<CRSLinearSolver>([](sofa::core::objectmodel::Base* object)
    {
        return py::cast(dynamic_cast<CRSLinearSolver*>(object));
    });
}

}
