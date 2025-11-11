#include "FissionChamberSpectra.h"
#include "NPApplication.h"

using namespace fission_chamber;
////////////////////////////////////////////////////////////////////////////////
FissionChamberSpectra::FissionChamberSpectra() {
  // Set Pointers:
  m_detector = std::dynamic_pointer_cast<FissionChamberDetector>(
      nptool::Application::GetApplication()->GetDetector("fission-chamber"));
  m_RawData = m_detector->m_RawData;
  m_Physics = m_detector->m_Physics;

  check_time = 0;

  // Declare Raw Spectra
  for (int i = 1; i < 12; i++) {
    m_raw_hist[Form("Q3Q2vQ1_Anode%i", i)] =
        new TH2F(Form("Q3Q2vQ1_Anode%i", i), Form("Q2/Q3vQ1_Anode%i", i), 2000,
                 0, 600000, 1000, 0, 10);
    m_raw_hist[Form("Q2vQ1_Anode%i", i)] =
        new TH2F(Form("Q2vQ1_Anode%i", i), Form("Q2vQ1_Anode%i", i), 1000, 0,
                 200000, 500, 0, 40000);
    m_raw_hist[Form("QmaxvQ1_Anode%i", i)] =
        new TH2F(Form("QmaxvQ1_Anode%i", i), Form("QmaxvQ1_Anode%i", i), 1000,
                 0, 200000, 500, 0, 4000);

    TString histo_name = Form("Q1_Anode%i", i);
    m_raw_hist[Form("Q1_Anode%i", i)] =
        new TH1F(histo_name, histo_name, 3000, 0, 600000);

    histo_name = Form("Qmax_Anode%i", i);
    m_raw_hist[Form("Qmax_Anode%i", i)] =
        new TH1F(histo_name, histo_name, 1000, 0, 10000);
  }
  m_raw_hist["Anode_ID"] = new TH1F("Anode_ID", "Anode_ID", 12, 0, 12);
  m_raw_hist["Q1vsAnode"] =
      new TH2F("Q1vsAnode", "Q1vsAnode", 12, 0, 12, 1000, 0, 200000);

  // Build Raw Canvas
  auto c_raw = new TCanvas("FC Q2vsQ1");
  c_raw->Divide(6, 2);
  c_raw->cd(1);
  m_raw_hist["Anode_ID"]->Draw();
  for (int i = 1; i < 12; i++) {
    c_raw->cd(i + 1);
    m_raw_hist[Form("Q2vQ1_Anode%i", i)]->Draw("colz");
    gPad->SetLogz();
  }
  auto c_raw2 = new TCanvas("FC raw inToF");
  m_raw_hist["inToF"] = new TH1F("Raw inToF ", "Raw inToF", 4000, -2000, 5000);
  m_raw_hist["inToF"]->Draw();

  auto c_raw3 = new TCanvas("FC Q1");
  c_raw3->Divide(6, 2);
  for (int i = 1; i < 12; i++) {
    c_raw3->cd(i);
    m_raw_hist[Form("Q1_Anode%i", i)]->Draw();
  }
  auto c_raw4 = new TCanvas("FC Qmax");
  c_raw4->Divide(6, 2);
  for (int i = 1; i < 12; i++) {
    c_raw4->cd(i);
    m_raw_hist[Form("Qmax_Anode%i", i)]->Draw();
  }

  m_graph_sampler = new TGraph();
  TH2F *h2 = new TH2F("FC Sample", "FC Sample", 100, -50, 100, 100, -200, 5000);
  auto c_raw_sampler = new TCanvas("FC Sampler");
  h2->GetXaxis()->SetTitle("Time (ns)");
  h2->Draw();
  m_graph_sampler->Draw("plsame");

  // Declare Phy Spectra
  m_phy_hist["inToF"] = new TH1F("inToF", "inToF", 4000, 0, 2000);
  m_phy_hist["inEnergy"] = new TH1F("inEnergy", "inEnergy", 8000, 0, 700);
  m_phy_hist["ToFvsID"] =
      new TH2F("ToFvsID", "ToFvsID", 11, 1, 12, 4000, 0, 2000);
  m_phy_hist["inEvsID"] =
      new TH2F("inEvsID", "inEvsID", 11, 1, 12, 4000, 0, 700);

  // Build Phy Canvas
  auto c_phy1 = new TCanvas("FC Phy 1");
  c_phy1->Divide(1, 2);
  c_phy1->cd(1);
  m_phy_hist["inToF"]->Draw();
  gPad->SetLogy();
  c_phy1->cd(2);
  m_phy_hist["inEnergy"]->Draw();
  gPad->SetLogy();

  auto c_phy2 = new TCanvas("FC Phy 2");
  c_phy2->Divide(1, 2);
  c_phy2->cd(1);
  m_phy_hist["ToFvsID"]->Draw("colz");
  c_phy2->cd(2);
  m_phy_hist["inEvsID"]->Draw("colz");
}

