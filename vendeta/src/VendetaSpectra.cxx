#include "VendetaSpectra.h"
#include "NPApplication.h"

using namespace vendeta;
////////////////////////////////////////////////////////////////////////////////
VendetaSpectra::VendetaSpectra() {
  // Set Pointers:
  m_detector =
    std::dynamic_pointer_cast<VendetaDetector>(nptool::Application::GetApplication()->GetDetector("vendeta"));
  m_RawData = m_detector->m_RawData;
  m_Physics = m_detector->m_Physics;

  check_time = 0;

  // Declare Raw Spectra
  for(int i=0; i<72; i++){
    TString side;
    TString arch;
    TString nbr;

    if(i<36) side = " / L_";
    else side = " / R_";

    if(i<12 || (i>=36 && i<48)) arch = "I_";
    else if((i>=12 && i<24)  || (i>=48 && i<60)) arch = "II_";
    else if((i>=24 && i<36)  || (i>=60 && i<72)) arch = "III_";
    int det = i%12 + 1;
    nbr = Form("%i",det);

    TString arch_name = side+arch+nbr;

    TString histo_name_HG = Form("PSD_HG_DET%i",i+1);
    histo_name_HG += arch_name;
    m_raw_hist_HG[i] = new TH2F(histo_name_HG, histo_name_HG, 1000, 0, 600000, 200, 0, 1.4);

    TString histo_name_LG = Form("PSD_LG_DET%i",i+1);
    histo_name_LG += arch_name;
    m_raw_hist_LG[i] = new TH2F(histo_name_LG, histo_name_LG, 1000, 0, 600000, 200, 0, 1.4);

    TString histo_name = Form("Q1_LG_DET%i",i+1);
    histo_name += arch_name;
    m_raw_Q1_LG[i] = new TH1F(histo_name, histo_name, 1000, 0, 600000);

    histo_name = Form("Q1_HG_DET%i",i+1);
    histo_name += arch_name;
    m_raw_Q1_HG[i] = new TH1F(histo_name, histo_name, 1000, 0, 600000);

    histo_name = Form("Qmax_LG_DET%i",i+1);
    histo_name += arch_name;
    m_raw_Qmax_LG[i] = new TH1F(histo_name, histo_name, 1000, 0, 50000);

    histo_name = Form("Qmax_HG_DET%i",i+1);
    histo_name += arch_name;
    m_raw_Qmax_HG[i] = new TH1F(histo_name, histo_name, 1000, 0, 50000);
  }


  // Build Raw Canvas
  auto c_raw_L1_HG = new TCanvas("Vendeta PSD L1 HG");
  auto c_raw_L1_LG = new TCanvas("Vendeta PSD L1 LG");
  c_raw_L1_HG->Divide(6, 2);
  c_raw_L1_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+1;
    c_raw_L1_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_L1_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }
  auto c_raw_L2_HG = new TCanvas("Vendeta PSD L2 HG");
  auto c_raw_L2_LG = new TCanvas("Vendeta PSD L2 LG");
  c_raw_L2_HG->Divide(6, 2);
  c_raw_L2_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+13;
    c_raw_L2_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_L2_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }
  auto c_raw_L3_HG = new TCanvas("Vendeta PSD L3 HG");
  auto c_raw_L3_LG = new TCanvas("Vendeta PSD L3 LG");
  c_raw_L3_HG->Divide(6, 2);
  c_raw_L3_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+25;
    c_raw_L3_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_L3_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }
  auto c_raw_R1_HG = new TCanvas("Vendeta PSD R1 HG");
  auto c_raw_R1_LG = new TCanvas("Vendeta PSD R1 LG");
  c_raw_R1_HG->Divide(6, 2);
  c_raw_R1_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+37;
    c_raw_R1_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_R1_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }
  auto c_raw_R2_HG = new TCanvas("Vendeta PSD R2 HG");
  auto c_raw_R2_LG = new TCanvas("Vendeta PSD R2 LG");
  c_raw_R2_HG->Divide(6, 2);
  c_raw_R2_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+49;
    c_raw_R2_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_R2_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }
  auto c_raw_R3_HG = new TCanvas("Vendeta PSD R3 HG");
  auto c_raw_R3_LG = new TCanvas("Vendeta PSD R3 LG");
  c_raw_R3_HG->Divide(6, 2);
  c_raw_R3_LG->Divide(6, 2);
  for(int i=0; i<12; i++){
    int det = i+61;
    c_raw_R3_HG->cd(i+1);
    m_raw_hist_HG[det-1]->Draw("colz");
    c_raw_R3_LG->cd(i+1);
    m_raw_hist_LG[det-1]->Draw("colz");
  }

  m_graph_sampler_LG = new TGraph();
  TH2F* hLG = new TH2F("Sample LG","Sample LG",100,-100,300,100,-200,5000);
  auto c_raw_sampler_LG = new TCanvas("Vendeta Sampler LG");
  hLG->GetXaxis()->SetTitle("Time (ns)");
  hLG->Draw();
  m_graph_sampler_LG->Draw("plsame");

  m_graph_sampler_HG = new TGraph();
  TH2F* hHG = new TH2F("Sample HG","Sample HG",100,-100,400,100,-200,5000);
  auto c_raw_sampler_HG = new TCanvas("Vendeta Sampler HG");
  hHG->GetXaxis()->SetTitle("Time (ns)");
  hHG->Draw();
  m_graph_sampler_HG->Draw("plsame");


  // Declare Phy Spectra
  /* m_phy_hist["IDvQ"] = new TH2F("IDvQPhy", "ID vs Q (Phy)", 192, 0, 192, 65534 / 4, 0, 65534); */
  /* m_phy_hist["TOF"] = new TH2F("TOF", "ID vs TOF (Phy)", 192, 0, 192, 65534 / 4, 0, 65534); */
  /* // Build Raw Canvas */
  /* auto c_phy = new TCanvas("Vendeta Phy"); */
  /* c_phy->Divide(2, 2); */
  /* c_phy->cd(1); */
  /* m_phy_hist["IDvQ"]->Draw("colz"); */
  /* c_phy->cd(2); */
  /* m_phy_hist["TOF"]->Draw("colz"); */
}

