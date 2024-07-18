#ifndef PROPTRACKINGACTION_H
#define PROPTRACKINGACTION_H
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Types.hh"

namespace G4Prop
{
	class PropTrackingAction : public G4UserTrackingAction
	{
	public:
		PropTrackingAction(G4double kineticEnergyMin) : G4UserTrackingAction()
		{
			fKineticEnergyMin = kineticEnergyMin;
		}

		~PropTrackingAction() override {};

	public:
		void PreUserTrackingAction(const G4Track *track) override {}
		void PostUserTrackingAction(const G4Track *track) override
		{
			const G4TrackVector *secondaries = track->GetStep()->GetSecondary();
			// if (track->GetTrackID() == 1)
			// {
			// 	G4cout << "Post User Tracking Action secondaries->size(): " << secondaries->size() << G4endl;
			// }

			for (auto secondaryTrack : *secondaries)
			{
				G4double kineticEnergy = secondaryTrack->GetKineticEnergy();

				if (kineticEnergy < fKineticEnergyMin)
				{
					fCulledEnergy += kineticEnergy;
					secondaryTrack->SetTrackStatus(fStopAndKill);
				}
			}

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

			G4Track *newTrack = new G4Track(*track);
			newTrack->SetKineticEnergy(track->GetKineticEnergy());
			newTrack->SetParentID(track->GetParentID());
			newTrack->SetTrackID(track->GetTrackID());
			newTrack->SetPosition(track->GetPosition());
			newTrack->SetMomentumDirection(track->GetMomentumDirection());
			newTrack->SetGlobalTime(track->GetGlobalTime());
			fTracks.push_back(newTrack);

			std::vector<G4int> secondaryIDs;
			secondaryIDs.reserve(secondaries->size());
			for (size_t i = 0; i < secondaries->size(); i++)
			{
				secondaryIDs.push_back(secondaries->at(i)->GetTrackID());
			}
			// Add a pair to the secondaries map with an already initialized vector to be populated by the secondaries later on
			// Doesn't work if the secondaries get tracked before the primary finishes.
			auto newSecondaries = boost::shared_ptr<std::vector<G4int>>(new std::vector<G4int>());
			newSecondaries->reserve(secondaries->size());
			fTrackToSecondariesMap.insert({newTrack->GetTrackID(), newSecondaries});
		}
		void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }
		G4double GetCulledEnergy() { return fCulledEnergy; }
		std::vector<G4Track *> GetTracks() { return fTracks; }
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &GetTrackToSecondariesMap() { return fTrackToSecondariesMap; }

	private:
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
		std::vector<G4Track *> fTracks;
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> fTrackToSecondariesMap; // Map between Tracks and their secondaries
	};
}
#endif // PROPTRACKINGACTION_H