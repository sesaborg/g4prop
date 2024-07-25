#include "g4prop/geant4/PropTrajectory.hh"
#include "G4AttDef.hh"
#include "G4AttDefStore.hh"
#include "G4AttValue.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryPoint.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"

G4Allocator<PropTrajectory> *&aPropTrajectoryAllocator()
{
    G4ThreadLocalStatic G4Allocator<PropTrajectory> *_instance = nullptr;
    return _instance;
}

PropTrajectory::PropTrajectory(const G4Track *aTrack)
    : G4Trajectory(aTrack)
{
    fFinalKineticEnergy = aTrack->GetKineticEnergy(); // Ok maybe this is supposed to be fInitialKineticEnergy...

    fpPointsContainer = new std::vector<G4VTrajectoryPoint *>;
    fpPointsContainer->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));
}

PropTrajectory::PropTrajectory(PropTrajectory &right) : G4Trajectory(right)
{
    fFinalKineticEnergy = right.fFinalKineticEnergy;
    fpPointsContainer = new std::vector<G4VTrajectoryPoint *>;
    for (auto &i : *right.fpPointsContainer)
    {
        auto rightPoint = (G4TrajectoryPoint *)i;
        fpPointsContainer->push_back(new G4TrajectoryPoint(*rightPoint));
    }
}

PropTrajectory::~PropTrajectory()
{
    if (fpPointsContainer != nullptr)
    {
        for (auto &i : *fpPointsContainer)
        {
            delete i;
        }
        fpPointsContainer->clear();
        delete fpPointsContainer;
    }
}

void PropTrajectory::AppendStep(const G4Step *aStep)
{
    fpPointsContainer->push_back(new G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition()));
    const G4Track *track = aStep->GetTrack();
    const G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
    if (track->GetCurrentStepNumber() > 0)
    {
        fFinalKineticEnergy = postStepPoint->GetKineticEnergy(); // aStep->GetPreStepPoint()->GetKineticEnergy() - aStep->GetTotalEnergyDeposit();
        fFinalMomentumDirection = postStepPoint->GetMomentumDirection();
        fFinalGlobalTime = postStepPoint->GetGlobalTime();
    }
}

void PropTrajectory::MergeTrajectory(G4VTrajectory *secondTrajectory)
{
    if (secondTrajectory == nullptr)
        return;

    auto seco = (PropTrajectory *)secondTrajectory;
    G4int ent = seco->GetPointEntries();
    for (G4int i = 1; i < ent; ++i)
    {
        // initial point of the second trajectory should not be merged
        //
        fpPointsContainer->push_back((*(seco->fpPointsContainer))[i]);
    }
    delete (*seco->fpPointsContainer)[0];
    seco->fpPointsContainer->clear();
}

void PropTrajectory::ShowTrajectory(std::ostream &os) const
{
    G4VTrajectory::ShowTrajectory(os);
}

void PropTrajectory::DrawTrajectory() const
{
    G4VTrajectory::DrawTrajectory();
}

const std::map<G4String, G4AttDef> *PropTrajectory::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String, G4AttDef> *store = G4AttDefStore::GetInstance("PropTrajectory", isNew);
    if (isNew)
    {
        // Get att defs from base class...
        //
        *store = *(G4Trajectory::GetAttDefs());

        G4String ID;

        ID = "FKE";
        (*store)[ID] = G4AttDef(ID, "Final kinetic energy", "Physics", "G4BestUnit", "G4double");
    }

    return store;
}

static G4String Path(const G4TouchableHandle &th)
{
    std::ostringstream oss;
    G4int depth = th->GetHistoryDepth();
    for (G4int i = depth; i >= 0; --i)
    {
        oss << th->GetVolume(i)->GetName() << ':' << th->GetCopyNumber(i);
        if (i != 0)
            oss << '/';
    }
    return oss.str();
}

std::vector<G4AttValue> *PropTrajectory::CreateAttValues() const
{
    // Create base class att values...
    std::vector<G4AttValue> *values = G4Trajectory::CreateAttValues();

    values->push_back(G4AttValue("FKE", G4BestUnit(fFinalKineticEnergy, "Energy"), ""));

    return values;
}
