#include <set>
#include <memory>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "../../public/SIREN/detector/DetectorModel.h"
#include "../../public/SIREN/detector/Axis1D.h"
#include "../../../geometry/public/SIREN/geometry/Geometry.h"

using namespace pybind11;
using namespace siren::detector;
class PyAxis1D : public siren::detector::Axis1D {
public:
    using Axis1D::Axis1D;

    bool compare(const Axis1D& density_distr) const override {
        PYBIND11_OVERRIDE_PURE_NAME(
            bool,
            Axis1D,
            "_compare",
            compare,
            density_distr
        );
    }

    Axis1D * clone() const override {
        PYBIND11_OVERRIDE_PURE_NAME(
            Axis1D *,
            Axis1D,
            "_clone",
            clone
        );
    }
    std::shared_ptr<Axis1D> create() const override {
        PYBIND11_OVERRIDE_PURE_NAME(
            std::shared_ptr<Axis1D>,
            Axis1D,
            "_create",
            create
        );
    }

    double GetX(const siren::math::Vector3D& xi) const override {
        PYBIND11_OVERRIDE_PURE(
            double,
            Axis1D,
            GetX,
            xi
        );
    }

    double GetdX(const siren::math::Vector3D& xi, const siren::math::Vector3D& direction) const override {
        PYBIND11_OVERRIDE_PURE(
            double,
            Axis1D,
            GetdX,
            xi,
            direction
        );
    }
};

void register_Axis1D(pybind11::module_ & m) {
    using namespace pybind11;
    using namespace siren::detector;

    class_<Axis1D, std::shared_ptr<Axis1D>, PyAxis1D>(m, "Axis1D")
        .def(init<>())
        .def("__eq__", [](const Axis1D &self, const Axis1D &other){ return self == other; })
        .def("__ne__", [](const Axis1D &self, const Axis1D &other){ return self != other; }) 
        .def("_clone", &Axis1D::clone)
        .def("_create", &Axis1D::create)
        .def("GetX", &Axis1D::GetX)
        .def("GetdX", &Axis1D::GetdX)
        .def("GetAxis", &Axis1D::GetAxis)
        .def("GetFp0", &Axis1D::GetFp0)
        ;
}
