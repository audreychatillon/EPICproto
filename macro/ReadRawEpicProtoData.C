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
  if(Pmbar==1150){
    ch->Add("../output/conversion/RawEpicData_run11.root");
    ch->Add("../output/conversion/RawEpicData_run12.root");
    PA = "V4b";
  }
  else if(Pmbar==1000){
    ch->Add("../output/conversion/RawEpicData_run13.root");
    ch->Add("../output/conversion/RawEpicData_run14.root");
    PA = "V4b";
  }
  else if(Pmbar==1170){
    ch->Add("../output/conversion/RawEpicData_run15.root");
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

  TH2F * h2_FCdiscri_perA[11];
  for(int a=0; a<11; a++){
    h2_FCdiscri_perA[a] = new TH2F(Form("Q2Q3vsQ1_A%ii_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("Q2Q3vsQ1_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),5000,0,100000,1000,0,10);
    h2_FCdiscri_perA[a]->SetDirectory(0);
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
    if(q3>0){
      h2_FCdiscri_perA[anode-1]->Fill(q1,q2/q3);
    }
  }// end of loop over the entries

  cout << endl;

  // --- ---- --- //
  // --- DRAW --- //
  // --- ---- --- //

  TCanvas * can_index = new TCanvas("index_check","index_check",0,0,600,400);
  can_index->cd(); gPad->SetLogy(); h1_index->Draw();

   TCanvas * can_FCdiscri[11];
   for(int a = 0 ; a<11; a++){
     can_FCdiscri[a] = new TCanvas(Form("FC_discri_A%i_P%imbar_PA%s",a+1,Pmbar,PA.c_str()),Form("FC_discri_A%i+P%imbar_PA%s",a+1,Pmbar,PA.c_str()),0,0,2200,1500);
    can_FCdiscri[a]->cd(); h2_FCdiscri_perA[a]->Draw("col");
   }

}
