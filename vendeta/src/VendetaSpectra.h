#ifndef VendetaSpectra_h
#define VendetaSpectra_h
// vendeta
#include "VendetaDetector.h"
// root
#include "TH1.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TGraph.h"
// std
#include <map>
#include <array>
#include <memory>
namespace vendeta {

  // forward declaration is necessary
  // class VendetaDetector;
  class VendetaSpectra {
   public:
    VendetaSpectra();
    ~VendetaSpectra(){};

   private:
    std::shared_ptr<vendeta::VendetaDetector> m_detector;
    vendeta::VendetaData* m_RawData;
    vendeta::VendetaPhysics* m_Physics;
    std::array<TH2*,72> m_raw_hist_LG;
    std::array<TH2*,72> m_raw_hist_HG;

    std::array<TH1*,72> m_raw_Q1_LG;
    std::array<TH1*,72> m_raw_Q1_HG;
    std::array<TH1*,72> m_raw_Qmax_LG;
    std::array<TH1*,72> m_raw_Qmax_HG;

    TGraph* m_graph_sampler_LG;
    TGraph* m_graph_sampler_HG;
   //std::map<std::string, TH2*> m_raw_hist;
    //std::map<std::string, TH2*> m_phy_hist;

   public:
    void FillRaw();
    void FillPhy();
    void Clear();
    double check_time;
 
};

} // namespace vendeta
#endif
