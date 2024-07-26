#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "icetray/I3Module.h"
#include "g4prop/I3G4PropModule.hh"
#include "G4ParticleDefinition.hh"
#include <boost/foreach.hpp>
#include "G4SystemOfUnits.hh"
#include "G4OpticalParameters.hh"

typedef I3G4PropModule g4prop;
namespace
{
    I3_MODULE(g4prop);
};

I3G4PropModule::~I3G4PropModule()
{
    log_info("Destroying g4prop.");
    delete G4Prop::randomEngine;
    delete G4Prop::runManager;
}

void I3G4PropModule::Configure()
{
    GetParameter("Geant4Commands", uiCommands_);
    GetParameter("RelativeCutoff", relativeCutoff_);
    log_info("Configuring g4prop.");
    if (!configured)
    {
        log_info("Configuring Geant4");
        G4Prop::ConfigureGeant4(relativeCutoff_);
        configured = true;
        log_info("g4prop's Geant4 has been configured.");
    }
    else
    {
        log_warn("g4prop's Geant4 has already been configured!");
    }
}

// void I3G4PropModule::Finish()
// {
//     log_info("g4prop has finished");
// }

// void I3G4PropModule::Process()
// {
//     log_info("g4prop Process()");
//     I3FramePtr frame = PopFrame();
//     if (!frame)
//         I3FramePtr frame(new I3Frame(I3Frame::DAQ));

//     log_info("I3Vector<double> test");
//     auto test = new I3VectorDouble(std::size_t(4));
//     (*test)[0] = 214.2;
//     (*test)[1] = 242.0;
//     (*test)[2] = 04.2;
//     (*test)[3] = 1.1;
//     log_info("I3Vector<double> test Put");
//     frame->Put("TestVector", I3VectorDoublePtr(test));

//     if (frame->GetStop() == I3Frame::DAQ)
//     {
//         DAQ(frame);
//         return;
//     }

//     PushFrame(frame);
//     return;
// }

