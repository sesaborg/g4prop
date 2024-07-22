#ifndef G4PROP_HH
#define G4PROP_HH

// #include "G4PropParticleTree.hh"
// Geant4 Includes
#include "QGSP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "globals.hh"
#include "G4RunManager.hh"

#include "g4prop/PropTrackingAction.hh"
#include "g4prop/PropSteppingAction.hh"
#include "g4prop/PropStackingAction.hh"
#include "g4prop/PropRunAction.hh"
#include "g4prop/PropDetectorConstruction.hh"
#include "g4prop/PropPrimaryGeneratorAction.hh"
#include "g4prop/PropEventAction.hh"
#include "G4ParticleGun.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include <boost/foreach.hpp>

using namespace std;

namespace G4Prop
{
	G4RunManager *runManager = nullptr;
	G4UImanager *uiManager = nullptr;
	G4VisManager *visManager = nullptr;
	CLHEP::HepRandomEngine *randomEngine = nullptr;
	G4double relativeCutoff = 0;

	/// @note @ref ConfigureGeant4() needs to be called before initializing or running Geant4.
	/// @param particleGun The G4ParticleGun to have shot.
	/// @param commandList The UI commands to pass to Geant4.
	/// @param storeTrajectory A G4int specifying how trajectories should be stored.
	void RunGeant4(G4ParticleGun *particleGun, std::vector<std::string> commandList, G4int storeTrajectory)
	{
		G4cout << "In!" << G4endl;
		// // For now ignore all args

		// G4VModularPhysicsList *p = new FTFP_BERT(0);
		// p->ReplacePhysics(new G4EmStandardPhysics_option4());

		// auto *o = new G4OpticalPhysics();
		// //	o->SetMaxNumPhotonsPerStep(40);
		// //	o->SetMaxBetaChangePerStep(0.1);
		// //	o->SetTrackSecondariesFirst(kCerenkov,true);

		// p->RegisterPhysics(o);

		// // Unsure if this should be some other engine/seed combo
		// G4Random::setTheEngine(new CLHEP::RanecuEngine);
		// G4Random::setTheSeed(12904);

		// auto *runManager = new G4RunManager;
		// runManager->SetUserInitialization(new PropDetectorConstruction());
		// runManager->SetUserInitialization(p);
		// runManager->SetUserAction(new PropSteppingAction(particleTree));
		// runManager->SetUserAction(new PropStackingAction(particleTree));
		// runManager->SetUserAction(new PropTrackingAction(particleTree));
		// runManager->SetUserAction(new PropRunAction(particleTree));

		runManager->SetUserAction(new PropPrimaryGeneratorAction(particleGun));
		runManager->SetUserAction(new PropSteppingAction(particleGun->GetParticleEnergy() * relativeCutoff));
		runManager->SetUserAction(new PropStackingAction());
		runManager->SetUserAction(new PropTrackingAction(particleGun->GetParticleEnergy() * relativeCutoff, storeTrajectory));
		runManager->SetUserAction(new PropRunAction());
		runManager->SetUserAction(new PropEventAction());
		runManager->SetNumberOfEventsToBeStored(1);

		BOOST_FOREACH (std::string &command, commandList)
		{
			uiManager->ApplyCommand(G4String(command));
			G4cout << command << G4endl;
		}
		// exit(-25);
		// uiManager->ApplyCommand("/run/verbose 3");
		// uiManager->ApplyCommand("/event/verbose 1");
		// uiManager->ApplyCommand("/tracking/verbose 2");
		// uiManager->ApplyCommand("/process/setVerbose 0 all");
		// uiManager->ApplyCommand("/process/verbose 0");
		// uiManager->ApplyCommand("/process/had/verbose 0");
		uiManager->ApplyCommand("/run/initialize");
		uiManager->ApplyCommand("/run/beamOn");
	}

