#include <set>
#include <memory>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>

#include "../../public/SIREN/interactions/DarkNewsDecay.h"
#include "../../../dataclasses/public/SIREN/dataclasses/Particle.h"
#include "../../../dataclasses/public/SIREN/dataclasses/InteractionRecord.h"
#include "../../../dataclasses/public/SIREN/dataclasses/InteractionSignature.h"
#include "../../../geometry/public/SIREN/geometry/Geometry.h"
#include "../../../utilities/public/SIREN/utilities/Random.h"
#include "../../../utilities/public/SIREN/utilities/Pybind11Trampoline.h"

namespace siren {
namespace interactions {
// Trampoline class for Decay
class pyDecay : public Decay {
public:
    using Decay::Decay;
    pyDecay(Decay && parent) : Decay(std::move(parent)) {}
    pybind11::object self;

    double TotalDecayLength(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE(
            self,
            Decay,
            double,
            TotalDecayLength,
            "TotalDecayLength",
            interaction
        )
    }

    double TotalDecayLengthForFinalState(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE(
            self,
            Decay,
            double,
            TotalDecayLengthForFinalState,
            "TotalDecayLengthForFinalState",
            interaction
        )
    }
    
    double TotalDecayWidth(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            double,
            TotalDecayWidth,
            "TotalDecayWidth",
            interaction
        )
    }

    double TotalDecayWidthForFinalState(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            double,
            TotalDecayWidthForFinalState,
            "TotalDecayWidthForFinalState",
            interaction
        )
    }

    double TotalDecayWidth(siren::dataclasses::ParticleType primary) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            double,
            TotalDecayWidth,
            "TotalDecayWidth",
            primary
        )
    }

    double DifferentialDecayWidth(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            double,
            DifferentialDecayWidth,
            "DifferentialDecayWidth",
            interaction
        )
    }

    void SampleFinalState(dataclasses::CrossSectionDistributionRecord & record, std::shared_ptr<siren::utilities::SIREN_random> random) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            void,
            SampleFinalState,
            "SampleFinalState",
            record,
            random
        )
    }

    std::vector<siren::dataclasses::InteractionSignature> GetPossibleSignatures() const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            std::vector<siren::dataclasses::InteractionSignature>,
            GetPossibleSignatures,
            "GetPossibleSignatures"
        )
    }

    std::vector<siren::dataclasses::InteractionSignature> GetPossibleSignaturesFromParent(siren::dataclasses::ParticleType primary_type) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            std::vector<siren::dataclasses::InteractionSignature>,
            GetPossibleSignaturesFromParents,
            "GetPossibleSignaturesFromParents",
            primary_type
        )
    }

    std::vector<std::string> DensityVariables() const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            std::vector<std::string>,
            DensityVariables,
            "DensityVariables"
        )
    }

    double FinalStateProbability(dataclasses::InteractionRecord const & record) const override {
        SELF_OVERRIDE_PURE(
            self,
            Decay,
            double,
            FinalStateProbability,
            "FinalStateProbability",
            record
        )
    }

    pybind11::object get_representation() {
        return self;
    }
};
// Trampoline class for DarkNewsDecay
class pyDarkNewsDecay : public DarkNewsDecay {
public:
    using DarkNewsDecay::DarkNewsDecay;
    pyDarkNewsDecay(DarkNewsDecay && parent) : DarkNewsDecay(std::move(parent)) {}
    pybind11::object self;

