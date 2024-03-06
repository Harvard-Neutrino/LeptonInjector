#include "LeptonInjector/interactions/DipoleDIS.h"

#include <map>                                             // for map, opera...
#include <set>                                             // for set, opera...
#include <array>                                           // for array
#include <cmath>                                           // for pow, log10
#include <tuple>                                           // for tie, opera...
#include <memory>                                          // for allocator
#include <string>                                          // for basic_string
#include <vector>                                          // for vector
#include <assert.h>                                        // for assert
#include <stddef.h>                                        // for size_t

#include <rk/rk.hh>                                        // for P4, Boost
#include <rk/geom3.hh>                                     // for Vector3

#include "LeptonInjector/interactions/CrossSection.h"     // for CrossSection
#include "LeptonInjector/dataclasses/InteractionRecord.h"  // for Interactio...
#include "LeptonInjector/dataclasses/Particle.h"           // for Particle
#include "LeptonInjector/utilities/Random.h"               // for LI_random

namespace LI {
namespace interactions {

namespace {
///Check whether a given point in phase space is physically realizable.
///Based on equations 5 of https://arxiv.org/pdf/1707.08573.pdf
///\param x Bjorken x of the interaction
///\param y Bjorken y of the interaction
///\param E Incoming neutrino in energy in the lab frame ($E_\nu$)
///\param M Mass of the target nucleon ($M_N$)
///\param m Mass of the secondary lepton ($m_HNL$)
bool kinematicallyAllowed(double x, double y, double E, double M, double m) {
    double Q2 = 2*M*E*x*y;
    double W2 = M*M + Q2/x * (1-x);
    double Er = E*y;
    double term = M*M - W2 - 2*x*E*M - x*x*M*M + 2*Er(x*M + E,2);
    return Er*Er - W2 - term*term/(4*E*E) > 0; // equation 5
}
}

DipoleDIS::DipoleDIS() {}

DipoleDIS::DipoleDIS(std::string pdfset_name, int pdfset_mem, double target_mass, double hnl_mass, double minumum_Q2, std::vector<LI::dataclasses::Particle::ParticleType> primary_types, std::vector<LI::dataclasses::Particle::ParticleType> target_types) : primary_types_(primary_types), target_types_(target_types), target_mass_(target_mass), hnl_mass_(hnl_mass), minimum_Q2_(minimum_Q2) {
    InitializeSignatures();
}

bool DipoleDIS::equal(CrossSection const & other) const {
    const DipoleDIS* x = dynamic_cast<const DipoleDIS*>(&other);

    if(!x)
        return false;
    else
        return
            std::tie(
            target_mass_,
            hnl_mass_,
            minimum_Q2_,
            signatures_,
            primary_types_,
            target_types_
            *pdf_,
            pids_)
            ==
            std::tie(
            x->target_mass_,
            x->hnl_mass_,
            x->minimum_Q2_,
            x->signatures_,
            x->primary_types_,
            x->target_types_
            *(x->pdf_),
            x->pids_);
}

void DipoleDIS::InitializeSignatures() {
    signatures_.clear();
    for(auto primary_type : primary_types_) {
        dataclasses::InteractionSignature signature;
        signature.primary_type = primary_type;

        if(not isNeutrino(primary_type)) {
            throw std::runtime_error("This DIS implementation only supports neutrinos as primaries!");
        }

        LI::dataclasses::Particle::ParticleType neutral_lepton_product = LI::dataclasses::Particle::ParticleType::unknown;

        if(int(primary_type) > 0) {
            neutral_lepton_product = LI::dataclasses::Particle::ParticleType::NuF4;
        } 
        else {
            neutral_lepton_product = LI::dataclasses::Particle::ParticleType::NuF4Bar;
        }
        signature.secondary_types.push_back(neutral_lepton_product);

        signature.secondary_types.push_back(LI::dataclasses::Particle::ParticleType::Hadrons);
        for(auto target_type : target_types_) {
            signature.target_type = target_type;

            signatures_.push_back(signature);

            std::pair<LI::dataclasses::Particle::ParticleType, LI::dataclasses::Particle::ParticleType> key(primary_type, target_type);
            signatures_by_parent_types_[key].push_back(signature);
        }
    }
}

double DipoleDIS::TotalCrossSection(dataclasses::InteractionRecord const & interaction) const {
    LI::dataclasses::Particle::ParticleType primary_type = interaction.signature.primary_type;
    rk::P4 p1(geom3::Vector3(interaction.primary_momentum[1], interaction.primary_momentum[2], interaction.primary_momentum[3]), interaction.primary_mass);
    rk::P4 p2(geom3::Vector3(interaction.target_momentum[1], interaction.target_momentum[2], interaction.target_momentum[3]), interaction.target_mass);
    double primary_energy;
    if(interaction.target_momentum[1] == 0 and interaction.target_momentum[2] == 0 and interaction.target_momentum[3] == 0) {
        primary_energy = interaction.primary_momentum[0];
    } else {
        rk::Boost boost_start_to_lab = p2.restBoost();
        rk::P4 p1_lab = boost_start_to_lab * p1;
        primary_energy = p1_lab.e();
    }
    // if we are below threshold, return 0
    if(primary_energy < InteractionThreshold(interaction))
        return 0;
    return TotalCrossSection(primary_type, primary_energy);
}

double DipoleDIS::TotalCrossSection(LI::dataclasses::Particle::ParticleType primary_type, double primary_energy) const {
    if(not primary_types_.count(primary_type)) {
        throw std::runtime_error("Supplied primary not supported by cross section!");
    }
    double log_energy = log10(primary_energy);

    if(log_energy < total_cross_section_.lower_extent(0)
            or log_energy > total_cross_section_.upper_extent(0)) {
        throw std::runtime_error("Interaction energy ("+ std::to_string(primary_energy) +
                ") out of cross section table range: ["
                + std::to_string(pow(10.,total_cross_section_.lower_extent(0))) + " GeV,"
                + std::to_string(pow(10.,total_cross_section_.upper_extent(0))) + " GeV]");
    }

    int center;
    total_cross_section_.searchcenters(&log_energy, &center);
    double log_xs = total_cross_section_.ndsplineeval(&log_energy, &center, 0);

    return unit * std::pow(10.0, log_xs);
}

// No implementation for DIS yet, just use non-target function
double DipoleDIS::TotalCrossSection(LI::dataclasses::Particle::ParticleType primary_type, double primary_energy, LI::dataclasses::Particle::ParticleType target_type) const {
		return DipoleDIS::TotalCrossSection(primary_type,primary_energy);
}


double DipoleDIS::DifferentialCrossSection(dataclasses::InteractionRecord const & interaction) const {
    rk::P4 p1(geom3::Vector3(interaction.primary_momentum[1], interaction.primary_momentum[2], interaction.primary_momentum[3]), interaction.primary_mass);
    rk::P4 p2(geom3::Vector3(interaction.target_momentum[1], interaction.target_momentum[2], interaction.target_momentum[3]), interaction.target_mass);
    double primary_energy;
    rk::P4 p1_lab;
    rk::P4 p2_lab;
    if(interaction.target_momentum[1] == 0 and interaction.target_momentum[2] == 0 and interaction.target_momentum[3] == 0) {
        primary_energy = interaction.primary_momentum[0];
        p1_lab = p1;
        p2_lab = p2;
    } else {
        rk::Boost boost_start_to_lab = p2.restBoost();
        p1_lab = boost_start_to_lab * p1;
        p2_lab = boost_start_to_lab * p2;
        primary_energy = p1_lab.e();
    }
    assert(interaction.signature.secondary_types.size() == 2);
    unsigned int lepton_index = (isLepton(interaction.signature.secondary_types[0])) ? 0 : 1;
    unsigned int other_index = 1 - lepton_index;

    std::array<double, 4> const & mom3 = interaction.secondary_momenta[lepton_index];
    std::array<double, 4> const & mom4 = interaction.secondary_momenta[other_index];
    rk::P4 p3(geom3::Vector3(mom3[1], mom3[2], mom3[3]), interaction.secondary_masses[lepton_index]);
    rk::P4 p4(geom3::Vector3(mom4[1], mom4[2], mom4[3]), interaction.secondary_masses[other_index]);

    rk::P4 q = p1 - p3;

    double Q2 = -q.dot(q);
    double y = 1.0 - p2.dot(p3) / p2.dot(p1);
    double x = Q2 / (2.0 * p2.dot(q));
    double lepton_mass = hnl_mass_;

    return DifferentialCrossSection(primary_energy, x, y, lepton_mass, Q2);
}

double DipoleDIS::DifferentialCrossSection(double energy, double x, double y, double secondary_lepton_mass, double Q2) const {
    double log_energy = log10(energy);
    // check preconditions
    if(log_energy < differential_cross_section_.lower_extent(0)
            || log_energy>differential_cross_section_.upper_extent(0))
        return 0.0;
    if(x <= 0 || x >= 1)
        return 0.0;
    if(y <= 0 || y >= 1)
        return 0.0;

    // we assume that:
    // the target is stationary so its energy is just its mass
    // the incoming neutrino is massless, so its kinetic energy is its total energy
    if(std::isnan(Q2)) {
        Q2 = 2.0 * energy * target_mass_ * x * y;
    }
    if(Q2 < minimum_Q2_) // cross section not calculated, assumed to be zero
        return 0;

    // cross section should be zero, but this check is missing from the original
    // CSMS calculation, so we must add it here
    if(!kinematicallyAllowed(x, y, energy, target_mass_, secondary_lepton_mass))
        return 0;

    double result = 16. * Constants::PI 
    for (int pid : pdf->flavors()) {
        
    }
    std::array<double,3> coordinates{{log_energy, log10(x), log10(y)}};
    std::array<int,3> centers;
    if(!differential_cross_section_.searchcenters(coordinates.data(), centers.data()))
        return 0;
    double result = pow(10., differential_cross_section_.ndsplineeval(coordinates.data(), centers.data(), 0));
    assert(result >= 0);
    return unit * result;
}

double DipoleDIS::InteractionThreshold(dataclasses::InteractionRecord const & interaction) const {
    // Consider implementing thershold at some point
    return 0;
}

void DipoleDIS::SampleFinalState(dataclasses::InteractionRecord& interaction, std::shared_ptr<LI::utilities::LI_random> random) const {
    // Uses Metropolis-Hastings Algorithm!
    // useful for cases where we don't know the supremum of our distribution, and the distribution is multi-dimensional
    if (differential_cross_section_.get_ndim() != 3) {
        throw std::runtime_error("I expected 3 dimensions in the cross section spline, but got " + std::to_string(differential_cross_section_.get_ndim()) +". Maybe your fits file doesn't have the right 'INTERACTION' key?");
    }

    rk::P4 p1(geom3::Vector3(interaction.primary_momentum[1], interaction.primary_momentum[2], interaction.primary_momentum[3]), interaction.primary_mass);
    rk::P4 p2(geom3::Vector3(interaction.target_momentum[1], interaction.target_momentum[2], interaction.target_momentum[3]), interaction.target_mass);

    // we assume that:
    // the target is stationary so its energy is just its mass
    // the incoming neutrino is massless, so its kinetic energy is its total energy
    // double s = target_mass_ * tinteraction.secondary_momentarget_mass_ + 2 * target_mass_ * primary_energy;
    // double s = std::pow(rk::invMass(p1, p2), 2);

    double primary_energy;
    rk::P4 p1_lab;
    rk::P4 p2_lab;
    if(interaction.target_momentum[1] == 0 and interaction.target_momentum[2] == 0 and interaction.target_momentum[3] == 0) {
        p1_lab = p1;
        p2_lab = p2;
        primary_energy = p1_lab.e();
    } else {
        // Rest frame of p2 will be our "lab" frame
        rk::Boost boost_start_to_lab = p2.restBoost();
        p1_lab = boost_start_to_lab * p1;
        p2_lab = boost_start_to_lab * p2;
        primary_energy = p1_lab.e();
    }

    unsigned int lepton_index = (isLepton(interaction.signature.secondary_types[0])) ? 0 : 1;
    unsigned int other_index = 1 - lepton_index;
    double m = hnl_mass_;

    double m1 = interaction.primary_mass;
    double m3 = m;
    double E1_lab = p1_lab.e();
    double E2_lab = p2_lab.e();


    // The out-going particle always gets at least enough energy for its rest mass
    double yMax = 1 - m / primary_energy;
    double logYMax = log10(yMax);

    // The minimum allowed value of y occurs when x = 1 and Q is minimized
    double yMin = minimum_Q2_ / (2 * E1_lab * E2_lab);
    double logYMin = log10(yMin);
    // The minimum allowed value of x occurs when y = yMax and Q is minimized
    // double xMin = minimum_Q2_ / ((s - target_mass_ * target_mass_) * yMax);
    double xMin = minimum_Q2_ / (2 * E1_lab * E2_lab * yMax);
    double logXMin = log10(xMin);

    bool accept;

    // kin_vars and its twin are 3-vectors containing [nu-energy, Bjorken X, Bjorken Y]
    std::array<double,3> kin_vars, test_kin_vars;

    // centers of the cross section spline tales.
    std::array<int,3> spline_table_center, test_spline_table_center;

    // values of cross_section from the splines.  By * Bx * Spline(E,x,y)
    double cross_section, test_cross_section;

    // No matter what, we're evaluating at this specific energy.
    kin_vars[0] = test_kin_vars[0] = log10(primary_energy);

    // check preconditions
    if(kin_vars[0] < differential_cross_section_.lower_extent(0)
            || kin_vars[0] > differential_cross_section_.upper_extent(0))
        throw std::runtime_error("Interaction energy out of cross section table range: ["
                + std::to_string(pow(10.,differential_cross_section_.lower_extent(0))) + " GeV,"
                + std::to_string(pow(10.,differential_cross_section_.upper_extent(0))) + " GeV]");

    // sample an intial point
    do {
        // rejection sample a point which is kinematically allowed by calculation limits
        double trialQ;
        do {
            kin_vars[1] = random->Uniform(logXMin,0);
            kin_vars[2] = random->Uniform(logYMin,logYMax);
            trialQ = (2 * E1_lab * E2_lab) * pow(10., kin_vars[1] + kin_vars[2]);
        } while(trialQ<minimum_Q2_ || !kinematicallyAllowed(pow(10., kin_vars[1]), pow(10., kin_vars[2]), primary_energy, target_mass_, m));

        accept = true;
        //sanity check: demand that the sampled point be within the table extents
        if(kin_vars[1] < differential_cross_section_.lower_extent(1)
                || kin_vars[1] > differential_cross_section_.upper_extent(1)) {
            accept = false;
        }
        if(kin_vars[2] < differential_cross_section_.lower_extent(2)
                || kin_vars[2] > differential_cross_section_.upper_extent(2)) {
            accept = false;
        }

        if(accept) {
            // finds the centers in the cross section spline table, returns true if it's successful
            // also sets the centers
            accept = differential_cross_section_.searchcenters(kin_vars.data(),spline_table_center.data());
        }
    } while(!accept);

    //TODO: better proposal distribution?
    double measure = pow(10., kin_vars[1] + kin_vars[2]); // Bx * By

    // Bx * By * xs(E, x, y)
    // evalutates the differential spline at that point
    cross_section = measure*pow(10., differential_cross_section_.ndsplineeval(kin_vars.data(), spline_table_center.data(), 0));

    // this is the magic part. Metropolis Hastings Algorithm.
    // MCMC method!
    const size_t burnin = 40; // converges to the correct distribution over multiple samplings.
    // big number means more accurate, but slower
    for(size_t j = 0; j <= burnin; j++) {
        // repeat the sampling from above to get a new valid point
        double trialQ;
        do {
            test_kin_vars[1] = random->Uniform(logXMin, 0);
            test_kin_vars[2] = random->Uniform(logYMin, logYMax);
            trialQ = (2 * E1_lab * E2_lab) * pow(10., test_kin_vars[1] + test_kin_vars[2]);
        } while(trialQ < minimum_Q2_ || !kinematicallyAllowed(pow(10., test_kin_vars[1]), pow(10., test_kin_vars[2]), primary_energy, target_mass_, m));

        accept = true;
        if(test_kin_vars[1] < differential_cross_section_.lower_extent(1)
                || test_kin_vars[1] > differential_cross_section_.upper_extent(1))
            accept = false;
        if(test_kin_vars[2] < differential_cross_section_.lower_extent(2)
                || test_kin_vars[2] > differential_cross_section_.upper_extent(2))
            accept = false;
        if(!accept)
            continue;

        accept = differential_cross_section_.searchcenters(test_kin_vars.data(), test_spline_table_center.data());
        if(!accept)
            continue;

        double measure = pow(10., test_kin_vars[1] + test_kin_vars[2]);
        double eval = differential_cross_section_.ndsplineeval(test_kin_vars.data(), test_spline_table_center.data(), 0);
        if(std::isnan(eval))
            continue;
        test_cross_section = measure * pow(10., eval);

        double odds = (test_cross_section / cross_section);
        accept = (cross_section == 0 || (odds > 1.) || random->Uniform(0, 1) < odds);

        if(accept) {
            kin_vars = test_kin_vars;
            cross_section = test_cross_section;
        }
    }
    double final_x = pow(10., kin_vars[1]);
    double final_y = pow(10., kin_vars[2]);

    interaction.interaction_parameters.resize(3);
    interaction.interaction_parameters[0] = E1_lab;
    interaction.interaction_parameters[1] = final_x;
    interaction.interaction_parameters[2] = final_y;

    double Q2 = 2 * E1_lab * E2_lab * pow(10.0, kin_vars[1] + kin_vars[2]);
    double p1x_lab = std::sqrt(p1_lab.px() * p1_lab.px() + p1_lab.py() * p1_lab.py() + p1_lab.pz() * p1_lab.pz());
    double pqx_lab = (m1*m1 + m3*m3 + 2 * p1x_lab * p1x_lab + Q2 + 2 * E1_lab * E1_lab * (final_y - 1)) / (2.0 * p1x_lab);
    double momq_lab = std::sqrt(m1*m1 + p1x_lab*p1x_lab + Q2 + E1_lab * E1_lab * (final_y * final_y - 1));
    double pqy_lab = std::sqrt(momq_lab*momq_lab - pqx_lab *pqx_lab);
    double Eq_lab = E1_lab * final_y;

    geom3::UnitVector3 x_dir = geom3::UnitVector3::xAxis();
    geom3::Vector3 p1_mom = p1_lab.momentum();
    geom3::UnitVector3 p1_lab_dir = p1_mom.direction();
    geom3::Rotation3 x_to_p1_lab_rot = geom3::rotationBetween(x_dir, p1_lab_dir);

    double phi = random->Uniform(0, 2.0 * M_PI);
    geom3::Rotation3 rand_rot(p1_lab_dir, phi);

    rk::P4 pq_lab(Eq_lab, geom3::Vector3(pqx_lab, pqy_lab, 0));
    pq_lab.rotate(x_to_p1_lab_rot);
    pq_lab.rotate(rand_rot);

    rk::P4 p3_lab((p1_lab - pq_lab).momentum(), m3);
    rk::P4 p4_lab = p2_lab + pq_lab;

    rk::P4 p3;
    rk::P4 p4;
    if(interaction.target_momentum[1] == 0 and interaction.target_momentum[2] == 0 and interaction.target_momentum[3] == 0) {
        p3 = p3_lab;
        p4 = p4_lab;
    } else {
        rk::Boost boost_lab_to_start = p2.labBoost();
        p3 = boost_lab_to_start * p3_lab;
        p4 = boost_lab_to_start * p4_lab;
    }

    interaction.secondary_momenta.resize(2);
    interaction.secondary_masses.resize(2);
    interaction.secondary_helicity.resize(2);

    interaction.secondary_momenta[lepton_index][0] = p3.e(); // p3_energy
    interaction.secondary_momenta[lepton_index][1] = p3.px(); // p3_x
    interaction.secondary_momenta[lepton_index][2] = p3.py(); // p3_y
    interaction.secondary_momenta[lepton_index][3] = p3.pz(); // p3_z
    interaction.secondary_masses[lepton_index] = p3.m();

    interaction.secondary_helicity[lepton_index] = interaction.primary_helicity;

    interaction.secondary_momenta[other_index][0] = p4.e(); // p4_energy
    interaction.secondary_momenta[other_index][1] = p4.px(); // p4_x
    interaction.secondary_momenta[other_index][2] = p4.py(); // p4_y
    interaction.secondary_momenta[other_index][3] = p4.pz(); // p4_z
    interaction.secondary_masses[other_index] = p4.m();

    interaction.secondary_helicity[other_index] = interaction.target_helicity;
}

double DipoleDIS::FinalStateProbability(dataclasses::InteractionRecord const & interaction) const {
    double dxs = DifferentialCrossSection(interaction);
    double txs = TotalCrossSection(interaction);
    if(dxs == 0) {
        return 0.0;
    } else {
        return dxs / txs;
    }
}

std::vector<LI::dataclasses::Particle::ParticleType> DipoleDIS::GetPossiblePrimaries() const {
    return std::vector<LI::dataclasses::Particle::ParticleType>(primary_types_.begin(), primary_types_.end());
}

std::vector<LI::dataclasses::Particle::ParticleType> DipoleDIS::GetPossibleTargetsFromPrimary(LI::dataclasses::Particle::ParticleType primary_type) const {
    return std::vector<LI::dataclasses::Particle::ParticleType>(target_types_.begin(), target_types_.end());
}

std::vector<dataclasses::InteractionSignature> DipoleDIS::GetPossibleSignatures() const {
    return std::vector<dataclasses::InteractionSignature>(signatures_.begin(), signatures_.end());
}

std::vector<LI::dataclasses::Particle::ParticleType> DipoleDIS::GetPossibleTargets() const {
    return std::vector<LI::dataclasses::Particle::ParticleType>(target_types_.begin(), target_types_.end());
}

std::vector<dataclasses::InteractionSignature> DipoleDIS::GetPossibleSignaturesFromParents(LI::dataclasses::Particle::ParticleType primary_type, LI::dataclasses::Particle::ParticleType target_type) const {
    std::pair<LI::dataclasses::Particle::ParticleType, LI::dataclasses::Particle::ParticleType> key(primary_type, target_type);
    if(signatures_by_parent_types_.find(key) != signatures_by_parent_types_.end()) {
        return signatures_by_parent_types_.at(key);
    } else {
        return std::vector<dataclasses::InteractionSignature>();
    }
}

std::vector<std::string> DipoleDIS::DensityVariables() const {
    return std::vector<std::string>{"Bjorken x", "Bjorken y"};
}

} // namespace interactions
} // namespace LI
