#include "RawEpicProto.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

RawEpicProto::RawEpicProto(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("RawEpicData_run15.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("RawEpicData_run15.root");
      }
      f->GetObject("RawEpicProto",tree);

   }
   Init(tree);
}

RawEpicProto::~RawEpicProto()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RawEpicProto::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RawEpicProto::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void RawEpicProto::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   AnodeNbr = 0;
   Time = 0;
   IncomingTof = 0;
   IncomingEnergy = 0;
   Cfd = 0;
   Qmax = 0;
   Q1 = 0;
   Q2 = 0;
   Q3 = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("AnodeNbr", &AnodeNbr, &b_AnodeNbr);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("TimeHF", TimeHF, &b_TimeHF);
   fChain->SetBranchAddress("IncomingTof", &IncomingTof, &b_IncomingTof);
   fChain->SetBranchAddress("IncomingEnergy", &IncomingEnergy, &b_IncomingEnergy);
   fChain->SetBranchAddress("Cfd", &Cfd, &b_Cfd);
   fChain->SetBranchAddress("Qmax", &Qmax, &b_Qmax);
   fChain->SetBranchAddress("Q1", &Q1, &b_Q1);
   fChain->SetBranchAddress("Q2", &Q2, &b_Q2);
   fChain->SetBranchAddress("Q3", &Q3, &b_Q3);
   fChain->SetBranchAddress("IndexQ1max", &IndexQ1max, &b_IndexQ1max);
   Notify();
}

bool RawEpicProto::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void RawEpicProto::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RawEpicProto::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void RawEpicProto::Loop()
{
//   In a ROOT session, you can do:
//      root> .L RawEpicProto.C
//      root> RawEpicProto t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
