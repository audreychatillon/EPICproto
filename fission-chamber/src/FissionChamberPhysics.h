#ifndef FissionChamberPhysics_h
#define FissionChamberPhysics_h

// STL
#include <vector>
using namespace std;
// ROOT
#include "TVector3.h"
namespace fission_chamber {
class FissionChamberPhysics {
  //////////////////////////////////////////////////////////////
  // data members are held into vectors in order
  // to allow multiplicity treatment
 private:
  
 public:

   vector<short>    Anode_ID;
   vector<bool>     FakeFission;
   vector<double>   inToF;
   vector<double>   Q1;
   vector<double>   Q2;
   vector<double>   Q3;
   vector<double>   firstQ2;
   vector<double>   Time;
   vector<double>   TimeHF;
   vector<double>   Qmax;
   vector<double>   inEnergy;
   vector<double>   Cfd;
   vector<double>   Cfd_bis;
   vector<double>   Q1_bis;
   vector<double>   Q2_bis;
   vector<double>   Q3_bis;

   //////////////////////////////////////////////////////////////
   // Constructor and destructor

   //////////////////////////////////////////////////////////////
   // Constructor and destructor

 public:
   FissionChamberPhysics(){};
   ~FissionChamberPhysics(){};

   //////////////////////////////////////////////////////////////
   // Inherited from TObject and overriden to avoid warnings
 public:
   void Clear(){
     
     Anode_ID.clear();
     Q1.clear();
     Q2.clear();
     Q3.clear();
     firstQ2.clear();
     Time.clear();
     TimeHF.clear();
     Qmax.clear();
     inEnergy.clear();
     inToF.clear();
     FakeFission.clear();
     Cfd.clear();
     Cfd_bis.clear();
     Q1_bis.clear();
     Q2_bis.clear();
     Q3_bis.clear();

   };//!

   //////////////////////////////////////////////////////////////
   // Getters and Setters
   // Prefer inline declaration to avoid unnecessary called of
   // frequently used methods
   // add //! to avoid ROOT creating dictionnary for the methods
 public:

   //////////////////////    SETTERS    ////////////////////////
   
   inline void SetHitFC(const short& ID, const double& q1,
       const double& q2, const double& q3, const double& fq2, const double& rawt, const double& qmax, const double& inE, const double& Tof, const double Timehf, const bool& isFakeFission, const double& cfd, const double& cfd_bis, const double& q1_bis, const double& q2_bis, const double& q3_bis) {
     Anode_ID.push_back(ID);
     Q1.push_back(q1);
     //std::cout << "Q2 " << q2 << " Q3 " << q3 << std::endl;
     Q2.push_back(q2);
     Q3.push_back(q3);
     firstQ2.push_back(fq2);
     Time.push_back(rawt);
     Qmax.push_back(qmax);
     inEnergy.push_back(inE);
     inToF.push_back(Tof);
     TimeHF.push_back(Timehf);
     FakeFission.push_back(isFakeFission);
     Cfd.push_back(cfd);
     Cfd_bis.push_back(cfd_bis);
     Q1_bis.push_back(q1_bis);
     Q2_bis.push_back(q2_bis);
     Q3_bis.push_back(q3_bis);
   }; //!

   //////////////////////    GETTERS    ////////////////////////
  
   inline unsigned int GetMult() const { return Anode_ID.size(); };


   //////////////////////////////////////////////////////////////
   // Required for ROOT dictionnary
   ClassDef(FissionChamberPhysics, 2)  // FissionChamberPhysics structure
};
}  // namespace fission_chamber
#endif