    double TotalDecayWidth(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            double,
            TotalDecayWidth,
            "TotalDecayWidth",
            std::cref(interaction)
        )
    }

    double TotalDecayWidthForFinalState(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            double,
            TotalDecayWidthForFinalState,
            "TotalDecayWidthForFinalState",
            std::cref(interaction)
        )
    }

    double TotalDecayWidth(siren::dataclasses::ParticleType primary) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            double,
            TotalDecayWidth,
            "TotalDecayWidth",
            primary
        )
    }

    double DifferentialDecayWidth(dataclasses::InteractionRecord const & interaction) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            double,
            DifferentialDecayWidth,
            "DifferentialDecayWidth",
            std::cref(interaction)
        )
    }

    void SampleRecordFromDarkNews(dataclasses::CrossSectionDistributionRecord & record, std::shared_ptr<siren::utilities::SIREN_random> random) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            void,
            SampleRecordFromDarkNews,
            "SampleRecordFromDarkNews",
            std::ref(record),
            random
        )
    }

    void SampleFinalState(dataclasses::CrossSectionDistributionRecord & record, std::shared_ptr<siren::utilities::SIREN_random> random) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            void,
            SampleFinalState,
            "SampleFinalState",
            std::ref(record),
            random
        )
    }

    std::vector<siren::dataclasses::InteractionSignature> GetPossibleSignatures() const override {
        SELF_OVERRIDE_PURE(
            self,
            DarkNewsDecay,
            std::vector<siren::dataclasses::InteractionSignature>,
            GetPossibleSignatures,
            "GetPossibleSignatures"
        )
    }

    std::vector<siren::dataclasses::InteractionSignature> GetPossibleSignaturesFromParent(siren::dataclasses::ParticleType primary_type) const override {
        SELF_OVERRIDE_PURE(
            self,
            DarkNewsDecay,
            std::vector<siren::dataclasses::InteractionSignature>,
            GetPossibleSignaturesFromParent,
            "GetPossibleSignaturesFromParent",
            primary_type
        )
    }

    std::vector<std::string> DensityVariables() const override {
        SELF_OVERRIDE_PURE(
            self,
            DarkNewsDecay,
            std::vector<std::string>,
            DensityVariables,
            "DensityVariables"
        )
    }

    double FinalStateProbability(dataclasses::InteractionRecord const & record) const override {
        SELF_OVERRIDE(
            self,
            DarkNewsDecay,
            double,
            FinalStateProbability,
            "FinalStateProbability",
            std::cref(record)
        )
    }

    pybind11::object get_representation() override {
        const DarkNewsDecay * ref;
        if(self) {
            ref = self.cast<DarkNewsDecay *>();
        } else {
            ref = this;
        }
        auto *tinfo = pybind11::detail::get_type_info(typeid(DarkNewsDecay));
        pybind11::function override_func =
            tinfo ? pybind11::detail::get_type_override(static_cast<const DarkNewsDecay *>(ref), tinfo, "get_representation") : pybind11::function();
        if (override_func) {
            pybind11::object o = override_func();
            if(not pybind11::isinstance<pybind11::dict>(o)) {
                throw std::runtime_error("get_representation must return a dict");
            }
            return o;
        }

        pybind11::object _self;
        if(this->self) {
            self = pybind11::reinterpret_borrow<pybind11::object>(this->self);
        } else {
            auto *tinfo = pybind11::detail::get_type_info(typeid(DarkNewsDecay));
            pybind11::handle self_handle = get_object_handle(static_cast<const DarkNewsDecay *>(this), tinfo);
            _self = pybind11::reinterpret_borrow<pybind11::object>(self_handle);
        }
        pybind11::dict d;
        if (pybind11::hasattr(self, "__dict__")) {
            d = _self.attr("__dict__");
        }
        return d;
    }
};
} // end interactions namespace
} // end LI namespace

