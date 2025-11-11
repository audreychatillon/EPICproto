#ifndef FissionChamberSpectra_h
#define FissionChamberSpectra_h
// fission-chamber
#include "FissionChamberDetector.h"
// root
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
// std
#include <map>
#include <memory>
namespace fission_chamber {

// forward declaration is necessary
// class FissionChamberDetector;
class FissionChamberSpectra {
public:
  FissionChamberSpectra();
  ~FissionChamberSpectra() {};

private:
  std::shared_ptr<fission_chamber::FissionChamberDetector> m_detector;
  fission_chamber::FissionChamberData *m_RawData;
  fission_chamber::FissionChamberPhysics *m_Physics;
  std::map<std::string, TH1 *> m_raw_hist;
  std::map<std::string, TH1 *> m_phy_hist;
  TGraph *m_graph_sampler;
  double check_time;

public:
  void FillRaw();
  void FillPhy();
  void Clear();
};

} // namespace fission_chamber
#endif
