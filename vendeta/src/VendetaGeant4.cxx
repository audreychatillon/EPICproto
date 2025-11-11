// C++ headers
#include <cmath>
#include <limits>
#include <sstream>
// G4 Geometry object
#include "G4Box.hh"
#include "G4Tubs.hh"

// G4 sensitive
#include "G4MultiFunctionalDetector.hh"
#include "G4SDManager.hh"

// G4 various object
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"

// NPTool header
// #include "InteractionScorers.hh"
// #include "NPOptionManager.h"
// #include "NPSDetectorFactory.hh"
// #include "NPSHitsMap.hh"
// #include "VendetaGeant4.hh"
// s#include "PlasticBar.hh"
// #include "ProcessScorers.hh"
// CLHEP header
#include "CLHEP/Random/RandGauss.h"

#include "NPApplication.h"
#include "NPFunction.h"
#include "NPG4MaterialManager.h"
#include "NPG4Session.h"
#include "VendetaGeant4.h"
using namespace std;
using namespace CLHEP;
using namespace vendeta;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace vendeta {
 const string Material = "BC400";
  
    // Energy and time Resolution
  const double EnergyThreshold = 0.1*MeV;
  /* const double ResoTime = 0.454*ns ; // reso Cf */
  const double ResoTime = 0.61*ns ; // reso U8
  /* const double ResoTime = 0.*ns ; // reso U8 */
  const double ResoEnergy = 0.1*MeV ;
  const double Thickness = 51.*mm ;
  const double Radius = 127./2*mm ;
  
  ////////////////////////////////////////////////////////////////////////////////
  // Nebula Specific Method
  VendetaGeant4::VendetaGeant4() {
    nptool::message("green", "vendeta", "VendetaGeant4", "Initializing simulation");
    m_detector = std::dynamic_pointer_cast<vendeta::VendetaDetector>(
        nptool::Application::GetApplication()->GetDetector("vendeta"));
        
  m_Event = new vendeta::VendetaData() ;

  // RGB Color + Transparency
  m_VisAl      = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));   
  m_VisEJ309   = new G4VisAttributes(G4Colour(0.2, 0.85, 0.85, 1));   
  m_VisMuMetal = new G4VisAttributes(G4Colour(0.55, 0.5, 0.5, 0.7));   
  m_VisPyrex   = new G4VisAttributes(G4Colour(0.1, 0.5, 0.7, 1));   
  m_VisEJ560   = new G4VisAttributes(G4Colour(0.6, 0.6, 0.2, 1));   
  m_VisInox    = new G4VisAttributes(G4Colour(0.6, 0.5, 0.6, 1));   

  // Material definition
  //m_Vacuum  = MaterialManager::getInstance()->GetMaterialFromLibrary("Vacuum");
 // m_Al      = MaterialManager::getInstance()->GetMaterialFromLibrary("Al");
  //m_Inox    = MaterialManager::getInstance()->GetMaterialFromLibrary("Inox");
  //m_EJ309   = MaterialManager::getInstance()->GetMaterialFromLibrary("EJ309");
  //m_EJ560   = MaterialManager::getInstance()->GetMaerialFromLibrary("EJ560");
  //m_Pyrex   = MaterialManager::getInstance()->GetMaterialFromLibrary("Pyrex");
  //m_MuMetal = MaterialManager::getInstance()->GetMaterialFromLibrary("mumetal");
  }
  ////////////////////////////////////////////////////////////////////////////////
  VendetaGeant4::~VendetaGeant4() {}
  ////////////////////////////////////////////////////////////////////////////////
