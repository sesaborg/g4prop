#ifndef PROPEVENTACTION_H
#define PROPEVENTACTION_H

#include "G4UserEventAction.hh"
#include "G4RichTrajectory.hh"

namespace G4Prop
{
    class PropEventAction : public G4UserEventAction
    {
    public:
        PropEventAction() : G4UserEventAction() {};
        ~PropEventAction() override {};

        void EndOfEventAction(const G4Event *anEvent) override
        {
            // fpEventManager->KeepTheCurrentEvent();

            auto vect = anEvent->GetTrajectoryContainer()->GetVector();
            for (G4VTrajectory *&j : *vect)
            {
                G4RichTrajectory &test = *(G4RichTrajectory *)j;
                fTrajectoryVector.push_back(boost::shared_ptr<G4RichTrajectory>(new G4RichTrajectory(test)));
            }
        }

        std::vector<boost::shared_ptr<G4RichTrajectory>> &GetTrajectories() { return fTrajectoryVector; }

    private:
        std::vector<boost::shared_ptr<G4RichTrajectory>> fTrajectoryVector = std::vector<boost::shared_ptr<G4RichTrajectory>>();
    };
}

#endif // PROPEVENTACTION_H