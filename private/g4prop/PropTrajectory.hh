#ifndef PROPTRAJECTORY_HH
#define PROPTRAJECTORY_HH
#include "G4TouchableHandle.hh"
#include "G4Trajectory.hh"
#include "G4ThreeVector.hh"
#include "trkgdefs.hh"

class PropTrajectory : public G4Trajectory
{
public:
    // Constructors/destructor
    //
    PropTrajectory() = default;
    PropTrajectory(const G4Track *aTrack);
    ~PropTrajectory() override;
    PropTrajectory(PropTrajectory &);
    PropTrajectory &operator=(const PropTrajectory &) = delete;

    G4bool operator==(const PropTrajectory &r) const { return (this == &r); }

    inline void *operator new(size_t);
    inline void operator delete(void *);

    // Other (virtual) member functions
    //
    void ShowTrajectory(std::ostream &os = G4cout) const override;
    void DrawTrajectory() const override;
    void AppendStep(const G4Step *aStep) override;
    void MergeTrajectory(G4VTrajectory *secondTrajectory) override;
    inline G4int GetPointEntries() const override;
    inline G4VTrajectoryPoint *GetPoint(G4int i) const override;

    inline G4double GetFinalKineticEnergy() const { return fFinalKineticEnergy; }
    inline G4ThreeVector GetFinalMomentumDirection() const { return fFinalMomentumDirection; }
    inline G4ThreeVector GetFinalPosition() const { return (*fpPointsContainer->end())->GetPosition(); }
    inline G4double GetFinalGlobalTime() const { return fFinalGlobalTime; }
    inline G4TrajectoryPoint *GetFinalPoint() const { return (G4TrajectoryPoint *)(*fpPointsContainer->end()); }

    // Get methods for HepRep style attributes
    //
    const std::map<G4String, G4AttDef> *GetAttDefs() const override;
    std::vector<G4AttValue> *CreateAttValues() const override;

private:
    std::vector<G4VTrajectoryPoint *> *fpPointsContainer = nullptr;
    G4double fFinalKineticEnergy = 0.0;
    G4ThreeVector fFinalMomentumDirection = G4ThreeVector(0, 0, 0);
    G4double fFinalGlobalTime = 0.0;
    G4int fTrackID = 0;
    G4int fParentID = 0;
};

extern G4TRACKING_DLL G4Allocator<PropTrajectory> *&aPropTrajectoryAllocator();

inline void *PropTrajectory::operator new(size_t)
{
    if (aPropTrajectoryAllocator() == nullptr)
    {
        aPropTrajectoryAllocator() = new G4Allocator<PropTrajectory>;
    }
    return (void *)(new G4Allocator<PropTrajectory>)->MallocSingle();
}

inline void PropTrajectory::operator delete(void *aPropTrajectory)
{
    aPropTrajectoryAllocator()->FreeSingle((PropTrajectory *)aPropTrajectory);
}

inline G4int PropTrajectory::GetPointEntries() const
{
    return G4int(fpPointsContainer->size());
}

inline G4VTrajectoryPoint *PropTrajectory::GetPoint(G4int i) const
{
    return (*fpPointsContainer)[i];
}
#endif // PROPTRAJECTORY_HH