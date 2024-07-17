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
		PropTrackingAction(G4double kineticEnergyMin, size_t reserveLength = 10000) : G4UserTrackingAction()
		{
			fKineticEnergyMin = kineticEnergyMin;
			fReserveLength = reserveLength;
			fTracks.reserve(reserveLength);
		}

		~PropTrackingAction() override {};

	public:
		void PreUserTrackingAction(const G4Track *track) override {}
		void PostUserTrackingAction(const G4Track *track) override
		{
			const G4TrackVector *secondaries = track->GetStep()->GetSecondary();

			for (auto secondaryTrack : *secondaries)
			{
				G4double kineticEnergy = secondaryTrack->GetKineticEnergy();

				if (kineticEnergy < fKineticEnergyMin)
				{
					fCulledEnergy += kineticEnergy;
					secondaryTrack->SetTrackStatus(fStopAndKill);
				}
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
			fTracks.insert(fTracks.end(), newTrack);
		}
		void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }
		G4double GetCulledEnergy() { return fCulledEnergy; }
		std::vector<G4Track *> GetTracks() { return fTracks; }

	private:
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
		std::vector<G4Track *> fTracks;
		// std::map<G4Track *, G4TrackVector> fTracks; // Map between Tracks and their secondaries
	};
}
#endif // PROPTRACKINGACTION_H