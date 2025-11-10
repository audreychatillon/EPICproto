// Main header
#include "FissionChamberDetector.h"
#include "FissionChamberSpectra.h"

// nptool core
#include "NPApplication.h"
#include "NPCalibrationManager.h"
#include "NPException.h"
#include "NPFunction.h"
#include "NPParticle.h"
#include "NPRootPlugin.h"

#include <TCanvas.h>
#include <TLine.h>

// For faster data conversion
#ifdef FASTERAC_FOUND
#include "fasterac/fasterac.h" // mandatory for any data type
#include "fasterac/group.h"    // group
#include "fasterac/qdc.h"      // qdc, qdc_counter
#include "fasterac/sampler.h"  // sampler counter
#include "fasterac/sampling.h" // sampler
#include "fasterac/utils.h"    // some specific tools
#endif
#include "SignalProcessorFC.h"

// For Geant4 Simulation
#ifdef Geant4_FOUND
/* #include "FissionChamberGeant4.h" */
#endif

// std
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace fission_chamber;
using namespace std;
using namespace ROOT::Math;
using namespace nptool;

/* TApplication app("app", nullptr, nullptr); */

////////////////////////////////////////////////////////////////////////////////
FissionChamberDetector::FissionChamberDetector() {
  m_RawData = new ::FissionChamberData();
  m_Physics = new ::FissionChamberPhysics();

  m_Q_RAW_Threshold = 15000; // adc channels
  m_Q_Threshold = 0;         // normal bars in MeV
  m_NumberOfFissionChamber = 0;

  m_Cal.InitCalibration();

  // === ========================== ===
  // === Analysis signal Parameters ===
  // === ========================== ===

  bad_mult_per_anode = 0;

  m_total_raw_event = 0;
  m_good_raw_event = 0;
  m_event_to_recovered = 0;

  m_Build_Sampler_Online = 0;
  m_Anode_Sampler_Online = 6;
}
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::ReadConfiguration(nptool::InputParser parser) {
  auto blocks = parser.GetAllBlocksWithToken("fission-chamber");
  vector<string> infoFC = {"POS"};
  vector<string> typeFC = {"TYPE"};
  string Type;
  if (blocks[0]->HasTokenList(typeFC)) {
    Type = blocks[0]->GetString("TYPE");
  }
  if (blocks[0]->HasTokenList(infoFC)) {
    cout << endl << "//// FissionChamber " << endl;
    vector<double> Pos = blocks[0]->GetVector3("POS", "mm");
    AddFissionChamber(Pos, Type);
  } else {
    cout << "ERROR: check your input file formatting " << endl;
    exit(1);
  }
  ReadConversionConfig();
}