	/// @brief
	/// @param cutoff
	void ConfigureGeant4(G4double cutoff)
	{
		relativeCutoff = cutoff;

		G4cout << "Deleting runManager" << G4endl;
		delete runManager;

		G4cout << "Deleting visManager" << G4endl;
		delete visManager;

		delete randomEngine;

		G4VModularPhysicsList *p = new QGSP_BERT();
		p->ReplacePhysics(new G4EmStandardPhysics_option4());

		G4cout << "Making Optical Physics" << G4endl;

		// auto *o = new G4OpticalPhysics();
		G4cout << "Registering Optical Physics" << G4endl;
		// p->RegisterPhysics(o);
		G4cout << "Setting Random Engine" << G4endl;
		// Unsure if this should be some other engine/seed combo

		randomEngine = new CLHEP::RanecuEngine;
		G4Random::setTheEngine(randomEngine);
		G4Random::setTheSeed(12904);

		G4cout << "Making New G4RunManager" << G4endl;
		runManager = new G4RunManager;
		G4cout << "Setting User Initialization" << G4endl;
		runManager->SetUserInitialization(new PropDetectorConstruction());
		runManager->SetUserInitialization(p);
		runManager->Initialize();

		uiManager = G4UImanager::GetUIpointer();

		visManager = new G4VisExecutive;
		visManager->Initialize();
	}

	std::vector<G4Track *> GetTracks()
	{
		return ((PropTrackingAction *)(runManager->GetUserTrackingAction()))->GetTracks();
	}

	std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &GetTrackToSecondariesMap()
	{
		return ((PropTrackingAction *)(runManager->GetUserTrackingAction()))->GetTrackToSecondariesMap();
	}

	std::vector<boost::shared_ptr<PropTrajectory>> &GetTrajectories()
	{
		return ((PropEventAction *)(runManager->GetUserEventAction()))->GetTrajectories();
	}

	// G4RunManager *GetG4RunManager() { return runManager; }
	// G4UImanager *GetG4UImanager() { return uiManager; }
	// G4VisManager *GetG4VisManager() { return visManager; }
};
namespace G4Prop
{
	// Construct this with the specified CHLEP units
	struct G4PropParticleProperties
	{
		int32_t Id;
		int32_t EncodingPDG;
		double Energy;					 // CLHEP::GeV
		G4ThreeVector Position;			 // CLHEP::mm
		G4ThreeVector MomentumDirection; // Unitless
		double Azimuth;					 // CLHEP::deg
		double Zenith;					 // CLHEP::deg
		double Time;					 // CHLEP::ns
	};

	void LoadParticleGuns(std::vector<G4PropParticleProperties> particlesToShoot, std::vector<G4ParticleGun *> &particleGuns)
	{
		for (size_t i = 0; i < particlesToShoot.size(); i++)
		{
			auto particle = particlesToShoot[i];
			G4cout << particlesToShoot[i].Id << G4endl;
			G4cout << G4ParticleTable::GetParticleTable()->FindParticle(particlesToShoot[i].EncodingPDG)->GetParticleName() << G4endl;

			G4ParticleGun *source = new G4ParticleGun();
			source->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(particle.EncodingPDG));
			source->SetNumberOfParticles(1);
			source->SetParticleEnergy(particle.Energy);
			source->SetParticleTime(particle.Time);
			// Instead of setting the particle where it should belong w.r.t to the external environment, just put it in the middle of a generic volume.
			source->SetParticlePosition(G4ThreeVector(0, 0, 0));
			source->SetParticleMomentumDirection(particle.MomentumDirection);

			particleGuns.insert(particleGuns.end(), source);
		}
	}

	void LoadParticleGun(G4PropParticleProperties particleToShoot, G4ParticleGun *&particleGun)
	{
		auto particle = particleToShoot;
		G4cout << particleToShoot.Id << G4endl;
		G4cout << G4ParticleTable::GetParticleTable()->FindParticle(particleToShoot.EncodingPDG)->GetParticleName() << G4endl;

		G4ParticleGun *source = new G4ParticleGun();
		source->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(particle.EncodingPDG));
		source->SetNumberOfParticles(1);
		source->SetParticleEnergy(particle.Energy);
		source->SetParticleTime(particle.Time);
		// Instead of setting the particle where it should belong w.r.t to the external environment, just put it in the middle of a generic volume.
		source->SetParticlePosition(G4ThreeVector(0, 0, 0));
		source->SetParticleMomentumDirection(particle.MomentumDirection);

		particleGun = source;
	}
}
#endif // G4PROP_HH