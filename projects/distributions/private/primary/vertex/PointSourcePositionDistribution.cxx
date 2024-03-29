#include "SIREN/distributions/primary/vertex/PointSourcePositionDistribution.h"

#include <array>                                                  // for array
#include <cmath>                                                  // for exp
#include <tuple>                                                  // for tie
#include <string>                                                 // for bas...
#include <vector>                                                 // for vector

#include "SIREN/interactions/CrossSection.h"            // for Cro...
#include "SIREN/interactions/InteractionCollection.h"  // for Cro...
#include "SIREN/dataclasses/InteractionRecord.h"         // for Int...
#include "SIREN/dataclasses/InteractionSignature.h"      // for Int...
#include "SIREN/dataclasses/Particle.h"                  // for Par...
#include "SIREN/detector/DetectorModel.h"                   // for Ear...
#include "SIREN/detector/Path.h"                         // for Path
#include "SIREN/detector/Coordinates.h"
#include "SIREN/distributions/Distributions.h"           // for Inj...
#include "SIREN/math/Vector3D.h"                         // for Vec...
#include "SIREN/utilities/Errors.h"                      // for Inj...
#include "SIREN/utilities/Random.h"                      // for SIREN_...

namespace siren {
namespace distributions {

using detector::DetectorPosition;
using detector::DetectorDirection;

namespace {
    double log_one_minus_exp_of_negative(double x) {
        if(x < 1e-1) {
            return std::log(x) - x/2.0 + x*x/24.0 - x*x*x*x/2880.0;
        } else if(x > 3) {
            double ex = std::exp(-x);
            double ex2 = ex * ex;
            double ex3 = ex2 * ex;
            double ex4 = ex3 * ex;
            double ex5 = ex4 * ex;
            double ex6 = ex5 * ex;
            return -(ex + ex2 / 2.0 + ex3 / 3.0 + ex4 / 4.0 + ex5 / 5.0 + ex6 / 6.0);
        } else {
            return std::log(1.0 - std::exp(-x));
        }
    }
}

//---------------
// class PointSourcePositionDistribution : public VertexPositionDistribution
//---------------

std::tuple<siren::math::Vector3D, siren::math::Vector3D> PointSourcePositionDistribution::SamplePosition(std::shared_ptr<siren::utilities::SIREN_random> rand, std::shared_ptr<siren::detector::DetectorModel const> detector_model, std::shared_ptr<siren::interactions::InteractionCollection const> interactions, siren::dataclasses::PrimaryDistributionRecord & record) const {
    siren::math::Vector3D dir(record.GetDirection());

    siren::math::Vector3D endcap_0 = origin;
    siren::math::Vector3D endcap_1 = origin + max_distance * dir;

    siren::detector::Path path(detector_model, DetectorPosition(endcap_0), DetectorDirection(dir), max_distance);
    path.ClipToOuterBounds();

    std::set<siren::dataclasses::ParticleType> const & possible_targets = interactions->TargetTypes();

    std::vector<siren::dataclasses::ParticleType> targets(possible_targets.begin(), possible_targets.end());
    std::vector<double> total_cross_sections(targets.size(), 0.0);
    siren::dataclasses::InteractionRecord fake_record;
    fake_record.signature.primary_type = record.type;
    fake_record.primary_mass = record.GetMass();
    fake_record.primary_momentum[0] = record.GetEnergy();
    double total_decay_length = interactions->TotalDecayLength(fake_record);
    for(unsigned int i=0; i<targets.size(); ++i) {
        siren::dataclasses::ParticleType const & target = targets[i];
        fake_record.signature.target_type = target;
        fake_record.target_mass = detector_model->GetTargetMass(target);
        for(auto const & cross_section : interactions->GetCrossSectionsForTarget(target)) {
            double cross_section_value = cross_section->TotalCrossSectionAllFinalStates(fake_record);
            total_cross_sections[i] += cross_section_value;
        }
    }
    double total_interaction_depth = path.GetInteractionDepthInBounds(targets, total_cross_sections, total_decay_length);
    if(total_interaction_depth == 0) {
        throw(siren::utilities::InjectionFailure("No available interactions along path!"));
    }
    double traversed_interaction_depth;
    if(total_interaction_depth < 1e-6) {
        traversed_interaction_depth = rand->Uniform() * total_interaction_depth;
    } else {
        double exp_m_total_interaction_depth = exp(-total_interaction_depth);

        double y = rand->Uniform();
        traversed_interaction_depth = -log(y * exp_m_total_interaction_depth + (1.0 - y));
    }

    double dist = path.GetDistanceFromStartAlongPath(traversed_interaction_depth, targets, total_cross_sections, total_decay_length);
    siren::math::Vector3D init_pos = path.GetFirstPoint();
    siren::math::Vector3D vertex = path.GetFirstPoint() + dist * path.GetDirection();

    return {init_pos, vertex};
}

double PointSourcePositionDistribution::GenerationProbability(std::shared_ptr<siren::detector::DetectorModel const> detector_model, std::shared_ptr<siren::interactions::InteractionCollection const> interactions, siren::dataclasses::InteractionRecord const & record) const {
    siren::math::Vector3D dir(record.primary_momentum[1], record.primary_momentum[2], record.primary_momentum[3]);
    dir.normalize();
    siren::math::Vector3D vertex(record.interaction_vertex); // m
    
    siren::math::Vector3D endcap_0 = origin;
    siren::math::Vector3D endcap_1 = origin + max_distance * dir;

    siren::detector::Path path(detector_model, DetectorPosition(endcap_0), DetectorDirection(dir), max_distance);
    path.ClipToOuterBounds();

    if(not path.IsWithinBounds(DetectorPosition(vertex)))
        return 0.0;
    

    std::set<siren::dataclasses::ParticleType> const & possible_targets = interactions->TargetTypes();

    std::vector<siren::dataclasses::ParticleType> targets(possible_targets.begin(), possible_targets.end());
    std::vector<double> total_cross_sections(targets.size(), 0.0);
    double total_decay_length = interactions->TotalDecayLength(record);
    siren::dataclasses::InteractionRecord fake_record = record;
    for(unsigned int i=0; i<targets.size(); ++i) {
        siren::dataclasses::ParticleType const & target = targets[i];
        fake_record.signature.target_type = target;
        fake_record.target_mass = detector_model->GetTargetMass(target);
        for(auto const & cross_section : interactions->GetCrossSectionsForTarget(target)) {
            total_cross_sections[i] += cross_section->TotalCrossSectionAllFinalStates(fake_record);
        }
    }
    double total_interaction_depth = path.GetInteractionDepthInBounds(targets, total_cross_sections, total_decay_length);

    path.SetPointsWithRay(path.GetFirstPoint(), path.GetDirection(), path.GetDistanceFromStartInBounds(DetectorPosition(vertex)));

    double traversed_interaction_depth = path.GetInteractionDepthInBounds(targets, total_cross_sections, total_decay_length);

    double interaction_density = detector_model->GetInteractionDensity(path.GetIntersections(), DetectorPosition(vertex), targets, total_cross_sections, total_decay_length);

    double prob_density;
    if(total_interaction_depth < 1e-6) {
        prob_density = interaction_density / total_interaction_depth;
    } else {
        prob_density = interaction_density * exp(-log_one_minus_exp_of_negative(total_interaction_depth) - traversed_interaction_depth);
    }

    return prob_density;
}

PointSourcePositionDistribution::PointSourcePositionDistribution() {}

PointSourcePositionDistribution::PointSourcePositionDistribution(siren::math::Vector3D origin, double max_distance, std::set<siren::dataclasses::ParticleType> target_types) : origin(origin), max_distance(max_distance), target_types(target_types) {}

std::string PointSourcePositionDistribution::Name() const {
    return "PointSourcePositionDistribution";
}

std::shared_ptr<PrimaryInjectionDistribution> PointSourcePositionDistribution::clone() const {
    return std::shared_ptr<PrimaryInjectionDistribution>(new PointSourcePositionDistribution(*this));
}

std::tuple<siren::math::Vector3D, siren::math::Vector3D> PointSourcePositionDistribution::InjectionBounds(std::shared_ptr<siren::detector::DetectorModel const> detector_model, std::shared_ptr<siren::interactions::InteractionCollection const> interactions, siren::dataclasses::InteractionRecord const & record) const {
    siren::math::Vector3D dir(record.primary_momentum[1], record.primary_momentum[2], record.primary_momentum[3]);
    dir.normalize();
    siren::math::Vector3D vertex(record.interaction_vertex); // m
    
    siren::math::Vector3D endcap_0 = origin;
    siren::math::Vector3D endcap_1 = origin + max_distance * dir;

    siren::detector::Path path(detector_model, DetectorPosition(endcap_0), DetectorDirection(dir), max_distance);
    path.ClipToOuterBounds();

    if(not path.IsWithinBounds(DetectorPosition(vertex)))
        return std::tuple<siren::math::Vector3D, siren::math::Vector3D>(siren::math::Vector3D(0, 0, 0), siren::math::Vector3D(0, 0, 0));
    return std::tuple<siren::math::Vector3D, siren::math::Vector3D>(path.GetFirstPoint(), path.GetLastPoint());
}

bool PointSourcePositionDistribution::equal(WeightableDistribution const & other) const {
    const PointSourcePositionDistribution* x = dynamic_cast<const PointSourcePositionDistribution*>(&other);

    if(!x)
        return false;
    else
        return (origin == x->origin
            and max_distance == x->max_distance
            and target_types == x->target_types);
}

bool PointSourcePositionDistribution::less(WeightableDistribution const & other) const {
    const PointSourcePositionDistribution* x = dynamic_cast<const PointSourcePositionDistribution*>(&other);
    return
        std::tie(origin, max_distance, target_types)
        <
        std::tie(origin, x->max_distance, x->target_types);
}

} // namespace distributions
} // namespace sirenREN
