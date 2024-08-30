#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"

#ifndef PROPDETECTORCONSTRUCTION_H
#define PROPDETECTORCONSTRUCTION_H
namespace G4Prop
{
    class PropDetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        /// @brief Default Constructor, causes the detector volume to be a 1km radius orb of G4_WATER with placeholder optical properties when Construct() is called. The orb is placed at the origin
        PropDetectorConstruction();

        /// @brief Specify the material and radius to be used in the default world volume.
        /// @param material Pointer to your special material.
        /// @param radius The radius of the world orb, in meters
        PropDetectorConstruction(G4Material* material, G4double radius = 1000.0 * CLHEP::m);

        /// @brief Provide your own world volume, bypassing the construction of a default one. This should probably be the only option moving forward.
        /// @param worldVolume 
        PropDetectorConstruction(G4VPhysicalVolume* worldVolume);
        ~PropDetectorConstruction() override;

        /// @brief Constructs the "detector" volume. For now, this is a large orb of a single material (water/ice) which has the initial particle placed at the center.
        /// @return The physical volume representing the simulation world
        G4VPhysicalVolume *Construct() override;
    private:
        G4Material* fpMaterial = nullptr;
        G4VPhysicalVolume* fpWorldVolume = nullptr;
        G4double fRadius = 0.;
    };
}
#endif // PROPDETECTORCONSTRUCTION_H