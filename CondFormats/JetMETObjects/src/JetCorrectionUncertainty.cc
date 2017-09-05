#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/SimpleJetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Math/PtEtaPhiE4D.h"
#include "Math/Vector3D.h"
#include "Math/LorentzVector.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
JetCorrectionUncertainty::JetCorrectionUncertainty ()
{
  mJetEta = -9999;
  mJetPt  = -9999;
  mJetPhi = -9999;
  mJetE   = -9999;
  mJetEMF = -9999;
  mLepPx  = -9999;
  mLepPy  = -9999;
  mLepPz  = -9999;
  mIsJetEset   = false;
  mIsJetPtset  = false;
  mIsJetPhiset = false;
  mIsJetEtaset = false;
  mIsJetEMFset = false;
  mIsLepPxset  = false;
  mIsLepPyset  = false;
  mIsLepPzset  = false;
  mAddLepToJet = false;
  mUncertainty = new SimpleJetCorrectionUncertainty();
}
/////////////////////////////////////////////////////////////////////////
JetCorrectionUncertainty::JetCorrectionUncertainty(const std::string& fDataFile, const std::string& fSection)
{
  mJetEta = -9999;
  mJetPt  = -9999;
  mJetPhi = -9999;
  mJetE   = -9999;
  mJetEMF = -9999;
  mLepPx  = -9999;
  mLepPy  = -9999;
  mLepPz  = -9999;
  mIsJetEset   = false;
  mIsJetPtset  = false;
  mIsJetPhiset = false;
  mIsJetEtaset = false;
  mIsJetEMFset = false;
  mIsLepPxset  = false;
  mIsLepPyset  = false;
  mIsLepPzset  = false;
  mAddLepToJet = false;
  mUncertainty = new SimpleJetCorrectionUncertainty(fDataFile,fSection);
}
/////////////////////////////////////////////////////////////////////////
JetCorrectionUncertainty::JetCorrectionUncertainty(const JetCorrectorParameters& fParameters)
{
  mJetEta = -9999;
  mJetPt  = -9999;
  mJetPhi = -9999;
  mJetE   = -9999;
  mJetEMF = -9999;
  mLepPx  = -9999;
  mLepPy  = -9999;
  mLepPz  = -9999;
  mIsJetEset   = false;
  mIsJetPtset  = false;
  mIsJetPhiset = false;
  mIsJetEtaset = false;
  mIsJetEMFset = false;
  mIsLepPxset  = false;
  mIsLepPyset  = false;
  mIsLepPzset  = false;
  mAddLepToJet = false;
  mUncertainty = new SimpleJetCorrectionUncertainty(fParameters);
}
/////////////////////////////////////////////////////////////////////////
JetCorrectionUncertainty::~JetCorrectionUncertainty ()
{
  delete mUncertainty;
}
/////////////////////////////////////////////////////////////////////////
JetCorrectionUncertainty::JetCorrectionUncertainty(const JetCorrectionUncertainty& rhs)
{
  mJetE = rhs.mJetE;
  mJetEta = rhs.mJetEta;
  mJetPt = rhs.mJetPt;
  mJetPhi = rhs.mJetPhi;
  mJetEMF = rhs.mJetEMF;
  mLepPx = rhs.mLepPx;
  mLepPy = rhs.mLepPy;
  mLepPz = rhs.mLepPz;
  mAddLepToJet = rhs.mAddLepToJet;
  mIsJetEset = rhs.mIsJetEset;
  mIsJetPtset = rhs.mIsJetPtset;
  mIsJetPhiset = rhs.mIsJetPhiset;
  mIsJetEtaset = rhs.mIsJetEtaset;
  mIsJetEMFset = rhs.mIsJetEMFset;
  mIsLepPxset = rhs.mIsLepPxset;
  mIsLepPyset = rhs.mIsLepPyset;
  mIsLepPzset = rhs.mIsLepPzset;
  mUncertainty = new SimpleJetCorrectionUncertainty(*rhs.mUncertainty);
}
/////////////////////////////////////////////////////////////////////////
void JetCorrectionUncertainty::setParameters(const std::string& fDataFile)
{
  //---- delete the mParameters pointer before setting the new address ---
  delete mUncertainty;
  mUncertainty = new SimpleJetCorrectionUncertainty(fDataFile);
}
/////////////////////////////////////////////////////////////////////////
float JetCorrectionUncertainty::getUncertainty(bool fDirection)
{
  float result;
  std::vector<float> vx,vy;
  vx = fillVector(mUncertainty->parameters().definitions().binVar());
  vy = fillVector(mUncertainty->parameters().definitions().parVar());
  result = mUncertainty->uncertainty(vx,vy[0],fDirection);
  mIsJetEset   = false;
  mIsJetPtset  = false;
  mIsJetPhiset = false;
  mIsJetEtaset = false;
  mIsJetEMFset = false;
  mIsLepPxset  = false;
  mIsLepPyset  = false;
  mIsLepPzset  = false;
  return result;
}
//------------------------------------------------------------------------
//--- Reads the parameter names and fills a vector of floats -------------
//------------------------------------------------------------------------
std::vector<float> JetCorrectionUncertainty::fillVector(const std::vector<std::string>& fNames)
{
  std::vector<float> result;
  for(unsigned i=0;i<fNames.size();i++)
    {
      if (fNames[i] == "JetEta")
        {
          if (!mIsJetEtaset) {
	    edm::LogError("JetCorrectionUncertainty::")<<" jet eta is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mJetEta);
	  }
        }
      else if (fNames[i] == "JetPt")
        {
          if (!mIsJetPtset){
	    edm::LogError("JetCorrectionUncertainty::")<<" jet pt is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mJetPt);
	  }
        }
      else if (fNames[i] == "JetPhi")
        {
          if (!mIsJetPhiset) {
	    edm::LogError("JetCorrectionUncertainty::")<<" jet phi is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mJetPt);
	  }
        }
      else if (fNames[i] == "JetE")
        {
          if (!mIsJetEset) {
	    edm::LogError("JetCorrectionUncertainty::")<<" jet energy is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mJetE);
	  }
        }
      else if (fNames[i] == "JetEMF")
        {
          if (!mIsJetEMFset) {
	    edm::LogError("JetCorrectionUncertainty::")<<" jet emf is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mJetEMF);
	  }
        }
      else if (fNames[i] == "LepPx")
        {
          if (!mIsLepPxset){
	    edm::LogError("JetCorrectionUncertainty::")<<" lepton px is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mLepPx);
	  }
        }
      else if (fNames[i] == "LepPy")
        {
          if (!mIsLepPyset){
	    edm::LogError("JetCorrectionUncertainty::")<<" lepton py is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mLepPy);
	  }
        }
      else if (fNames[i] == "LepPz")
        {
          if (!mIsLepPzset){
	    edm::LogError("JetCorrectionUncertainty::")<<" lepton pz is not set";
	    result.push_back(-999.0);
	  } else {
	    result.push_back(mLepPz);
	  }
        }

      else {
	edm::LogError("JetCorrectionUncertainty::")<<" unknown parameter "<<fNames[i];
	result.push_back(-999.0);
      }
    }
  return result;
}
//------------------------------------------------------------------------
//--- Calculate the PtRel (needed for the SLB) ---------------------------
//------------------------------------------------------------------------
float JetCorrectionUncertainty::getPtRel()
{
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float> > PtEtaPhiELorentzVector;
  typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<float> > XYZVector;
  PtEtaPhiELorentzVector jet;
  XYZVector lep;
  jet.SetPt(mJetPt);
  jet.SetEta(mJetEta);
  jet.SetPhi(mJetPhi);
  jet.SetE(mJetE);
  lep.SetXYZ(mLepPx,mLepPy,mLepPz);
  float lj_x = (mAddLepToJet) ? lep.X()+jet.Px() : jet.Px();
  float lj_y = (mAddLepToJet) ? lep.Y()+jet.Py() : jet.Py();
  float lj_z = (mAddLepToJet) ? lep.Z()+jet.Pz() : jet.Pz();
  // absolute values squared
  float lj2  = lj_x*lj_x+lj_y*lj_y+lj_z*lj_z;
  float pTrel2 = -999.0;
  if (lj2 > 0) {
    float lep2 = lep.X()*lep.X()+lep.Y()*lep.Y()+lep.Z()*lep.Z();
    // projection vec(mu) to lepjet axis
    float lepXlj = lep.X()*lj_x+lep.Y()*lj_y+lep.Z()*lj_z;
    // absolute value squared and normalized
    float pLrel2 = lepXlj*lepXlj/lj2;
    // lep2 = pTrel2 + pLrel2
    pTrel2 = lep2-pLrel2;
  } else
    edm::LogError("JetCorrectionUncertainty")<<" not positive lepton-jet momentum: "<<lj2;
  return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
}
//------------------------------------------------------------------------
//--- Setters ------------------------------------------------------------
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setJetEta(float fEta)
{
  mJetEta = fEta;
  mIsJetEtaset = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setJetPt(float fPt)
{
  mJetPt = fPt;
  mIsJetPtset  = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setJetPhi(float fPhi)
{
  mJetPhi = fPhi;
  mIsJetPhiset  = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setJetE(float fE)
{
  mJetE = fE;
  mIsJetEset   = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setJetEMF(float fEMF)
{
  mJetEMF = fEMF;
  mIsJetEMFset = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setLepPx(float fPx)
{
  mLepPx = fPx;
  mIsLepPxset  = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setLepPy(float fPy)
{
  mLepPy = fPy;
  mIsLepPyset  = true;
}
//------------------------------------------------------------------------
void JetCorrectionUncertainty::setLepPz(float fPz)
{
  mLepPz = fPz;
  mIsLepPzset  = true;
}
//------------------------------------------------------------------------
namespace {
const std::vector<std::string> labels_ = {
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
//------------------------------------------------------------------------
JetCorrectionUncertaintyCollection::JetCorrectionUncertaintyCollection( std::string inputTxtFile, std::vector<label_type> chosen_sections ) {
  std::cout<<std::endl;
  uncertainties_.clear();
  for(auto cs : chosen_sections) {
    push_back(findKey(cs),JetCorrectionUncertainty(inputTxtFile,cs));
  }
  std::sort(uncertainties_.begin(),uncertainties_.end(),[ ]( const auto& lhs, const auto& rhs)
  {
    return lhs.first < rhs.first;
  });
}
//------------------------------------------------------------------------
std::string
JetCorrectionUncertaintyCollection::findLabel( key_type k ){
   return labels_[k];
}
//------------------------------------------------------------------------
void JetCorrectionUncertaintyCollection::getSections(std::string inputFile,
                                                   std::vector<std::string> & outputs )
{
  outputs.clear();
  std::ifstream input( inputFile.c_str() );
  while( !input.eof() ) {
    char buff[10000];
    input.getline(buff,10000);
    std::string in(buff);
    if ( in[0] == '[' ) {
      std::string tok = getSection(in);
      if ( tok != "" ) {
        outputs.push_back( tok );
      }
    }
  }
  std::cout << "Found these sections for file: " << std::endl;
  copy(outputs.begin(),outputs.end(), std::ostream_iterator<std::string>(std::cout, "\n") );
}
//------------------------------------------------------------------------
// Add a JetCorrectorParameter object, possibly with flavor.
void JetCorrectionUncertaintyCollection::push_back( key_type i, value_type const & j) {
  std::cout << "i=" << i << "  ";
  uncertainties_.emplace_back(i,j);
}
//------------------------------------------------------------------------
// Access the JetCorrectorParameter via the key k.
// key_type is hashed to deal with the three collections
JetCorrectionUncertainty const & JetCorrectionUncertaintyCollection::operator[]( key_type k ) const {
  collection_type::const_iterator ibegin = uncertainties_.begin(), iend = uncertainties_.end(), i = ibegin;
  for ( ; i != iend; ++i ) {
    if ( k == i->first ) return i->second;
  }

  throw cms::Exception("InvalidInput") << " cannot find key " << static_cast<int>(k)
                                       << " in the JEC payload, this usually means you have to change the global tag" << std::endl;
}
//------------------------------------------------------------------------
// Get a list of valid keys. These will contain hashed keys
// that are aware of all three collections.
void JetCorrectionUncertaintyCollection::validKeys(std::vector<key_type> & keys ) const {
  keys.clear();
  for ( collection_type::const_iterator ibegin = uncertainties_.begin(),
        iend = uncertainties_.end(), i = ibegin; i != iend; ++i ) {
    keys.push_back( i->first );
  }
}
//------------------------------------------------------------------------
// Find the key corresponding to each label
JetCorrectionUncertaintyCollection::key_type
JetCorrectionUncertaintyCollection::findKey( std::string const & label ) const {
  // Check the default uncertainties
  std::vector<std::string>::const_iterator found =
    find( labels_.begin(), labels_.end(), label );
  if ( found != labels_.end() ) {
    return static_cast<key_type>(found - labels_.begin());
  }

  // Didn't find default uncertainties, throw exception
  throw cms::Exception("InvalidInput") << " Cannot find label " << label << std::endl;

}
//------------------------------------------------------------------------
#include "FWCore/Utilities/interface/typelookup.h"
TYPELOOKUP_DATA_REG(JetCorrectionUncertaintyCollection);
//------------------------------------------------------------------------
