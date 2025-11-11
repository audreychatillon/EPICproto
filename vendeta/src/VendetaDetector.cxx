// Main header
#include "VendetaDetector.h"
#include "VendetaSpectra.h"
//#include "NPParticle.h"
// nptool core
#include "NPApplication.h"
#include "NPCalibrationManager.h"
#include "NPException.h"
#include "NPFunction.h"
#include "NPRootPlugin.h"

#include <TCanvas.h>

// For faster data conversion
#ifdef FASTERAC_FOUND
#include "fasterac/fasterac.h" // mandatory for any data type
#include "fasterac/group.h"    // group
#include "fasterac/qdc.h"      // qdc, qdc_counter
#include "fasterac/utils.h"    // some specific tools
#include "fasterac/sampling.h" // sampler
#include "fasterac/sampler.h"  // sampler counter
#endif
#include "SignalProcessor.h"

// For Geant4 Simulation
#ifdef Geant4_FOUND
/* #include "VendetaGeant4.h" */
#endif

// std
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace vendeta;
using namespace std;
using namespace ROOT::Math;

////////////////////////////////////////////////////////////////////////////////
VendetaDetector::VendetaDetector() {
  m_RawData = new ::VendetaData();
  m_Physics = new ::VendetaPhysics();

  m_Q_RAW_Threshold = 15000; // adc channels
  m_Q_Threshold = 0;         // normal bars in MeV
  m_NumberOfVendeta = 0;

  m_Cal.InitCalibration();
  m_AnodeNumber=-1;

  // === ========================== ===
  // === Analysis signal Parameters ===
  // === ========================== ===

  Samp_size_ns = 2;
  VEND_cfd_frac = 1./3.;
  VEND_cfd_delay = 4.; // ns
  End_short_gate_LG = 22.; //ns
  End_short_gate_HG = 22.; //ns
  End_long_gate_LG = 160.; //ns
  End_long_gate_HG = 160.; //ns
  Start_gate_LG = 20.; // (ns) same start for long and short gates
  Start_gate_HG = 20.; // (ns) same start for long and short gates
  bad_mult_per_anode = 0;
  m_Build_Sampler_Online = 0;
  m_LG_Channel_Online = 10;
  m_HG_Channel_Online = 10;
}
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::ReadConfiguration(nptool::InputParser parser) {
  auto blocks = parser.GetAllBlocksWithToken("vendeta");
  cout << "//// "<< blocks.size() << " detectors found" << endl;
  vector<string> info = {"R","THETA","PHI"};
  int i = 0;
  for(auto block : blocks){
    if(block->HasTokenList(info)){
      //cout << endl << "//// Vendeta " << i+1 <<  endl;
      double R = blocks[i]->GetDouble("R","mm",1);
      double Theta = blocks[i]->GetDouble("Theta","deg",1);
      double Phi = blocks[i]->GetDouble("Phi","deg",1);
      AddVendeta(R,Theta,Phi);
      i++;
    }
    else{
      cout << "ERROR: check your input file formatting " << endl;
      exit(1);
    }
  }
  ReadConversionConfig();
}

