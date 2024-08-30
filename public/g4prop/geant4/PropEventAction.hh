#ifndef PROPEVENTACTION_H
#define PROPEVENTACTION_H

#include "G4UserEventAction.hh"
#include "G4RichTrajectory.hh"

namespace G4Prop
{
    /// @brief A G4UserEventAction that constructs the trajectory vector after an event, if configured to do so.
    class PropEventAction : public G4UserEventAction
    {
    public:
        PropEventAction() : G4UserEventAction() {};
        ~PropEventAction() override
        {
            // G4cout << "PropEventAction has been deleted." << G4endl;
            // exit(-25);
        };

        /// @brief Builds the trajectory vector after an event.
        /// @param anEvent 
        void EndOfEventAction(const G4Event *anEvent) override
        {
            // fpEventManager->KeepTheCurrentEvent();

            G4cout << "Building fTrajectoryVector" << G4endl;
            auto vect = anEvent->GetTrajectoryContainer()->GetVector();
            for (G4VTrajectory *&j : *vect)
            {
                PropTrajectory &test = *(PropTrajectory *)j;
                fTrajectoryVector.push_back(boost::shared_ptr<PropTrajectory>(new PropTrajectory(test)));
            }
        }

        std::vector<boost::shared_ptr<PropTrajectory>> &GetTrajectories() { return fTrajectoryVector; }

        std::vector<boost::shared_ptr<PropTrajectory>> fTrajectoryVector = std::vector<boost::shared_ptr<PropTrajectory>>();
    };
}

#endif // PROPEVENTACTION_H