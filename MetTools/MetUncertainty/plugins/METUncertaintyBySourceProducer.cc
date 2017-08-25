// Class:      METUncertaintyBySourceProducer
//
/**\class METUncertaintyBySourceProducer METUncertaintyBySourceProducer.cc
MetTools/METUncertaintyBySourceProducer/plugins/METUncertaintyBySourceProducer.cc

Description: [one line class summary]

Implementation: [Notes on implementation]
*/
// system include files
#include <memory>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <TMath.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// new includes
#include "CommonTools/Utils/interface/PtComparator.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Math/interface/LorentzVector.h"



//Class declaration
class METUncertaintyBySourceProducer : public edm::EDProducer {
public:
  explicit METUncertaintyBySourceProducer(const edm::ParameterSet&);
private:
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  edm::InputTag JetTag_;
  edm::InputTag slimmedMETTag_;
  edm::EDGetTokenT<edm::View<pat::Jet> > JetTok_;
  edm::EDGetTokenT<edm::View<pat::MET> > slimmedmetTok_ ;
  std::vector <std::string> sources_;
  std::string JECFile_;
  int numSources_;

  edm::Handle< edm::View<pat::Jet> > jets;
  edm::Handle< edm::View<pat::MET> > slimmedMETs;

  float slimmedmet;
  float slimmedmetJECUp;
  float slimmedmetJECDown;
  float genmet;
};
// constants, enums and typedefs

// static data member definitions

// constructors and destructor
METUncertaintyBySourceProducer::METUncertaintyBySourceProducer(const edm::ParameterSet& iConfig) {//changed from no bracket and a ":"
// JetTag_(iConfig.getParameter<edm::InputTag>("JetTag")),
// JetTok_(consumes<edm::View<pat::Jet> >(JetTag_)),
// slimmedMETTag_(iConfig.getParameter<edm::InputTag>("slimmedMETTag")),
// slimmedmetTok_(consumes<edm::View<pat::MET> >(slimmedMETTag_)),
// sources_(iConfig.getParameter<std::vector <std::string> >("sources")),
// JECFile_(iConfig.getParameter<std::string>("JECFile")),
// numSources_(iConfig.getParameter<int>("numSources"))
  JetTok_         = consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag> ("JetTag"));
  slimmedmetTok_  = consumes<edm::View<pat::MET> >(iConfig.getParameter<edm::InputTag> ("slimmedMETTag"));
  sources_ = iConfig.getParameter<std::vector <std::string> >("sources");
  JECFile_ = iConfig.getParameter<std::string>("JECFile");
  numSources_ = iConfig.getParameter<int>("numSources");

  produces<std::vector<pat::Jet> >(); //I have two LorentzVectors (slimmedmetJECUp(Down), and no idea how to "produce" them)
}

// member functions

// ------------ method called to produce the data  ------------
void
METUncertaintyBySourceProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  //I have no idea how to initialize a JetCorrectorParameters and JetCorrectionUncertainty
  //edm::ESHandle<JetCorrectionUncertaintyCollection> JetCorUncColl;//or do we want the paramters collection?? Can't imagine why...
  //iSetup.get<JetCorrectionsRecord>().get(JetType_,JetCorUncColl);//will this work??? Who knows
  //JetCorrectorParameters const & JetCorPar = (*JetCorUncColl)["Uncertainty"]; //also unknown...
  //auto jecUnc = std::make_unique<JetCorrectionUncertainty>(JetCorPar);//just going to leave these in comments in case they're useful

  iEvent.getByToken(JetTok_, jets);
  auto newJets_up  = std::make_unique<std::vector<pat::Jet>>();
  newJets_up->reserve(jets->size());
  auto newJets_down  = std::make_unique<std::vector<pat::Jet>>();
  newJets_down->reserve(jets->size());

  math::XYZTLorentzVector sumJet;
  math::XYZTLorentzVector sumJetJECUp;
  math::XYZTLorentzVector sumJetJECDown;

  for (edm::View<pat::Jet>::const_iterator itJet = jets->begin(); itJet != jets->end(); itJet++) {
    sumJet+=itJet->p4();
    // construct the Jet from the ref -> save ref to original object
    unsigned int idx = std::distance(jets->begin(),itJet);
    edm::RefToBase<pat::Jet> jetRef = jets->refAt(idx);
    edm::Ptr<pat::Jet> jetPtr = jets->ptrAt(idx);
    pat::Jet ajet_down(jetPtr);
    pat::Jet ajet_up(jetPtr);
    math::XYZTLorentzVector vjet_down = ajet_down.p4();
    math::XYZTLorentzVector vjet_up = ajet_up.p4();

    //loop over chosen sources
    for (int i=0;i<numSources_;i++){
      //not sure if this is how you define a JetCorrectorParameter/JetCorrectionUncertainty in CMSSW
      JetCorrectorParameters jcp(JECFile_,sources_[i]); //auto jcp  = std::make_unique<JetCorrectorParameters>(JECFile,sources[i]);
      JetCorrectionUncertainty jcu(jcp); //auto jcu  = std::make_unique<JetCorrectionUncertainty>(jcp);

      //Get JEC unc for this jet, both up and down
      //Need to set the pT and eta twice because resets once getUncertainty() is called
      jcu.setJetEta(itJet->eta());
      jcu.setJetPt(itJet->pt());   //jecUnc->setJetEta(itJet->eta()); //jecUnc->setJetPt(itJet->pt());
      double unc_up = jcu.getUncertainty(true);
      jcu.setJetEta(itJet->eta());
      jcu.setJetPt(itJet->pt());
      double unc_down = jcu.getUncertainty(false);//Does not work unless resetting the eta and pt

      //safety check if uncertainty is not available for a jet
      if(unc_up==-999.) unc_up = 0;
      if(unc_down==-999.) unc_down = 0;

      double jesUncScale_up = 1.0; //no variation - just store scale factor & uncertainty
      double jesUncScale_down = 1.0; //no variation - just store scale factor & uncertainty

      //downward variation
      jesUncScale_down = 1 - unc_down;
      //upward variation
      jesUncScale_up = 1 + unc_up;

      //apply variation, sum up changes (not saving the jets)
      vjet_up *= jesUncScale_up;   //this is a p4
      vjet_down *= jesUncScale_down;
      sumJetJECUp += vjet_up;
      sumJetJECDown += vjet_down; //ajet_up.setP4(vjet_up); //newJets_up->push_back(ajet_up);
    } //end sources loop
  } //end jets loop

  //using slimmedMET. Probably want genMET (??)
  iEvent.getByToken(slimmedmetTok_, slimmedMETs);
  slimmedmet=slimmedMETs->at(0).pt();
  genmet=slimmedMETs->at(0).genMET()->pt(); //unsure if correct syntax, so not using yet
  math::XYZTLorentzVector metP4 = slimmedMETs->at(0).p4();

  //how do I return these?? Should I return phi, too?
  slimmedmetJECUp   = (metP4 + (sumJet - sumJetJECUp)).Pt();
  slimmedmetJECDown = (metP4 + (sumJet-sumJetJECDown)).Pt();

} //end produce

DEFINE_FWK_MODULE(METUncertaintyBySourceProducer);