void FissionChamberDetector::ReadConversionConfig() {
  std::ifstream ifs("./configs/ConfigFC.dat");
  if (ifs.is_open()) {
    nptool::InputParser parser("./configs/ConfigFC.dat", false);
    auto blocks = parser.GetAllBlocksWithToken("ConfigFC");
    /* cout << "//// Read Conversion Configuration" << endl; */
    int i = 0;
    for (auto block : blocks) {
      m_Build_Sampler_Online = block->GetInt("sampler_online");
      m_Anode_Sampler_Online = block->GetInt("sampler_anode");
      int anode = block->GetInt("anode", 1);

      FC_cfd_frac_perA[anode - 1] = 1. / block->GetInt("cfd_frac", 1);
      FC_cfd_delay_perA[anode - 1] = block->GetInt("cfd_delay", 1);
      Start_short_gate_FC_perA[anode - 1] = block->GetInt("Q2_gate1", 1);
      End_short_gate_FC_perA[anode - 1] = block->GetInt("Q2_gate2", 1);
      Start_Q3_gate_FC_perA[anode - 1] = block->GetInt("Q3_gate1", 1);
      End_Q3_gate_FC_perA[anode - 1] = block->GetInt("Q3_gate2", 1);
      Start_long_gate_FC_perA[anode - 1] = block->GetInt("long_gate1", 1);
      End_long_gate_FC_perA[anode - 1] = block->GetInt("long_gate2", 1);

      FC_cfd_frac_perA_bis[anode - 1] = 1. / block->GetInt("cfd_frac_bis", 1);
      FC_cfd_delay_perA_bis[anode - 1] = block->GetInt("cfd_delay_bis", 1);
      Start_short_gate_FC_perA_bis[anode - 1] =
          block->GetInt("Q2_gate1_bis", 1);
      End_short_gate_FC_perA_bis[anode - 1] = block->GetInt("Q2_gate2_bis", 1);
      Start_Q3_gate_FC_perA_bis[anode - 1] = block->GetInt("Q3_gate1_bis", 1);
      End_Q3_gate_FC_perA_bis[anode - 1] = block->GetInt("Q3_gate2_bis", 1);
      Start_long_gate_FC_perA_bis[anode - 1] =
          block->GetInt("long_gate1_bis", 1);
      End_long_gate_FC_perA_bis[anode - 1] = block->GetInt("long_gate2_bis", 1);
    }
  } else {
    std::cout
        << ("//// No FC conversion file found, using default parameters \n")
        << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::AddFissionChamber(double R, double Theta,
                                               double Phi) {
  TVector3 Pos(R * sin(Theta) * cos(Phi), R * sin(Theta) * sin(Phi),
               R * cos(Theta));
  m_FissionChamberPosition.push_back(Pos);
  m_NumberOfFissionChamber++;
}

void FissionChamberDetector::AddFissionChamber(vector<double> pos,
                                               string Type) {
  if (Type == "Pu") {
    double Zpos = -29.6; // position of anode 1 (i=0)
    for (int i = 0; i < 11; i++) {
      // cout << "anode " << i+1 << " : " << Zpos << endl;
      TVector3 AnodePos(
          pos[0], pos[1],
          pos[2] +
              Zpos); // for Pu anodes separated by 5.8mm and 6 mm and anode 6 in
                     // center, deposit is not on anode but anode is middle
                     // point betwen the two cathodes containing the deposits
      if (i % 2 == 0) {
        Zpos += 6.0; // Add 6 for even iterations
      } else {
        Zpos += 5.8; // Add 5.8 for odd iterations
      }
      m_AnodePosition.push_back(AnodePos);
    }
  }

  // Old version with 5.5mm interanode distance
  // if (Type == "Pu"){
  //       for(int i = 0; i<11; i++){
  //         TVector3 AnodePos(pos[0], pos[1], pos[2] -27.5 + i*5.5); //for Pu
  //         anodes separated by 5.5mm and anode 6 in center, deposit is not on
  //         anode but anode is middle point betwen the two cathodes containing
  //         the deposits m_AnodePosition.push_back(AnodePos);
  //       }
  // }

  if (Type == "Cf") {
    for (int i = 0; i < 11; i++) {
      TVector3 AnodePos(
          pos[0], pos[1],
          pos[2] - 27.5 + i * 5.5 -
              2.5); // for Cf chamber only one anode (AnodeNbr = 6) and only one
                    // cathode, -2.5mm because we can directly set deposit
                    // position instead of using anode position
      m_AnodePosition.push_back(AnodePos);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitializeDataInputConversion(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("FC_", "", this);
  input->Attach("HF", "", this);
  input->Attach("PULSER", "", this);
  input->Attach("FAKE_FISSION", "", this);
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitializeDataInputRaw(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("fission_chamber", "fission_chamber::FissionChamberData",
                &m_RawData);
}
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitializeDataOutputRaw(
    std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("fission_chamber", "fission_chamber::FissionChamberData",
                 &m_RawData);
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitializeDataOutputPhysics(
    std::shared_ptr<nptool::VDataOutput> output) {
  output->Attach("fission_chamber", "fission_chamber::FissionChamberPhysics",
                 &m_Physics);
};
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitializeDataInputPhysics(
    std::shared_ptr<nptool::VDataInput> input) {
  input->Attach("fission_chamber", "fission_chamber::FissionChamberPhysics",
                &m_Physics);
};

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::BuildPhysicalEvent() {

  unsigned int multFC = m_RawData->GetFCMult();

  nptool::Particle *neutron = new nptool::Particle("1n");

  for (int i = 0; i < multFC; i++) {
    // std::cout << m_RawData->GetFCAnodeNbr(i)  << std::endl;
    short AnodeNumber = m_RawData->GetFCAnodeNbr(i);
    bool isFakeFission = false;
    isFakeFission = m_RawData->GetFCisFakeFission(i);
    double Time_FC = m_RawData->GetFCTime(i);
    double cfd = m_RawData->GetFCCfd(i);
    double FC_offset = m_Cal.GetValue(
        "FissionChamber_ANODE" + nptool::itoa(AnodeNumber) + "_TIMEOFFSET", 0);
    double Time_HF = m_RawData->GetHFTime(i);
    double incomingToF = Time_FC - Time_HF - FC_offset;

    // Get Anode position, in pulser mode (fakefission) we use the anode 6 by
    // default
    TVector3 AnodePos = GetAnodePosition(6);
    if (AnodeNumber > 0) {
      TVector3 AnodePos = GetAnodePosition(AnodeNumber);
    }

    if (incomingToF < 0) {
      incomingToF += m_Cal.GetValue("FissionChamber_PULSE_TIMEOFFSET", 0);
    }

    double FlyPath = 21000;
    double incomingE = 0;
    neutron->SetBeta((FlyPath / incomingToF) / nptool::c_light);
    incomingE = neutron->GetEnergy();
    m_Physics->SetHitFC(AnodeNumber, m_RawData->GetFCQ1(i),
                        m_RawData->GetFCQ2(i), m_RawData->GetFCQ3(i),
                        m_RawData->GetFCfirstQ2(i), Time_FC,
                        m_RawData->GetFCQmax(i), incomingE, incomingToF,
                        Time_HF, isFakeFission, m_RawData->GetFCCfd(i),
                        m_RawData->GetFCCfd_bis(i), m_RawData->GetFCQ1_bis(i),
                        m_RawData->GetFCQ2_bis(i), m_RawData->GetFCQ3_bis(i));
  }
}

////////////////////////////////////////////////////////////////////////////////

void FissionChamberDetector::BuildRawEvent(const std::string &daq,
                                           const std::string &label,
                                           void *data) {
// std::cout << "Start Build Raw FC" << std::endl;
#ifdef FASTERAC_FOUND

  // Static variable
  static unsigned short lbl;
  static unsigned int ID;
  static long double timestamp;
  static unsigned char alias;
  static qdc_t_x4 hf_data;
  static qdc_t_x2 fc_data;
  static faster_data_p group_data;
  static qdc_t_x2 fission_chamber_data_x2;
  static qdc_t_x3 fission_chamber_data_x3;
  static qdc_t_x4 fission_chamber_data_x4;
  static double TimeHF;

  // Extract Data
  lbl = faster_data_label(data);
  timestamp = faster_data_clock_ns(data);
  alias = faster_data_type_alias(data); //  type of the data

  Double_t ff = 15;
  if (alias == QDC_TDC_X1_TYPE_ALIAS) {
    if (label == "HF") {
      faster_data_load(data, &hf_data);
      TimeHF = (double)timestamp + (double)(qdc_conv_dt_ns(hf_data.tdc));
    }
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double Time = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      m_RawData->SetAnodeNbr(-1);
      // std::cout << "FFtime " << Time << std::endl;
      m_RawData->SetFCTime(Time);
      // std::cout << "FFQ1 " << fc_data.q1 << std::endl;
      m_RawData->SetFCQ1(fc_data.q1);
      m_RawData->SetFCQ2(0);
      m_RawData->SetFCQ3(0);
      m_RawData->SetFCfirstQ2(0);
      m_RawData->SetFCQmax(0);
      m_RawData->SetFakeFissionStatus(true);
      // std::cout << "FF HF " << TimeHF << std::endl;
      m_RawData->SetTimeHF(TimeHF);
      m_RawData->SetFCCfd(-1);
      m_RawData->SetFCCfd_bis(-1);
      m_RawData->SetFCQ1_bis(fc_data.q1);
      m_RawData->SetFCQ2_bis(0);
      m_RawData->SetFCQ3_bis(0);
    }
  } else if (alias == QDC_TDC_X2_TYPE_ALIAS) {
    if (label == "PULSER" || label == "FAKE_FISSION") {
      faster_data_load(data, &fc_data);
      double Time = timestamp + (double)(qdc_conv_dt_ns(fc_data.tdc));
      m_RawData->SetAnodeNbr(-1);
      m_RawData->SetFCTime(Time);
      m_RawData->SetFCQ1(fc_data.q1);
      m_RawData->SetFCQ2(fc_data.q2);
      m_RawData->SetFCQ3(0);
      m_RawData->SetFCfirstQ2(0);
      m_RawData->SetFCQmax(0);
      m_RawData->SetFakeFissionStatus(true);
      m_RawData->SetTimeHF(TimeHF);
      m_RawData->SetFCCfd(-1);
      m_RawData->SetFCCfd_bis(-1);
      m_RawData->SetFCQ1_bis(fc_data.q1);
      m_RawData->SetFCQ2_bis(fc_data.q2);
      m_RawData->SetFCQ3_bis(0);
    }
  } else if (alias == QDC_TDC_X4_TYPE_ALIAS) {
    /* else{ */
    /*   nptool::message("yellow", "fission_chamber",
     * "FissionChamber::BuildRawEvent", "Warning: unexpected label on
     * QDC_TDC_X1_TYPE : "+label); */
    /* } */
  } else if (alias == SAMPLER_DATA_TYPE_ALIAS) {
    if (label.front() == 'F') {
      m_total_raw_event++;

      // sampler_data samp = (sampler_data)faster_data_load_p(data);
      sampler samp;
      faster_data_load(data, &samp);
      int width_ns = (int)faster_data_load_size(data);
      // int nb_pts = width_ns/2;
      int nbr_of_samples = sampler_samples_num(data);
      int sampler_before_threshold = sampler_before_th(data);
      int sampler_before_threshold_ns = sampler_before_th_ns(data);

      ID = Label2FC(label);
      double BLRoffset = 0;

      bool ApplyFilter = false;
      int AnodeNb = Label2FC(label);
      // if(AnodeNb==11 || AnodeNb==6 || AnodeNb==9 || AnodeNb == 10){
      if (AnodeNb > 0) {
        ApplyFilter = false;
      }

      static vector<double> Signal;
      Signal.clear();
      for (int i = 0; i < nbr_of_samples; i++)
        Signal.push_back(samp.data[i] - BLRoffset);
      // Signal[0]=Signal[1]=0;

      SignalProcessorFC signalProcessor(Signal, ApplyFilter);
      signalProcessor.applyLowPassFilter();

      double Q_CFD_min;
      double QmaxCFD, Qmax = -10000;
      int iQmax, imax;
      double T_cfd, Q2, Q3, Qlong, firstQ2;
      double T_cfd_bis, Q2_bis, Q3_bis, Qlong_bis;

      static bool FC_Triggered = false, FC_Threshold = false;
      T_cfd_bis = signalProcessor.calculateCFD(
          FC_cfd_frac_perA_bis[ID - 1], FC_cfd_delay_perA_bis[ID - 1], 2, Qmax,
          iQmax, imax, Q_CFD_min, FC_Triggered, FC_Threshold);

      FC_Triggered = false, FC_Threshold = false;
      T_cfd = signalProcessor.calculateCFD(
          FC_cfd_frac_perA[ID - 1], FC_cfd_delay_perA[ID - 1], 2, Qmax, iQmax,
          imax, Q_CFD_min, FC_Triggered, FC_Threshold);

      /* if(ID ==1){
         signalProcessor.drawFilteredSignal();
         signalProcessor.drawRawSignal();
         signalProcessor.drawSignalCFD();
         }
         */

      if (FC_Triggered && FC_Threshold) {
        m_good_raw_event++;

        /*if(m_good_raw_event%10000==0)
          cout << m_total_raw_event << " " << m_good_raw_event << " / " <<
          (double)m_good_raw_event/(double)m_total_raw_event*100 << "%" << endl;
          */

        // cout << "signal size "<< Signal.size() << endl;
        double signal_size = Signal.size();

        /////
        /// gate integration for first cfd
        /////

        double startT = T_cfd - Start_short_gate_FC_perA[ID - 1];
        double endT = T_cfd + End_short_gate_FC_perA[ID - 1];
        startT = max(0., startT); // to make sure gate starts in signal
        endT =
            min(endT, signal_size * 2); // to make sure that gate ends in signal
        Q2 = signalProcessor.integrateSignal(2, startT, endT);
        // std::cout << "Q2 " << Q2 << std::endl;

        firstQ2 = signalProcessor.GetAmpStartGate(2, startT);

        double startT3 = T_cfd + Start_Q3_gate_FC_perA[ID - 1];
        double endT3 = T_cfd + End_Q3_gate_FC_perA[ID - 1];
        startT3 =
            min(signal_size * 2, startT3); // to make sure gate starts in signal
        endT3 = min(endT3,
                    signal_size * 2); // to make sure that gate ends in signal
        Q3 = signalProcessor.integrateSignal(2, startT3, endT3);
        // std::cout << "Q3 " << Q3 << std::endl;

        double startT2 = T_cfd - Start_long_gate_FC_perA[ID - 1];
        double endT2 = T_cfd + End_long_gate_FC_perA[ID - 1];
        startT2 = max(0., startT2); // to make sure gate starts in signal
        endT2 = min(endT2,
                    signal_size * 2); // to make sure that gate ends in signal
        Qlong = signalProcessor.integrateSignal(2, startT2, endT2);

        /////
        /// gate integration for second cfd
        /////

        // cout << "T_cfd_bis " << T_cfd_bis << endl;
        if (T_cfd_bis != -10000) {
          double startT_bis = T_cfd_bis - Start_short_gate_FC_perA_bis[ID - 1];
          double endT_bis = T_cfd_bis + End_short_gate_FC_perA_bis[ID - 1];
          startT_bis = max(0., startT_bis); // to make sure gate starts in
                                            // signal
          endT_bis =
              min(endT_bis,
                  signal_size * 2); // to make sure that gate ends in signal
          Q2_bis = signalProcessor.integrateSignal(2, startT_bis, endT_bis);
          // std::cout << "Q2_bis " << Q2_bis << std::endl;

          double startT3_bis = T_cfd_bis + Start_Q3_gate_FC_perA_bis[ID - 1];
          double endT3_bis = T_cfd_bis + End_Q3_gate_FC_perA_bis[ID - 1];
          startT3_bis = min(signal_size * 2,
                            startT3_bis); // to make sure gate starts in signal
          endT3_bis =
              min(endT3_bis,
                  signal_size * 2); // to make sure that gate ends in signal
          Q3_bis = signalProcessor.integrateSignal(2, startT3_bis, endT3_bis);
          // std::cout << "Q3 " << Q3 << std::endl;

          double startT2_bis = T_cfd_bis - Start_long_gate_FC_perA_bis[ID - 1];
          double endT2_bis = T_cfd_bis + End_long_gate_FC_perA_bis[ID - 1];
          startT2_bis =
              max(0., startT2_bis); // to make sure gate starts in signal
          endT2_bis =
              min(endT2_bis,
                  signal_size * 2); // to make sure that gate ends in signal
          Qlong_bis =
              signalProcessor.integrateSignal(2, startT2_bis, endT2_bis);
        } else {
          Q2_bis = 1;
          Q3_bis = 1;
          Qlong_bis = 0;
        }

        double Qtot, FC_cfd;
        // bool FakeFission = label==Label_PULSER;
        int FilterType = 0;

        /*if (T_cfd >= 25){
                cout << "end cfd>25 " << endT2 << endl;
                cout << "Q1 " << Qlong << endl;
        }

        if (T_cfd >= 50){
                cout << "cfd " << T_cfd << endl;
                cout << "startT " << startT2 << endl;
                cout << "Q1 " << Qlong << endl;
        }


        if (Qlong > 1e6){
                cout << "Q1 " << Qlong << endl;
                cout << "cfd " << T_cfd << endl;
                cout << "start " << startT2 << endl;
                cout << "stop " << endT2 << endl;
        }

        if (Q2 > 1e6){
                cout << "Q2 " << Q2 << endl;
        }

        if (Q3 > 1e6){
                cout << "Q3 " << Q3 << endl;
                cout << "stop " << endT3 << endl;
        }*/

        double TimeFC =
            (double)timestamp + (double)T_cfd - sampler_before_threshold_ns;
        //        if(Qlong>12000){
        // std::cout << "Anode nbr " << Label2FC(label) << std::endl;
        m_RawData->SetAnodeNbr(Label2FC(label));
        // std::cout << "Qlong " << Qlong << std::endl;
        m_RawData->SetFCQ1(Qlong);
        // std::cout << "Q2 " << Q2 << std::endl;
        m_RawData->SetFCQ2(Q2);
        m_RawData->SetFCQ3(Q3);
        // std::cout << "Qmax " << Qmax << std::endl;
        m_RawData->SetFCfirstQ2(firstQ2);
        m_RawData->SetFCQmax(Qmax);
        // std::cout << "TimeFC " << TimeFC << std::endl;
        m_RawData->SetFCTime(TimeFC);
        m_RawData->SetFakeFissionStatus(false);
        // std::cout << "TimeHF " << TimeHF << std::endl;
        m_RawData->SetTimeHF(TimeHF);
        m_RawData->SetFCCfd(T_cfd);
        m_RawData->SetFCCfd_bis(T_cfd_bis);
        m_RawData->SetFCQ1_bis(Qlong_bis);
        m_RawData->SetFCQ2_bis(Q2_bis);
        m_RawData->SetFCQ3_bis(Q3_bis);

        if (m_Build_Sampler_Online == 1 && ID == m_Anode_Sampler_Online) {
          m_RawData->SetSample(sampler_before_threshold);
          m_RawData->SetSample(T_cfd);
          for (int i = 0; i < nbr_of_samples; i++) {
            m_RawData->SetSample(Signal[i]);
          }
        }
        // }
        /////// MACRO TO DRAW SIGNALS
        /*if(ID ==1){
          auto c = new TCanvas();
          c->Divide(2,1);
          c->cd(1);
          signalProcessor.drawRawSignal();
          auto l = new TLine(startT,-100000,startT,100000);
          l->SetLineWidth(3);
          l->SetLineColor(4);
          l->Draw();
          auto l2 = new TLine(endT,-100000,endT,100000);
          l2->SetLineWidth(3);
          l2->SetLineColor(2);
          l2->Draw();
          auto l4 = new TLine(endT2,-100000,endT2,100000);
          l4->SetLineWidth(3);
          l4->SetLineColor(kRed-8);
          l4->Draw();
          auto l3 = new TLine(T_cfd,-100000,T_cfd,100000);
          l3->SetLineWidth(3);
          l3->SetLineColor(6);
          l3->Draw();

          c->cd(2);
          signalProcessor.drawSignalCFD();
          l3->Draw();
          cout << ID << " Tcfd: " << T_cfd << " Q1: " << Qlong << " Q2: " << Q2
          << endl; gPad->WaitPrimitive(); c->Close();
          }*/
        ///////////////////// END MACRO

      } // If Triggered
      else {
        if (Qmax > 350) {
          m_event_to_recovered++;
          // cout << "percentage of event to recovred= "  <<
          // (double)m_event_to_recovered/(double)m_total_raw_event*100 << "%"
          // << endl;
        }
      }
    } // end FC sampler
  } // end if SAMPLER data
  else {
    nptool::message("yellow", "fission_chamber",
                    "FissionChamber::BuildRawEvent",
                    "Warning: unexpected data type on label " + label);
    return;
  }
  // std::cout << "End Build Raw FC" << std::endl;
#endif
};

////////////////////////////////////////////////////////////////////////////////
unsigned int FissionChamberDetector::Label2FCdet(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) // format: FC_anode
    return 0;
  else { // format: FC_det_anode
    string number = label.substr(pos1 + 1, pos2 - pos1 - 1);
    return stoi(number);
  }
}
////////////////////////////////////////////////////////////////////////////////
unsigned int FissionChamberDetector::Label2FCanode(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) { // format: FC_anode
    string number = label.substr(pos1 + 1);
    return stoi(number);
  } else { // format: FC_det_anode
    string number = label.substr(pos2 + 1);
    return stoi(number);
  }
}
////////////////////////////////////////////////////////////////////////////////
unsigned int FissionChamberDetector::Label2ID(const std::string &label) {
  // generic to handle FC_det_anode or FC_anode
  int id;
  string number;
  size_t pos1 = label.find("_");
  size_t pos2 = label.find("_", pos1 + 1);
  if (pos2 == string::npos) { // FC_anode
    number = label.substr(pos1 + 1);
    id = stoi(number) - 1;
  } else { // FC_det_anode
    number = label.substr(pos1 + 1, pos2 - pos1 - 1);
    int det = stoi(number);
    number = label.substr(pos2 + 1);
    int anode = stoi(number);
    id = (det - 1) * 11 + anode - 1;
  }
  return id;
}

////////////////////////////////////////////////////////////////////////////////

unsigned int FissionChamberDetector::Label2FC(const std::string &label) {
  static string number;
  // Remove 'FC_' from label 'FC_8'
  number = label.substr(3, label.length());
  return atoi(number.c_str());
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitSimulation(std::string simtype) {
  // store the loaded simulation
  m_simtype = simtype;

  // load the plugin
  auto handle = nptool::Application::GetApplication()->LoadPlugin(
      "fission_chamber-" + m_simtype, true);
  // build the class
#ifdef Geant4_FOUND
  /* auto func = (std::shared_ptr<nptool::geant4::VDetector>(*)())dlsym(handle,
   * "ConstructDetectorSimulation"); */
  /* if (func) */
  /*   m_Geant4 = (*func)(); */
  /* else */
  /* throw(nptool::Error("FissionChamberDetector", "Fail to load Geant4
   * module")); */
#endif
}
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::ConstructGeometry() {
#ifdef Geant4_FOUND
  /* if (m_Geant4) { */
  /*   // should load the library here and find the external constructor */
  /*   m_Geant4->ConstructDetector(); */
  /* } */
#endif
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::InitSpectra() {
  m_Spectra = std::make_shared<fission_chamber::FissionChamberSpectra>();
};
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::FillSpectra() {
  m_Spectra->FillRaw();
  m_Spectra->FillPhy();
};
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::WriteSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::CheckSpectra() {};
////////////////////////////////////////////////////////////////////////////////
void FissionChamberDetector::ClearSpectra() { m_Spectra->Clear(); };

///////////////////////////////////////////////////////////////////////////////
extern "C" {
std::shared_ptr<nptool::VDetector> ConstructDetector() {
  return make_shared<::FissionChamberDetector>();
};
}
