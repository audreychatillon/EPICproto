# for data analysis and conversion
add_library(NPVendeta SHARED VendetaDetector.h VendetaDetector.cxx VendetaData.h VendetaData.cxx VendetaPhysics.h SignalProcessor.h SignalProcessor.cpp VendetaSpectra.cxx VendetaSpectra.h)
target_link_libraries(NPVendeta PUBLIC ROOT::RIO ROOT::Tree ROOT::Physics ROOT::Imt ROOT::Hist ROOT::Gpad NPRoot nptool )

if(FASTERAC_FOUND)
  target_compile_definitions(NPVendeta PUBLIC FASTERAC_FOUND)
  target_link_libraries(NPVendeta PUBLIC fasterac )
endif()

# for simulation
if(Geant4_FOUND)
add_library(NPVendetaGeant4 SHARED VendetaGeant4.h VendetaGeant4.cxx)
target_link_libraries(NPVendetaGeant4 PUBLIC NPVendeta NPGeant4 nptool)
endif()

install(DIRECTORY ./ DESTINATION ${CMAKE_INCLUDE_OUTPUT_DIRECTORY} FILES_MATCHING PATTERN "*.h" )
ROOT_GENERATE_DICTIONARY(VendetaDict VendetaData.h VendetaPhysics.h DEPENDENCIES VendetaData.h VendetaPhysics.h LINKDEF Vendetalinkdef.hh MODULE NPVendeta)
