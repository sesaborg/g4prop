#include "g4prop/geant4/PropStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

using namespace G4Prop;

G4ClassificationOfNewTrack PropStackingAction::ClassifyNewTrack(const G4Track *track) { return fUrgent; }

PropStackingAction::PropStackingAction() : G4UserStackingAction() {}
PropStackingAction::~PropStackingAction() = default;