////////////////////////////////////////////////////////////////////////////////
void VendetaSpectra::FillRaw() {
  auto size = m_RawData->GetHGMult();
  if(size>0){
    double HG_Time = m_RawData->GetHGTime(0);
    if(HG_Time>check_time){
      check_time = HG_Time;
    }
    else{ 
      return;
    }
  }

  TH2F* hHG[72];
  TH2F* hLG[72];

  TH1F* hQ1LG[72];
  TH1F* hQ1HG[72];
  TH1F* hQmaxLG[72];
  TH1F* hQmaxHG[72];

  for(int i=0; i<72; i++){
    hHG[i] = (TH2F*)(m_raw_hist_HG[i]);
    hLG[i] = (TH2F*)(m_raw_hist_LG[i]);

    hQ1LG[i] = (TH1F*)(m_raw_Q1_LG[i]);
    hQ1HG[i] = (TH1F*)(m_raw_Q1_HG[i]);

    hQmaxLG[i] = (TH1F*)(m_raw_Qmax_LG[i]);
    hQmaxHG[i] = (TH1F*)(m_raw_Qmax_HG[i]);
  }


  for (unsigned int i = 0; i < size; i++) {
    int det = m_RawData->GetHGDetectorNbr(i);
    hHG[det-1]->Fill(m_RawData->GetHGQ1(i), m_RawData->GetHGQ2(i)/m_RawData->GetHGQ1(i));
    hQ1HG[det-1]->Fill(m_RawData->GetHGQ1(i));
    hQmaxHG[det-1]->Fill(m_RawData->GetHGQmax(i));
  }

  size = m_RawData->GetLGMult();
  for (unsigned int i = 0; i < size; i++) {
    int det = m_RawData->GetLGDetectorNbr(i);
    hLG[det-1]->Fill(m_RawData->GetLGQ1(i), m_RawData->GetLGQ2(i)/m_RawData->GetLGQ1(i));
    hQ1LG[det-1]->Fill(m_RawData->GetLGQ1(i));
    hQmaxLG[det-1]->Fill(m_RawData->GetLGQmax(i));
  }

  auto g_sampler = (TGraph*)(m_graph_sampler_LG);
  int size_sample_LG = m_RawData->GetLGSampleSize();
  int sampler_before_th;
  if(size_sample_LG>0) {
    sampler_before_th = m_RawData->GetLGSample(0);
  }
  for(unsigned int i=1; i<size_sample_LG; i++){
    double sample = m_RawData->GetLGSample(i);
    int time_sample = -2*sampler_before_th + 2*(i-1);
    g_sampler->SetPoint(i-1,time_sample,sample);
  }

  auto g_sampler_HG = (TGraph*)(m_graph_sampler_HG);
  int size_sample_HG = m_RawData->GetHGSampleSize();
  if(size_sample_HG>0) {
    sampler_before_th = m_RawData->GetHGSample(0);
  }
  for(unsigned int i=1; i<size_sample_HG; i++){
    double sample = m_RawData->GetHGSample(i);
    int time_sample = -2*sampler_before_th + 2*(i-1);
    g_sampler_HG->SetPoint(i-1,time_sample,sample);
  }


}
////////////////////////////////////////////////////////////////////////////////
void VendetaSpectra::FillPhy() {

}
////////////////////////////////////////////////////////////////////////////////
void VendetaSpectra::Clear() {

  for(int i=0; i<72; i++){
    m_raw_hist_HG[i+1]->Clear();
    m_raw_hist_LG[i+1]->Clear();

    m_raw_Q1_LG[i+1]->Clear();
    m_raw_Q1_HG[i+1]->Clear();

    m_raw_Qmax_LG[i+1]->Clear();
    m_raw_Qmax_HG[i+1]->Clear();
  }
}
