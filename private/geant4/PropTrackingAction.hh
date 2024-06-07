#include "G4UserTrackingAction.hh"
#include "G4Track.hh"

class PropTrackingAction : public G4UserTrackingAction {
public:
	PropTrackingAction();
	virtual ~PropTrackingAction();

public:
	void PreUserTrackingAction(const G4Track* track);
	void PostUserTrackingAction(const G4Track* track);
	void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }
	G4double GetCulledEnergy() { return fCulledEnergy; }

private:
	G4double fKineticEnergyMin;
	G4double fCulledEnergy;
}
