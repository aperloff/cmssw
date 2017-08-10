// This is the file "FactorizedJetCorrectionUncertaintyCalculator.cc".
// This is the implementation of the class FactorizedJetCorrectionUncertaintyCalculator.
// Author: Alexx Perloff, Emily MacDonald
// Email:  alexx.stephen.perloff@cern.ch, emacdonald16@tamu.edu

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertaintyCalculator.h"
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
//--- Default FactorizedJetCorrectionUncertaintyCalculator constructor -------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertaintyCalculator::FactorizedJetCorrectionUncertaintyCalculator()
{
}
//------------------------------------------------------------------------
//--- FactorizedJetCorrectionUncertaintyCalculator constructor ---------------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertaintyCalculator::FactorizedJetCorrectionUncertaintyCalculator(const std::string& fLevels, const std::string& fFiles)
{
  initCorrectors(fLevels, fFiles);
}
//------------------------------------------------------------------------
//--- FactorizedJetCorrectionUncertaintyCalculator constructor ---------------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertaintyCalculator::FactorizedJetCorrectionUncertaintyCalculator(const JetCorrectionUncertaintyCollection& fParameters)
{
  std::vector<int> keys;
  fParameters.validKeys(keys);
  for(unsigned i=0; i<keys.size(); i++) {
    if(keys[i]>=JetCorrectionUncertaintyCollection::N_LEVELS) {
      std::stringstream sserr;
      sserr<<"unknown correction level "<<i;
      handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
    }
    mCorrectors.push_back(new SimpleJetCorrectionUncertainty(fParameters[i]));
    mBinTypes.push_back(mapping(mCorrectors[i]->parameters().definitions().binVar()));
    mParTypes.push_back(mapping(mCorrectors[i]->parameters().definitions().parVar()));
  }
}

