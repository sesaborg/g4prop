// #include "G4HadronicProcess.hh"
// #include "PropTrackingAction.hh"
// #include "G4TrackStatus.hh"
// #include "g4prop/G4PropParticleTree.hh"
#include "g4prop/geant4/PropTrackingAction.hh"
#include "G4TrackingManager.hh"

using namespace G4Prop;

PropTrackingAction::PropTrackingAction(G4double kineticEnergyMin, G4int storeTrajectory) : G4UserTrackingAction(), fCulledEnergy(0)
{
    fStoreTrajectory = storeTrajectory;
    fKineticEnergyMin = kineticEnergyMin;
}

PropTrackingAction::~PropTrackingAction() = default;

void PropTrackingAction::PreUserTrackingAction(const G4Track *track)
{
    if (fStoreTrajectory == 0)
    {
        fpTrackingManager->SetStoreTrajectory(0);
    }
    else
    {
        fpTrackingManager->SetStoreTrajectory(1);
        if (fStoreTrajectory == 3)
        {
            fpTrackingManager->SetTrajectory(new PropTrajectory(track));
        }
    }
}

void PropTrackingAction::PostUserTrackingAction(const G4Track *track)
{
    const G4TrackVector *secondaries = track->GetStep()->GetSecondary();

    // It would be nice to convert the cutoff into a more comprehensive predicate (based on particle type, energy, etc)
    for (auto secondaryTrack : *secondaries)
    {
        G4double kineticEnergy = secondaryTrack->GetKineticEnergy();

        if (kineticEnergy < fKineticEnergyMin)
        {
            fCulledEnergy += kineticEnergy;
            secondaryTrack->SetTrackStatus(fStopAndKill);
        }
    }

    // Only if we aren't storing trajectories should we create our own list of particles.
    if (fStoreTrajectory == 0)
    {
        if (track->GetTrackID() > 1)
        {
            // Now in the context of this particle as a secondary of another particle...
            // Find the list of secondaries of this track's parent, then insert this track's ID into that list.
            // I've done it this way because the secondaries don't get their trackID's until after they are initialized.
            // G4cout << "TrackID: " << track->GetTrackID() << " Parent's TrackID: " << track->GetParentID() << G4endl;
            boost::shared_ptr<std::vector<G4int>> &parentSecondaries = fTrackToSecondariesMap.at(track->GetParentID());
            parentSecondaries->push_back(track->GetTrackID());
        }

        if (fTracks.size() == fTracks.capacity())
        {
            G4cout << "Reserving more memory for fTracks" << G4endl;
            G4cout << "Old Capacity: " << fTracks.capacity() << G4endl;
            // Doubles the size of the fTracks vector (hopefully without needing excessive reallocation and copying, but hey what are the chances of that)
            fTracks.reserve((size_t)(fTracks.capacity() * 2));
            G4cout << "New Capacity: " << fTracks.capacity() << G4endl;
        }

        G4Track *newTrack = new G4Track(*track);
        newTrack->SetKineticEnergy(track->GetKineticEnergy());
        newTrack->SetParentID(track->GetParentID());
        newTrack->SetTrackID(track->GetTrackID());
        newTrack->SetPosition(track->GetPosition());
        newTrack->SetMomentumDirection(track->GetMomentumDirection());
        newTrack->SetGlobalTime(track->GetGlobalTime());
        fTracks.push_back(newTrack);

        // Add a pair to the secondaries map with an already initialized vector to be populated by the secondaries later on
        // Doesn't work if the secondaries get tracked before the primary finishes, which can happen if you do something
        // different with the stacking action (such as delaying processing)!

        // G4cout << "New Secondaries: " << secondaries->size() << G4endl;
        auto newSecondaries = boost::shared_ptr<std::vector<G4int>>(new std::vector<G4int>());
        newSecondaries->reserve(secondaries->size());
        // G4cout << "Trying to Insert into fTrackToSecondariesMap!" << G4endl;
        fTrackToSecondariesMap.insert({newTrack->GetTrackID(), newSecondaries});
        // G4cout << "Inserted into fTrackToSecondariesMap!" << G4endl;
    }
}