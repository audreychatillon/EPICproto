#include "RawEpicProto.h"
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


void run(int Pmbar)
{

  // --- ---------- --- //
  // --- INPUT DATA --- //
  // --- ---------- --- //
  string PA;
  TChain * ch = new TChain("RawEpicProto");
  if(Pmbar==810){
    ch->Add("../output/conversion/RawEpicData_run16.root");
    PA = "V4b";
  }
  else if(Pmbar==1000){
    ch->Add("../output/conversion/RawEpicData_run13.root");
    ch->Add("../output/conversion/RawEpicData_run14.root");
    PA = "V4b";
  }
  else if(Pmbar==1150){// beam not centered
    ch->Add("../output/conversion/RawEpicData_run11.root");
    ch->Add("../output/conversion/RawEpicData_run12.root");
    PA = "V4b";
  }
  else if(Pmbar==1170){
    ch->Add("../output/conversion/RawEpicData_run15.root");
    ch->Add("../output/conversion/RawEpicData_run17.root");
    ch->Add("../output/conversion/RawEpicData_run18.root");
    ch->Add("../output/conversion/RawEpicData_run19.root");
    PA = "V4b";
  }
  else{
    ch->Add("../output/conversion/RawEpicData_run7.root");
    Pmbar = 1100;
    PA = "V82";
  }
  ch->ls();
  RawEpicProto raw(ch);
  cout << "P = " << Pmbar << " mbar" << endl;
  cout << "FC readout with PA" << PA << endl;

  // --- ---------- --- //
  // --- HISTOGRAMS --- //
  // --- ---------- --- //

  TH1I * h1_index = new TH1I("indexQ1max","indexQ1max",20,-1.5,18.5);
  h1_index->SetDirectory(0);

  TH1F * h1_Q1_perA[11];
  TH2F * h2_FCdiscri_perA[11];
  TH2F * h2_Q1vsE_perA[11];
  for(int a=0; a<11; a++){
    h1_Q1_perA[a] = new TH1F(Form("Q1_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q1_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),50000,0,500000);
    h1_Q1_perA[a]->SetDirectory(0);
    h2_FCdiscri_perA[a] = new TH2F(Form("Q2Q3vsQ1_A%ii_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q2Q3vsQ1_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),5000,0,100000,1000,0,10);
    h2_FCdiscri_perA[a]->SetDirectory(0);
    h2_Q1vsE_perA[a] = new TH2F(Form("Q1vsE_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q1vsE_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),400,0,100,5000,0,250000);
    h2_Q1vsE_perA[a]->SetDirectory(0);
  }

 
  // --- --------- --- //
  // --- VARIABLES --- //
  // --- --------- --- //

  // --- ---- --- //
  // --- LOOP --- //
  // --- ---- --- //

  ULong64_t nentries = (ULong64_t)ch->GetEntries();
  cout << "number of entries: " << nentries << endl;
  for(ULong64_t Entry=0; Entry<nentries; Entry++){

    raw.GetEntry(Entry);
    if ((Entry % 1000000)==0) cout << "\r === Entry = " << Entry << " === " << flush;

    //cout << "anode vector size : " << raw.AnodeNbr->size() << endl;
    //cout << "IndexQ1max = " << raw.IndexQ1max << endl;
    h1_index->Fill(raw.IndexQ1max);
    short anode = raw.AnodeNbr->at(raw.IndexQ1max);
    double q1 = raw.Q1->at(raw.IndexQ1max);
    double q2 = raw.Q2->at(raw.IndexQ1max);
    double q3 = raw.Q3->at(raw.IndexQ1max);
    double iE = raw.IncomingEnergy->at(raw.IndexQ1max);
    if(q3>0 && 1 < iE && iE < 100){
      h1_Q1_perA[anode-1]->Fill(q1);
      h2_FCdiscri_perA[anode-1]->Fill(q1,q2/q3);
      h2_Q1vsE_perA[anode-1]->Fill(iE,q1);
    }
  }// end of loop over the entries

  cout << endl;

  // --- ---- --- //
  // --- DRAW --- //
  // --- ---- --- //

  TCanvas * can_index = new TCanvas("index_check","index_check",0,0,600,400);
  can_index->cd(); gPad->SetLogy(); h1_index->Draw();

  TCanvas * can_Q1[11];
  for(int a = 0 ; a<11; a++){
    can_Q1[a] = new TCanvas(Form("Q1_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q1_A%i+P%imbar_PA%s",a+1,Pmbar,PA.c_str()),0,0,2200,1500);
    can_Q1[a]->cd(); h1_Q1_perA[a]->Draw();
  }

  TCanvas * can_Q1vsE[11];
  for(int a = 0 ; a<11; a++){
    can_Q1vsE[a] = new TCanvas(Form("Q1vsE_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q1vsE_A%i+P%imbar_PA%s",a+1,Pmbar,PA.c_str()),0,0,2200,1500);
    can_Q1vsE[a]->cd(); h2_Q1vsE_perA[a]->Draw("colz");
  }


  TCanvas * can_FCdiscri[11];
  for(int a = 0 ; a<11; a++){
    can_FCdiscri[a] = new TCanvas(Form("FC_discri_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("FC_discri_A%i+P%imbar_PA%s",a+1,Pmbar,PA.c_str()),0,0,2200,1500);
   can_FCdiscri[a]->cd(); h2_FCdiscri_perA[a]->Draw("col");
  }

  TFile * fsave = new TFile(Form("../output/conversion/RawEPICprotoData_histo_at_P%imbar_PA%s.root",Pmbar,PA.c_str()),"recreate");
  for(int a=0; a<11; a++){
 	can_Q1[a]->Write();
	h1_Q1_perA[a]->Write();
	can_Q1vsE[a]->Write();
	h2_Q1vsE_perA[a]->Write();
	can_FCdiscri[a]->Write();
	h2_FCdiscri_perA[a]->Write();
  }
  fsave->ls();
  fsave->Close();

}
