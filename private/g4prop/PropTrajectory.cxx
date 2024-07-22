#include "g4prop/PropTrajectory.cxx"

G4Allocator<PropTrajectory> *&aPropTrajectoryAllocator()
{
    G4ThreadLocalStatic G4Allocator<PropTrajectory> *_instance = nullptr;
    return _instance;
}

PropTrajectory::PropTrajectory(const G4Track *aTrack)
    : G4Trajectory(aTrack)
{
    fFinalKineticEnergy = aTrack->GetKineticEnergy();

    // // Insert the first rich trajectory point (see note above)...
    // //
    // fpRichPointsContainer = new RichTrajectoryPointsContainer;
    // fpRichPointsContainer->push_back(new G4RichTrajectoryPoint(aTrack));
}

PropTrajectory::PropTrajectory(PropTrajectory &right) : G4Trajectory(right)
{

    fFinalKineticEnergy = right.fFinalKineticEnergy;
}

PropTrajectory::~PropTrajectory()
{
}

void PropTrajectory::AppendStep(const G4Step *aStep)
{
    const G4Track *track = aStep->GetTrack();
    const G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
    if (track->GetCurrentStepNumber() > 0)
    {
        fFinalKineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy() - aStep->GetTotalEnergyDeposit();
    }
}

void PropTrajectory::MergeTrajectory(G4VTrajectory *secondTrajectory)
{
    if (secondTrajectory == nullptr)
        return;

    auto seco = (PropTrajectory *)secondTrajectory;
    G4int ent = seco->GetPointEntries();
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

#ifdef G4ATTDEBUG
    G4cout << G4AttCheck(values, GetAttDefs());
#endif

    return values;
}
