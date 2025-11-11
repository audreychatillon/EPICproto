#ifndef FissionChamberDetector_h
#define FissionChamberDetector_h

#include "FissionChamberData.h"
#include "FissionChamberPhysics.h"
#include "Math/Vector3D.h"
#include "NPCalibrationManager.h"
#include "NPVDetector.h"
#include <TApplication.h>
#include <TCanvas.h>

#ifdef Geant4_FOUND
/* #include "NPG4VDetector.h" */
#endif

namespace fission_chamber {

class FissionChamberSpectra;
class FissionChamberDetector : public nptool::VDetector {
public: // Constructor and Destructor
  FissionChamberDetector();
  ~FissionChamberDetector() {};

public: // Data member
  fission_chamber::FissionChamberData *m_RawData;
  fission_chamber::FissionChamberPhysics *m_Physics;
  std::shared_ptr<fission_chamber::FissionChamberSpectra> m_Spectra;
  nptool::CalibrationManager m_Cal;

public: // inherrited from nptool::VPlugin
  std::vector<std::string> GetDependencies() { return {"root"}; };

public: // inherrited from nptool::VDetector
  //  Read stream at ConfigFile to pick-up parameters of detector (Position,...)
  //  using Token
  void ReadConfiguration(nptool::InputParser);
  void ReadConversionConfig();

  //  Add Parameter to the CalibrationManger
  void AddParameterToCalibrationManager() {};

  void InitializeDataInputConversion(std::shared_ptr<nptool::VDataInput>);

  //  Activated associated Branches and link it to the private member
  //  DetectorData address In this method mother Branches (Detector) have to be
  //  activated
  void InitializeDataInputRaw(std::shared_ptr<nptool::VDataInput>);

  //  Activated associated Branches and link it to the private member
  //  DetectorPhysics address In this method mother Branches (Detector) AND
  //  daughter leaf (parameter) have to be activated
  void InitializeDataInputPhysics(std::shared_ptr<nptool::VDataInput>);

  //  Create associated branches and associate private member DetectorPhysics
  //  address
  void InitializeDataOutputRaw(std::shared_ptr<nptool::VDataOutput>);

  //  Create associated branches and associate private member DetectorPhysics
  //  address
  void InitializeDataOutputPhysics(std::shared_ptr<nptool::VDataOutput>);

  //  This method is called at each event read from the Input Tree. Aime is to
  //  build treat Raw dat in order to extract physical parameter.
  void BuildPhysicalEvent();

  void BuildRawEvent(const std::string &daq, const std::string &label,
                     void *ptr);

  //  Those two method all to clear the Event Physics or Data
  void ClearEventPhysics() { m_Physics->Clear(); };
  void ClearEventData() { m_RawData->Clear(); };

  // Method related to the TSpectra classes, aimed at providing a framework for
  // online applications Instantiate the Spectra class and the histogramm
  // throught it
  void InitSpectra();
  // Fill the spectra hold by the spectra class
  void FillSpectra();
  // Write the spectra to a file
  void WriteSpectra();
  // Used for Online mainly, perform check on the histo and for example change
  // their color if issues are found
  void CheckSpectra();
  // Used for Online only, clear all the spectra hold by the Spectra class
  void ClearSpectra();
  // Used for interoperability with other framework
  void SetRawDataPointer(void *) {};

public:
  unsigned int Label2FCdet(const std::string &label);
  unsigned int Label2FCanode(const std::string &label);
  unsigned int Label2ID(const std::string &label);
  unsigned int Label2FC(const std::string &label);
  double CalculateNeutronEnergy(double &flightTime, double &distance);
  // void CleanArray(double array[200]);

  // parameters used in the analysis
private:
  // parameter
  double m_BarWidth;
  double m_BarLength;
  double m_TdiffRange;

  // thresholds
  float m_Q_RAW_Threshold;
  float m_Q_Threshold;
  float m_V_Threshold;

  int m_total_raw_event;
  int m_good_raw_event;
  int m_event_to_recovered;

  // Calib Q
  double fCalQUp(const int &i);
  double fCalQDown(const int &i);

  // array settings
  unsigned int m_NumberOfFissionChamber;
  vector<TVector3> m_FissionChamberPosition;
  vector<TVector3> m_AnodePosition;
  void AddFissionChamber(double R, double Theta, double Phi);
  void AddFissionChamber(vector<double> Pos, string Type);

  // last TimeHF
  double m_TimeHF;

  // CFD and calculs parameters
  double Samp_size_ns;
  double VEND_cfd_frac;
  double VEND_cfd_delay; // ns

  double End_short_gate; // ns
  double End_long_gate;  // ns
  //		const double End_long_gate = 80.; //ns
  double Start_gates; // (ns) same start for long and short gates
  int bad_mult_per_anode;
  bool b_fill_tree;
  // Default parameters, anode ID:       1    2      3     4     5     6     7
  // 8     9     10    11
  double FC_cfd_frac_perA[11]{1. / 4, 1. / 5, 1. / 5, 1. / 5, 1. / 3, 1. / 5,
                              1. / 3, 1. / 4, 1. / 4, 1. / 4, 1. / 4};
  double FC_cfd_delay_perA[11]{4.,  6., 6., 6., 8., 6.,
                               10., 6., 6., 6., 8.}; // ns
  double Start_short_gate_FC_perA[11]{3,   3., 20., 3.,  20., 3.,
                                      20., 3., 20., 20., 20.};
  double End_short_gate_FC_perA[11]{6.,  6.,  10., 6.,  10., 6.,
                                    10., 10., 10., 10., 10.};

  double Start_Q3_gate_FC_perA[11]{3,   3., 20., 3.,  20., 3.,
                                   20., 3., 20., 20., 20.};
  double End_Q3_gate_FC_perA[11]{6.,  6.,  10., 6.,  10., 6.,
                                 10., 10., 10., 10., 10.};

  double Start_long_gate_FC_perA[11]{3,   3., 20., 3.,  20., 3.,
                                     20., 3., 20., 20., 20.};
  double End_long_gate_FC_perA[11]{25,  25., 75., 25., 75., 25.,
                                   75., 25., 75., 75., 75.};

  double FC_cfd_frac_perA_bis[11];
  double FC_cfd_delay_perA_bis[11]; // ns
  double Start_short_gate_FC_perA_bis[11];
  double End_short_gate_FC_perA_bis[11];

  double Start_Q3_gate_FC_perA_bis[11];
  double End_Q3_gate_FC_perA_bis[11];

  double Start_long_gate_FC_perA_bis[11];
  double End_long_gate_FC_perA_bis[11];

  int m_Build_Sampler_Online;
  int m_Anode_Sampler_Online;

public:
  unsigned int GetNumberFissionChamber() { return m_NumberOfFissionChamber; };
  TVector3 GetFissionChamberPosition(const unsigned int N) {
    return m_FissionChamberPosition[N - 1];
  }
  TVector3 GetAnodePosition(const unsigned int N) {
    return m_AnodePosition[N - 1];
  }

public:
  void SetQThreshold(float t) { m_Q_Threshold = t; };
  void SetVThreshold(float t) { m_V_Threshold = t; };
  // number of detectors

  // simulation
public:
  void InitSimulation(std::string simtype);
  void ConstructGeometry();
  std::string m_simtype;

#ifdef Geant4_FOUND
  /* private: */
  /*  std::shared_ptr<nptool::geant4::VDetector> m_Geant4; */
#endif
};
} // namespace fission_chamber
#endif