/*
G4LogicalVolume* Vendeta::BuildSensitiveCell(){

  if(!m_SensitiveCell){
    G4Tubs* scin_cell = new G4Tubs("Vendeta_scin", 0, Vendeta_NS::Radius, Vendeta_NS::Thickness*0.5, 0., 360*deg);
    m_SensitiveCell = new G4LogicalVolume(scin_cell,m_EJ309,"logic_Vendeta_scin",0,0,0);
    m_SensitiveCell->SetVisAttributes(m_VisEJ309);
   // m_SensitiveCell->SetSensitiveDetector(m_VendetaScorer);
  }

  return m_SensitiveCell;
}
*/
  // Construct detector and inialise sensitive part.
  // Called After DetecorConstruction::AddDetector Method
  void VendetaGeant4::ConstructDetector() {
    nptool::message("green", "vendeta", "VendetaGeant4", "Constructing Geometry");

    // FIXME add dummy wall until use of input file is made
    auto g4session = nptool::geant4::Session::GetSession();
    auto world = g4session->GetWorldLogicalVolume();

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  // Read sensitive part and fill the Root tree.
  // Called at in the EventAction::EndOfEventAvtion
  void VendetaGeant4::ReadSensitive(const G4Event*) {
    m_Event->Clear();
    /*
        ///////////
        // PlasticBar scorer
        PlasticBar::PS_PlasticBar* PlasticScorer_Module = (PlasticBar::PS_PlasticBar*)m_ModuleScorer->GetPrimitive(0);
        PlasticBar::PS_PlasticBar* PlasticScorer_Veto = (PlasticBar::PS_PlasticBar*)m_VetoScorer->GetPrimitive(0);
        // Should we put a ProcessScorer here to get the info if the particle is first neutron and give it to
       NebulaData
       ?

        double Time_up, Time_down;
        double Energy_tmp, Light_tmp;

        //////////// TRIAL TO GET THE OPTICAL INDEX FROM MATERIAL PROPERTIES /////////////
        // Trying to get Optical Index from Material directly
        // const G4Material* aMaterial =
       MaterialManager::GetMaterialManager()->GetMaterialFromLibrary(vendeta::Material);
        // G4MaterialPropertiesTable* aMaterialPropertiesTable = aMaterial->GetMaterialPropertiesTable();
        // if(!aMaterialPropertiesTable->PropertyExists("RINDEX")){
        //   MaterialIndex = !aMaterialPropertiesTable->GetConstProperty("RINDEX");
        // }
        // else{
        //   MaterialIndex = 0;
        // }
        // cout <<
        //
       MaterialManager::GetMaterialManager()->GetMaterialFromLibrary(vendeta::Material)->GetMaterialPropertiesTable()->GetMaterialPropertyNames()[0]
        // << endl;
        //////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////// MODULE SCORER //////////////////////////////////
        unsigned int ModuleHits_size = PlasticScorer_Module->GetMult();
        for (unsigned int i = 0; i < ModuleHits_size; i++) {
          vector<unsigned int> level = PlasticScorer_Module->GetLevel(i);
          Energy_tmp = PlasticScorer_Module->GetEnergy(i);
          Light_tmp = PlasticScorer_Module->GetLight(i);
          Energy = RandGauss::shoot(Energy_tmp, Energy_tmp * vendeta::ResoEnergy);
          Light = RandGauss::shoot(Light_tmp, Light_tmp * vendeta::ResoLight);

          if (Light > vendeta::LightThreshold) {
            int DetectorNbr = level[0];
            double Position = RandGauss::shoot(PlasticScorer_Module->GetPosition(i), vendeta::ResoPosition);

            m_Event->SetChargeUp(DetectorNbr,
                                 Light * exp(-(vendeta::ModuleHeight / 2 - Position) / vendeta::Attenuation));
            m_Event->SetChargeDown(DetectorNbr,
                                   Light * exp(-(vendeta::ModuleHeight / 2 + Position) /
       vendeta::Attenuation));

            // Take TOF and Position and compute Tup and Tdown
            double Time = RandGauss::shoot(PlasticScorer_Module->GetTime(i), vendeta::ResoTime);

            Time_up = (vendeta::ModuleHeight / 2 - Position) / (c_light / vendeta::MaterialIndex) + Time;
            m_Event->SetTimeUp(DetectorNbr, Time_up);

            Time_down = (vendeta::ModuleHeight / 2 + Position) / (c_light / vendeta::MaterialIndex) + Time;
            m_Event->SetTimeDown(DetectorNbr, Time_down);
          }
        }
        // cout << endl;

        ///////////////////////////////// VETO SCORER //////////////////////////////////
        unsigned int VetoHits_size = PlasticScorer_Veto->GetMult();
        for (unsigned int i = 0; i < VetoHits_size; i++) {
          vector<unsigned int> level = PlasticScorer_Veto->GetLevel(i);
          Energy_tmp = PlasticScorer_Veto->GetEnergy(i);
          Light_tmp = PlasticScorer_Veto->GetLight(i);
          Energy = RandGauss::shoot(Energy_tmp, Energy_tmp * vendeta::ResoEnergy);
          Light = RandGauss::shoot(Light_tmp, Light_tmp * vendeta::ResoLight);

          if (Light > vendeta::LightThreshold) {
            double Time = RandGauss::shoot(PlasticScorer_Veto->GetTime(i), vendeta::ResoTime);
            // cout << "Time is " << Time << endl;
            double Position = RandGauss::shoot(PlasticScorer_Veto->GetPosition(i), vendeta::ResoPosition);
            // cout << "Position is " << Position << endl;
            int DetectorNbr = level[0] + m_TotalModule;
            // cout << "Veto ID: " << DetectorNbr << endl;

            m_Event->SetChargeUp(DetectorNbr,
                                 Light * exp(-(vendeta::VetoHeight / 2 - Position) / vendeta::Attenuation));
            m_Event->SetChargeDown(DetectorNbr,
                                   Light * exp(-(vendeta::VetoHeight / 2 + Position) / vendeta::Attenuation));

            Time_up = (vendeta::VetoHeight / 2 - Position) / (c_light / vendeta::MaterialIndex) + Time;
            // cout << "Time_up is " << Time_up << endl;
            m_Event->SetTimeUp(DetectorNbr, Time_up);

            Time_down = (vendeta::VetoHeight / 2 + Position) / (c_light / vendeta::MaterialIndex) + Time;
            // cout << "Time_down is " << Time_down << endl;
            m_Event->SetTimeDown(DetectorNbr, Time_down);
          }
        }
    */
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  ////////////////////////////////////////////////////////////////
  void VendetaGeant4::InitializeScorers() {
    /*    // This check is necessary in case the geometry is reloaded
        bool already_exist = false;
        m_ModuleScorer = CheckScorer("NebulaModuleScorer", already_exist);
        m_VetoScorer = CheckScorer("NebulaVetoScorer", already_exist);

        if (already_exist)
          return;

        // Otherwise the scorer is initialise
        // Module
        vector<int> level;
        level.push_back(0);
        G4VPrimitiveScorer* ModulePlasticBar = new PlasticBar::PS_PlasticBar("ModulePlasticBar", level, 0);
        G4VPrimitiveScorer* ModuleInteraction =
            new InteractionScorers::PS_Interactions("ModuleInteraction", ms_InterCoord, 0);
        G4VPrimitiveScorer* ModuleProcess = new ProcessScorers::PS_Process("ModuleProcess", 0);
        // and register it to the multifunctionnal detector
        m_ModuleScorer->RegisterPrimitive(ModulePlasticBar);
        m_ModuleScorer->RegisterPrimitive(ModuleInteraction);
        m_ModuleScorer->RegisterPrimitive(ModuleProcess);
        G4SDManager::GetSDMpointer()->AddNewDetector(m_ModuleScorer);

        // Veto
        G4VPrimitiveScorer* VetoPlasticBar = new PlasticBar::PS_PlasticBar("VetoPlasticBar", level, 0);
        G4VPrimitiveScorer* VetoInteraction = new InteractionScorers::PS_Interactions("VetoInteraction",
      ms_InterCoord, 0); G4VPrimitiveScorer* VetoProcess = new ProcessScorers::PS_Process("ModuleProcess", 0);
        // and register it to the multifunctionnal detector
        m_VetoScorer->RegisterPrimitive(VetoPlasticBar);
        m_VetoScorer->RegisterPrimitive(VetoInteraction);
        m_VetoScorer->RegisterPrimitive(VetoProcess);
        G4SDManager::GetSDMpointer()->AddNewDetector(m_VetoScorer);
    */
  }
  ////////////////////////////////////////////////////////////////////////////////
  extern "C" {
  std::shared_ptr<nptool::geant4::VDetector> ConstructDetectorSimulation() {
    return make_shared<vendeta::VendetaGeant4>();
  };
  }
} // namespace vendeta
