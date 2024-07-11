#ifndef PROPSTEPPINGACTION_H
#define PROPSTEPPINGACTION_H
#include "G4UserSteppingAction.hh"
#include "globals.hh"

namespace G4Prop
{
	class PropSteppingAction : public G4UserSteppingAction
	{
	public:
		PropSteppingAction();
		PropSteppingAction(G4double initialParticleEnergy);
		~PropSteppingAction() override;

	public:
		void UserSteppingAction(const G4Step *step) override;
		void SetMinimumKineticEnergy(G4double kineticEnergyMin) { fKineticEnergyMin = kineticEnergyMin; }
		G4double GetCulledEnergy() { return fCulledEnergy; }

	private:
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
	};
}
#endif // PROPSTEPPINGACTION_H