void I3G4PropModule::DAQ(I3FramePtr frame)
{
    log_info("g4prop DAQ");
    I3MCTreeConstPtr i3MCTree = frame->Get<I3MCTreeConstPtr>();
    if (!i3MCTree)
    {
        PushFrame(frame);
        return;
    }

    auto newI3MCTree = boost::shared_ptr<I3MCTree>(new I3MCTree(*i3MCTree));

    log_info("I3MCTree has %zu nodes. It's children are:", i3MCTree->size());
    auto iter = i3MCTree->begin();
    for (size_t i = 0; i < i3MCTree->size(); i++)
    {
        auto &originalParticle = *iter;
        log_info(" -- %i  %s  (%f m, %f m, %f m)  (%f deg, %f deg)  %f ns  %f GeV  %f m  %f MeV/c^2  %i",
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
                 originalParticle.HasMass() ? originalParticle.GetMass() / (I3Units::MeV / (I3Constants::c * I3Constants::c)) : NAN,
                 originalParticle.GetPdgEncoding());

        if (configured)
        {

            G4ParticleDefinition *geant4Particle = G4ParticleTable::GetParticleTable()->FindParticle(originalParticle.GetPdgEncoding());
            if (geant4Particle != 0)
            {
                log_info(" -- In Geant4: %s", geant4Particle->GetParticleName().c_str());
                G4Prop::G4PropParticleProperties particleToShoot = (G4Prop::G4PropParticleProperties){
                    originalParticle.GetMinorID(),
                    originalParticle.GetPdgEncoding(),
                    originalParticle.GetEnergy() / I3Units::GeV * CLHEP::GeV,
                    G4ThreeVector(originalParticle.GetPos().GetX() / I3Units::mm * CLHEP::mm,
                                  originalParticle.GetPos().GetY() / I3Units::mm * CLHEP::mm,
                                  originalParticle.GetPos().GetZ() / I3Units::mm * CLHEP::mm),
                    G4ThreeVector(cos(originalParticle.GetAzimuth() / I3Units::rad) * cos(originalParticle.GetZenith() / I3Units::rad),
                                  sin(originalParticle.GetAzimuth() / I3Units::rad) * cos(originalParticle.GetZenith() / I3Units::rad),
                                  sin(originalParticle.GetZenith() / I3Units::rad)),
                    originalParticle.GetAzimuth() / I3Units::deg * CLHEP::deg,
                    originalParticle.GetZenith(),
                    originalParticle.GetTime()};

                G4ParticleGun *particleGun;
                G4Prop::LoadParticleGun(particleToShoot, particleGun);

                log_info("\nG4Particle Configuration Check:\n\tParticle Definition Name: %s\n\tParticle Energy: %f GeV\n\tParticle Position: (%f m, %f m, %f m)\n", particleGun->GetParticleDefinition()->GetParticleName().c_str(), particleGun->GetParticleEnergy() / CLHEP::GeV, particleGun->GetParticlePosition().getX() / CLHEP::m, particleGun->GetParticlePosition().getY() / CLHEP::m, particleGun->GetParticlePosition().getZ() / CLHEP::m);
                G4Prop::RunGeant4(particleGun, uiCommands_, 3);
                if (false)
                {
                    log_info("Using G4Track Methods");
                    std::vector<G4Track *> tracks = G4Prop::GetTracks();
                    // std::vector<I3Particle> particles;
                    std::map<I3ParticleID, I3Particle> particleMap;
                    boost::bimap<I3ParticleID, G4int> particleToTrackMap;
                    std::map<G4int, G4int> trackToParentMap;
                    std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &trackToSecondariesMap = G4Prop::GetTrackToSecondariesMap();

                    // log_info("%lu", trackToSecondariesMap.size());

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
                            // log_info("Inserted originalParticleID = %i, trackID = %i", originalParticle.GetID().minorID, track->GetTrackID());
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

                            // log_info(" -- %i  %s  (%f m, %f m, %f m)  (%f deg, %f deg)  %f ns  %f GeV  %f m  %f MeV/c^2  %i",
                            //          particle.GetMinorID(),
                            //          particle.GetTypeString().c_str(),
                            //          particle.GetPos().GetX() / I3Units::m,
                            //          particle.GetPos().GetY() / I3Units::m,
                            //          particle.GetPos().GetZ() / I3Units::m,
                            //          particle.GetAzimuth() / I3Units::deg,
                            //          particle.GetZenith() / I3Units::deg,
                            //          particle.GetTime() / I3Units::ns,
                            //          particle.GetEnergy() / I3Units::GeV,
                            //          particle.GetLength() / I3Units::m,
                            //          particle.HasMass() ? particle.GetMass() / (I3Units::MeV / (I3Constants::c * I3Constants::c)) : NAN,
                            //          particle.GetPdgEncoding());

                            // particles.insert(particles.end(), particle);
                            // trackToParentMap.insert({track->GetTrackID(), track->GetParentID()});
                            // if (track->GetParentID() == 1) {

                            // }
                            particleToTrackMap.insert({particle.GetID(), track->GetTrackID()});
                            // log_info("Inserted particleID = %i, trackID = %i", particle.GetID().minorID, track->GetTrackID());
                            trackToParentMap.insert({track->GetTrackID(), track->GetParentID()});
                            particleMap.insert({particle.GetID(), particle});
                            // trackToChildMap.insert({track->GetTrackID(), track->Get})

                            // log_info("trackID: %i  parentID: %i  minorID: %i", track->GetTrackID(), track->GetParentID(), particle.GetID().minorID);
                        }
                    }

                    // The primary particle should be the first, with the particle vector being sorted by TrackID.

                    log_info("There are %lu particles to insert.", particleMap.size());
                    // InsertToTree(newI3MCTree, &trackToSecondariesMap, &particleToTrackMap, &particleMap, originalParticle.GetID());
                    // for (size_t i = 0; i < particles.size(); i++)
                    // {
                    //     // log_info("Inserting node...");
                    //     // log_info("\nHere we have a particle known as:\n\tmajorID: %lu\n\tminorID: %i", particles.at(i).GetMajorID(), particles.at(i).GetMinorID());
                    //     I3ParticleID id = particles.at(i).GetID();
                    //     // log_info("id = %i", id.minorID);
                    //     G4int trackID = particleToTrackMap.left.at(id);
                    //     if (trackToSecondariesMap.find(trackID))
                    //     // log_info("trackID = %i", trackID);
                    //     G4int parentID = trackToParentMap.at(trackID);
                    //     // log_info("parentID = %i", parentID);
                    //     I3ParticleID parentParticleID = particleToTrackMap.right.at(parentID);
                    //     // log_info("parentParticleID = %i", parentParticleID.minorID);
                    //     // log_info("trackID: %i  parentID: %i  minorID: %i  parentMinorID: %i", trackID, parentID, id.minorID, parentParticleID.minorID);
                    //     newI3MCTree->append_child(parentParticleID, particles[i]);

                    //     newI3MCTree->append_children()
                    // }
                    std::function<void(const I3ParticleID &)> insertToTree = [newI3MCTree, particleToTrackMap, particleMap, trackToSecondariesMap, &insertToTree](const I3ParticleID &id)
                    {
                        G4int trackID = particleToTrackMap.left.at(id);
                        boost::shared_ptr<std::vector<G4int>> secondaryTrackIDs = trackToSecondariesMap.at(trackID);
                        // log_info("Insert To Tree %i %lu", trackID, secondaryTrackIDs->size());
                        // log_info("%i %lu", trackID, secondaryTrackIDs->size());
                        std::vector<I3Particle> secondaryParticles;
                        // BOOST_FOREACH (auto &secondaryTrackID, *secondaryTrackIDs)
                        // {
                        //     log_info("%i", secondaryTrackID);
                        //     // secondaryParticles.insert(secondaryParticles.end(), particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                        // }

                        BOOST_FOREACH (auto &secondaryTrackID, *secondaryTrackIDs)
                        {
                            // log_info("%i", secondaryTrackID);
                            secondaryParticles.insert(secondaryParticles.end(), particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                        }

                        newI3MCTree->append_children(id, secondaryParticles);

                        BOOST_FOREACH (auto &secondaryParticle, secondaryParticles)
                        {
                            insertToTree(secondaryParticle.GetID());
                        }
                    };

                    insertToTree(originalParticle.GetID());
                }

                else
                {
                    log_info("Using G4RichTrajectory Methods");
                    // auto &trajectories = G4Prop::GetTrajectories();
                    auto &trajectories = G4Prop::GetTrajectories();
                    // G4cout << trajectories.size() << G4endl;
                    log_info("Got %lu Trajectories", trajectories.size());
                    auto trajectorySecondaryMap = std::map<G4int, boost::shared_ptr<std::vector<G4int>>>();

                    std::map<I3ParticleID, I3Particle> particleMap;
                    boost::bimap<I3ParticleID, G4int> particleToTrackMap;

                    for (auto &trajectory : trajectories)
                    {

                        // log_info("%i", trajectory->GetTrackID());
                        auto secondariesList = new std::vector<G4int>();
                        trajectorySecondaryMap.insert({trajectory->GetTrackID(), boost::shared_ptr<std::vector<G4int>>(secondariesList)});

                        if (trajectory->GetParentID() != 0)
                        {
                            trajectorySecondaryMap.at(trajectory->GetParentID())->push_back(trajectory->GetTrackID());
                        }

                        if (trajectory->GetTrackID() == 1)
                        {
                            // Special case where the primary particle in Geant4 is the same as the original particle.
                            particleToTrackMap.insert({originalParticle.GetID(), trajectory->GetTrackID()});
                            particleMap.insert({originalParticle.GetID(), originalParticle});
                            // log_info("Inserted originalParticleID = %i, trackID = %i", originalParticle.GetID().minorID, track->GetTrackID());
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
                            // Assuming all the particles are in the ice
                            particle.SetLocationType(I3Particle::LocationType::InIce);
                            particle.SetShape(I3Particle::ParticleShape::MCTrack);
                            particle.SetEnergy(trajectory->GetFinalKineticEnergy() / CLHEP::GeV * I3Units::GeV);
                            particle.SetPdgEncoding(trajectory->GetPDGEncoding());
                            particleToTrackMap.insert({particle.GetID(), trajectory->GetTrackID()});
                            particleMap.insert({particle.GetID(), particle});
                        }
                    }

                    for (auto &trajectory : trajectories)
                    {
                        // If a trajectory has secondaries associated with it, assume it's finished and assign a track length to its I3Particle.
                        if (trajectorySecondaryMap.at(trajectory->GetTrackID())->size() > 0)
                        {
                            particleMap.at(particleToTrackMap.right.at(trajectory->GetTrackID())).SetLength(trajectory->GetTrackLength() / CLHEP::m * I3Units::m);
                        }
                    }

                    std::function<void(const I3ParticleID &)> insertToTree = [newI3MCTree, particleToTrackMap, particleMap, trajectorySecondaryMap, &insertToTree](const I3ParticleID &id)
                    {
                        G4int trackID = particleToTrackMap.left.at(id);
                        boost::shared_ptr<std::vector<G4int>> secondaryTrackIDs = trajectorySecondaryMap.at(trackID);
                        // log_info("Insert To Tree %i %lu", trackID, secondaryTrackIDs->size());
                        // log_info("%i %lu", trackID, secondaryTrackIDs->size());
                        std::vector<I3Particle> secondaryParticles;
                        // BOOST_FOREACH (auto &secondaryTrackID, *secondaryTrackIDs)
                        // {
                        //     log_info("%i", secondaryTrackID);
                        //     // secondaryParticles.insert(secondaryParticles.end(), particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                        // }

                        BOOST_FOREACH (auto &secondaryTrackID, *secondaryTrackIDs)
                        {
                            // log_info("%i", secondaryTrackID);
                            secondaryParticles.insert(secondaryParticles.end(), particleMap.at(particleToTrackMap.right.at(secondaryTrackID)));
                        }

                        newI3MCTree->append_children(id, secondaryParticles);

                        BOOST_FOREACH (auto &secondaryParticle, secondaryParticles)
                        {
                            insertToTree(secondaryParticle.GetID());
                        }
                    };

                    insertToTree(originalParticle.GetID());
                }
            }
            else
            {
                log_warn(" -- In Geant4: Unsupported PDG!");
            }
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
