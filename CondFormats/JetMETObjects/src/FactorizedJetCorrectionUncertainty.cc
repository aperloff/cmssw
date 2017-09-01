// This is the file "FactorizedJetCorrectionUncertainty.cc".
// This is the implementation of the class FactorizedJetCorrectionUncertainty.
// Author: Alexx Perloff, Emily MacDonald
// Email:  alexx.stephen.perloff@cern.ch, emacdonald16@tamu.edu

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/SimpleJetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/Utilities.h"
#include "Math/PtEtaPhiE4D.h"
#include "Math/Vector3D.h"
#include "Math/LorentzVector.h"
#include <vector>
#include <string>
#include <sstream>

//------------------------------------------------------------------------
//--- Default FactorizedJetCorrectionUncertainty constructor -------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertainty::FactorizedJetCorrectionUncertainty()
{
}
//------------------------------------------------------------------------
//--- FactorizedJetCorrectionUncertainty constructor ---------------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertainty::FactorizedJetCorrectionUncertainty(const std::string& fLevels, const std::string& fFiles):
mCalc(fLevels,fFiles)
{
}
//------------------------------------------------------------------------
//--- FactorizedJetCorrectionUncertainty constructor ---------------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertainty::FactorizedJetCorrectionUncertainty(const JetCorrectionUncertaintyCollection& fParameters):
  mCalc(fParameters)
{
}

//------------------------------------------------------------------------
//--- Returns the correction ---------------------------------------------
//------------------------------------------------------------------------
float FactorizedJetCorrectionUncertainty::getUncertainty()
{
  return mCalc.getUncertainty(mValues);
}
//------------------------------------------------------------------------
//--- Returns the vector of subcorrections, up to a given level ----------
//------------------------------------------------------------------------
std::vector<float> FactorizedJetCorrectionUncertainty::getSubUncertainties()
{
  return mCalc.getSubUncertainties(mValues);
}
//------------------------------------------------------------------------
//--- Setters ------------------------------------------------------------
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setNPV(int fNPV)
{

  mValues.setNPV (fNPV);
}
void FactorizedJetCorrectionUncertainty::setJetEta(float fEta)
{
  mValues.setJetEta( fEta );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJetPt(float fPt)
{
  mValues.setJetPt(fPt);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJetPhi(float fPhi)
{
  mValues.setJetPhi( fPhi );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJetE(float fE)
{
  mValues.setJetE(fE);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJetEMF(float fEMF)
{
  mValues.setJetEMF( fEMF );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJetA(float fA)
{
  mValues.setJetA( fA );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setRho(float fRho)
{
  mValues.setRho(fRho);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJPTrawP4(const TLorentzVector& fJPTrawP4)
{
  mValues.setJPTrawP4(fJPTrawP4);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setJPTrawOff(float fJPTrawOff)
{
  mValues.setJPTrawOff(fJPTrawOff);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setLepPx(float fPx)
{
  mValues.setLepPx( fPx );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setLepPy(float fPy)
{
  mValues.setLepPy( fPy );
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setLepPz(float fPz)
{
  mValues.setLepPz(fPz);
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertainty::setAddLepToJet(bool fAddLepToJet)
{
  mValues.setAddLepToJet(fAddLepToJet);
}