////////////////////////////////////////////////////////////////////////////////
void FissionChamberSpectra::FillRaw() {

  int FC_mult = m_RawData->GetFCMult();

  TH1F *hID = (TH1F *)m_raw_hist["Anode_ID"];
  TH2F *hQ1vsID = (TH2F *)m_raw_hist["Q1vsAnode"];
  for (int i = 0; i < FC_mult; i++) {

    if (!m_RawData->GetFCisFakeFission(i)) {
      hQ1vsID->Fill(m_RawData->GetFCAnodeNbr(i), m_RawData->GetFCQ1(i));
      hID->Fill(m_RawData->GetFCAnodeNbr(i));
    } else {
      hQ1vsID->Fill(0.5, m_RawData->GetFCQ1(i), 1);
      hID->Fill(0.5);
    }
  }
  TH2F *hQ3Q2vsQ1[11];
  TH2F *hQ2vsQ1[11];
  TH2F *hQmaxvsQ1[11];
  TH1F *hQ1[11];
  TH1F *hQmax[11];
  TH1F *hinTof;
  hinTof = (TH1F *)m_raw_hist["inToF"];
  for (int i = 0; i < 11; i++) {
    hQ3Q2vsQ1[i] = (TH2F *)(m_raw_hist[Form("Q3Q2vQ1_Anode%i", i + 1)]);
    hQ2vsQ1[i] = (TH2F *)(m_raw_hist[Form("Q2vQ1_Anode%i", i + 1)]);
    hQmaxvsQ1[i] = (TH2F *)(m_raw_hist[Form("QmaxvQ1_Anode%i", i + 1)]);
    hQ1[i] = (TH1F *)(m_raw_hist[Form("Q1_Anode%i", i + 1)]);
    hQmax[i] = (TH1F *)(m_raw_hist[Form("Qmax_Anode%i", i + 1)]);
  }
  if (FC_mult == 1 && !m_RawData->GetFCisFakeFission(0)) {
    int anodeID = m_RawData->GetFCAnodeNbr(0);
    double Q1 = m_RawData->GetFCQ1(0);
    double Q2 = m_RawData->GetFCQ2(0);
    double Q3 = m_RawData->GetFCQ3(0);
    double Qmax = m_RawData->GetFCQmax(0);
    double TimeFC = m_RawData->GetFCTime(0);
    double TimeHF = m_RawData->GetHFTime(0);
    if (anodeID > 0 && TimeFC > check_time) {
      // if(anodeID>0){
      check_time = TimeFC;
      hQ3Q2vsQ1[anodeID - 1]->Fill(Q1, Q2 / Q3);
      hQ2vsQ1[anodeID - 1]->Fill(Q1, Q2);
      hQmaxvsQ1[anodeID - 1]->Fill(Q1, Qmax);
      hQ1[anodeID - 1]->Fill(Q1);
      hQmax[anodeID - 1]->Fill(Qmax);
      hinTof->Fill(TimeFC - TimeHF);
    }
  }

  auto g_sampler = (TGraph *)(m_graph_sampler);
  int size_sampler = m_RawData->GetFCSampleSize();
  int sampler_before_th;
  if (size_sampler > 0) {
    sampler_before_th = m_RawData->GetFCSample(0);
  }
  for (unsigned int i = 1; i < size_sampler; i++) {
    double sample = m_RawData->GetFCSample(i);
    int time_sample = -2 * sampler_before_th + 2 * (i - 1);
    g_sampler->SetPoint(i - 1, time_sample, sample);
  }
}
////////////////////////////////////////////////////////////////////////////////
void FissionChamberSpectra::FillPhy() {

  int FC_mult = m_Physics->GetMult();

  TH1F *hinTof = (TH1F *)(m_phy_hist["inToF"]);
  TH1F *hinEnergy = (TH1F *)(m_phy_hist["inEnergy"]);
  TH2F *hTofvsID = (TH2F *)(m_phy_hist["ToFvsID"]);
  TH2F *hinEvsID = (TH2F *)(m_phy_hist["inEvsID"]);
  if (FC_mult == 1 && !m_Physics->FakeFission[0]) {
    double inTOF = m_Physics->inToF[0];
    double inE = m_Physics->inEnergy[0];
    double ID = m_Physics->Anode_ID[0];

    hinTof->Fill(m_Physics->inToF[0]);
    hinEnergy->Fill(m_Physics->inEnergy[0]);

    hTofvsID->Fill(ID, inTOF);
    hinEvsID->Fill(ID, inE);
  }
}
////////////////////////////////////////////////////////////////////////////////
void FissionChamberSpectra::Clear() {
  for (auto h : m_raw_hist)
    h.second->Reset();
  for (auto h : m_phy_hist)
    h.second->Reset();
}
