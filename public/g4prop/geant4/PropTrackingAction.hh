#ifndef PROPTRACKINGACTION_H
#define PROPTRACKINGACTION_H
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Types.hh"
#include "boost/shared_ptr.hpp"
#include "g4prop/geant4/PropTrajectory.hh"

namespace G4Prop
{
	class PropTrackingAction : public G4UserTrackingAction
	{
	public:
		PropTrackingAction(G4double kineticEnergyMin, G4int storeTrajectory) : G4UserTrackingAction()
		{
			fStoreTrajectory = storeTrajectory;
			fKineticEnergyMin = kineticEnergyMin;
		}

		~PropTrackingAction() override {};

	public:
		void PreUserTrackingAction(const G4Track *track) override
		{
			fpTrackingManager->SetStoreTrajectory(1);
			fpTrackingManager->SetTrajectory(new PropTrajectory(track));
		}
		void PostUserTrackingAction(const G4Track *track) override
		{
			const G4TrackVector *secondaries = track->GetStep()->GetSecondary();
			if (track->GetTrackID() == 1)
			{
				G4cout << "Post User Tracking Action secondaries->size(): " << secondaries->size() << G4endl;
			}

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
					// Find the parent of this track's list of secondaries, then insert this track's ID into that list.
					// It's done this way because the secondaries don't get their trackID's until after they are initialized.
					// G4cout << "TrackID: " << track->GetTrackID() << " Parent's TrackID: " << track->GetParentID() << G4endl;
					boost::shared_ptr<std::vector<G4int>> &parentSecondaries = fTrackToSecondariesMap.at(track->GetParentID());
					parentSecondaries->push_back(track->GetTrackID());
				}

				if (fTracks.size() == fTracks.capacity())
				{
					G4cout << "Reserving more memory for fTracks" << G4endl;
					G4cout << fTracks.capacity() << G4endl;
					fTracks.reserve((size_t)(fTracks.capacity() * 2));
					G4cout << fTracks.capacity() << G4endl;
				}
				// G4cout << "New Track!" << G4endl;
				G4Track *newTrack = new G4Track(*track);
				newTrack->SetKineticEnergy(track->GetKineticEnergy());
				newTrack->SetParentID(track->GetParentID());
				newTrack->SetTrackID(track->GetTrackID());
				newTrack->SetPosition(track->GetPosition());
				newTrack->SetMomentumDirection(track->GetMomentumDirection());
				newTrack->SetGlobalTime(track->GetGlobalTime());
				fTracks.push_back(newTrack);

				// Add a pair to the secondaries map with an already initialized vector to be populated by the secondaries later on
				// Doesn't work if the secondaries get tracked before the primary finishes.
				// G4cout << "New Secondaries: " << secondaries->size() << G4endl;
				auto newSecondaries = boost::shared_ptr<std::vector<G4int>>(new std::vector<G4int>());
				newSecondaries->reserve(secondaries->size());
				// G4cout << "Trying to Insert into fTrackToSecondariesMap!" << G4endl;
				fTrackToSecondariesMap.insert({newTrack->GetTrackID(), newSecondaries});
				// G4cout << "Inserted into fTrackToSecondariesMap!" << G4endl;
			}
		}

		void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }
		G4double GetCulledEnergy() { return fCulledEnergy; }
		std::vector<G4Track *> GetTracks() { return fTracks; }
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &GetTrackToSecondariesMap() { return fTrackToSecondariesMap; }

	private:
		G4int fStoreTrajectory;
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
		std::vector<G4Track *> fTracks;
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> fTrackToSecondariesMap; // Map between Tracks and their secondaries
	};
}
#endif // PROPTRACKINGACTION_H