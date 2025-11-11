#include "FissionChamberData.h"
ClassImp(fission_chamber::FissionChamberData);

//////////////////////////////////////////////////////////////////////
void fission_chamber::FissionChamberData::Clear() {
  fFC_Qmax.clear();
  fFC_AnodeNbr.clear();
  fFC_Q1.clear();
  fFC_Q2.clear();
  fFC_Q3.clear();
  fFC_firstQ2.clear();
  fFC_Time.clear();
  fFC_TimeHF.clear();
  fFC_isFakeFission.clear();
  fFC_Sampler.clear();
  fFC_Cfd.clear();
  fFC_RawTof.clear();
  fFC_IncomingE.clear();
}
