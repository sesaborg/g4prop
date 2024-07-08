#ifndef G4PROP_HH
#define G4PROP_HH

#include "G4PropParticleTree.hh"
// Geant4 Includes
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "globals.hh"
#include "G4RunManager.hh"

#include "g4prop/PropTrackingAction.hh"
#include "g4prop/PropSteppingAction.hh"
#include "g4prop/PropStackingAction.hh"
#include "g4prop/PropRunAction.hh"
#include "g4prop/PropDetectorConstruction.hh"

using namespace std;

template <typename Tree,typename Particle>
void RunGeant4(const G4PropParticleTree<Tree,Particle> &particleTree) {
// For now ignore all args

	G4VModularPhysicsList* p = new FTFP_BERT(0);
	p->ReplacePhysics(new G4EmStandardPhysics_option4());

	auto* o = new G4OpticalPhysics();
//	o->SetMaxNumPhotonsPerStep(40);
//	o->SetMaxBetaChangePerStep(0.1);
//	o->SetTrackSecondariesFirst(kCerenkov,true);

	p->RegisterPhysics(o);


// Unsure if this should be some other engine/seed combo
	G4Random::setTheEngine(new CLHEP::RanecuEngine);
	G4Random::setTheSeed(12904);

	auto* runManager = new G4RunManager;
	runManager->SetUserInitialization(new PropDetectorConstruction());
	runManager->SetUserInitialization(p);
	runManager->SetUserAction(new PropSteppingAction(particleTree));
	runManager->SetUserAction(new PropStackingAction(particleTree));
	runManager->SetUserAction(new PropTrackingAction(particleTree));
	runManager->SetUserAction(new PropRunAction(particleTree));
}
#endif //G4PROP_HH