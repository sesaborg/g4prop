#include "G4UserRunAction.hh"
#include "G4OpticalPhoton.hh"

#ifndef PROPRUNACTION_H
#define PROPRUNACTION_H

class PropRunAction : public G4UserRunAction {
public:
    PropRunAction();
    ~PropRunAction() override;
    void BeginOfRunAction(const G4Run *aRun) override;
    void EndOfRunAction(const G4Run *aRun) override;
};
#endif //PROPRUNACTION_H