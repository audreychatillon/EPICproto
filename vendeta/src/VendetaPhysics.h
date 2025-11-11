#ifndef VendetaPhysics_h
#define VendetaPhysics_h

// STL
#include <vector>
using namespace std;
// ROOT
#include "TVector3.h"
namespace vendeta {
  class VendetaPhysics {
    //////////////////////////////////////////////////////////////
    // data members are hold into vectors in order
    // to allow multiplicity treatment
    public:

      vector<int>      Vendeta_LG_DetectorNumber;
    public:
      vector<double>   Vendeta_LG_Q1;
      vector<double>   Vendeta_LG_Q2;
      vector<double>   Vendeta_LG_Time;
      vector<double>   Vendeta_LG_Qmax;
      vector<double>   Vendeta_LG_Cfd;
      vector<vector<double>>   Vendeta_LG_Sampler;
      vector<double>   Vendeta_LG_RiseTime;

      vector<int>      Vendeta_HG_DetectorNumber;
      vector<double>   Vendeta_HG_Q1;
      vector<double>   Vendeta_HG_Q2;
      vector<double>   Vendeta_HG_Time;
      vector<double>   Vendeta_HG_Qmax;
      vector<double>   Vendeta_HG_Cfd;
      vector<vector<double>>   Vendeta_HG_Sampler;
      vector<double>   Vendeta_HG_RiseTime;

      //////////////////////////////////////////////////////////////
      // Constructor and destructor

      //////////////////////////////////////////////////////////////
      // Constructor and destructor

    public:
      VendetaPhysics(){};
      ~VendetaPhysics(){};

      //////////////////////////////////////////////////////////////
      // Inherited from TObject and overriden to avoid warnings
    public:
      void Clear(){
        Vendeta_LG_DetectorNumber.clear();
        Vendeta_LG_Q1.clear();
        Vendeta_LG_Q2.clear();
        Vendeta_LG_Time.clear();
        Vendeta_LG_Qmax.clear();
	Vendeta_LG_Cfd.clear();
	Vendeta_LG_Sampler.clear();
	Vendeta_LG_RiseTime.clear();
        
        Vendeta_HG_DetectorNumber.clear();
        Vendeta_HG_Q1.clear();
        Vendeta_HG_Q2.clear();
        Vendeta_HG_Time.clear();
        Vendeta_HG_Qmax.clear();
	Vendeta_HG_Cfd.clear();
	Vendeta_HG_Sampler.clear();
	Vendeta_HG_RiseTime.clear();

      };//!

      //////////////////////////////////////////////////////////////
      // Getters and Setters
      // Prefer inline declaration to avoid unnecessary called of
      // frequently used methods
      // add //! to avoid ROOT creating dictionnary for the methods
    public:

      //////////////////////    SETTERS    ////////////////////////

      inline void SetHitLG(const unsigned short& ID, const double& Q1,
          const double& Q2, const double& T, const double& Qmax, const double& Cfd, const vector<double>& Sampler, const double& RiseTime) {
        Vendeta_LG_DetectorNumber.push_back(ID);
        Vendeta_LG_Q1.push_back(Q1);
        Vendeta_LG_Q2.push_back(Q2);
        Vendeta_LG_Time.push_back(T);
        Vendeta_LG_Qmax.push_back(Qmax);
	Vendeta_LG_Cfd.push_back(Cfd);
	Vendeta_LG_Sampler.push_back(Sampler);
	Vendeta_LG_RiseTime.push_back(RiseTime);
      };  //!

      //inline void SetHitHG(const unsigned short& ID, const double& Q1,
      //    const double& Q2, const double& T, const double& Qmax, const double& Cfd) {
      //  Vendeta_HG_DetectorNumber.push_back(ID);
      //  Vendeta_HG_Q1.push_back(Q1);
      //  Vendeta_HG_Q2.push_back(Q2);
      //  Vendeta_HG_Time.push_back(T);
      //  Vendeta_HG_Qmax.push_back(Qmax);
      //  Vendeta_HG_Cfd.push_back(Cfd);
      //};  //!

      inline void SetHitHG(const unsigned short& ID, const double& Q1,
          const double& Q2, const double& T, const double& Qmax, const double& Cfd, const vector<double>& Sampler, const double& RiseTime) {
        Vendeta_HG_DetectorNumber.push_back(ID);
        Vendeta_HG_Q1.push_back(Q1);
        Vendeta_HG_Q2.push_back(Q2);
        Vendeta_HG_Time.push_back(T);
        Vendeta_HG_Qmax.push_back(Qmax);
        Vendeta_HG_Cfd.push_back(Cfd);
	Vendeta_HG_Sampler.push_back(Sampler);
	Vendeta_HG_RiseTime.push_back(RiseTime);
      };  //!

      //////////////////////    GETTERS    ////////////////////////
      
      inline unsigned int GetLGMult() const{ return Vendeta_LG_DetectorNumber.size(); };
      inline unsigned int GetHGMult() const{ return Vendeta_HG_DetectorNumber.size(); };
      vector<vector<double>> GetSampler() {return  Vendeta_HG_Sampler; };
      vector<vector<double>> GetSamplerLG() {return  Vendeta_LG_Sampler; };

      //////////////////////////////////////////////////////////////
      // Required for ROOT dictionnary
      ClassDef(VendetaPhysics, 1)  // VendetaPhysics structure
  };
}  // namespace vendeta
#endif