void register_DarkNewsDecay(pybind11::module_ & m) {
    using namespace pybind11;
    using namespace siren::interactions;

    // Bindings for pyDarkNewsDecay
    class_<siren::interactions::pyDarkNewsDecay> pyDarkNewsDecay(m, "pyDarkNewsDecay");

    pyDarkNewsDecay
        .def(init<>())
        .def("__eq__", [](const siren::interactions::DarkNewsDecay &self, const siren::interactions::DarkNewsDecay &other){ return self == other; })
        .def("equal", &siren::interactions::DarkNewsDecay::equal)
        .def("TotalDecayWidth",overload_cast<siren::dataclasses::InteractionRecord const &>(&DarkNewsDecay::TotalDecayWidth, const_))
        .def("TotalDecayWidth",overload_cast<siren::dataclasses::ParticleType>(&DarkNewsDecay::TotalDecayWidth, const_))
        .def("TotalDecayWidthForFinalState",&DarkNewsDecay::TotalDecayWidthForFinalState)
        .def("DifferentialDecayWidth",&DarkNewsDecay::DifferentialDecayWidth)
        .def("GetPossibleSignatures",&DarkNewsDecay::GetPossibleSignatures)
        .def("GetPossibleSignaturesFromParent",&DarkNewsDecay::GetPossibleSignaturesFromParent)
        .def("DensityVariables",&DarkNewsDecay::DensityVariables)
        .def("FinalStateProbability",&DarkNewsDecay::FinalStateProbability)
        .def("SampleFinalState",&DarkNewsDecay::SampleFinalState)
        .def("SampleRecordFromDarkNews",&DarkNewsDecay::SampleRecordFromDarkNews)
        .def("get_representation", &pyDarkNewsDecay::get_representation)
        .def(pybind11::pickle(
            [](siren::interactions::pyDarkNewsDecay & cpp_obj) {
                return pybind11::make_tuple(cpp_obj.get_representation());
            },
            [](const pybind11::tuple &t) {
                if (t.size() != 1) {
                    throw std::runtime_error("Invalid state!");
                }
                auto cpp_state = std::unique_ptr<siren::interactions::pyDarkNewsDecay>(new siren::interactions::pyDarkNewsDecay);
                auto py_state = t[0].cast<pybind11::dict>();
                return std::make_pair(std::move(cpp_state), py_state);
            })
        )
        ;


    class_<DarkNewsDecay, std::shared_ptr<DarkNewsDecay>, Decay, siren::interactions::pyDarkNewsDecay> DarkNewsDecay(m, "DarkNewsDecay");

    DarkNewsDecay
        .def(init<>())
        .def("__eq__", [](const siren::interactions::DarkNewsDecay &self, const siren::interactions::DarkNewsDecay &other){ return self == other; })
        .def("equal", &siren::interactions::DarkNewsDecay::equal)
        .def("TotalDecayWidth",overload_cast<siren::dataclasses::InteractionRecord const &>(&DarkNewsDecay::TotalDecayWidth, const_))
        .def("TotalDecayWidth",overload_cast<siren::dataclasses::ParticleType>(&DarkNewsDecay::TotalDecayWidth, const_))
        .def("TotalDecayWidthForFinalState",&DarkNewsDecay::TotalDecayWidthForFinalState)
        .def("DifferentialDecayWidth",&DarkNewsDecay::DifferentialDecayWidth)
        .def("GetPossibleSignatures",&DarkNewsDecay::GetPossibleSignatures)
        .def("GetPossibleSignaturesFromParent",&DarkNewsDecay::GetPossibleSignaturesFromParent)
        .def("DensityVariables",&DarkNewsDecay::DensityVariables)
        .def("FinalStateProbability",&DarkNewsDecay::FinalStateProbability)
        .def("SampleFinalState",&DarkNewsDecay::SampleFinalState)
        .def("SampleRecordFromDarkNews",&DarkNewsDecay::SampleRecordFromDarkNews)
        .def("get_representation", &DarkNewsDecay::get_representation)
        .def(pybind11::pickle(
            [](siren::interactions::DarkNewsDecay & cpp_obj) {
                return pybind11::make_tuple(cpp_obj.get_representation());
            },
            [](const pybind11::tuple &t) {
                if (t.size() != 1) {
                    throw std::runtime_error("Invalid state!");
                }
                auto cpp_state = std::unique_ptr<siren::interactions::DarkNewsDecay>(new siren::interactions::pyDarkNewsDecay);
                auto py_state = t[0].cast<pybind11::dict>();
                return std::make_pair(std::move(cpp_state), py_state);
            })
        )
        ;
}
