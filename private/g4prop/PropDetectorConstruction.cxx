#include "PropDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"

PropDetectorConstruction::PropDetectorConstruction() = default;

PropDetectorConstruction::~PropDetectorConstruction() = default;

G4VPhysicalVolume *PropDetectorConstruction::Construct() {
    auto nist = G4NistManager::Instance();
    nist->SetVerbose(4);
    G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
    const size_t entries = 2;
    G4double PhotonEnergy[entries] = { 1 * eV, 10 * eV };
    G4double RefractiveIndex[entries] = { 1.3, 1.3 };
    G4double AbsorptionLength[entries] = { 50 * m, 50 * m };
    G4double ScatteringLength[entries] = { 50 * m, 50 * m };
    auto* wp = new G4MaterialPropertiesTable();
    wp->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, entries);
    wp->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionLength, entries);
    wp->AddProperty("RAYLEIGH", PhotonEnergy, ScatteringLength, entries);
    water->SetMaterialPropertiesTable(wp);

    //G4Orb* world = new G4Orb("d_World", var["WorldRadius"] * m);
    auto* world = new G4Orb("d_World", 100 * m);
    auto* world_logic = new G4LogicalVolume(world, water, "d_World");
    //fLimit = new G4UserLimits();
    //fLimit->SetUserMinEkine(1.*MeV);
    //world_logic->SetUserLimits(fLimit);

    G4VPhysicalVolume* world_phys = new G4PVPlacement(nullptr, G4ThreeVector(), world_logic,
                                                      "d_World", nullptr, false, 0, true);
    auto* lab = new G4Orb("Lab", 100 * m);
    auto* lab_logic = new G4LogicalVolume(lab, water, "Lab");
    new G4PVPlacement(nullptr, G4ThreeVector(), lab_logic, "Lab", world_logic, false, 0, true);
    return world_phys;
}