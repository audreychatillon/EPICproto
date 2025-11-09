#ifndef FissionChamberData_h
#define FissionChamberData_h
/*****************************************************************************
 * Original Author: Cyril Lenain     contact: lenain@cea.fr                  *
 *                                                                           *
 * Creation Date  : Octtber 2022                                             *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold FissionChamber Raw data                                         *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// STL
#include <vector>
#include <iostream>
using namespace std;
// ROOT
#include "TObject.h"
namespace fission_chamber {
  class FissionChamberData {
    //////////////////////////////////////////////////////////////
    // data members are hold into vectors in order
    // to allow multiplicity treatment
    private:
      
      vector<double> fFC_Qmax;
      vector<double> fFC_TimeHF;
      vector<double> fFC_Q1;
      vector<double> fFC_Q2;
      vector<double> fFC_Q3;
      vector<double> fFC_firstQ2;
      vector<short>  fFC_AnodeNbr;
      vector<double> fFC_Time;
      vector<bool>   fFC_isFakeFission;
      vector<double> fFC_Sampler;
      vector<double> fFC_Cfd;
      vector<double> fFC_Cfd_bis;
      vector<double> fFC_Q1_bis;
      vector<double> fFC_Q2_bis;
      vector<double> fFC_Q3_bis;

      //////////////////////////////////////////////////////////////
      // Constructor and destructor
    public:
      FissionChamberData(){};
      ~FissionChamberData(){};

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

      // Energy
      inline void SetFCQ1(const double& Q1) {fFC_Q1.push_back(Q1);};//!
      inline void SetFCQ2(const double& Q2) {fFC_Q2.push_back(Q2);};//!
      inline void SetFCQ3(const double& Q3) {fFC_Q3.push_back(Q3);};//!
      inline void SetFCfirstQ2(const double& firstQ2) {fFC_firstQ2.push_back(firstQ2);};//!
      inline void SetFCTime(const double& Time) {fFC_Time.push_back(Time);};//!
      inline void SetTimeHF(const double& Time) {fFC_TimeHF.push_back(Time);};//!
      inline void SetFCQmax(const double& Qmax) {fFC_Qmax.push_back(Qmax);};//
      inline void SetAnodeNbr(const unsigned short& ID) {fFC_AnodeNbr.push_back(ID);};//
      inline void SetFakeFissionStatus(const bool& IsFF) {fFC_isFakeFission.push_back(IsFF);};//
      inline void SetSample(const double& sample) {fFC_Sampler.push_back(sample);};//
      inline void SetFCCfd(const double& Cfd) {fFC_Cfd.push_back(Cfd);};//!
      inline void SetFCCfd_bis(const double& Cfd) {fFC_Cfd_bis.push_back(Cfd);};//!
      inline void SetFCQ1_bis(const double& Q1) {fFC_Q1_bis.push_back(Q1);};//!
      inline void SetFCQ2_bis(const double& Q2) {fFC_Q2_bis.push_back(Q2);};//!
      inline void SetFCQ3_bis(const double& Q3) {fFC_Q3_bis.push_back(Q3);};//!

      //////////////////////    GETTERS    ////////////////////////
      inline UShort_t GetFCMult() const {return fFC_AnodeNbr.size();}
      inline int GetFCAnodeNbr(const unsigned int &i) const {return fFC_AnodeNbr[i];}//!
      inline double   GetFCQ1(const unsigned int &i) const {return fFC_Q1[i];}//!
      inline double   GetFCQ2(const unsigned int &i) const {return fFC_Q2[i];}//!
      inline double   GetFCQ3(const unsigned int &i) const {return fFC_Q3[i];}//!
      inline double   GetFCfirstQ2(const unsigned int &i) const {return fFC_firstQ2[i];}//!
      inline double   GetFCTime(const unsigned int &i) const {return fFC_Time[i];}//!
      inline double   GetHFTime(const unsigned int &i) const {return fFC_TimeHF[i];}//!
      inline double   GetFCQmax(const unsigned int &i) const {return fFC_Qmax[i];}//!
      inline bool   GetFCisFakeFission(const unsigned int &i) const {return fFC_isFakeFission[i];}//!
      inline double   GetFCSample(const unsigned int &i) const {return fFC_Sampler[i];}//!
      inline int   GetFCSampleSize() const {return fFC_Sampler.size();}//!
      inline double   GetFCCfd(const unsigned int &i) const {return fFC_Cfd[i];}//!
      inline double   GetFCCfd_bis(const unsigned int &i) const {return fFC_Cfd_bis[i];}//
      inline double   GetFCQ1_bis(const unsigned int &i) const {return fFC_Q1_bis[i];}//!
      inline double   GetFCQ2_bis(const unsigned int &i) const {return fFC_Q2_bis[i];}//!
      inline double   GetFCQ3_bis(const unsigned int &i) const {return fFC_Q3_bis[i];}//!

      //////////////////////////////////////////////////////////////
      // Required for ROOT dictionnary
      ClassDef(FissionChamberData, 2) // FissionChamberData structure
  };
} // namespace fission_chamber
#endif


