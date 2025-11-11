#include "SignalProcessor.h"
#include <iostream>
#include <algorithm>
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TSystem.h"
#include <numeric>


SignalProcessor::SignalProcessor(const std::vector<double>& inputSamples, bool applyFilter_ =false) : samples(inputSamples), applyFilter(applyFilter_) {}

void SignalProcessor::applyLowPassFilter() {
  filteredSignal.clear();
  // Apply a low pass filter if asked 
  if(applyFilter){
    for (size_t i = 1; i < samples.size() - 1; ++i) {
      double filteredValue = (samples[i - 1] + samples[i] + samples[i + 1]) / 3.0;
      filteredSignal.push_back(filteredValue);
    }
  }
  else{
    filteredSignal = samples;
  }
}

double SignalProcessor::calculateCFD(const double fract, const int delay, const int dt, double &Qmax, int &iQmax, int &imax, double &Q_CFD_min, bool &Triggered, bool &Threshold) {
  
  signalCFD.clear();
  signalCFD = filteredSignal;
  
  Q_CFD_min = 1000;
  Qmax = -1000;
  
  //Convert to CFD signal
  for (int i = 0; i < filteredSignal.size(); ++i) {
    double Q2 = filteredSignal[i];

    samplesX.push_back(i*2+1);
    if (Q2 > Qmax) {
      Qmax = Q2;
      iQmax = i;
    }
    if (i >= delay/dt && i < samples.size()) {
      double cfdValue = Q2 * fract - filteredSignal[i - delay/dt];
      signalCFD[i - delay/dt] = cfdValue;
      //signalCFD[i] = cfdValue;
      if (cfdValue < Q_CFD_min) {
        imax = i - delay/dt; 
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
  for (int i = imax; i >= 0 ; i--) {
    if (signalCFD[i] > 0) {
      min = i;
      max = i + 2;
      Triggered = true;
      break;
    }
  }

  int countThresh = 0;
  int joker = 2;
  
  //Test if the signal is significant compared to the fluctuations
  //We need to have at least 3 consecutive points below -80
  for (int i = min; i <= imax + 3; i++) {
    if (signalCFD[i] < -80) {
      countThresh++;
      if (countThresh >= 2) {
        Threshold = true;
        break;
      }
    } else if (signalCFD[i] > -80 && countThresh > 0) {
      if (joker > 0) {
        joker--;
        continue;
      } else {
        Threshold = false;
        break;
      }
    } else if (signalCFD[i] > -80 && countThresh == 0) {
      continue;
    }
  }

  //Extract the CFD time using a quadratic fit on the CFD signal descent
  if (Triggered && Threshold) {

    double x1 = min;
    double x2 = min + 1;
    double x3 = min + 2;
    double y1 = signalCFD[min];
    double y2 = signalCFD[min + 1];
    double y3 = signalCFD[min + 2];

    double a, b, cc, d;
    b  = ((y2-y3) - (x2*x2-x3*x3)*(y1-y2)/(x1*x1-x2*x2)) / (-(x2*x2-x3*x3)/(x1+x2) + x2 - x3);
    a  = -b / (x1 + x2) + (y1 - y2) / (x1*x1 - x2*x2);
    cc = y1 - b*x1 - a*x1*x1;

    d = b*b - 4.*a*cc;
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

double SignalProcessor::integrateSignal(int binWidth, double startTime, double endTime) {
  
  // Temporal bounds verification
  if (startTime >= endTime || filteredSignal.empty()) {
    std::cerr << "Erreur : Bornes temporelles invalides ou filteredSignal vide." << std::endl;
    return 0.0;
  }

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
  for (int i = startIndex +1; i < endIndex; ++i) {
    /* std::cout << i << std::endl; */
    integral += filteredSignal[i] * binWidth;
  }
  /* std::cout << endTime << " " << endIndex << " " << startFraction <<" " << endFraction <<" " <<" int " << integral << std::endl; */
  // Adding fractions at the endpoints
  integral += filteredSignal[startIndex] * startFraction * binWidth;
  if (endIndex < filteredSignal.size()){ //to avoid going out of the signal
        integral += filteredSignal[endIndex] * endFraction * binWidth;
  }

  return integral;
}

double SignalProcessor::calcRisetime(int dt = 2){

	if (filteredSignal.size() < 2) return -1;
    	int n = filteredSignal.size();

    	// Find global maximum (pulse peak)
    	double ymax = -1e99;
    	int iMax = -1;
    	for (int i = 0; i < n; i++) {
    	    if (filteredSignal[i] > ymax) {
    	        ymax = filteredSignal[i];
    	        iMax = i;
    	    }
    	}
    	if (iMax < 1){
		//std::cout << "WARNING in calcRiseTime : signal max not found" << std::endl;
	       	return -1; // invalid peak position
	}

    	// Find minimum before the maximum (baseline)
    	double ymin = 1e99;
    	for (int i = 0; i <= iMax; i++) {
    	    if (filteredSignal[i] < ymin) ymin = filteredSignal[i];
    	}
	//std::cout << "Check if ymin is very different of 0 : " << ymin << ". If close to zero searching for minimum is not necessary!" << std::endl;

    	double y10 = ymin + 0.1 * (ymax - ymin);
    	double y90 = ymin + 0.9 * (ymax - ymin);
    	double t10 = -1, t90 = -1;

    	// Scan to find 10% and 90% crossings
    	for (int i = 1; i <= iMax; i++) {
    	    double y1 = filteredSignal[i-1];
    	    double y2 = filteredSignal[i];
    	    double x1 = (i-1) * dt;
    	    double x2 = i * dt;

    	    // Crossing 10%
    	    if (t10 < 0 && y1 < y10 && y2 >= y10) {
    	        double frac = (y10 - y1) / (y2 - y1);
    	        t10 = x1 + frac * (x2 - x1);
    	    }
    	    // Crossing 90%
    	    if (y1 < y90 && y2 >= y90) {
    	        double frac = (y90 - y1) / (y2 - y1);
    	        t90 = x1 + frac * (x2 - x1);
    	        break;  // we’re done after 90%
    	    }
    	}
    	if (t10 < 0 || t90 < 0){
		//std::cout << "WARNING in calcRiseTime : negative t10 or t90" << std::endl;
	       	return -1; // didn’t find crossings
	}
	//std::cout << "Rise time = " << t90 - t10 << std::endl;
    	return t90 - t10;
}

void SignalProcessor::drawRawSignal() const {
  // TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600);
  TGraph* graph = new TGraph(samples.size(), samplesX.data(), samples.data());
  graph->SetTitle("Signal");
  graph->Draw("APL");
  // canvas->Update();
  gPad->WaitPrimitive();
  // gSystem->Sleep(5000);
}

void SignalProcessor::drawFilteredSignal() const {
  // TCanvas* canvas = new TCanvas("SignalCanvas", "Signal Canvas", 800, 600);
  TGraph* graph = new TGraph(filteredSignal.size(), samplesX.data(), filteredSignal.data());
  graph->SetTitle("Filtered Signal");
  graph->SetMarkerStyle(20);
  graph->Draw("APL");
  // canvas->Update();
  gPad->WaitPrimitive();
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
}

void SignalProcessor::drawSignalCFD() const {
  // TCanvas* canvas = new TCanvas("CFDCanvas", "CFD Canvas", 800, 600);
  TGraph* graph = new TGraph(signalCFD.size(), samplesX.data(), signalCFD.data());
  graph->SetTitle("Signal CFD");
  graph->Draw("AL");
  // canvas->Update();
  // gSystem->Sleep(5000);  // Pause pendant 5 secondes
  gPad->WaitPrimitive();
}
