#include "SignalProcessorFC.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TSystem.h"
#include <algorithm>
#include <iostream>
#include <numeric>

SignalProcessorFC::SignalProcessorFC(const std::vector<double> &inputSamples,
                                     bool applyFilter_ = false)
    : samples(inputSamples), applyFilter(applyFilter_) {}

void SignalProcessorFC::applyLowPassFilter() {
  filteredSignal.clear();
  // Apply a low pass filter if asked
  if (applyFilter) {
    for (size_t i = 1; i < samples.size() - 1; ++i) {
      double filteredValue =
          (samples[i - 1] + samples[i] + samples[i + 1]) / 3.0;
      filteredSignal.push_back(filteredValue);
    }
  } else {
    filteredSignal = samples;
  }
}

double SignalProcessorFC::calculateCFD(const double fract, const int delay,
                                       const int dt, double &Qmax, int &iQmax,
                                       int &imax, double &Q_CFD_min,
                                       bool &Triggered, bool &Threshold) {

  signalCFD.clear();
  signalCFD = filteredSignal;

  Q_CFD_min = 1000;
  Qmax = -1000;
  double CFD_threshold = 10.; // used to be 80.

  // Convert to CFD signal
  for (int i = 0; i < filteredSignal.size(); ++i) {
    double Q2 = filteredSignal[i];

    samplesX.push_back(i * 2 + 1);
    if (Q2 > Qmax) {
      Qmax = Q2;
      iQmax = i;
    }
    if (i >= delay / dt && i < samples.size()) {
      double cfdValue = Q2 * fract - filteredSignal[i - delay / dt];
      signalCFD[i] = cfdValue;
      if (cfdValue < Q_CFD_min) {
        imax = i - delay / dt;
        Q_CFD_min = cfdValue;
      }
    }
  }

  int min, max;
  double T_cfd;
  Triggered = false;
  Threshold = false;
  min = 0, max = 0;

  // imax -> Peak Position of the CFD signal
  // Test the presence of the positive bump at the beginning of the CFD signal
  for (int i = imax; i >= 0; i--) {
    if (signalCFD[i] > 0) {
      min = i;
      max = i + 2;
      Triggered = true;
      break;
    }
  }

  int countThresh = 0;
  int joker = 2;

  // Test if the signal is significant compared to the fluctuations
  // We need to have at least 3 consecutive points below -CFD_threshold
  for (int i = min; i <= imax + 3; i++) {
    if (signalCFD[i] < -CFD_threshold) {
      countThresh++;
      if (countThresh >= 2) { // Owen : previous comment says at least 3 but
                              // code says at least 2
        Threshold = true;
        break;
      }
    } else if (signalCFD[i] > -CFD_threshold && countThresh > 0) {
      if (joker > 0) {
        joker--;
        continue;
      } else {
        Threshold = false;
        break;
      }
    } else if (signalCFD[i] > -CFD_threshold && countThresh == 0) {
      continue;
    }
  }

  // Extract the CFD time using a quadratic fit on the CFD signal descent
  if (Triggered && Threshold) {

    double x1 = min;
    double x2 = min + 1;
    double x3 = min + 2;
    double y1 = signalCFD[min];
    double y2 = signalCFD[min + 1];
    double y3 = signalCFD[min + 2];

    double a, b, cc, d;
    b = ((y2 - y3) - (x2 * x2 - x3 * x3) * (y1 - y2) / (x1 * x1 - x2 * x2)) /
        (-(x2 * x2 - x3 * x3) / (x1 + x2) + x2 - x3);
    a = -b / (x1 + x2) + (y1 - y2) / (x1 * x1 - x2 * x2);
    cc = y1 - b * x1 - a * x1 * x1;

    d = b * b - 4. * a * cc;
    if (std::abs(a) > 1e-15) {
      if (d >= 0) {
        T_cfd = 0.5 * (-b - sqrt(d)) / a;
      } else {
        Triggered = false;
        T_cfd = -10000;
      }
    } else {
      T_cfd = -cc / b;
    }
    T_cfd = T_cfd * dt; // sampler to ns
  } else {
    T_cfd = -10000;
  }
  // To remove negative Q
  if (T_cfd <= 6) {
    Triggered = false;
  }
  return T_cfd;
}

double SignalProcessorFC::integrateSignal(int binWidth, double startTime,
                                          double endTime) {

  // Temporal bounds verification
  if (startTime >= endTime || filteredSignal.empty()) {
    // std::cerr << "Erreur : Bornes temporelles invalides ou filteredSignal
    // vide." << std::endl; std::cout << "start " << startTime << std::endl;
    // std::cout << "end " << endTime << std::endl;
    return 0.0;
  }

  // std::cout << "signal size" << filteredSignal.size() << std::endl;

  // Calculating start and end indices for the temporal boundaries
  int startIndex = static_cast<int>(startTime / binWidth);
  int endIndex = static_cast<int>(endTime / binWidth);

  // Fraction of the initial bin
  double startFraction = 1.0 - (startTime - startIndex * binWidth) / binWidth;

  // Fraction of the final bin
  double endFraction = (endTime - endIndex * binWidth) / binWidth;

  /* std::cout << startIndex <<" "<< endIndex << std::endl; */
  // Sum of complete bins between the bounds
  double integral = 0.0;
  /* for (int i = startIndex + 1; i < endIndex; ++i) { */
  for (int i = startIndex + 1; i < endIndex; ++i) {
    if (filteredSignal[i] > 1e6) {
    }
    integral += filteredSignal[i] * binWidth;
  }
  /* std::cout << endTime << " " << endIndex << " " << startFraction <<" " <<
   * endFraction <<" " <<" int " << integral << std::endl; */
  // Adding fractions at the endpoints
  integral += filteredSignal[startIndex] * startFraction * binWidth;
  if (endIndex < filteredSignal.size()) { // to avoid going out of the signal
    integral += filteredSignal[endIndex] * endFraction * binWidth;
  }

  return integral;
}

void SignalProcessorFC::drawRawSignal() const {
  /* TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600);
   */
  TGraph *graph = new TGraph(samples.size(), samplesX.data(), samples.data());
  graph->SetTitle("Signal");
  graph->Draw("APL");

  /* canvas->Update(); */
  /* gPad->WaitPrimitive(); */
  /* gSystem->Sleep(5000); */
}

double SignalProcessorFC::GetAmpStartGate(int binWidth, double startGate) {

  // Calculating start index for the temporal boundarie
  int startIndex = static_cast<int>(startGate / binWidth);

  return filteredSignal[startIndex];
}

void SignalProcessorFC::drawFilteredSignal() const {
  // TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600);
  TGraph *graph =
      new TGraph(filteredSignal.size(), samplesX.data(), filteredSignal.data());
  graph->SetTitle("Filtered Signal");
  graph->SetMarkerStyle(20);
  graph->Draw("APL");
  // canvas->Update();
  /* gPad->WaitPrimitive(); */
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
}

void SignalProcessorFC::drawSignalCFD() const {
  // TCanvas* canvas = new TCanvas("CFDCanvas", "CFD Canvas", 800, 600);
  TGraph *graph =
      new TGraph(signalCFD.size(), samplesX.data(), signalCFD.data());
  graph->SetTitle("Signal CFD");
  graph->Draw("AL");
  // canvas->Update();
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
  /* gPad->WaitPrimitive(); */
}
