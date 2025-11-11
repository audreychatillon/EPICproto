#ifndef VendetaDetector_h
#define VendetaDetector_h

#include "Math/Vector3D.h"
#include "NPCalibrationManager.h"
#include "NPVDetector.h"
#include "VendetaData.h"
#include "VendetaPhysics.h"
#include <TCanvas.h>

#ifdef Geant4_FOUND
/* #include "NPG4VDetector.h" */
#endif

namespace vendeta {

  class VendetaSpectra;
  class VendetaDetector : public nptool::VDetector {
   public: // Constructor and Destructor
    VendetaDetector();
    ~VendetaDetector(){};

   public: // Data member
    vendeta::VendetaData* m_RawData;
    vendeta::VendetaPhysics* m_Physics;
    std::shared_ptr<vendeta::VendetaSpectra> m_Spectra;
    nptool::CalibrationManager m_Cal;

   public: // inherrited from nptool::VPlugin
    std::vector<std::string> GetDependencies() { return {"root"}; };

   public: // inherrited from nptool::VDetector
    //  Read stream at ConfigFile to pick-up parameters of detector (Position,...)
    //  using Token
    void ReadConfiguration(nptool::InputParser);
    void ReadConversionConfig();

    //  Add Parameter to the CalibrationManger
    void AddParameterToCalibrationManager(){};

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

    void BuildRawEvent(const std::string& daq, const std::string& label, void* ptr);

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
    void SetRawDataPointer(void*){};

   public:
    unsigned int Label2ID(const std::string& label, bool& is_up);
    /* unsigned int Label2FC(const std::string& label); */
    unsigned int Label2Vendeta(const std::string& label, bool& is_LG);
    unsigned int Label2SBT(const std::string& label, bool& is_left);
    double CalculateNeutronEnergy(double& flightTime, double& distance);
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

    // Calib Q
    double fCalQUp(const int& i);
    double fCalQDown(const int& i);

    // array settings
    unsigned int m_NumberOfVendeta;
    vector<TVector3> m_VendetaPosition;
    void AddVendeta(double R, double Theta, double Phi );

    // CFD and calculs parameters
    double Samp_size_ns;
    double VEND_cfd_frac;
    double VEND_cfd_delay; // ns

    double End_short_gate_LG; //ns
    double End_short_gate_HG; //ns
    double End_long_gate_LG; //ns
    double End_long_gate_HG; //ns
    double Start_gate_LG; // (ns) same start for long and short gates
    double Start_gate_HG; // (ns) same start for long and short gates
    int bad_mult_per_anode;
    bool b_fill_tree;
    int m_Build_Sampler_Online;
    int m_LG_Channel_Online;
    int m_HG_Channel_Online;

    int m_AnodeNumber;
   public:
    unsigned int GetNumberVendeta() { return m_NumberOfVendeta; };
    TVector3 GetVendetaPosition(const unsigned int N) { return m_VendetaPosition[N - 1]; }

   public:
    void SetQThreshold(float t) { m_Q_Threshold = t; };
    void SetVThreshold(float t) { m_V_Threshold = t; };
    // number of detectors

    void SetAnodeNumber(int AnodeNbr) {m_AnodeNumber = AnodeNbr; };
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
} // namespace vendeta
#endif

