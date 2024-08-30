#include "g4prop/geant4/PropDetectorConstruction.hh"
#include "G4NistManager.hh"

#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"

using namespace G4Prop;

PropDetectorConstruction::PropDetectorConstruction() = default;

PropDetectorConstruction::PropDetectorConstruction(G4Material * material, G4double radius)
{
    fpMaterial = material;
    fRadius = radius;
}

G4Prop::PropDetectorConstruction::PropDetectorConstruction(G4VPhysicalVolume *worldVolume)
{
    fpWorldVolume = worldVolume;
}

PropDetectorConstruction::~PropDetectorConstruction() = default;

G4VPhysicalVolume *PropDetectorConstruction::Construct()
{
    if (fpWorldVolume != nullptr) {
        return fpWorldVolume;
    }
    auto nist = G4NistManager::Instance();
    nist->SetVerbose(0);

    G4Material *material;

    if (fpMaterial != nullptr) {
        material = fpMaterial;
    } else {
        // These are placeholder material properties! 
        G4Material *material = nist->FindOrBuildMaterial("G4_WATER");
        const size_t entries = 2;
        G4double PhotonEnergy[entries] = {1 * eV, 10 * eV};
        G4double RefractiveIndex[entries] = {1.3, 1.3};
        G4double AbsorptionLength[entries] = {50 * m, 50 * m};
        G4double ScatteringLength[entries] = {50 * m, 50 * m};
        auto *mp = new G4MaterialPropertiesTable();
        mp->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, entries);
        mp->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionLength, entries);
        mp->AddProperty("RAYLEIGH", PhotonEnergy, ScatteringLength, entries);
        material->SetMaterialPropertiesTable(mp);
    }

    auto *world = new G4Orb("d_World", fRadius == 0. ? 1000 * m : fRadius);
    auto *world_logic = new G4LogicalVolume(world, material, "d_World");

    G4VPhysicalVolume *world_phys = new G4PVPlacement(nullptr, G4ThreeVector(), world_logic,
                                                      "d_World", nullptr, false, 0, true);
    auto *lab = new G4Orb("Lab", fRadius == 0. ? 1000 * m : fRadius);
    auto *lab_logic = new G4LogicalVolume(lab, material, "Lab");
    new G4PVPlacement(nullptr, G4ThreeVector(), lab_logic, "Lab", world_logic, false, 0, true);
    return world_phys;
}