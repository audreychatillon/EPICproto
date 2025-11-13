//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Nov 12 16:36:44 2025 by ROOT version 6.32.06
// from TTree RawTree/nptool tree
// found on file: test11.root
//////////////////////////////////////////////////////////

#ifndef RawEpic_h
#define RawEpic_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class RawEpic {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
 //fission_chamber::FissionChamberData *fission_chamber;
   vector<short>   fFC_DetNbr;
   vector<short>   fFC_AnodeNbr;
   vector<double>  fFC_Time;
   vector<double>  fFC_TimeHF;
   vector<double>  fFC_IncomingTof;
   vector<double>  fFC_Cfd;
   vector<double>  fFC_Qmax;
   vector<double>  fFC_Q1;
   vector<double>  fFC_Q2;
   vector<double>  fFC_Q3;
   vector<double>  fFC_Cfd_bis;
   vector<double>  fFC_Q1_bis;
   vector<double>  fFC_Q2_bis;
   vector<double>  fFC_Q3_bis;
   vector<double>  fFC_Sampler;
   vector<double>  fFC_firstQ2;
   vector<bool>    fFC_isFakeFission;

   // List of branches
   TBranch        *b_fission_chamber_fFC_DetNbr;   //!
   TBranch        *b_fission_chamber_fFC_AnodeNbr;   //!
   TBranch        *b_fission_chamber_fFC_Time;   //!
   TBranch        *b_fission_chamber_fFC_TimeHF;   //!
   TBranch        *b_fission_chamber_fFC_IncomingTof;   //!
   TBranch        *b_fission_chamber_fFC_Cfd;   //!
   TBranch        *b_fission_chamber_fFC_Qmax;   //!
   TBranch        *b_fission_chamber_fFC_Q1;   //!
   TBranch        *b_fission_chamber_fFC_Q2;   //!
   TBranch        *b_fission_chamber_fFC_Q3;   //!
   TBranch        *b_fission_chamber_fFC_Cfd_bis;   //!
   TBranch        *b_fission_chamber_fFC_Q1_bis;   //!
   TBranch        *b_fission_chamber_fFC_Q2_bis;   //!
   TBranch        *b_fission_chamber_fFC_Q3_bis;   //!
   TBranch        *b_fission_chamber_fFC_Sampler;   //!
   TBranch        *b_fission_chamber_fFC_firstQ2;   //!
   TBranch        *b_fission_chamber_fFC_isFakeFission;   //!

   RawEpic(TTree *tree=0);
   virtual ~RawEpic();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
