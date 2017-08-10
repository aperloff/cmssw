#ifndef JetCorrectionUncertainty_h
#define JetCorrectionUncertainty_h

#include "CondFormats/Serialization/interface/Serializable.h"

#include <string>
#include <vector>
class SimpleJetCorrectionUncertainty;
class JetCorrectorParameters;

class JetCorrectionUncertainty 
{
  public:
    JetCorrectionUncertainty();
    JetCorrectionUncertainty(const std::string& fDataFile);
    JetCorrectionUncertainty(const JetCorrectorParameters& fParameters);
    ~JetCorrectionUncertainty();

    void setParameters  (const std::string& fDataFile);
    void setJetEta      (float fEta);
    void setJetPt       (float fPt); 
    void setJetE        (float fE);
    void setJetPhi      (float fE);
    void setJetEMF      (float fEMF); 
    void setLepPx       (float fLepPx);
    void setLepPy       (float fLepPy);
    void setLepPz       (float fLepPz);
    void setAddLepToJet (bool fAddLepToJet) {mAddLepToJet = fAddLepToJet;}
    float getUncertainty(bool fDirection);

 private:
  JetCorrectionUncertainty(const JetCorrectionUncertainty&);
  JetCorrectionUncertainty& operator= (const JetCorrectionUncertainty&);
  std::vector<float> fillVector(const std::vector<std::string>& fNames);
  float getPtRel();
  //---- Member Data ---------
  float mJetE;
  float mJetEta;
  float mJetPt;
  float mJetPhi;
  float mJetEMF; 
  float mLepPx;
  float mLepPy;
  float mLepPz;
  bool  mAddLepToJet;
  bool  mIsJetEset;
  bool  mIsJetPtset;
  bool  mIsJetPhiset;
  bool  mIsJetEtaset;
  bool  mIsJetEMFset; 
  bool  mIsLepPxset;
  bool  mIsLepPyset;
  bool  mIsLepPzset;
  SimpleJetCorrectionUncertainty* mUncertainty;
};

class JetCorrectionUncertaintyCollection {
  //---------------- JetCorrectionUncertaintyCollection class ----------------

 public:
  enum Level_t {
     AbsoluteStat                     = 0,
     AbsoluteScale                    = 1,
     AbsoluteFlavMap                  = 2,
     AbsoluteMPFBias                  = 3,
     Fragmentation                    = 4,
     SinglePionECAL                   = 5,
     SinglePionHCAL                   = 6,
     FlavorQCD                        = 7,
     TimePtEta                        = 8,
     RelativeJEREC1                   = 9,
     RelativeJEREC2                   = 10,
     RelativeJERHF                    = 11,
     RelativePtBB                     = 12,
     RelativePtEC1                    = 13,
     RelativePtEC2                    = 14,
     RelativePtHF                     = 15,
     RelativeBal                      = 16,
     RelativeFSR                      = 17,
     RelativeStatFSR                  = 18,
     RelativeStatEC                   = 19,
     RelativeStatHF                   = 20,
     PileUpDataMC                     = 21,
     PileUpPtRef                      = 22,
     PileUpPtBB                       = 23,
     PileUpPtEC1                      = 24,
     PileUpPtEC2                      = 25,
     PileUpPtHF                       = 26,
     PileUpMuZero                     = 27,
     PileUpEnvelope                   = 28,
     SubTotalPileUp                   = 29,
     SubTotalRelative                 = 30,
     SubTotalPt                       = 31,
     SubTotalScale                    = 32,
     SubTotalAbsolute                 = 33,
     SubTotalMC                       = 34,
     Total                            = 35,     
     TotalNoFlavor                    = 36,
     TotalNoTime                      = 37,
     TotalNoFlavorNoTime              = 38,
     FlavorZJet                       = 39,
     FlavorPhotonJet                  = 40,
     FlavorPureGluon                  = 41,
     FlavorPureQuark                  = 42,
     FlavorPureCharm                  = 43,
     FlavorPureBottom                 = 44,
     TimeRunBCD                       = 45,
     TimeRunEF                        = 46,
     TimeRunG                         = 47,
     TimeRunH                         = 48,
     CorrelationGroupMPFInSitu        = 49,
     CorrelationGroupIntercalibration = 50,
     CorrelationGroupbJES             = 51,
     CorrelationGroupFlavor           = 52,
     CorrelationGroupUncorrelated     = 53,
     N_LEVELS                         = 54
  };

  typedef int                            key_type;
  typedef std::string                    label_type;
  typedef JetCorrectorParameters         value_type;
  typedef std::pair<key_type,value_type> pair_type;
  typedef std::vector<pair_type>         collection_type;

  // Default constructor
  JetCorrectionUncertaintyCollection() { uncertainties_.clear(); }

  // Constructor
  JetCorrectionUncertaintyCollection( std::string inputTxtFile, std::vector<label_type> chosen_sections );

  // Add a JetCorrectorParameter object, possibly with flavor.
  void push_back( key_type i, value_type const & j);

  // Access the JetCorrectorParameter via the key k.
  // key_type is hashed to deal with the three collections
  JetCorrectorParameters const & operator[]( key_type k ) const;

  // Access the JetCorrectorParameter via a string.
  // Will find the hashed value for the label, and call via that
  // operator.
  JetCorrectorParameters const & operator[]( std::string const & label ) const {
    return operator[]( findKey(label) );
  }

  // Get a list of valid keys. These will contain hashed keys
  // that are aware of all three collections.
  void validKeys(std::vector<key_type> & keys ) const;

  // Helper method to find all of the sections in a given
  // parameters file
  static void getSections(std::string inputFile,
                          std::vector<std::string> & outputs );

  // Find the label corresponding to each key
  static std::string findLabel( key_type k );

  const collection_type& getUncertainties() const  {return uncertainties_;}

 protected:

  // Find the key corresponding to each label
  key_type findKey( std::string const & label ) const;

  collection_type uncertainties_;

 COND_SERIALIZABLE;

};

#endif

