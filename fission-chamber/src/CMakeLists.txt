# for data analysis and conversion
add_library(NPFissionChamber SHARED FissionChamberDetector.h FissionChamberDetector.cxx FissionChamberData.h FissionChamberData.cxx FissionChamberPhysics.h FissionChamberSpectra.cxx FissionChamberSpectra.h SignalProcessorFC.h SignalProcessorFC.cpp)
target_link_libraries(NPFissionChamber PUBLIC ROOT::RIO ROOT::Tree ROOT::Gpad ROOT::Physics ROOT::Imt NPRoot NPPhysicsEssential nptool )

if(FASTERAC_FOUND)
  target_compile_definitions(NPFissionChamber PUBLIC FASTERAC_FOUND)
  target_link_libraries(NPFissionChamber PUBLIC fasterac )
endif()

# if(PHYSICSESSENTIAL_FOUND)
#   target_compile_definitions(NPFissionChamber PUBLIC PHYSICSESENTIAL_FOUND)
#   target_link_libraries(NPFissionChamber PUBLIC physics-essential )
# endif()

# for simulation
if(Geant4_FOUND)
add_library(NPFissionChamberGeant4 SHARED FissionChamberGeant4.h FissionChamberGeant4.cxx)
target_link_libraries(NPFissionChamberGeant4 PUBLIC NPFissionChamber NPGeant4 nptool)
endif()

install(DIRECTORY ./ DESTINATION ${CMAKE_INCLUDE_OUTPUT_DIRECTORY} FILES_MATCHING PATTERN "*.h" )
ROOT_GENERATE_DICTIONARY(FissionChamberDict FissionChamberData.h FissionChamberPhysics.h DEPENDENCIES FissionChamberData.h FissionChamberPhysics.h LINKDEF FissionChamberlinkdef.hh MODULE NPFissionChamber)
