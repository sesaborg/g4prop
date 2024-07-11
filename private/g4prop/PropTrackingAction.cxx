#include "G4HadronicProcess.hh"
#include "PropTrackingAction.hh"
#include "G4TrackStatus.hh"

using namespace G4Prop;

PropTrackingAction::PropTrackingAction() : G4UserTrackingAction(), fCulledEnergy(0) {}

PropTrackingAction::PropTrackingAction(G4double kineticEnergyMin) : PropTrackingAction()
{
	fKineticEnergyMin = kineticEnergyMin;
}

void PropTrackingAction::PreUserTrackingAction(const G4Track *track)
{
}

void PropTrackingAction::PostUserTrackingAction(const G4Track *track)
{
	const G4TrackVector *secondaries = track->GetStep()->GetSecondary();

	for (auto secondaryTrack : *secondaries)
	{
		G4double kineticEnergy = secondaryTrack->GetKineticEnergy();

		if (kineticEnergy < fKineticEnergyMin)
		{
			fCulledEnergy += kineticEnergy;
			secondaryTrack->SetTrackStatus(fStopAndKill);

			// I3MCTree code goes here
		}
	}
}

PropTrackingAction::~PropTrackingAction() = default;
