i3_project(g4prop
        PYTHON_DIR python
        DOCS_DIR resources/docs
)

SET(LIB_${PROJECT_NAME}_SOURCEFILES

        # Geant4 Files
        private/g4prop/geant4/PropDetectorConstruction.cxx
        private/g4prop/geant4/PropRunAction.cxx
        private/g4prop/geant4/PropStackingAction.cxx
        private/g4prop/geant4/PropSteppingAction.cxx
        private/g4prop/geant4/PropTrajectory.cxx

        # Icetray-specific bindings
        private/g4prop/I3G4PropModule.cxx
)

SET(LIB_${PROJECT_NAME}_PYBINDINGS_SOURCEFILES
        private/pybindings/module.cxx
)

SET(LIB_${PROJECT_NAME}_PROJECTS
        serialization icetray dataclasses phys-services simclasses sim-services
)

SET(LIB_${PROJECT_NAME}_TOOLS python boost clhep geant4)

i3_add_library(${PROJECT_NAME}
        ${LIB_${PROJECT_NAME}_SOURCEFILES}
        USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
        USE_PROJECTS ${LIB_${PROJECT_NAME}_PROJECTS}
)