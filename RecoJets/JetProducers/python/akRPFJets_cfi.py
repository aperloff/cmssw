import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *

##############################################################################
# Standard AKVR Jets
##############################################################################
akVRPFJets = cms.EDProducer(
    "FastjetJetProducer",
    PFJetParameters,
    AnomalousCellParameters,
    jetAlgorithm = cms.string("AntiKt"),
    jetPtMin = cms.double(50.0)
    rParam = cms.double(345) #rho_default=2*m_{X} (GeV)
    )

##############################################################################
# AKVR jets with various pileup subtraction schemes
##############################################################################
akVRPFJetsCHS = akVRPFJets.clone(
    src = cms.InputTag("pfNoPileUpJME")
    )

akVRPFJetsPuppi = akVRPFJets.clone(
    src = cms.InputTag("puppi")
    )

##############################################################################
# Preclustered constituents for substructure, various subtraction schemes
##############################################################################
akVRPFJetsCHSConstituents = cms.EDProducer("PFJetConstituentSelector",
                                           src = cms.InputTag("akVRPFJetsCHS"),
                                           cut = cms.string("pt > 100.0 && abs(rapidity()) < 2.4")
                                           )

akVRPFJetsPuppiConstituents = cms.EDProducer("PFJetConstituentSelector",
                                             src = cms.InputTag("akVRPFJetsPuppi"),
                                             cut = cms.string("pt > 100.0 && abs(rapidity()) < 2.4")
                                             )

##############################################################################
# Substructure algorithms
##############################################################################
akVRPFJetsCHSSoftDrop = akVRPFJets.clone(
    useSoftDrop = cms.bool(True),
    src = cms.InputTag("akVRPFJetsCHSConstituents", "constituents"),
    zcut = cms.double(0.1),
    beta = cms.double(0.0),
    R0   = cms.double(0.8),
    useExplicitGhosts = cms.bool(True),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets"),
    jetPtMin = 100.0
    )

akVRPFJetsPuppiSoftDrop = akVRPFJetsCHSSoftDrop.clone(
    src = cms.InputTag("akVRPFJetsPuppiConstituents", "constituents")
    )
