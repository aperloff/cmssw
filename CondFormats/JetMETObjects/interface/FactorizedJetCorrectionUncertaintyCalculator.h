// This is the header file "FactorizedJetCorrectionUncertaintyCalculator.h". This is the interface for the
// class FactorizedJetCorrectionUncertaintyCalculator.
// Author: Alexx Perloff, Emily MacDonald
// Email:  alexx.stephen.perloff@cern.ch, emacdonald16@tamu.edu

#ifndef CondFormats_JetMETObjects_FactorizedJetCorrectionUncertaintyCalculator_h
#define CondFormats_JetMETObjects_FactorizedJetCorrectionUncertaintyCalculator_h


#include <vector>
#include <string>
#include "TLorentzVector.h"

class SimpleJetCorrectionUncertainty;
class JetCorrectorParameters;
class JetCorrectionUncertaintyCollection;

class FactorizedJetCorrectionUncertaintyCalculator
{
  public:

    class VariableValues
    {
    public:
      friend class FactorizedJetCorrectionUncertainty;
      friend class FactorizedJetCorrectionUncertaintyCalculator;
      VariableValues();
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

      void reset();
    private:
      //---- Member Data ---------
      int   mNPV;
      float mJetE;
      float mJetEta;
      float mJetPt;
      float mJetPhi;
      float mJetEMF;
      float mJetA;
      float mRho;
      float mJPTrawE;
      float mJPTrawEt;
      float mJPTrawPt;
      float mJPTrawEta;
      float mJPTrawOff;
      float mLepPx;
      float mLepPy;
      float mLepPz;
      bool  mAddLepToJet;
      bool  mIsNPVset;
      bool  mIsJetEset;
      bool  mIsJetPtset;
      bool  mIsJetPhiset;
      bool  mIsJetEtaset;
      bool  mIsJetEMFset;
      bool  mIsJetAset;
      bool  mIsRhoset;
      bool  mIsJPTrawP4set;
      bool  mIsJPTrawOFFset;
      bool  mIsLepPxset;
      bool  mIsLepPyset;
      bool  mIsLepPzset;
      bool  mIsAddLepToJetset;
    };

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
    FactorizedJetCorrectionUncertaintyCalculator() {}
    FactorizedJetCorrectionUncertaintyCalculator(const std::string& fLevels, const std::string& fTags);
    FactorizedJetCorrectionUncertaintyCalculator(const JetCorrectionUncertaintyCollection& fParameters);
    ~FactorizedJetCorrectionUncertaintyCalculator() {}
    float getUncertainty(VariableValues&) const;
    //was std::vector<float>
    float getSubUncertainties(VariableValues&) const;


  private:
  //---- Member Functions ----
    FactorizedJetCorrectionUncertaintyCalculator(const FactorizedJetCorrectionUncertaintyCalculator&);
    FactorizedJetCorrectionUncertaintyCalculator& operator= (const FactorizedJetCorrectionUncertaintyCalculator&);
    float getLepPt(const VariableValues&)    const;
    float getRelLepPt(const VariableValues&) const;
    float getPtRel(const VariableValues&)    const;
    std::string parseOption(const std::string& ss, const std::string& type) const;
    std::string removeSpaces(const std::string& ss) const;
    std::vector<std::string> parseLevels(const std::string& ss) const;
    void initCorrectors(const std::string& fLevels, const std::string& fFiles);
    void checkConsistency(const std::vector<std::string>& fLevels, const std::vector<std::string>& fTags);
    std::vector<float> fillVector(const std::vector<VarTypes>& fVarTypes, const VariableValues&) const;
    std::vector<VarTypes> mapping(const std::vector<std::string>& fNames) const;
    LevelTypes findKey( std::string const & level ) const;
    //---- Member Data ---------
    std::vector<LevelTypes> mLevels;
    std::vector<std::vector<VarTypes> > mParTypes,mBinTypes;
    std::vector<SimpleJetCorrectionUncertainty const*> mCorrectors;
};
#endif
