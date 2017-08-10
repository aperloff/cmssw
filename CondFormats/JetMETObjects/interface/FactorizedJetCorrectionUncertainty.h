// This is the header file "FactorizedJetCorrectionUncertainty.h". This is the interface for the 
// class FactorizedJetCorrectionUncertainty.
// Author: Alexx Perloff, Emily MacDonald
// Email:  alexx.stephen.perloff@cern.ch, emacdonald16@tamu.edu

#ifndef FACTORIZED_JET_CORRECTION_UNCERTAINTY_H
#define FACTORIZED_JET_CORRECTION_UNCERTAINTY_H

#include "CondFormats/Serialization/interface/Serializable.h"

#include <vector>
#include <string>
#include "TLorentzVector.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertaintyCalculator.h"

class SimpleJetCorrectionUncertainty;
class JetCorrectorParameters;
class JetCorrectionUncertaintyCollection;

class FactorizedJetCorrectionUncertainty
{
  public:
    enum VarTypes   {kJetPt,kJetEta,kJetPhi,kJetE,kJetEMF,kRelLepPt,kPtRel,kNPV,kJetA,kRho,kJPTrawE,kJPTrawEt,kJPTrawPt,kJPTrawEta,kJPTrawOff};
    enum LevelTypes {
     kAbsoluteStat,
     kAbsoluteScale,
     kAbsoluteFlavMap,
     kAbsoluteMPFBias,
     kFragmentation,
     kSinglePionECAL,
     kSinglePionHCAL,
     kFlavorQCD,
     kTimePtEta,
     kRelativeJEREC1,
     kRelativeJEREC2,
     kRelativeJERHF,
     kRelativePtBB,
     kRelativePtEC1,
     kRelativePtEC2,
     kRelativePtHF,
     kRelativeBal,
     kRelativeFSR,
     kRelativeStatFSR,
     kRelativeStatEC,
     kRelativeStatHF,
     kPileUpDataMC,
     kPileUpPtRef,
     kPileUpPtBB,
     kPileUpPtEC1,
     kPileUpPtEC2,
     kPileUpPtHF,
     kPileUpMuZero,
     kPileUpEnvelope,
     kSubTotalPileUp,
     kSubTotalRelative,
     kSubTotalPt,
     kSubTotalScale,
     kSubTotalAbsolute,
     kSubTotalMC,
     kTotal,
     kTotalNoFlavor,
     kTotalNoTime,
     kTotalNoFlavorNoTime,
     kFlavorZJet,
     kFlavorPhotonJet,
     kFlavorPureGluon,
     kFlavorPureQuark,
     kFlavorPureCharm,
     kFlavorPureBottom,
     kTimeRunBCD,
     kTimeRunEF,
     kTimeRunG,
     kTimeRunH,
     kCorrelationGroupMPFInSitu,
     kCorrelationGroupIntercalibration,
     kCorrelationGroupbJES,
     kCorrelationGroupFlavor,
     kCorrelationGroupUncorrelated
    };
    FactorizedJetCorrectionUncertainty();
    FactorizedJetCorrectionUncertainty(const std::string& fLevels, const std::string& fTags);
    FactorizedJetCorrectionUncertainty(const JetCorrectionUncertaintyCollection& fParameters);

    void setNPV		(int   fNPV);
    void setJetEta      (float fEta);
    void setJetPt       (float fPt); 
    void setJetE        (float fE);
    void setJetPhi      (float fE);
    void setJetEMF      (float fEMF); 
    void setJetA        (float fA);
    void setRho         (float fRho);
    void setJPTrawP4    (const TLorentzVector& fJPTrawP4);
    void setJPTrawOff   (float fJPTrawOff);
    void setLepPx       (float fLepPx);
    void setLepPy       (float fLepPy);
    void setLepPz       (float fLepPz);
    void setAddLepToJet (bool fAddLepToJet);

    float getUncertainty();
    std::vector<float> getSubUncertainties();
    
       
  private:
  //---- Member Functions ----  
    FactorizedJetCorrectionUncertainty(const FactorizedJetCorrectionUncertainty&);
    FactorizedJetCorrectionUncertainty& operator= (const FactorizedJetCorrectionUncertainty&);
    //---- Member Data ---------
    FactorizedJetCorrectionUncertaintyCalculator::VariableValues mValues;
    FactorizedJetCorrectionUncertaintyCalculator mCalc;
};
#endif