//------------------------------------------------------------------------
//--- FactorizedJetCorrectionUncertaintyCalculator destructor ----------------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertaintyCalculator::~FactorizedJetCorrectionUncertaintyCalculator()
{
  for(unsigned i=0;i<mCorrectors.size();i++)
    delete mCorrectors[i];
}
//------------------------------------------------------------------------
//--- initialises the correctors -----------------------------------------
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::initCorrectors(const std::string& fLevels, const std::string& fFiles)
{
  //---- Read the CorrectionLevels string and parse the requested sub-correction levels.
  std::vector<std::string> tmp = parseLevels(removeSpaces(fLevels));
  for(unsigned i=0;i<tmp.size();i++) {
    try{
      mLevels.push_back(findKey(tmp[i]));
    }
    catch (const cms::Exception& e) {
      std::stringstream sserr;
      sserr<<"unknown correction level "<<tmp[i];
      handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
    }
  }
  //---- Read the parameter filenames string and parse the requested sub-correction tags.
  std::vector<std::string> Files = parseLevels(removeSpaces(fFiles));
  //---- Check the consistency between tags and requested sub-corrections.
  checkConsistency(tmp,Files);
  //---- Create instances of the requested sub-correctors.
  for(unsigned i=0;i<mLevels.size();i++) {
    mCorrectors.push_back(new SimpleJetCorrectionUncertainty(Files[i],tmp[i]));
    mBinTypes.push_back(mapping(mCorrectors[i]->parameters().definitions().binVar()));
    mParTypes.push_back(mapping(mCorrectors[i]->parameters().definitions().parVar()));
  }
}
//------------------------------------------------------------------------
//--- Mapping between variable names and variable types ------------------
//------------------------------------------------------------------------
std::vector<FactorizedJetCorrectionUncertaintyCalculator::VarTypes> FactorizedJetCorrectionUncertaintyCalculator::mapping(const std::vector<std::string>& fNames) const
{
  std::vector<VarTypes> result;
  for(unsigned i=0;i<fNames.size();i++) {
    std::string ss = fNames[i];
    if (ss=="JetPt")
      result.push_back(kJetPt);
    else if (ss=="JetEta")
      result.push_back(kJetEta);
    else if (ss=="JetPhi")
      result.push_back(kJetPhi);
    else if (ss=="JetE")
      result.push_back(kJetE);
    else if (ss=="JetEMF")
      result.push_back(kJetEMF);
    else if (ss=="RelLepPt")
      result.push_back(kRelLepPt);
    else if (ss=="PtRel")
      result.push_back(kPtRel);
    else if (ss=="NPV")
      result.push_back(kNPV);
    else if (ss=="JetA")
      result.push_back(kJetA);
    else if (ss=="Rho")
      result.push_back(kRho);
    else if (ss=="JPTrawE")
      result.push_back(kJPTrawE);
    else if (ss=="JPTrawEt")
      result.push_back(kJPTrawEt);
    else if (ss=="JPTrawPt")
      result.push_back(kJPTrawPt);
    else if (ss=="JPTrawEta")
      result.push_back(kJPTrawEta);
    else if (ss=="JPTrawOff")
      result.push_back(kJPTrawOff);
    else {
      std::stringstream sserr;
      sserr<<"unknown parameter name: "<<ss;
      handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
    }
  }
  return result;
}
//------------------------------------------------------------------------
//--- Mapping between level names and level types ------------------------
//------------------------------------------------------------------------
namespace {
const std::vector<std::string> levels_ = {
     "AbsoluteStat",
     "AbsoluteScale",
     "AbsoluteFlavMap",
     "AbsoluteMPFBias",
     "Fragmentation",
     "SinglePionECAL",
     "SinglePionHCAL",
     "FlavorQCD",
     "TimePtEta",
     "RelativeJEREC1",
     "RelativeJEREC2",
     "RelativeJERHF",
     "RelativePtBB",
     "RelativePtEC1",
     "RelativePtEC2",
     "RelativePtHF",
     "RelativeBal",
     "RelativeFSR",
     "RelativeStatFSR",
     "RelativeStatEC",
     "RelativeStatHF",
     "PileUpDataMC",
     "PileUpPtRef",
     "PileUpPtBB",
     "PileUpPtEC1",
     "PileUpPtEC2",
     "PileUpPtHF",
     "PileUpMuZero",
     "PileUpEnvelope",
     "SubTotalPileUp",
     "SubTotalRelative",
     "SubTotalPt",
     "SubTotalScale",
     "SubTotalAbsolute",
     "SubTotalMC",
     "Total",
     "TotalNoFlavor",
     "TotalNoTime",
     "TotalNoFlavorNoTime",
     "FlavorZJet",
     "FlavorPhotonJet",
     "FlavorPureGluon",
     "FlavorPureQuark",
     "FlavorPureCharm",
     "FlavorPureBottom",
     "TimeRunBCD",
     "TimeRunEF",
     "TimeRunG",
     "TimeRunH",
     "CorrelationGroupMPFInSitu",
     "CorrelationGroupIntercalibration",
     "CorrelationGroupbJES",
     "CorrelationGroupFlavor",
     "CorrelationGroupUncorrelated"
 };
}
FactorizedJetCorrectionUncertaintyCalculator::LevelTypes FactorizedJetCorrectionUncertaintyCalculator::findKey( std::string const & level ) const {
  // Check the default uncertainties
  std::vector<std::string>::const_iterator found =
    find( levels_.begin(), levels_.end(), level );
  if ( found != levels_.end() ) {
    return static_cast<FactorizedJetCorrectionUncertaintyCalculator::LevelTypes>(found - levels_.begin());
  }

  // Didn't find default uncertainties, throw exception
  throw cms::Exception("InvalidInput") << " Cannot find level " << level << std::endl;
}
//------------------------------------------------------------------------
//--- Consistency checker ------------------------------------------------
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::checkConsistency(const std::vector<std::string>& fLevels, const std::vector<std::string>& fTags)
{
  //---- First check: the number of tags must be equal to the number of sub-corrections.
  if (fLevels.size() != fTags.size()) {
    std::stringstream sserr;
    sserr<<"number of correction levels: "<<fLevels.size()<<" doesn't match # of tags: "<<fTags.size();
    handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
  }
  //---- Second check: each tag must contain the corresponding sub-correction level.
  for(unsigned int i=0;i<fTags.size();i++) {
    if ((int)fTags[i].find(fLevels[i])<0) {
      std::stringstream sserr;
      sserr<<"inconsistent tag: "<<fTags[i]<<" for "<<"the requested correction: "<<fLevels[i];
      handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
    }
  }
}
//------------------------------------------------------------------------
//--- String parser ------------------------------------------------------
//------------------------------------------------------------------------
std::vector<std::string> FactorizedJetCorrectionUncertaintyCalculator::parseLevels(const std::string& ss) const
{
  std::vector<std::string> result;
  unsigned int pos(0),j,newPos;
  int i;
  std::string tmp;
  //---- The ss string must be of the form: "LX:LY:...:LZ"
  while (pos<ss.length()) {
    tmp = "";
    i = ss.find(":" , pos);
    if (i<0 && pos==0) {
      result.push_back(ss);
      pos = ss.length();
    }
    else if (i<0 && pos>0) {
      for(j=pos;j<ss.length();j++)
        tmp+=ss[j];
      result.push_back(tmp);
      pos = ss.length();
    }
    else {
      newPos = i;
      for(j=pos;j<newPos;j++)
        tmp+=ss[j];
      result.push_back(tmp);
      pos = newPos+1;
    }
  }
  return result;
}
//------------------------------------------------------------------------
//--- String parser ------------------------------------------------------
//------------------------------------------------------------------------
std::string FactorizedJetCorrectionUncertaintyCalculator::parseOption(const std::string& ss, const std::string& type) const
{
  std::string result;
  int pos1(-1),pos2(-1);
  //---- The ss string must be of the form: "type1:option1&type2:option2&..."
  pos1 = ss.find(type+":");
  if (pos1<0)
    result = "";
  else {
    pos2 = ss.find("&",pos1+type.length()+1);
    if (pos2<0)
      result = ss.substr(pos1+type.length()+1,ss.length()-pos1-type.length()-1);
    else
      result = ss.substr(pos1+type.length()+1,pos2-pos1-type.length()-1);
  }
  return result;
}
//------------------------------------------------------------------------
//--- String manipulator -------------------------------------------------
//------------------------------------------------------------------------
std::string FactorizedJetCorrectionUncertaintyCalculator::removeSpaces(const std::string& ss) const
{
  std::string result("");
  std::string aChar;
  for(unsigned int i=0;i<ss.length();i++) {
    aChar = ss.substr(i,1);
    if (aChar != " ")
      result+=aChar;
  }
  return result;
}
//------------------------------------------------------------------------
//--- Returns the correction ---------------------------------------------
//------------------------------------------------------------------------
float FactorizedJetCorrectionUncertaintyCalculator::getUncertainty(FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
  std::vector<float> && vv = getSubUncertainties(iValues);
  return vv.back();
}
//------------------------------------------------------------------------
//--- Returns the vector of subcorrections, up to a given level ----------
//------------------------------------------------------------------------
std::vector<float> FactorizedJetCorrectionUncertaintyCalculator::getSubUncertainties( FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
  float scale,factor;
  std::vector<float> factors;
  std::vector<float> vx,vy;
  factor = 1;
  for(unsigned int i=0;i<mLevels.size();i++) {
    vx = fillVector(mBinTypes[i],iValues);
    vy = fillVector(mParTypes[i],iValues);
    //if (mLevels[i]==kL2 || mLevels[i]==kL6)
      //mCorrectors[i]->setInterpolation(true);
    scale = mCorrectors[i]->uncertainty(vx,vy[0],true);
    iValues.mJetE  *= scale;
    iValues.mJetPt *= scale;
    factor *= scale;
    factors.push_back(factor);
  }
  iValues.reset();
  return factors;
}
//------------------------------------------------------------------------
//--- Reads the parameter names and fills a vector of floats -------------
//------------------------------------------------------------------------
std::vector<float> FactorizedJetCorrectionUncertaintyCalculator::fillVector(const std::vector<VarTypes>& fVarTypes,
								const FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
//  std::vector<VarTypes> fVarTypes = _fVarTypes;
  std::vector<float> result;
  for(unsigned i=0;i<fVarTypes.size();i++) {
    if (fVarTypes[i] == kJetEta) {
      if (!iValues.mIsJetEtaset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet eta is not set");
      result.push_back(iValues.mJetEta);
    }
    else if (fVarTypes[i] == kNPV) {
      if (!iValues.mIsNPVset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","number of primary vertices is not set");
      result.push_back(iValues.mNPV);
    }
    else if (fVarTypes[i] == kJetPt) {
      if (!iValues.mIsJetPtset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet pt is not set");
      result.push_back(iValues.mJetPt);
    }
    else if (fVarTypes[i] == kJetPhi) {
      if (!iValues.mIsJetPhiset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet phi is not set");
      result.push_back(iValues.mJetPhi);
    }
    else if (fVarTypes[i] == kJetE) {
      if (!iValues.mIsJetEset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet E is not set");
      result.push_back(iValues.mJetE);
    }
    else if (fVarTypes[i] == kJetEMF) {
      if (!iValues.mIsJetEMFset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet EMF is not set");
      result.push_back(iValues.mJetEMF);
    }
    else if (fVarTypes[i] == kJetA) {
      if (!iValues.mIsJetAset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","jet area is not set");
      result.push_back(iValues.mJetA);
    }
    else if (fVarTypes[i] == kRho) {
      if (!iValues.mIsRhoset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","fastjet density Rho is not set");
      result.push_back(iValues.mRho);
    }
    else if (fVarTypes[i] == kJPTrawE) {
      if (!iValues.mIsJPTrawP4set)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","raw CaloJet P4 for JPT is not set");
      result.push_back(iValues.mJPTrawE);
    }
    else if (fVarTypes[i] == kJPTrawEt) {
      if (!iValues.mIsJPTrawP4set)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","raw CaloJet P4 for JPT is not set");
      result.push_back(iValues.mJPTrawEt);
    }
    else if (fVarTypes[i] == kJPTrawPt) {
      if (!iValues.mIsJPTrawP4set)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","raw CaloJet P4 for JPT is not set");
      result.push_back(iValues.mJPTrawPt);
    }
    else if (fVarTypes[i] == kJPTrawEta) {
      if (!iValues.mIsJPTrawP4set)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","raw CaloJet P4 for JPT is not set");
      result.push_back(iValues.mJPTrawEta);
    }
    else if (fVarTypes[i] == kJPTrawOff) {
      if (!iValues.mIsJPTrawOFFset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","Offset correction for JPT is not set");
      result.push_back(iValues.mJPTrawOff);
    }
    else if (fVarTypes[i] == kRelLepPt) {
      if (!iValues.mIsJetPtset||!iValues.mIsAddLepToJetset||!iValues.mIsLepPxset||!iValues.mIsLepPyset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","can't calculate rel lepton pt");
      result.push_back(getRelLepPt(iValues));
    }
    else if (fVarTypes[i] == kPtRel) {
      if (!iValues.mIsJetPtset||!iValues.mIsJetEtaset||!iValues.mIsJetPhiset||!iValues.mIsJetEset||!iValues.mIsAddLepToJetset||!iValues.mIsLepPxset||!iValues.mIsLepPyset||!iValues.mIsLepPzset)
        handleError("FactorizedJetCorrectionUncertaintyCalculator","can't calculate ptrel");
      result.push_back(getPtRel(iValues));
    }
    else {
      std::stringstream sserr;
      sserr<<"unknown parameter "<<fVarTypes[i];
      handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
    }
  }
  return result;
}
//------------------------------------------------------------------------
//--- Calculate the lepPt (needed for the SLB) ---------------------------
//------------------------------------------------------------------------
float FactorizedJetCorrectionUncertaintyCalculator::getLepPt(const FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
  return std::sqrt(iValues.mLepPx*iValues.mLepPx + iValues.mLepPy*iValues.mLepPy);
}
//------------------------------------------------------------------------
//--- Calculate the relLepPt (needed for the SLB) ---------------------------
//------------------------------------------------------------------------
float FactorizedJetCorrectionUncertaintyCalculator::getRelLepPt(const FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
  float lepPt = getLepPt(iValues);
  return (iValues.mAddLepToJet) ? lepPt/(iValues.mJetPt + lepPt) : lepPt/iValues.mJetPt;
}
//------------------------------------------------------------------------
//--- Calculate the PtRel (needed for the SLB) ---------------------------
//------------------------------------------------------------------------
float FactorizedJetCorrectionUncertaintyCalculator::getPtRel(const FactorizedJetCorrectionUncertaintyCalculator::VariableValues& iValues) const
{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float> >
    PtEtaPhiELorentzVector;
  typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<float> >
    XYZVector;
  PtEtaPhiELorentzVector jet;
  XYZVector lep;
  jet.SetPt(iValues.mJetPt);
  jet.SetEta(iValues.mJetEta);
  jet.SetPhi(iValues.mJetPhi);
  jet.SetE(iValues.mJetE);
  lep.SetXYZ(iValues.mLepPx,iValues.mLepPy,iValues.mLepPz);
  float lj_x = (iValues.mAddLepToJet) ? lep.X()+jet.Px() : jet.Px();
  float lj_y = (iValues.mAddLepToJet) ? lep.Y()+jet.Py() : jet.Py();
  float lj_z = (iValues.mAddLepToJet) ? lep.Z()+jet.Pz() : jet.Pz();
  // absolute values squared
  float lj2  = lj_x*lj_x+lj_y*lj_y+lj_z*lj_z;
  if (lj2<=0) {
    std::stringstream sserr;
    sserr<<"lepton+jet momentum sq is not positive: "<<lj2;
    handleError("FactorizedJetCorrectionUncertaintyCalculator",sserr.str());
  }
  float lep2 = lep.X()*lep.X()+lep.Y()*lep.Y()+lep.Z()*lep.Z();
  // projection vec(mu) to lepjet axis
  float lepXlj = lep.X()*lj_x+lep.Y()*lj_y+lep.Z()*lj_z;
  // absolute value squared and normalized
  float pLrel2 = lepXlj*lepXlj/lj2;
  // lep2 = pTrel2 + pLrel2
  float pTrel2 = lep2-pLrel2;
  return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
}


//------------------------------------------------------------------------
//--- Default FactorizedJetCorrectionUncertaintyCalculator::VariableValues constructor -------------------------
//------------------------------------------------------------------------
FactorizedJetCorrectionUncertaintyCalculator::VariableValues::VariableValues()
{
  mJetEta  = -9999;
  mJetPt   = -9999;
  mJetPhi  = -9999;
  mJetE    = -9999;
  mJetEMF  = -9999;
  mJetA    = -9999;
  mRho     = -9999;
  mLepPx   = -9999;
  mLepPy   = -9999;
  mLepPz   = -9999;
  mNPV     = -9999;
  mJPTrawE = -9999;
  mJPTrawEt = -9999;
  mJPTrawPt = -9999;
  mJPTrawEta = -9999;
  mJPTrawOff = -9999;
  mAddLepToJet      = false;
  mIsNPVset         = false;
  mIsJetEset        = false;
  mIsJetPtset       = false;
  mIsJetPhiset      = false;
  mIsJetEtaset      = false;
  mIsJetEMFset      = false;
  mIsJetAset        = false;
  mIsRhoset         = false;
  mIsJPTrawP4set    = false;
  mIsJPTrawOFFset   = false;
  mIsLepPxset       = false;
  mIsLepPyset       = false;
  mIsLepPzset       = false;
  mIsAddLepToJetset = false;
}

//------------------------------------------------------------------------
//--- Setters ------------------------------------------------------------
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setNPV(int fNPV)
{
  mNPV = fNPV;
  mIsNPVset = true;
}
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetEta(float fEta)
{
  mJetEta = fEta;
  mIsJetEtaset = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetPt(float fPt)
{
  mJetPt = fPt;
  mIsJetPtset  = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetPhi(float fPhi)
{
  mJetPhi = fPhi;
  mIsJetPhiset  = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetE(float fE)
{
  mJetE = fE;
  mIsJetEset   = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetEMF(float fEMF)
{
  mJetEMF = fEMF;
  mIsJetEMFset = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJetA(float fA)
{
  mJetA = fA;
  mIsJetAset = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setRho(float fRho)
{
  mRho = fRho;
  mIsRhoset = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJPTrawP4(const TLorentzVector& fJPTrawP4)
{
  mJPTrawE   = fJPTrawP4.Energy();
  mJPTrawEt  = fJPTrawP4.Et();
  mJPTrawPt  = fJPTrawP4.Pt();
  mJPTrawEta = fJPTrawP4.Eta();
  mIsJPTrawP4set = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setJPTrawOff(float fJPTrawOff)
{
  mJPTrawOff = fJPTrawOff;
  mIsJPTrawOFFset = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setLepPx(float fPx)
{
  mLepPx = fPx;
  mIsLepPxset  = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setLepPy(float fPy)
{
  mLepPy = fPy;
  mIsLepPyset  = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setLepPz(float fPz)
{
  mLepPz = fPz;
  mIsLepPzset  = true;
}
//------------------------------------------------------------------------
void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::setAddLepToJet(bool fAddLepToJet)
{
  mAddLepToJet = fAddLepToJet;
  mIsAddLepToJetset = true;
}

void FactorizedJetCorrectionUncertaintyCalculator::VariableValues::reset()
{
  mIsNPVset       = false;
  mIsJetEset      = false;
  mIsJetPtset     = false;
  mIsJetPhiset    = false;
  mIsJetEtaset    = false;
  mIsJetEMFset    = false;
  mIsJetAset      = false;
  mIsRhoset       = false;
  mIsJPTrawP4set  = false;
  mIsJPTrawOFFset = false;
  mIsLepPxset     = false;
  mIsLepPyset     = false;
  mIsLepPzset     = false;
  mAddLepToJet    = false;
}
