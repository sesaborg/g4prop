.. _g4prop-main:

g4prop
===============

This project was started by Matthew Thomson (mdthomso@ualberta.ca)

About
-----

g4prop is an implementation of Geant4 used for propagating the high energy byproducts of even 
higher energy events. The unique portion of g4prop is the kinetic energy cutoff that scales 
with the kinetic energy of each primary particle. This was done to limit the amount of time
Geant4 spends simulating all the interactions in a shower, since g4prop is intended to be primarily used
to develop beginning of the shower.

g4prop requires Geant4 (ideally with trajectory storing enabled) to be installed. To use
g4prop in a simulation, all you need is an event generator that produces an I3MCTree. This MCTree
will be consumed by g4prop and the childless particles in the tree will be propagated to the best of 
Geant4's abilities. Note that the most widely used Geant4 physics lists have a hard cutoff of 100 TeV,
and Geant4 will crash if it is passed a particle with a higher kinetic energy. For now (without expanding 
the physics lists beyond 100 TeV), this is an unavoidable limit.

As of 2024-08-30, g4prop has not been thoroughly tested and probably has a significant number of lurking bugs.
This module requires a fair amount more work before it can be used in simulations (for example, the random service isn't customizable at all!).
Also, adding some good pybindings would be nice. Any and all improvements are welcome!

Parameters
^^^^^^^^^^

**Geant4Commands**:

  Vector of strings to be passed to the Geant4 ui. The default set of commands is:
  {
    "/process/optical/cerenkov/setMaxPhotons 20",
    "/process/optical/cerenkov/setMaxBetaChange 40",
    "/tracking/verbose 0",
    "/process/optical/cerenkov/setTrackSecondariesFirst false"
  }

**RelativeCutoff**:

    The simple fractional cutoff used to limit the size of the shower that Geant4 produces.
    Given a particle from your event generator's MCTree, g4prop will propagate particles down
    to kinetic energies greater than the initial particle's energy times the relative cutoff.
    So, if you have a 100 TeV particle and 1 TeV particle in your MCTree, g4prop will have
    two separate cutoffs as it simulates these particles separately and individually. Default value of 0.001

**G4StoreTrajectory**:

    An integer enumeration that is used by g4prop/Geant4. It controls how tracks/trajectories are stored.
    If G4StoreTrajectory is 0, g4prop uses a hacky method to copy the transient track data for use in constructing the new MCTree. Trajectories are not stored.
    If G4StoreTrajectory is 3, g4prop uses Geant4's trajectory-storing methods with a custom implementation of the trajectory class. Tracks are not stored.
    Normally, if G4StoreTrajectory was 1 or 2, Geant4 would use default trajectory classes. However, g4prop's implementation doesn't currently allow this.