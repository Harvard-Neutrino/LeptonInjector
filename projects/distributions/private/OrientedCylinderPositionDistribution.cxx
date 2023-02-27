#include "LeptonInjector/detector/Path.h"
#include "LeptonInjector/math/Vector3D.h"
#include "LeptonInjector/detector/EarthModel.h"
#include "LeptonInjector/detector/EarthModelCalculator.h"

#include "LeptonInjector/crosssections/CrossSection.h"

#include "LeptonInjector/utilities/Random.h"
#include "LeptonInjector/utilities/Particle.h"

#include "LeptonInjector/distributions/Distributions.h"
#include "LeptonInjector/distributions/OrientedCylinderPositionDistribution.h"

#include "LeptonInjector/utilities/Errors.h"

namespace LI {
namespace distributions {

//---------------
// class OrientedCylinderPositionDistribution : VertexPositionDistribution
//---------------
//
LI::math::Vector3D OrientedCylinderPositionDistribution::SampleFromDisk(std::shared_ptr<LI::utilities::LI_random> rand, LI::math::Vector3D const & dir) const {
    double t = rand->Uniform(0, 2 * M_PI);
    double r = radius * std::sqrt(rand->Uniform());
    LI::math::Vector3D pos(r * cos(t), r * sin(t), 0.0);
    LI::math::Quaternion q = rotation_between(LI::math::Vector3D(0,0,1), dir);
    return q.rotate(pos, false);
}

LI::math::Vector3D OrientedCylinderPositionDistribution::SamplePosition(std::shared_ptr<LI::utilities::LI_random> rand, std::shared_ptr<LI::detector::EarthModel const> earth_model, std::shared_ptr<LI::crosssections::CrossSectionCollection const> cross_sections, LI::crosssections::InteractionRecord & record) const {
    LI::math::Vector3D dir(record.primary_momentum[1], record.primary_momentum[2], record.primary_momentum[3]);
    dir.normalize();
    LI::math::Vector3D pca = SampleFromDisk(rand, dir);

    /*
    std::pair<LI::math::Vector3D, LI::math::Vector3D> GetBounds(earth_model, cross_sections, pca);

    LI::math::Vector3D p0;
    LI::math::Vector3D p1;

    LI::detector::Path path(earth_model, earth_model->GetEarthCoordPosFromDetCoordPos(endcap_0), earth_model->GetEarthCoordDirFromDetCoordDir(dir), endcap_length*2);
    path.ExtendFromStartByColumnDepth(lepton_depth);
    path.ClipToOuterBounds();
    */
    return LI::math::Vector3D();
}

double OrientedCylinderPositionDistribution::GenerationProbability(std::shared_ptr<LI::detector::EarthModel const> earth_model, std::shared_ptr<LI::crosssections::CrossSectionCollection const> cross_sections, LI::crosssections::InteractionRecord const & record) const {
    return 0.0;
}

std::pair<LI::math::Vector3D, LI::math::Vector3D> OrientedCylinderPositionDistribution::InjectionBounds(std::shared_ptr<LI::detector::EarthModel const> earth_model, std::shared_ptr<LI::crosssections::CrossSectionCollection const> cross_sections, LI::crosssections::InteractionRecord const & interaction) const {
    return std::make_pair(LI::math::Vector3D(), LI::math::Vector3D());
}

bool OrientedCylinderPositionDistribution::AreEquivalent(std::shared_ptr<LI::detector::EarthModel const> earth_model, std::shared_ptr<LI::crosssections::CrossSectionCollection const> cross_sections, std::shared_ptr<WeightableDistribution const> distribution, std::shared_ptr<LI::detector::EarthModel const> second_earth_model, std::shared_ptr<LI::crosssections::CrossSectionCollection const> second_cross_sections) const {
    return false;
}

} // namespace distributions
} // namespace LeptonInjector
