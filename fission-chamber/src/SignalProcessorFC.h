#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <TCanvas.h>
#include <vector>

class SignalProcessorFC {
private:
  bool applyFilter;
  std::vector<double> samples;
  std::vector<double> samplesX;
  std::vector<double> filteredSignal;
  std::vector<double> signalCFD;

public:
  SignalProcessorFC(const std::vector<double> &inputSamples, bool applyFilter_);
  void applyLowPassFilter();
  double calculateCFD(double fract, const int delay, const int dt, double &Qmax,
                      int &iQmax, int &imax, double &Q_CFD_min, bool &Triggered,
                      bool &Threshold);
  double integrateSignal(int binWidth, double startGate, double endGate);
  double GetAmpStartGate(int binWidth, double startGate);
  void drawRawSignal() const;
  void drawFilteredSignal() const;
  void drawSignalCFD() const;
};

#endif // SIGNALPROCESSOR_H
