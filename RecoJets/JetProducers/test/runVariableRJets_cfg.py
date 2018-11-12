"""
Created:        --
Last Updated:   16 October   2017

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University

-----

Configuration for producing mini-AOD file with VR jet content
Borrowed from:
https://github.com/monoTopHelicityWG/B2GMonoTop/blob/master/test/run_B2GTTbarTreeMaker_MonoTopNew_Toolbox.py
"""
import FWCore.ParameterSet.Config as cms

process = cms.Process("VRAna")


#----------------------------------------------------------------------------------------
### SETUP
print " SETUP "
process.load("FWCore.MessageService.MessageLogger_cfi")
#process.load("Configuration.EventContent.EventContent_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '94X_mc2017_realistic_v10'
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)
process.options.numberOfThreads = cms.untracked.uint32(12)
process.options.numberOfStreams = cms.untracked.uint32(0)
isMC = True


#----------------------------------------------------------------------------------------
### INPUT
print " INPUT "
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#      'root://cmsxrootd.fnal.gov///store/user/rymuelle/MonoTop/Youngdo_MT_Private_Samples/mt_had_1tev_rh_miniaod.root'
#        '/store/mc/RunIISummer17MiniAOD/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/MINIAODSIM/NoPU_magnetOn_92X_upgrade2017_realistic_v10-v1/70000/DCB457CD-56A0-E711-BD81-48FD8E2824A3.root'
#        '/store/mc/RunIISummer17DRStdmix/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/AODSIM/NoPU_magnetOn_92X_upgrade2017_realistic_v10-v1/70000/8EDA0410-A49F-E711-AAC5-48FD8E282497.root'
        '/store/mc/RunIIFall17DRStdmix/QCD_Pt-15to7000_TuneCP5_Flat2017_13TeV_pythia8/AODSIM/NoPU_94X_mc2017_realistic_v10-v2/40000/FEA723D1-8ED8-E711-AC13-0025905B857A.root'
    )
)

#----------------------------------------------------------------------------------------
### CHS
process.load('CommonTools.ParticleFlow.pfNoPileUpJME_cff')
process.pfPileUpJME.checkClosestZVertex = False

#----------------------------------------------------------------------------------------
### Puppi (https://twiki.cern.ch/twiki/bin/viewauth/CMS/PUPPI)
process.load('CommonTools/PileupAlgos/Puppi_cff')
process.puppi.candName = cms.InputTag('particleFlow')
process.puppi.vertexName = cms.InputTag('offlinePrimaryVertices')
#process.puppi.useExistingWeights = cms.bool(True)
#process.puppiOnTheFly = process.puppi.clone()
#process.puppiOnTheFly.useExistingWeights = True


#----------------------------------------------------------------------------------------
#print " Variable-R Jets "

from RecoJets.JetProducers.akVRPFJets_cfi import akVRPFJets,akVRPFJetsCHS,akVRPFJetsPuppi,akVRPFJetsPuppiSoftDrop,akVRPFJetsPuppiConstituents
setattr(process,'akVRPFJets',akVRPFJets)
setattr(process,'akVRPFJetsCHS',akVRPFJetsCHS)
setattr(process,'akVRPFJetsPuppi',akVRPFJetsPuppi)
#setattr(process,'akVRPFJetsPuppiConstituents',akVRPFJetsPuppiConstituents)
#setattr(process,'akVRPFJetsPuppiSoftDrop',akVRPFJetsPuppiSoftDrop)

#process.load("RecoJets/JetProducers/akVRPFJets_cfi")
#process.akVRPFJets.variableRMin = 0.4
#process.akVRPFJets.variableRMax = 1.2
#process.akVRPFJets.variableRRho = 300

#process.akVRPFJetsPuppi.variableRMin = 0.4
#process.akVRPFJetsPuppi.variableRMax = 1.2

#process.akVRPFJetsCHS.variableRMin = 0.4
#process.akVRPFJetsCHS.variableRMax = 1.2

#process.akVRPFJetsPuppiSoftDrop.variableRMin = 0.4
#process.akVRPFJetsPuppiSoftDrop.variableRMax = 1.2

#process.akVRPFJetsCHSSoftDrop.variableRMin = 0.4
#process.akVRPFJetsCHSSoftDrop.variableRMax = 1.2

#from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
#from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
#
#addJetCollection(
#   process,
#   labelName = 'AKVRPFJETS',
#   jetSource = cms.InputTag('akVRPFJets',''),
#   algo = 'AntiKtVariableR',
#   jetCorrections = None,
#   btagDiscriminators = ['None'], # turn-off b tagging
#   getJetMCFlavour = False # jet flavor needs to be disabled for groomed fat jets
#   )

#from RecoJets.Configuration.RecoPFJets_cff import *
#setattr(process,'ak4PFJets',ak4PFJets)
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets, ak4PFJetsCHS, ak4PFJetsPuppi
setattr(process,'ak4PFJets',ak4PFJets)
setattr(process,'ak4PFJetsCHS',ak4PFJetsCHS)
setattr(process,'ak4PFJetsPuppi',ak4PFJetsPuppi)

# dump everything into a task so it can run unscheduled
process.myTask = cms.Task()
process.myTask.add(*[getattr(process,prod) for prod in process.producers_()])
process.myTask.add(*[getattr(process,filt) for filt in process.filters_()])
#process.myTack.add()

#----------------------------------------------------------------------------------------
### Out
print " OUT "
# If you want to output the newly recoconstruted jets
process.out = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('testVR.root'),
    outputCommands = cms.untracked.vstring(
#        "keep *_*_*_*"
#        "drop *",
#      "keep *PatJetsAK4PF*_*",
#      "keep *PatJetsAK8PFCHS*_*",
#      "keep *PatJetsAK8PFPuppi*_*",
#      "keep *PatJetsCA12PFPuppi*_*",
      "keep *_*akVRPFJets*_*_*",
      "keep *_*ak4PFJets*_*_*",
      )
    )
process.endpath = cms.EndPath(process.out) 
process.endpath.associate(process.myTask)

#process.TFileService = cms.Service("TFileService",
#      fileName = cms.string("testVR.root"),
#      closeFileFast = cms.untracked.bool(True)
#  )

#process.add_(cms.Service("Tracer", dumpPathsAndConsumes = cms.untracked.bool(True)))
#process.add_(cms.Service("ProductRegistryDumper"))
## THE END ##
