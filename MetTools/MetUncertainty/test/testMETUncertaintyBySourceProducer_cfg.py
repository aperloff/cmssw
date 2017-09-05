import FWCore.ParameterSet.Config as cms

process = cms.Process("OWNPARTICLES") #dafuq is this


process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring([ #miniAOD
        "file:/uscms_data/d3/emacdona/WorkingArea/CMSSW_8_0_24_patch1/src/801F9D94-38F4-E611-A911-0025905B860E.root",
    ])
)

process.METUncertaintyByJECSource = cms.EDProducer('METUncertaintyBySourceProducer',
    numSources = cms.int32(2), #Make sure to change to reflect number of sources
    sources = cms.vstring(['AbsoluteStat','PileUpPtEC2']), #List any sources
    #JECFile is the text file of JEC uncertainties, separated by source
    JECFile = cms.string('/uscms_data/d3/emacdona/WorkingArea/CMSSW_9_3_0_pre1/src/CondFormats/Summer16_03Feb2017_V1_MC_UncertaintySources_AK4PFchs.txt'),
    collName = cms.string('randomSources'), #change for name of collection (total, subtotal, relative, etc.)
    slimmedMETTag = cms.InputTag("slimmedMETs"),
    JetTag  = cms.InputTag('slimmedJets'),
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('edmOutput.root')
)

process.p = cms.Path(process.METUncertaintyByJECSource)

process.e = cms.EndPath(process.out)
