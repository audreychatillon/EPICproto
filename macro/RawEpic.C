#include "RawEpic.h"

RawEpic::RawEpic(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test11.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test11.root");
      }
      f->GetObject("RawTree",tree);

   }
   Init(tree);
}

RawEpic::~RawEpic()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RawEpic::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RawEpic::LoadTree(Long64_t entry)
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

void RawEpic::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fFC_DetNbr", &fFC_DetNbr, &b_fission_chamber_fFC_DetNbr);
   fChain->SetBranchAddress("fFC_AnodeNbr", &fFC_AnodeNbr, &b_fission_chamber_fFC_AnodeNbr);
   fChain->SetBranchAddress("fFC_Time", &fFC_Time, &b_fission_chamber_fFC_Time);
   fChain->SetBranchAddress("fFC_TimeHF", &fFC_TimeHF, &b_fission_chamber_fFC_TimeHF);
   fChain->SetBranchAddress("fFC_IncomingTof", &fFC_IncomingTof, &b_fission_chamber_fFC_IncomingTof);
   fChain->SetBranchAddress("fFC_Cfd", &fFC_Cfd, &b_fission_chamber_fFC_Cfd);
   fChain->SetBranchAddress("fFC_Qmax", &fFC_Qmax, &b_fission_chamber_fFC_Qmax);
   fChain->SetBranchAddress("fFC_Q1", &fFC_Q1, &b_fission_chamber_fFC_Q1);
   fChain->SetBranchAddress("fFC_Q2", &fFC_Q2, &b_fission_chamber_fFC_Q2);
   fChain->SetBranchAddress("fFC_Q3", &fFC_Q3, &b_fission_chamber_fFC_Q3);
   fChain->SetBranchAddress("fFC_Cfd_bis", &fFC_Cfd_bis, &b_fission_chamber_fFC_Cfd_bis);
   fChain->SetBranchAddress("fFC_Q1_bis", &fFC_Q1_bis, &b_fission_chamber_fFC_Q1_bis);
   fChain->SetBranchAddress("fFC_Q2_bis", &fFC_Q2_bis, &b_fission_chamber_fFC_Q2_bis);
   fChain->SetBranchAddress("fFC_Q3_bis", &fFC_Q3_bis, &b_fission_chamber_fFC_Q3_bis);
   fChain->SetBranchAddress("fFC_Sampler", &fFC_Sampler, &b_fission_chamber_fFC_Sampler);
   fChain->SetBranchAddress("fFC_firstQ2", &fFC_firstQ2, &b_fission_chamber_fFC_firstQ2);
   fChain->SetBranchAddress("fFC_isFakeFission", &fFC_isFakeFission, &b_fission_chamber_fFC_isFakeFission);
   Notify();
}

bool RawEpic::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void RawEpic::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RawEpic::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void RawEpic::Loop()
{
//   In a ROOT session, you can do:
//      root> .L RawEpic.C
//      root> RawEpic t
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
