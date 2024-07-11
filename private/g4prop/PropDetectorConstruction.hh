#include "G4VUserDetectorConstruction.hh"

#ifndef PROPDETECTORCONSTRUCTION_H
#define PROPDETECTORCONSTRUCTION_H
namespace G4Prop
{
    class PropDetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        PropDetectorConstruction();
        ~PropDetectorConstruction() override;

        G4VPhysicalVolume *Construct() override;
    };
}
#endif // PROPDETECTORCONSTRUCTION_H