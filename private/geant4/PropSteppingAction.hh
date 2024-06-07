#include "G4UserSteppingAction.hh"
#include "globals.hh"

class PropSteppingAction : public G4UserSteppingAction {
public:
	PropSteppingAction();
	~PropSteppingAction();

public:
	virtual void UserSteppingAction(const G4Step* step);
	void SetMinimumKineticEnergy(G4double kineticEnergyMin) { fKineticEnergyMin = kineticEnergyMin; }
	G4double GetCulledEnergy() { return fCulledEnergy; }

private:
	G4double fKineticEnergyMin;
	G4double fCulledEnergy;
}
