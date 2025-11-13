#include "RawEpic.h"
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Riostream.h>
#include <TROOT.h>

#include <TCanvas.h>
#include <TChain.h>
#include <TCutG.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>

double NeutronEnergy_MeV(double L_m, double t_ns)
{
    const double c = 299792458.0;  
    const double mn_MeV = 939.565; 
    double t_s = t_ns * 1.0e-9;
    double v = L_m / t_s;
    double beta = v / c;
    if (beta >= 1.0) return -1.0; // cas non physique
    double gamma = 1.0 / std::sqrt(1.0 - beta * beta);
    double Ek = (gamma - 1.0) * mn_MeV;

    return Ek; 
}

void run(int RunNumber)
{

  // --- ---------- --- //
  // --- INPUT DATA --- //
  // --- ---------- --- //

  TChain * ch = new TChain("RawTree");
  ch->Add(Form("../output/conversion/epic_proto_run%i.root",RunNumber));
  ch->ls();
  RawEpic raw(ch);


  // --- ---------- --- //
  // --- HISTOGRAMS --- //
  // --- ---------- --- //

  TH1I * h1_mult = new TH1I("TotMult","Number of Anode per event",13,-0.5,12.5);
  TH1I * h1_multA = new TH1I("TotMultPerAnode","Total number of occurences",13,-0.5,12.5);
  TH2I * h2_multTotA = new TH2I("TotMultPerEvt_vs_A","TotMultPerEvt_vs_A",13,-0.5,12.5,13,-0.5,12.5);
  h1_mult->SetDirectory(0);
  h1_multA->SetDirectory(0);
  h2_multTotA->SetDirectory(0);

  TH2I * h2_multHF_vs_multFC = new TH2I("TotMultHF_vs_TotMultFC","TotMultHF_vs_TotMultFC",13,-0.5,12.5,13,-0.5,12.5);
  TH1D * h1_1_deltaTimeHF = new TH1D("DeltaTimeHF_ns_1","DeltaTimeHF_part1",1000,1750,1850);
  TH1D * h1_2_deltaTimeHF = new TH1D("DeltaTimeHF_ns_1","DeltaTimeHF_part1",1000,3500,3600);
  TH1D * h1_3_deltaTimeHF = new TH1D("DeltaTimeHF_ns_2","DeltaTimeHF_part2",100000,16000000,17000000);
  TH1D * h1_4_deltaTimeHF = new TH1D("DeltaTimeHF_ns_3","DeltaTimeHF_part3",100000,32600000,33600000);
  TH1D * h1_5_deltaTimeHF = new TH1D("DeltaTimeHF_ns_3","DeltaTimeHF_part3",100000,82700000,82800000);
  h2_multHF_vs_multFC->SetDirectory(0);
  h1_1_deltaTimeHF->SetDirectory(0);
  h1_2_deltaTimeHF->SetDirectory(0);
  h1_3_deltaTimeHF->SetDirectory(0);
  h1_4_deltaTimeHF->SetDirectory(0);
  h1_5_deltaTimeHF->SetDirectory(0);

  TH2F * h2_inTofRaw = new TH2F("inRawToF_vs_A","inRawToF_vs_A",13,-0.5,12.5,2000,-100,1900);
  TH1F * h1_inTofRaw[11];
  TH1F * h1_inTofRaw_Q1max[11];
  TH2F * h2_inTofRaw_Q1max = new TH2F("inRawToF_vs_A_Q1max","inRawToF_vs_A_Q1max",13,-0.5,12.5,2000,-100,1900);
  TH1F * h1_inEne_Q1max[11];
  TH2F * h2_inEne_Q1max = new TH2F("inEne_vs_A_Q1max","inEne_vs_A_Q1max",13,-0.5,12.5,500,0,50);
  TH2F * h2_inEne_vs_Q1_Q1max[11];
  h2_inTofRaw->SetDirectory(0);
  h2_inTofRaw_Q1max->SetDirectory(0);
  h2_inEne_Q1max->SetDirectory(0);
  for(short a=0; a<11; a++){
    h1_inTofRaw[a] = new TH1F(Form("inTofRaw_A%02d",a+1),Form("incoming raw Tof [ns] at Anode %d",a+1),2000,-100,1900);
    h1_inTofRaw_Q1max[a] = new TH1F(Form("inTofRaw_A%02d_Q1max",a+1),Form("incoming raw Tof [ns] at Anode %d defined with Q1max",a+1),2000,-100,1900);
    h1_inEne_Q1max[a] = new TH1F(Form("inEne_A%02d",a+1),Form("incoming neutron energy [MeV] at Anode %d",a+1),500,0,50);
    h2_inEne_vs_Q1_Q1max[a] = new TH2F(Form("inEne_A%02d_vs_Q1_Q1max",a+1),Form("incoming energy [MeV] vs Q1 at Anode %d defined with Q1max",a+1),1000,0,200000,50,0,50);
    h1_inTofRaw[a]->SetDirectory(0);
    h1_inTofRaw_Q1max[a]->SetDirectory(0);
    h1_inEne_Q1max[a]->SetDirectory(0);
    h2_inEne_vs_Q1_Q1max[a]->SetDirectory(0);
  }

  TH1I * h1_indexQ1max = new TH1I("indexQ1max","indexQ1max",20,-1.5,18.5);
  h1_indexQ1max->SetDirectory(0);
 
  // --- --------- --- //
  // --- VARIABLES --- //
  // --- --------- --- //

  int    cptHFlost = 0;
  double TimeHF_current  =  0. ;
  double TimeHF_previous = 1.e+12 ;

  // calibration
  double FC_TofOffset[11] = {-1004., -1002., -1004., -1002., -999., -998., -1017., -1016., -1010., -1008., -1009.};

  // for output tree
  vector<short>   AnodeNbr;             AnodeNbr.reserve(11);
  vector<double>  Time;                 Time.reserve(11);
  double TimeHF[2];
  vector<double>  IncomingTof;          IncomingTof.reserve(11);
  vector<double>  IncomingEnergy;       IncomingEnergy.reserve(11);
  vector<double>  Cfd;                  Cfd.reserve(11);
  vector<double>  Qmax;                 Qmax.reserve(11);
  vector<double>  Q1;                   Q1.reserve(11);
  vector<double>  Q2;                   Q2.reserve(11);
  vector<double>  Q3;                   Q3.reserve(11);
  short           IndexQ1max;  

  TFile * fout = new TFile(Form("RawEpicData_run%i.root",RunNumber),"recreate"); 
  TTree * tout = new TTree("RawEpicProto",Form("Raw data for EPIC proto run%i",RunNumber));
  tout->Branch("AnodeNbr",&AnodeNbr);
  tout->Branch("Time",&Time);
  tout->Branch("TimeHF",TimeHF,"TimeHF[2]/D");
  tout->Branch("IncomingTof",&IncomingTof);
  tout->Branch("IncomingEnergy",&IncomingEnergy);
  tout->Branch("Cfd",&Cfd);
  tout->Branch("Qmax",&Qmax);
  tout->Branch("Q1",&Q1);
  tout->Branch("Q2",&Q2);
  tout->Branch("Q3",&Q3);
  tout->Branch("IndexQ1max",&IndexQ1max,"IndexQ1max/S");

  // --- ---- --- //
  // --- LOOP --- //
  // --- ---- --- //

  ULong64_t nentries = (ULong64_t)ch->GetEntries();
  cout << "number of entries: " << nentries << endl;
  for(ULong64_t Entry=0; Entry<nentries; Entry++){

    raw.GetEntry(Entry);
    if ((Entry % 1000000)==0) cout << "\r === Entry = " << Entry << " === " << flush;

    h1_mult->Fill((int)raw.fFC_AnodeNbr.size());	
    h2_multHF_vs_multFC->Fill((int)raw.fFC_Time.size(),(int)raw.fFC_TimeHF.size());	
    if((int)raw.fFC_TimeHF.size()==1 && (int)raw.fFC_Time.size()==0){
    	TimeHF_current = raw.fFC_TimeHF[0];
        TimeHF[0] = TimeHF_previous;
        TimeHF[1] = TimeHF_current;
        double DT = TimeHF_current - TimeHF_previous;
        if(1750<=DT && DT<1850)                 h1_1_deltaTimeHF->Fill(DT);
        else if(3500<=DT && DT<=3600)            h1_2_deltaTimeHF->Fill(DT);
        else if(16000000<=DT && DT<17000000)    h1_3_deltaTimeHF->Fill(DT);
        else if(32600000<=DT && DT<33600000)    h1_4_deltaTimeHF->Fill(DT);
        else if(82600000<=DT && DT<82800000)    h1_5_deltaTimeHF->Fill(DT);
        else{
		  cout << setprecision(20) << "deltaTimeHF = " << TimeHF_current - TimeHF_previous << endl;
        }
	TimeHF_previous = TimeHF_current;
    }
    else if ((int)raw.fFC_TimeHF.size()==0) cptHFlost++;
    
    // analysis if FC 
    if((int)raw.fFC_AnodeNbr.size()==0 || (int)raw.fFC_TimeHF.size()==0) continue;
    
    AnodeNbr = raw.fFC_AnodeNbr;
    Time = raw.fFC_Time;	
    Cfd = raw.fFC_Cfd;  
    Q1 = raw.fFC_Q1;  
    Q2 = raw.fFC_Q2;  
    Q3 = raw.fFC_Q3;  
 
    double Q1max = 0;
    int    Anode_Q1max = 0 ;
    double inTof_Q1max = 0 ;
    IndexQ1max = -1;
    for(int mult=0; mult<(int)raw.fFC_AnodeNbr.size(); mult++){
      if(TimeHF_current != raw.fFC_TimeHF[mult]){
        cout << "TimeHF_current : " << TimeHF_current << ", fFC_TimeHF[" <<mult << "] = " << raw.fFC_TimeHF[mult] << endl;
      }
      double tof_ns = raw.fFC_Time[mult] - TimeHF[1] + FC_TofOffset[raw.fFC_AnodeNbr[mult]-1];
      if(tof_ns<0) tof_ns += 1788.;
      IncomingTof.push_back(tof_ns);
      IncomingEnergy.push_back(NeutronEnergy_MeV(10,tof_ns));

      h1_multA->Fill(raw.fFC_AnodeNbr[mult]);
      h2_multTotA->Fill(raw.fFC_AnodeNbr[mult],raw.fFC_AnodeNbr.size());
      h1_inTofRaw[raw.fFC_AnodeNbr[mult]-1]->Fill(tof_ns);
      h2_inTofRaw->Fill(raw.fFC_AnodeNbr[mult],tof_ns);

      if(raw.fFC_Q1[mult]>Q1max) {
	IndexQ1max = mult;
        Q1max = raw.fFC_Q1[mult];
        Anode_Q1max = raw.fFC_AnodeNbr[mult];
      }
    } // end of loop over the entres of the fFC_AnodeNbr vector
    h1_indexQ1max->Fill(IndexQ1max);
    if(Anode_Q1max>0){
      inTof_Q1max = raw.fFC_Time[IndexQ1max] - TimeHF[1] + FC_TofOffset[Anode_Q1max-1];
      if(inTof_Q1max<0) inTof_Q1max += 1788.;
      h1_inTofRaw_Q1max[Anode_Q1max-1]->Fill(inTof_Q1max);
      h2_inTofRaw_Q1max->Fill(Anode_Q1max,inTof_Q1max);
      h1_inEne_Q1max[Anode_Q1max-1]->Fill(NeutronEnergy_MeV(10,inTof_Q1max));
      h2_inEne_Q1max->Fill(Anode_Q1max,NeutronEnergy_MeV(10,inTof_Q1max));
      h2_inEne_vs_Q1_Q1max[Anode_Q1max-1]->Fill(Q1max,NeutronEnergy_MeV(10,inTof_Q1max));
      // fill TTree
      tout->Fill();
    }
    else{
      cout << "error Anode_Q1max = " << Anode_Q1max << endl;
    }

    // clear vectors for next event
    AnodeNbr.clear();
    Time.clear();
    IncomingTof.clear();
    IncomingEnergy.clear();
    Cfd.clear();
    Q1.clear();
    Q2.clear();
    Q3.clear();

  }// end of loop over the entries

  fout->cd();
  tout->Write();
  h2_inTofRaw_Q1max->Write();
  h2_inEne_Q1max->Write();
  for(int a = 0 ; a < 11 ; a++){
      h1_inTofRaw_Q1max[a]->Write();
      h1_inEne_Q1max[a]->Write();
      h2_inEne_vs_Q1_Q1max[a]->Write();
  }
  fout->ls();
  fout->Close();

  cout << endl << "Number of entries with no HF = " << cptHFlost << endl;

  // --- ---- --- //
  // --- DRAW --- //
  // --- ---- --- //

  TCanvas * can_index = new TCanvas("indexQ1max","indexQ1max",0,0,1000,600);
  can_index->cd();
  h1_indexQ1max->Draw();

  TCanvas * can_HF = new TCanvas("HF","HF",0,0,2500,2000);
  can_HF->Divide(3,2);
  can_HF->cd(1); h2_multHF_vs_multFC->Draw("col");
  can_HF->cd(2); h1_1_deltaTimeHF->Draw();
  can_HF->cd(3); h1_2_deltaTimeHF->Draw();
  can_HF->cd(4); h1_3_deltaTimeHF->Draw();
  can_HF->cd(5); h1_4_deltaTimeHF->Draw();

  TCanvas * can_mult = new TCanvas("mult","mult",0,0,2000,1000);
  can_mult->Divide(3,1);
  can_mult->cd(1); gPad->SetGridx(); gPad->SetLogy();  h1_mult->Draw();
  can_mult->cd(2); gPad->SetGridx();                   h1_multA->Draw();
  can_mult->cd(3); gPad->SetGridx(); gPad->SetLogz();  h2_multTotA->Draw("colz");

  TCanvas * can_inTofRaw = new TCanvas("inTofRaw","inTofRaw_allMults",0,0,2000,1200);
  can_inTofRaw->Divide(6,2);
  TCanvas * can_inTofRaw_Q1max = new TCanvas("inTofRaw_AnodeWithQ1max","inTofRaw_Anode_With_Q1max",0,0,2000,1200);
  can_inTofRaw_Q1max->Divide(6,2);
  TCanvas * can_inEne_Q1max = new TCanvas("inEne_AnodeWithQ1max","inEne_Anode_With_Q1max",0,0,2000,1200);
  can_inEne_Q1max->Divide(6,2);
  TCanvas * can_inEne_vs_Q1_Q1max = new TCanvas("inEne_vs_Q1_Q1max","inEne_vs_Q1_Q1max",0,0,2000,1200);
  can_inEne_vs_Q1_Q1max->Divide(6,2);
  for(int i=0; i<11;i++){
    can_inTofRaw->cd(i+1);       h1_inTofRaw[i]->Draw();
    can_inTofRaw_Q1max->cd(i+1); h1_inTofRaw_Q1max[i]->Draw();
    can_inEne_Q1max->cd(i+1);       gPad->SetLogy();  h1_inEne_Q1max[i]->Draw();
    can_inEne_vs_Q1_Q1max->cd(i+1); gPad->SetLogz();  h2_inEne_vs_Q1_Q1max[i]->Draw("colz");
  }  
  can_inTofRaw->cd(12);          gPad->SetLogz(); h2_inTofRaw->Draw("colz");
  can_inTofRaw_Q1max->cd(12);    gPad->SetLogz(); h2_inTofRaw_Q1max->Draw("colz");
  can_inEne_vs_Q1_Q1max->cd(12); gPad->SetLogz(); h2_inEne_Q1max->Draw("colz");

}
