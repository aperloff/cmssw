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
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrectionUncertaintyCalculator.h"

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
  std::string collectionName;

  edm::Handle< edm::View<pat::Jet> > jets;
  edm::Handle< edm::View<pat::MET> > slimmedMETs;
  typedef std::vector<pat::MET> MetVector;

};
// constants, enums and typedefs

// static data member definitions

// constructors and destructor
METUncertaintyBySourceProducer::METUncertaintyBySourceProducer(const edm::ParameterSet& iConfig) :
JetTok_(consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("JetTag"))),
slimmedmetTok_(consumes<edm::View<pat::MET> >(iConfig.getParameter<edm::InputTag>("slimmedMETTag"))),
sources_(iConfig.getParameter<std::vector <std::string> >("sources")),
JECFile_(iConfig.getParameter<std::string>("JECFile")),
collectionName(iConfig.getParameter<std::string>("collName"))
{
  produces<MetVector>( collectionName+"METUp" ).setBranchAlias( collectionName+"METUp" );
  produces<MetVector>( collectionName+"METDown" ).setBranchAlias( collectionName+"METDown" );
}

// member functions

// ------------ method called to produce the data  ------------
void
METUncertaintyBySourceProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  iEvent.getByToken(JetTok_, jets);
  iEvent.getByToken(slimmedmetTok_, slimmedMETs);

  pat::MET(METJECUp);
  pat::MET(METJECDown);
  std::unique_ptr<MetVector> METJECUpvec( new MetVector );
  std::unique_ptr<MetVector> METJECDownvec( new MetVector );
  METJECUpvec->reserve(1);
  METJECDownvec->reserve(1);

  auto newJets_up  = std::make_unique<std::vector<pat::Jet> >();
  newJets_up->reserve(jets->size());
  auto newJets_down  = std::make_unique<std::vector<pat::Jet> >();
  newJets_down->reserve(jets->size());

  math::XYZTLorentzVector diff_up(0,0,0,0); //reset sum(jet_corr-jet)
  math::XYZTLorentzVector diff_down(0,0,0,0);

  for (edm::View<pat::Jet>::const_iterator itJet = jets->begin(); itJet != jets->end(); itJet++) {
    // construct the Jet from the ref -> save ref to original object
    unsigned int idx = std::distance(jets->begin(),itJet);
    edm::RefToBase<pat::Jet> jetRef = jets->refAt(idx);
    edm::Ptr<pat::Jet> jetPtr = jets->ptrAt(idx);
    pat::Jet ajet_down(jetPtr); pat::Jet ajet_up(jetPtr);
    math::XYZTLorentzVector vjet_down = ajet_down.p4();
    math::XYZTLorentzVector vjet_up = ajet_up.p4();

    //Create a jcuc of all the sources per jet
    //loop over sources, creating a different jcu for each source, and pushing back to the jcuc (key, jcu)
    JetCorrectionUncertaintyCollection jcuc;
    for (unsigned int i=0; i<sources_.size(); i++){
      JetCorrectorParameters jcp(JECFile_, sources_[i]);
      JetCorrectionUncertainty jcu(jcp);
      jcuc.push_back(jcuc.findKey(sources_[i]),jcu);
    }

    //Create FactorizedJetCorrectionUncertainty, set eta and pt, retrieve total uncertainty
    auto fcuc = std::make_unique<FactorizedJetCorrectionUncertainty>(jcuc);
    fcuc->setJetEta(itJet->eta());
    fcuc->setJetPt(itJet->pt());
    double tot_unc = fcuc->getUncertainty();

    double jecUncScale_up = 1.0; //no variation - just store scale factor & uncertainty
    double jecUncScale_down = 1.0; //no variation - just store scale factor & uncertainty

    //downward variation
    jecUncScale_down = 1 - tot_unc;
    //upward variation
    jecUncScale_up = 1 + tot_unc;

    //apply variation
    vjet_up *= jecUncScale_up;   //this is a p4
    vjet_down *= jecUncScale_down;

    //sum differences between scaled jet and original
    diff_up += (vjet_up - ajet_up.p4()); //still a p4
    diff_down += (vjet_down - ajet_down.p4()); //still a p4

  } //end jets loop

  //using slimmedMET
  double slimmedmet=slimmedMETs->at(0).pt();
  math::XYZTLorentzVector metP4 = slimmedMETs->at(0).p4(); //slimmedMETs is handle

  //calculate "corrected" MET for both up/down variation
  METJECUp.setP4(metP4 + diff_up);
  METJECDown.setP4(metP4 + diff_down);

  METJECUpvec->push_back(METJECUp);
  METJECDownvec->push_back(METJECDown);
  std::cout<<std::endl<<"Checking MET. . ."<<std::endl<<"MET: "<<slimmedmet<<", corrUp: "<<METJECUpvec->at(0).pt()<<", corrDown: "<<METJECDownvec->at(0).pt()<<std::endl;



  iEvent.put(std::move(METJECUpvec),  collectionName+"METUp" );
  iEvent.put(std::move(METJECDownvec), collectionName+"METDown" );
} //end produce

DEFINE_FWK_MODULE(METUncertaintyBySourceProducer);
