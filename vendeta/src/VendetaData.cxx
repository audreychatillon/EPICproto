#include "VendetaData.h"
ClassImp(vendeta::VendetaData);

//////////////////////////////////////////////////////////////////////
void vendeta::VendetaData::Clear() {
  
  fVendeta_HG_DetectorNbr.clear();
  fVendeta_HG_Q1.clear();
  fVendeta_HG_Q2.clear();
  fVendeta_HG_Time.clear();
  fVendeta_HG_Qmax.clear();
  fVendeta_HG_IsSat.clear();
  fVendeta_HG_Sampler.clear();
  fVendeta_HG_Cfd.clear();
  fVendeta_HG_RiseTime.clear();
 
  fVendeta_LG_DetectorNbr.clear();
  fVendeta_LG_Q1.clear();
  fVendeta_LG_Q2.clear();
  fVendeta_LG_Time.clear();
  fVendeta_LG_Qmax.clear();
  fVendeta_LG_IsSat.clear();
  fVendeta_LG_Sampler.clear();
  fVendeta_LG_Cfd.clear();
  fVendeta_LG_RiseTime.clear();
}