void VendetaDetector::ReadConversionConfig(){

  std::ifstream ifs("./configs/ConfigVENDETA.dat");
  if(ifs.is_open()){
    nptool::InputParser parser ("./configs/ConfigVENDETA.dat", false);
    auto blocks = parser.GetAllBlocksWithToken("ConfigVENDETA");
    /* vector<string> info = {"FRACTION"}; */
    cout << "//// Read Conversion Configuration" << endl;
    int i = 0;
    for(auto block : blocks){
      /* if(block->HasToken("ConfigVENDETA")){ */
      /* cout << endl<< "//// "; */
      //VEND_cfd_frac = 1./ block->GetInt("cfd_frac");
      m_Build_Sampler_Online = block->GetInt("sampler_online");
      m_LG_Channel_Online = block->GetInt("sampler_LG_channel");
      m_HG_Channel_Online = block->GetInt("sampler_HG_channel");
      VEND_cfd_frac = block->GetDouble("cfd_frac", "");
      VEND_cfd_delay = block->GetInt("cfd_delay"); // ns

      Start_gate_LG = block->GetInt("start_gate_LG"); // ns
      Start_gate_HG = block->GetInt("start_gate_HG"); // ns

      End_short_gate_LG = block->GetInt("end_short_gate_LG"); // ns
      End_short_gate_HG = block->GetInt("end_short_gate_HG"); // ns

      End_long_gate_LG = block->GetInt("end_long_gate_LG"); // ns
      End_long_gate_HG = block->GetInt("end_long_gate_HG"); // ns

      //Start_short_gate = block->GetInt("short_gate1"); //ns
      ////End_short_gate = block->GetInt("short_gate2"); //ns
      //Start_long_gate = block->GetInt("long_gate1"); //ns
      ////End_long_gate = block->GetInt("long_gate2"); //ns
      ////Start_gates = block->GetInt("short_gate1"); // (ns

      /* } */
    }
  }
  else{
    std::cout << ("//// Vendeta signal gates \n No configuraton file found, using defaut parameters \n ");
  }
}

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::AddVendeta(double R, double Theta, double Phi) {
  TVector3 Pos(R*sin(Theta)*cos(Phi),R*sin(Theta)*sin(Phi),R*cos(Theta));
  m_VendetaPosition.push_back(Pos);
  m_NumberOfVendeta++;
}

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitializeDataInputConversion(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("R_", "", this);
  input->Attach("L_", "", this);
}
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitializeDataInputRaw(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("vendeta", "vendeta::VendetaData", &m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitializeDataOutputRaw(std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("vendeta", "vendeta::VendetaData", &m_RawData);
}

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitializeDataInputPhysics(std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("vendeta", "vendeta::VendetaPhysics", &m_Physics);
};

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitializeDataOutputPhysics(std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("vendeta", "vendeta::VendetaPhysics", &m_Physics);
};

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::BuildPhysicalEvent() {
	//cout << "Start Build PHy Event" << endl;
 /* cout << m_AnodeNumber << endl; */
/* m_AnodeNumber = 6; */
  if(m_AnodeNumber==-1){
	  //cout << "AnNbr " << m_AnodeNumber << endl;
    return;
   }

 /* mAnodeNumber=1; */

  if(m_AnodeNumber>=0){
	  if(m_AnodeNumber>11){
		  cout << "!!!!PROBLEM!!!! Anode number larger than 11" << endl;
	  }
    //cout << "AnNbr " << m_AnodeNumber << endl;
    unsigned int multLG = m_RawData->GetLGMult();
    //cout << "multLG = " << m_RawData->GetLGMult() << endl;
    for (unsigned int i = 0; i < multLG; i++) {
      short LG_ID = m_RawData->GetLGDetectorNbr(i);
      //cout << "LG ID " << LG_ID << endl;
      double LG_offset = 0;
      if(m_AnodeNumber>0){
      	LG_offset = m_Cal.GetValue("Vendeta_DET"+nptool::itoa(LG_ID)+"_LG_ANODE"+nptool::itoa(m_AnodeNumber)+"_TIMEOFFSET",0);
      }
      //cout << setprecision(15) <<  "BUILD PHY " << m_RawData->GetLGTime(i) <<endl; 
      //cout << "LG offset " << LG_offset << endl;
      //cout << "Before SethitLG" << endl;

      vector<double> sampler;
      int size_LG_sampler = m_RawData->GetLGSampleSize();
      if(size_LG_sampler>0){
        for(unsigned int j=0; j<size_LG_sampler; j++){
                sampler.push_back(m_RawData->GetLGSample(j));
        }
      }

      //m_Physics->SetHitLG(LG_ID, m_RawData->GetLGQ1(i), m_RawData->GetLGQ2(i), m_RawData->GetLGTime(i) + LG_offset, m_RawData->GetLGQmax(i), m_RawData->GetLGCfd(i));
      m_Physics->SetHitLG(LG_ID, m_RawData->GetLGQ1(i), m_RawData->GetLGQ2(i), m_RawData->GetLGTime(i) + LG_offset, m_RawData->GetLGQmax(i), m_RawData->GetLGCfd(i), sampler, m_RawData->GetLGRiseTime(i));

    }

    //cout << "multHG = " << m_RawData->GetHGMult() << endl;
    unsigned int multHG = m_RawData->GetHGMult();
    for (unsigned int i = 0; i < multHG; i++) {
      short HG_ID = m_RawData->GetHGDetectorNbr(i);
      //cout << "HG ID " << HG_ID << endl;
      double HG_offset = 0;
      if(m_AnodeNumber>0){
      	HG_offset = m_Cal.GetValue("Vendeta_DET"+nptool::itoa(HG_ID)+"_HG_ANODE"+nptool::itoa(m_AnodeNumber)+"_TIMEOFFSET",0);
      }
      //cout << "HG offset " << HG_offset << endl;
      
      vector<double> sampler;
      int size_HG_sampler = m_RawData->GetHGSampleSize();
      if(size_HG_sampler>0){
	for(unsigned int j=0; j<size_HG_sampler; j++){
		sampler.push_back(m_RawData->GetHGSample(j));
        }
      }

      //m_Physics->SetHitHG(HG_ID, m_RawData->GetHGQ1(i), m_RawData->GetHGQ2(i), m_RawData->GetHGTime(i) + HG_offset, m_RawData->GetHGQmax(i), m_RawData->GetHGCfd(i));
      m_Physics->SetHitHG(HG_ID, m_RawData->GetHGQ1(i), m_RawData->GetHGQ2(i), m_RawData->GetHGTime(i) + HG_offset, m_RawData->GetHGQmax(i), m_RawData->GetHGCfd(i), sampler, m_RawData->GetHGRiseTime(i));
    }

    m_AnodeNumber=-1;
  }

}

////////////////////////////////////////////////////////////////////////////////

void VendetaDetector::BuildRawEvent(const std::string& daq, const std::string& label, void* data) {

#ifdef FASTERAC_FOUND
  //std::cout << "Start Build Raw V" << std::endl;
  // Static variable
  static unsigned short lbl;
  static unsigned int ID;
  static long double timestamp;
  static unsigned char alias;
  static bool is_LG;
  static qdc_t_x4                 hf_data;
  static qdc_t_x2                 fc_data;
  static faster_data_p            group_data;
  static qdc_t_x2                 vendeta_data_x2;
  static qdc_t_x3                 vendeta_data_x3;
  static qdc_t_x4                 vendeta_data_x4;
  static double TimeHF;

  // Extract Data
  lbl = faster_data_label(data);
  timestamp = faster_data_clock_ns(data);
  alias = faster_data_type_alias(data);     //  type of the data

  Double_t ff = 15;
  if (alias == QDC_TDC_X1_TYPE_ALIAS){
    if(label == "HF"){
      faster_data_load(data, &hf_data);
      TimeHF = (double)timestamp + (double)(qdc_conv_dt_ns(hf_data.tdc));
    }
  }
  else if (alias == QDC_TDC_X2_TYPE_ALIAS) {
  }
  else if (alias == QDC_TDC_X4_TYPE_ALIAS){
    // Fill Vendeta QDC / TDC
    if(label.front() == 'L' || label.front()== 'R'){
      faster_data_load(data, &vendeta_data_x4);
      is_LG = false;
      ID  = Label2Vendeta(label, is_LG);
      if(is_LG){
        m_RawData->SetLGDetectorNbr(ID);
        m_RawData->SetLGTime( (double)timestamp + (double)(qdc_conv_dt_ns(vendeta_data_x4.tdc)));
        m_RawData->SetLGQ1(vendeta_data_x4.q3);
        m_RawData->SetLGQ2(vendeta_data_x4.q2);
        m_RawData->SetLGQmax(-10000);
        m_RawData->SetLGIsSat(vendeta_data_x4.q1_saturated);
      }
      else{
        m_RawData->SetHGDetectorNbr(ID);
        m_RawData->SetHGTime( (double)timestamp + (double)(qdc_conv_dt_ns(vendeta_data_x4.tdc)));
        m_RawData->SetHGQ1(vendeta_data_x4.q3);
        m_RawData->SetHGQ2(vendeta_data_x4.q2);
        m_RawData->SetHGQmax(-10000);
        m_RawData->SetHGIsSat(vendeta_data_x4.q1_saturated);
      }
    }
    else{
      nptool::message("yellow", "vendeta", "Vendeta::BuildRawEvent", "Warning: unexpected label on QDC_TDC_X1_TYPE : "+label);
    }
  }
  else if( alias == SAMPLER_DATA_TYPE_ALIAS){
    if(label.front()=='R' || label.front()=='L' ){

      sampler samp;
      faster_data_load(data,&samp);
      //int width_ns = (int)faster_data_load_size(data);
      int width_ns = sampler_total_width(data);
      //int nb_pts = width_ns/2;
      int nbr_of_samples = sampler_samples_num(data);
      int sampler_before_threshold    = sampler_before_th(data);
      int sampler_before_threshold_ns = sampler_before_th_ns(data);

      ID = Label2Vendeta(label, is_LG);
      // if signal is from Low or High gain
      static vector<double> Signal;
      Signal.clear();
      for(int i=0;i<nbr_of_samples;i++)	Signal.push_back(samp.data[i]);
      //Signal[0]=Signal[1]=0; //to be checked

      SignalProcessor signalProcessor(Signal,false);
      signalProcessor.applyLowPassFilter();

      double Q_CFD_min;
      double QmaxCFD, Qmax=-10000;
      int iQmax, imax;
      double T_cfd, Qshort, Qlong, RiseTime;
      static bool Triggered=false, Threshold=false;
      T_cfd = signalProcessor.calculateCFD(VEND_cfd_frac, VEND_cfd_delay, 2, Qmax, iQmax, imax, Q_CFD_min, Triggered, Threshold);
      //RiseTime = signalProcessor.calcRisetime(2);
      if(Triggered && Threshold){
        /*double startT = T_cfd-Start_gates;
        double endT = T_cfd+End_short_gate;
        Qshort = signalProcessor.integrateSignal(2,startT,endT);
        double startT2 = T_cfd-Start_gates;
        double endT2 = T_cfd+End_long_gate;
        Qlong  = signalProcessor.integrateSignal(2,startT2,endT2);*/

        if(is_LG){
	  //charge integration
          double start_LG  = T_cfd - Start_gate_LG;
          double end_short = T_cfd + End_short_gate_LG;
          double end_long  = T_cfd + End_long_gate_LG;
	  double signal_size = Signal.size();
	  start_LG = max(0., start_LG); //to make sure gate starts in signal
	  end_short = min(end_short, signal_size*2); //to make sure that gate ends in signal
	  end_long = min(end_long, signal_size*2); //to make sure that gate ends in signal
          Qshort = signalProcessor.integrateSignal(2,start_LG,end_short);
          Qlong  = signalProcessor.integrateSignal(2,start_LG,end_long);
	  
	  //std::cout << "Det nbr " << ID << std::endl;
          m_RawData->SetLGDetectorNbr(ID);
	  //std::cout << "LG Time " << (double)timestamp + (double)(T_cfd)  - sampler_before_threshold_ns<< std::endl;
          m_RawData->SetLGTime((double)timestamp + (double)(T_cfd) - sampler_before_threshold_ns);
	  //std::cout << "LG Qlong " << Qlong << std::endl;
          m_RawData->SetLGQ1(Qlong);
	  //std::cout << "LG Qshort " << Qshort << std::endl;
          m_RawData->SetLGQ2(Qshort);
	  //std::cout << "LG Qmax " << Qmax << std::endl;
          m_RawData->SetLGQmax(Qmax);
          m_RawData->SetLGIsSat(0);
	  m_RawData->SetLGCfd(T_cfd);
	  m_RawData->SetLGRiseTime(RiseTime);

          //if(m_Build_Sampler_Online==1 && ID==m_LG_Channel_Online){
          //  m_RawData->SetLGSample(sampler_before_threshold);
	  //  m_RawData->SetLGSample(T_cfd);
          //  for(int i=0; i<nbr_of_samples; i++){
          //    m_RawData->SetLGSample(Signal[i]);
          //  }
          //}
        }
        else{
	  //charge integration
          double start_HG  = T_cfd - Start_gate_HG;
          double end_short = T_cfd + End_short_gate_HG;
          double end_long  = T_cfd + End_long_gate_HG;
	  double signal_size = Signal.size();
	  start_HG = max(0., start_HG); //to make sure gate starts in signal
          end_short = min(end_short, signal_size*2); //to make sure that gate ends in signal
          end_long = min(end_long, signal_size*2); //to make sure that gate ends in signal
          Qshort = signalProcessor.integrateSignal(2,start_HG,end_short);
          Qlong  = signalProcessor.integrateSignal(2,start_HG,end_long);

	  //std::cout << "Det nbr " << ID << std::endl;
          m_RawData->SetHGDetectorNbr(ID);
          //std::cout << "HG Time " << (double)timestamp + (double)(T_cfd) - sampler_before_threshold_ns << std::endl;
          m_RawData->SetHGTime((double)timestamp + (double)(T_cfd) - sampler_before_threshold_ns);
          //std::cout << "HG Qlong " << Qlong << std::endl;
          m_RawData->SetHGQ1(Qlong);
	  //std::cout << "HG Qshort " << Qshort << std::endl;
          m_RawData->SetHGQ2(Qshort);
	  //m_RawData->SetHGQ2((double)timestamp);
	  //std::cout << "HG Qmax " << Qmax << std::endl;
          m_RawData->SetHGQmax(Qmax);
          m_RawData->SetHGIsSat(0);
	  m_RawData->SetHGCfd(T_cfd);
	  m_RawData->SetHGRiseTime(RiseTime);


          if(m_Build_Sampler_Online==1 && ID==m_HG_Channel_Online){
            m_RawData->SetHGSample(sampler_before_threshold);
	    m_RawData->SetHGSample(T_cfd);
            for(int i=0; i<nbr_of_samples; i++){
              m_RawData->SetHGSample(Signal[i]);
            }
          }
        }
      }
    } // end if Vendeta sampler
  }// end if SAMPLER data
  else{
    nptool::message("yellow", "vendeta", "Vendeta::BuildRawEvent",
        "Warning: unexpected data type on label " + label);
    return;
  }
  //std::cout << "End Build Raw V" << std::endl;
#endif
};

////////////////////////////////////////////////////////////////////////////////
unsigned int VendetaDetector::Label2Vendeta(const std::string& label, bool& is_LG) {

  // ex. of label : L_I_12_LG; (I,II,III)=(0,1,2); ID =I+12=12
  if (label.substr(label.length()-2) == "LG")
    is_LG = true;
  else
    is_LG = false;

  // Find pos. of number in label
  size_t pos = label.find_first_of("123456789");
  // Isolate the number in label
  std::string substring = label.substr(pos);
  int N;
  std::istringstream(substring) >> N;

  static std::unordered_map<string,int> roman2Ark{
    {"L_I",0}, {"L_II",12}, {"L_III",24},
      {"R_I",36}, {"R_II",48}, {"R_III",60}
  };

  // Isolate 'L_II' part
  std::string number = label.substr(0,pos-1);
  // ID = Arch + N
  int ID = roman2Ark[number] + N;
  return ID;
}

double VendetaDetector::CalculateNeutronEnergy(double& flightTime, double& distance) {
  // Constants
  double speedOfLight = 299792458; // m/s
  double neutronMass = 939.5654133; // MeV/c^2

  // Convert flight time to seconds and distance to meters
  double flightTimeSeconds = flightTime * 1e-9; // nanoseconds -> seconds
  double distanceMeters = distance * 1e-3; // millimeters -> meters

  // Calculate neutron velocity (distance / time)
  double velocity = distanceMeters / flightTimeSeconds;

  // Calculate relativistic kinetic energy of the neutron (E = gamma * m * c^2 - m * c^2)
  // where gamma = 1 / sqrt(1 - v^2 / c^2)
  double gamma = 1 / TMath::Sqrt(1 - TMath::Power(velocity / speedOfLight, 2));
  double kineticEnergy = gamma * neutronMass - neutronMass;

  return kineticEnergy;
}

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitSimulation(std::string simtype) {
  // store the loaded simulation
  m_simtype = simtype;

  // load the plugin
  auto handle = nptool::Application::GetApplication()->LoadPlugin("vendeta-" + m_simtype, true);
  // build the class
#ifdef Geant4_FOUND
  /* auto func = (std::shared_ptr<nptool::geant4::VDetector>(*)())dlsym(handle, "ConstructDetectorSimulation"); */
  /* if (func) */
  /*   m_Geant4 = (*func)(); */
  /* else */
  /*   throw(nptool::Error("VendetaDetector", "Fail to load Geant4 module")); */
#endif
}
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::ConstructGeometry() {
#ifdef Geant4_FOUND
  /* if (m_Geant4) { */
  /*   // should load the library here and find the external constructor */
  /*   m_Geant4->ConstructDetector(); */
  /* } */
#endif
}

////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::InitSpectra() { m_Spectra = std::make_shared<vendeta::VendetaSpectra>(); };
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::FillSpectra() {
  m_Spectra->FillRaw();
  m_Spectra->FillPhy();
};
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::WriteSpectra(){};
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::CheckSpectra(){};
////////////////////////////////////////////////////////////////////////////////
void VendetaDetector::ClearSpectra() { m_Spectra->Clear(); };


////////////////////////////////////////////////////////////////////////////////
extern "C" {
  std::shared_ptr<nptool::VDetector> ConstructDetector() { return make_shared<::VendetaDetector>();}
}
