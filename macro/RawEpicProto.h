//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Nov 13 09:39:06 2025 by ROOT version 6.32.06
// from TTree RawEpicProto/Raw data for EPIC proto run15
// found on file: RawEpicData_run15.root
//////////////////////////////////////////////////////////

#ifndef RawEpicProto_h
#define RawEpicProto_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class RawEpicProto {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<short>   *AnodeNbr;
   vector<double>  *Time;
   Double_t        TimeHF[2];
   vector<double>  *IncomingTof;
   vector<double>  *IncomingEnergy;
   vector<double>  *Cfd;
   vector<double>  *Qmax;
   vector<double>  *Q1;
   vector<double>  *Q2;
   vector<double>  *Q3;
   Short_t         IndexQ1max;

   // List of branches
   TBranch        *b_AnodeNbr;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_TimeHF;   //!
   TBranch        *b_IncomingTof;   //!
   TBranch        *b_IncomingEnergy;   //!
   TBranch        *b_Cfd;   //!
   TBranch        *b_Qmax;   //!
   TBranch        *b_Q1;   //!
   TBranch        *b_Q2;   //!
   TBranch        *b_Q3;   //!
   TBranch        *b_IndexQ1max;   //!

   RawEpicProto(TTree *tree=0);
   virtual ~RawEpicProto();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
