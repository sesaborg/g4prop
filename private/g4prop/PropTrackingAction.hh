#ifndef PROPTRACKINGACTION_H
#define PROPTRACKINGACTION_H
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Types.hh"

class PropTrackingAction : public G4UserTrackingAction {
public:
	PropTrackingAction();
	~PropTrackingAction() override;

public:
	void PreUserTrackingAction(const G4Track* track) override;
	void PostUserTrackingAction(const G4Track* track) override;
	void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }
	G4double GetCulledEnergy() { return fCulledEnergy; }

private:
	G4double fKineticEnergyMin;
	G4double fCulledEnergy;
};
#endif //PROPTRACKINGACTION_H