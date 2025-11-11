#ifndef VendetaData_h
#define VendetaData_h
/*****************************************************************************
 * Original Author: Cyril Lenain     contact: lenain@cea.fr                  *
 *                                                                           *
 * Creation Date  : Octtber 2022                                             *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold Vendeta Raw data                                         *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// STL
#include <vector>
using namespace std;
// ROOT
#include "TObject.h"
namespace vendeta {
  class VendetaData {
    //////////////////////////////////////////////////////////////
    // data members are hold into vectors in order
    // to allow multiplicity treatment
    private:
  
      vector<unsigned short>   fVendeta_HG_DetectorNbr;
      vector<double>   fVendeta_HG_Q1;
      vector<double>   fVendeta_HG_Q2;
      vector<double>   fVendeta_HG_Time;
      vector<double>   fVendeta_HG_Qmax;
      vector<bool>     fVendeta_HG_IsSat; 
      vector<double>   fVendeta_HG_Sampler;
      vector<double>   fVendeta_HG_Cfd;
      vector<double>   fVendeta_HG_RiseTime;

      vector<unsigned short>   fVendeta_LG_DetectorNbr;
      vector<double>   fVendeta_LG_Q1;
      vector<double>   fVendeta_LG_Q2;
      vector<double>   fVendeta_LG_Time;
      vector<double>   fVendeta_LG_Qmax;
      vector<bool>     fVendeta_LG_IsSat;
      vector<double>   fVendeta_LG_Sampler; 
      vector<double>   fVendeta_LG_Cfd;
      vector<double>   fVendeta_LG_RiseTime;
      //////////////////////////////////////////////////////////////
      // Constructor and destructor
    public:
      VendetaData(){};
      ~VendetaData(){};

      //////////////////////////////////////////////////////////////
      // Inherited from TObject and overriden to avoid warnings
    public:
      void Clear();
      void Clear(const Option_t*) {};
      void Dump() const;

      //////////////////////////////////////////////////////////////
      // Getters and Setters
      // Prefer inline declaration to avoid unnecessary called of
      // frequently used methods
      // add //! to avoid ROOT creating dictionnary for the methods
    public:
      //////////////////////    SETTERS    ////////////////////////

      inline void SetLGDetectorNbr(const unsigned short& DetNbr) {fVendeta_LG_DetectorNbr.push_back(DetNbr);};//!
      inline void SetLGQ1(const double& Q1) {fVendeta_LG_Q1.push_back(Q1);};//!
      inline void SetLGQ2(const double& Q2) {fVendeta_LG_Q2.push_back(Q2);};//!
      inline void SetLGTime(const double& Time) {fVendeta_LG_Time.push_back(Time);};//!
      inline void SetLGQmax(const double& Qmax) {fVendeta_LG_Qmax.push_back(Qmax);};//
      inline void SetLGIsSat(const bool& IsSat) {fVendeta_LG_IsSat.push_back(IsSat);};//
      inline void SetLGSample(const double& sample) {fVendeta_LG_Sampler.push_back(sample);};//
      inline void SetLGCfd(const double& Cfd) {fVendeta_LG_Cfd.push_back(Cfd);};//!
      inline void SetLGRiseTime(const double& RiseTime) {fVendeta_LG_RiseTime.push_back(RiseTime);};//!

      inline void SetHGDetectorNbr(const unsigned short& DetNbr) {fVendeta_HG_DetectorNbr.push_back(DetNbr);};//!
      inline void SetHGQ1(const double& Q1) {fVendeta_HG_Q1.push_back(Q1);};//!
      inline void SetHGQ2(const double& Q2) {fVendeta_HG_Q2.push_back(Q2);};//!
      inline void SetHGTime(const double& Time) {fVendeta_HG_Time.push_back(Time);};//!
      inline void SetHGQmax(const double& Qmax) {fVendeta_HG_Qmax.push_back(Qmax);};//
      inline void SetHGIsSat(const bool& IsSat) {fVendeta_HG_IsSat.push_back(IsSat);};//
      inline void SetHGSample(const double& sample) {fVendeta_HG_Sampler.push_back(sample);};//
      inline void SetHGCfd(const double& Cfd) {fVendeta_HG_Cfd.push_back(Cfd);};//!
      inline void SetHGRiseTime(const double& RiseTime) {fVendeta_HG_RiseTime.push_back(RiseTime);};//!

      //////////////////////    GETTERS    ////////////////////////

      inline UShort_t GetLGMult() const {return fVendeta_LG_DetectorNbr.size();}
      inline UShort_t GetLGDetectorNbr(const unsigned int &i) const {return fVendeta_LG_DetectorNbr[i];}//!
      inline double GetLGQ1(const unsigned int &i) const {return fVendeta_LG_Q1[i];}//!
      inline double GetLGQ2(const unsigned int &i) const {return fVendeta_LG_Q2[i];}//!
      inline double GetLGTime(const unsigned int &i) const {return fVendeta_LG_Time[i];}//!
      inline double GetLGQmax(const unsigned int &i) const {return fVendeta_LG_Qmax[i];}//!
      inline bool     GetLGIsSat(const unsigned int &i) const {return fVendeta_LG_IsSat[i];}//!
      inline double     GetLGSample(const unsigned int &i) const {return fVendeta_LG_Sampler[i];}//!
      inline int     GetLGSampleSize() const {return fVendeta_LG_Sampler.size();}//!
      inline double GetLGCfd(const unsigned int &i) const {return fVendeta_LG_Cfd[i];}//!
      inline double GetLGRiseTime(const unsigned int &i) const {return fVendeta_LG_RiseTime[i];}//!

      inline UShort_t GetHGMult() const {return fVendeta_HG_DetectorNbr.size();}
      inline UShort_t GetHGDetectorNbr(const unsigned int &i) const {return fVendeta_HG_DetectorNbr[i];}//!
      inline double GetHGQ1(const unsigned int &i) const {return fVendeta_HG_Q1[i];}//!
      inline double GetHGQ2(const unsigned int &i) const {return fVendeta_HG_Q2[i];}//!
      inline double GetHGTime(const unsigned int &i) const {return fVendeta_HG_Time[i];}//!
      inline double GetHGQmax(const unsigned int &i) const {return fVendeta_HG_Qmax[i];}//!
      inline bool     GetHGIsSat(const unsigned int &i) const {return fVendeta_HG_IsSat[i];}//!
      inline double     GetHGSample(const unsigned int &i) const {return fVendeta_HG_Sampler[i];}//!
      inline int     GetHGSampleSize() const {return fVendeta_HG_Sampler.size();}//!
      inline double GetHGCfd(const unsigned int &i) const {return fVendeta_HG_Cfd[i];}//!
      inline double GetHGRiseTime(const unsigned int &i) const {return fVendeta_HG_RiseTime[i];}//!

      //////////////////////////////////////////////////////////////
      // Required for ROOT dictionnary
      ClassDef(VendetaData, 1) // VendetaData structure
  };
} // namespace vendeta
#endif


