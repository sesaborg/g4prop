#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "icetray/I3Module.h"
#include "g4prop/I3G4PropModule.hh"
#include "G4ParticleDefinition.hh"
#include <boost/foreach.hpp>
#include "G4SystemOfUnits.hh"
#include "G4OpticalParameters.hh"
#include "sim-services/I3SimConstants.h"

typedef I3G4PropModule g4prop;
namespace
{
    I3_MODULE(g4prop);
};

I3G4PropModule::~I3G4PropModule()
{
    log_debug("Destroying g4prop.");
    delete G4Prop::randomEngine;
    delete G4Prop::runManager;
}

void I3G4PropModule::Configure()
{
    GetParameter("Geant4Commands", uiCommands_);
    GetParameter("RelativeCutoff", relativeCutoff_);
    log_debug("Configuring g4prop.");
    if (!configured)
    {
        log_debug("Configuring Geant4.");
        G4Prop::ConfigureGeant4(relativeCutoff_);
        configured = true;
        log_debug("g4prop's Geant4 has been configured.");
    }
    else
    {
        log_warn("g4prop's Geant4 has already been configured!");
    }
}

void I3G4PropModule::DAQ(I3FramePtr frame)
{
    log_trace("g4prop DAQ");
    I3MCTreeConstPtr i3MCTree = frame->Get<I3MCTreeConstPtr>();
    if (!i3MCTree)
    {
        // If there isn't anything to propagate, go next
        PushFrame(frame);
        return;
    }

    // Copy the MCTree
    auto newI3MCTree = boost::shared_ptr<I3MCTree>(new I3MCTree(*i3MCTree));

    log_debug("I3MCTree has %zu nodes. It's children are:", i3MCTree->size());
    auto iter = i3MCTree->begin();
    for (size_t i = 0; i < i3MCTree->size(); i++)
    {
        auto &originalParticle = *iter;
        log_trace(" -- %i  %s  (%f m, %f m, %f m)  (%f deg, %f deg)  %f ns  %f GeV  %f m   %i",
                  originalParticle.GetMinorID(),
                  originalParticle.GetTypeString().c_str(),
                  originalParticle.GetPos().GetX() / I3Units::mm,
                  originalParticle.GetPos().GetY() / I3Units::mm,
                  originalParticle.GetPos().GetZ() / I3Units::mm,
                  originalParticle.GetAzimuth() / I3Units::deg,
                  originalParticle.GetZenith() / I3Units::deg,
                  originalParticle.GetTime() / I3Units::ns,
                  originalParticle.GetEnergy() / I3Units::GeV,
                  originalParticle.GetLength() / I3Units::m,
                  originalParticle.GetPdgEncoding());

        // If a event generator given an MCTree with some structure, we need to ignore the particles that have already finished propagating.
        if (i3MCTree->number_of_children(originalParticle) != 0)
        {
            iter++;
            continue;
        }

        if (configured)
        {
            G4ParticleDefinition *geant4Particle = G4ParticleTable::GetParticleTable()->FindParticle(originalParticle.GetPdgEncoding());
            if (geant4Particle != 0)
            {
                log_debug(" -- In Geant4: %s", geant4Particle->GetParticleName().c_str());

                // Construct the G4PropParticleProperties struct, all the required info for the particle gun
                // Note that nothing prevents you from constucting your own G4ParticleGuns.
                G4Prop::G4PropParticleProperties particleToShoot = (G4Prop::G4PropParticleProperties){
                    originalParticle.GetMinorID(),
                    originalParticle.GetPdgEncoding(),
                    originalParticle.GetEnergy() / I3Units::GeV * CLHEP::GeV,
                    G4ThreeVector(originalParticle.GetPos().GetX() / I3Units::mm * CLHEP::mm,
                                  originalParticle.GetPos().GetY() / I3Units::mm * CLHEP::mm,
                                  originalParticle.GetPos().GetZ() / I3Units::mm * CLHEP::mm),
                    // I'd double check these direction conversions between Icetray and Geant4.
                    G4ThreeVector(cos(originalParticle.GetAzimuth() / I3Units::rad) * cos(originalParticle.GetZenith() / I3Units::rad),
                                  sin(originalParticle.GetAzimuth() / I3Units::rad) * cos(originalParticle.GetZenith() / I3Units::rad),
                                  sin(originalParticle.GetZenith() / I3Units::rad)),
                    originalParticle.GetAzimuth() / I3Units::deg * CLHEP::deg,
                    originalParticle.GetZenith() / I3Units::deg * CLHEP::deg,
                    originalParticle.GetTime() / I3Units::ns * CLHEP::ns};

                G4ParticleGun *particleGun;
                G4Prop::LoadParticleGun(particleToShoot, particleGun);

                log_debug("\nG4Particle Configuration Check:\n\tParticle Definition Name: %s\n\tParticle Energy: %f GeV\n\tParticle Position: (%f m, %f m, %f m)\n", particleGun->GetParticleDefinition()->GetParticleName().c_str(), particleGun->GetParticleEnergy() / CLHEP::GeV, particleGun->GetParticlePosition().getX() / CLHEP::m, particleGun->GetParticlePosition().getY() / CLHEP::m, particleGun->GetParticlePosition().getZ() / CLHEP::m);
                G4Prop::RunGeant4(particleGun, uiCommands_, 3);

                std::map<I3ParticleID, I3Particle> particleMap;
                boost::bimap<I3ParticleID, G4int> particleToTrackMap;
                auto trackToSecondariesMap = std::map<G4int, boost::shared_ptr<std::vector<G4int>>>();

                // I'm not a compsci major, so here's a weird recursive function that constructs an MCTree from the melange of maps and vectors I've accumulated.
                std::function<void(const I3ParticleID &)> insertToTree = [newI3MCTree, particleToTrackMap, particleMap, trackToSecondariesMap, &insertToTree](const I3ParticleID &id)
                {
                    G4int trackID = particleToTrackMap.left.at(id);
                    boost::shared_ptr<std::vector<G4int>> secondaryTrackIDs = trackToSecondariesMap.at(trackID);

                    log_trace("Insert To Tree %i %lu", trackID, secondaryTrackIDs->size());
                    log_trace("%i %lu", trackID, secondaryTrackIDs->size());

                    std::vector<I3Particle> secondaryParticles;

                    BOOST_FOREACH (auto &secondaryTrackID, *secondaryTrackIDs)
                    {
                        log_trace("%i", secondaryTrackID);
                        secondaryParticles.push_back(particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                        // secondaryParticles.insert(secondaryParticles.end(), particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                    }

                    newI3MCTree->append_children(id, secondaryParticles);

                    BOOST_FOREACH (auto &secondaryParticle, secondaryParticles)
                    {
                        insertToTree(secondaryParticle.GetID());
                    }
                };

                // The track storing method is old and might not work as expected, I would avoid if possible
                if (storeTrajectory_ == 0)
                {
                    log_warn("Using G4Track Methods. This is not recommended!");
                    std::vector<G4Track *> tracks = G4Prop::GetTracks();

                    std::map<G4int, G4int> trackToParentMap;
                    trackToSecondariesMap = G4Prop::GetTrackToSecondariesMap();

                    // log_trace("%lu", trackToSecondariesMap.size());

                    // Definition of an operator to compare G4Track* by their TrackID, for use in std::sort.
                    struct
                    {
                        bool operator()(G4Track *a, G4Track *b) const { return a->GetTrackID() < b->GetTrackID(); }
                    } G4TrackCompare;

                    std::sort(tracks.begin(), tracks.end(), G4TrackCompare);

                    for (size_t i = 0; i < tracks.size(); i++)
                    {
                        G4Track *track = tracks[i];
                        if (track->GetTrackID() == 1)
                        {
                            // Special case where the primary particle in Geant4 is the same as the original particle.
                            particleToTrackMap.insert({originalParticle.GetID(), track->GetTrackID()});
                            particleMap.insert({originalParticle.GetID(), originalParticle});
                            log_debug("Inserted originalParticleID = %i, trackID = %i", originalParticle.GetID().minorID, track->GetTrackID());
                            continue;
                        }
                        else
                        {
                            // Otherwise, construct a new I3Particle to be appended to the I3MCTree.

                            G4ThreeVector g4pos = track->GetPosition();
                            I3Position i3pos = I3Position(g4pos.getX() / CLHEP::m * I3Units::m, g4pos.getY() / CLHEP::m * I3Units::m, g4pos.getZ() / CLHEP::m * I3Units::m);
                            G4ThreeVector g4dir = track->GetMomentumDirection();
                            I3Direction i3dir = I3Direction(g4dir.getX(), g4dir.getY(), g4dir.getZ());
                            I3Particle particle(i3pos, i3dir, track->GetGlobalTime() / CLHEP::ns * I3Units::ns);
                            particle.SetEnergy(track->GetKineticEnergy() / CLHEP::GeV * I3Units::GeV);
                            particle.SetPdgEncoding(track->GetParticleDefinition()->GetPDGEncoding());

                            log_trace(" -- %i  %s  (%f m, %f m, %f m)  (%f deg, %f deg)  %f ns  %f GeV  %f m   %i",
                                      particle.GetMinorID(),
                                      particle.GetTypeString().c_str(),
                                      particle.GetPos().GetX() / I3Units::m,
                                      particle.GetPos().GetY() / I3Units::m,
                                      particle.GetPos().GetZ() / I3Units::m,
                                      particle.GetAzimuth() / I3Units::deg,
                                      particle.GetZenith() / I3Units::deg,
                                      particle.GetTime() / I3Units::ns,
                                      particle.GetEnergy() / I3Units::GeV,
                                      particle.GetLength() / I3Units::m,
                                      particle.GetPdgEncoding());

                            particleToTrackMap.insert({particle.GetID(), track->GetTrackID()});
                            log_trace("Inserted particleID = %i, trackID = %i", particle.GetID().minorID, track->GetTrackID());
                            trackToParentMap.insert({track->GetTrackID(), track->GetParentID()});
                            particleMap.insert({particle.GetID(), particle});
                            log_trace("trackID: %i  parentID: %i  minorID: %i", track->GetTrackID(), track->GetParentID(), particle.GetID().minorID);
                        }
                    }

                    // The primary particle should be the first when the particle vector has been sorted by TrackID.

                    log_debug("There are %lu particles to insert.", particleMap.size());

                    insertToTree(originalParticle.GetID());
                }
                else if (storeTrajectory_ == 3)
                {
                    log_trace("Using PropTrajectory Methods");
                    auto &trajectories = G4Prop::GetTrajectories();
                    log_debug("Got %lu Trajectories", trajectories.size());

                    for (auto &trajectory : trajectories)
                    {
                        auto secondariesList = new std::vector<G4int>();
                        trackToSecondariesMap.insert({trajectory->GetTrackID(), boost::shared_ptr<std::vector<G4int>>(secondariesList)});

                        if (trajectory->GetParentID() != 0)
                        {
                            trackToSecondariesMap.at(trajectory->GetParentID())->push_back(trajectory->GetTrackID());
                        }

                        if (trajectory->GetTrackID() == 1)
                        {
                            // Special case where the primary particle in Geant4 is the same as the original particle.
                            particleToTrackMap.insert({originalParticle.GetID(), trajectory->GetTrackID()});
                            particleMap.insert({originalParticle.GetID(), originalParticle});
                            log_trace("Inserted originalParticleID = %i, trackID = %i", originalParticle.GetID().minorID, trajectory->GetTrackID());
                            continue;
                        }
                        else
                        {
                            // Otherwise, construct a new I3Particle to be appended to the I3MCTree.
                            G4ThreeVector g4pos = trajectory->GetFinalPosition();
                            I3Position i3posOriginal = originalParticle.GetPos();
                            I3Position i3pos = I3Position(g4pos.getX() / CLHEP::m * I3Units::m + i3posOriginal.GetX(),
                                                          g4pos.getY() / CLHEP::m * I3Units::m + i3posOriginal.GetY(),
                                                          g4pos.getZ() / CLHEP::m * I3Units::m + i3posOriginal.GetZ());
                            G4ThreeVector g4dir = trajectory->GetFinalMomentumDirection();
                            I3Direction i3dir = I3Direction(g4dir.getX(), g4dir.getY(), g4dir.getZ());
                            I3Particle particle(i3pos, i3dir, trajectory->GetFinalGlobalTime() / CLHEP::ns * I3Units::ns);

                            // Assuming all the particles are in ice for now...
                            // A more sophisticated simulation would use a custom world volume with rock/air/ice/water and proper coordinates to determine whether the particle is in Ice (see clsim's Geant4 detector implementation)
                            particle.SetLocationType(I3Particle::LocationType::InIce);
                            particle.SetEnergy(trajectory->GetFinalKineticEnergy() / CLHEP::GeV * I3Units::GeV);
                            particle.SetPdgEncoding(trajectory->GetPDGEncoding());
                            I3Particle::ParticleType particleList[] = {I3Particle::EMinus, I3Particle::EPlus, I3Particle::Pi0, I3Particle::PiMinus, I3Particle::PiPlus, I3Particle::MuMinus, I3Particle::MuPlus, I3Particle::TauMinus, I3Particle::TauPlus};

                            // bool supported = false;
                            // for (auto &type : I3SimConstants::ShowerParameters::GetSupportedTypes())
                            // {
                            //     if (particle.GetType() == type)
                            //     {
                            //         supported = true;
                            //         break;
                            //     }
                            // }

                            // if (!supported)
                            // {
                            //     log_debug("%s is not a supported type, according to I3SimConstants.", particle.GetTypeString().c_str());
                            //     if (I3SimConstants::IsHadron(particle.GetType()))
                            //     {
                            //         log_debug("... but it is a hadron.");
                            //         particle.SetType(I3Particle::Hadrons);
                            //     }
                            //     else
                            //     {
                            //         log_debug("... and it is some bizarre particle from Geant4.");
                            //         particle.SetType(I3Particle::unknown);
                            //     }
                            //     // If the particle is supported, just leave it with its PDG encoding.
                            // }

                            particleToTrackMap.insert({particle.GetID(), trajectory->GetTrackID()});
                            particleMap.insert({particle.GetID(), particle});
                        }
                    }

                    for (auto &trajectory : trajectories)
                    {
                        // If a trajectory has secondaries associated with it, assume it's finished (decayed or smashed) and assign a track length to its I3Particle.
                        if (trackToSecondariesMap.at(trajectory->GetTrackID())->size() > 0)
                        {
                            // I would STRONGLY recommend double checking this! I don't know if this is the correct way to assign track lengths.
                            particleMap.at(particleToTrackMap.right.at(trajectory->GetTrackID())).SetLength(trajectory->GetTrackLength() / CLHEP::m * I3Units::m);
                        }
                    }

                    insertToTree(originalParticle.GetID());
                }
            }
            else
            {
                log_warn(" -- In Geant4: Unsupported PDG!"); // This will happen for any of the weird Icetray particle types (ie Hadrons, Cascade) in the original MCTree.
            }

            // Move to the next particle in the original MCTree.
            iter++;
        }
    }

    frame->Delete("I3MCTree");
    frame->Put("I3MCTree", newI3MCTree);
    log_info("Pushing DAQ Frame.");
    PushFrame(frame);
}

bool I3G4PropModule::ShouldDoProcess(I3FramePtr frame)
{
    return true;
}
