#include "TROOT.h"
#include "TSystem.h"

#include <iostream>
#include <string>

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertainty.h"

using std::cout;
using std::endl;
using std::flush;
using std::string;

int main()
{
  //gROOT->ProcessLine("#include <vector>");
  //gSystem->Load("libFWCoreFWLite.so");
  //FWLiteEnabler::enable();
  ////////////// Setup the file and source information ///////////////////////
   cout << "Doing setup ... " << flush;
  string JECFile    = "/uscms_data/d2/aperloff/YOURWORKINGAREA/JEC/gitty/CMSSW_9_3_0_pre1/src/JECDatabase/textFiles/Summer16_23Sep2016V6_MC/Summer16_23Sep2016V6_MC_UncertaintySources_AK4PF.txt"; 
  string source = "AbsoluteStat";
  cout << "DONE" << endl;
  ////////////// Test the JetCorrectorParameters object ///////////////////////
  cout << "Testing JetCorrectorParameters ... " << flush;
  JetCorrectorParameters jcp(JECFile,source);
  cout << "DONE" << endl;
  ////////////// Test the JetCorrectionUncertainty object ///////////////////////
  cout << "Testing JetCorrectionUncertainty ..." << flush;
  JetCorrectionUncertainty jcu(jcp);
  JetCorrectionUncertainty jcu2(JECFile,source);
  cout << "DONE" << endl;
  ////////////// Test the JetCorrectionUncertaintyCollection object ///////////////////////
  cout << "Testing JetCorrectionUncertaintyCollection ..." << endl;
  JetCorrectionUncertaintyCollection jcuc;
  jcuc.push_back(0,jcu);
  jcuc.push_back(1,jcu2);
  cout << "DONE" << endl;
  ////////////// Test the JetCorrectionUncertaintyCollection object ///////////////////////
  cout << "Testing FactorizedJetCorrectionUncertainty ..." << flush;
  FactorizedJetCorrectionUncertainty fjcu(jcuc);
  cout << "DONE" << endl;
  return 0;
}
