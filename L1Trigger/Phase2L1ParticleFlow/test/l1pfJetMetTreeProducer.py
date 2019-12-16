import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process("IN2", eras.phase2_trigger)
process.load('Configuration.StandardSequences.Services_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load('Configuration.Geometry.GeometryExtended2023D35Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D35_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_upgrade2023_realistic_v2', '')

from inputs104X_TTbar_PU200_cff import readFiles
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring('/store/relval/CMSSW_9_3_7/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/93X_upgrade2023_realistic_v5_2023D17noPU-v2/10000/7EC7DD7F-782C-E811-B469-0CC47A4D76A0.root'),
    #fileNames = cms.untracked.vstring('file:inputs104X_TTbar_PU200_job1.root',
    #                                  'file:inputs104X_TTbar_PU200_job2.root',
    #                                  'file:inputs104X_TTbar_PU200_job3.root',
    #                                  'file:inputs104X_TTbar_PU200_job4.root',
    #                                  'file:inputs104X_TTbar_PU200_job5.root',
                                      #'root://131.225.204.161:1094//eos/uscms/store/user/lpctrig/benwu/Phase2L1/Gio_010319/TTbar_PU200/inputs104X_TTbar_PU200_job1.root'
                                      #'root://131.225.204.161:1094//eos/uscms/store/user/lpctrig/benwu/Phase2L1/Gio_010319/TTbar_PU200/inputs104X_TTbar_PU200_job2.root'
                                      #'root://131.225.204.161:1094//eos/uscms/store/user/lpctrig/benwu/Phase2L1/Gio_010319/TTbar_PU200/inputs104X_TTbar_PU200_job3.root'
                                      #'root://131.225.204.161:1094//eos/uscms/store/user/lpctrig/benwu/Phase2L1/Gio_010319/TTbar_PU200/inputs104X_TTbar_PU200_job4.root'
                                      #'root://131.225.204.161:1094//eos/uscms/store/user/lpctrig/benwu/Phase2L1/Gio_010319/TTbar_PU200/inputs104X_TTbar_PU200_job5.root'
    #                                  ),
    fileNames = readFiles,
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    inputCommands = cms.untracked.vstring("keep *", 
        "drop l1tEMTFHit2016Extras_simEmtfDigis_CSC_HLT",
        "drop l1tEMTFHit2016Extras_simEmtfDigis_RPC_HLT",
        "drop l1tEMTFHit2016s_simEmtfDigis__HLT",
        "drop l1tEMTFTrack2016Extras_simEmtfDigis__HLT",
        "drop l1tEMTFTrack2016s_simEmtfDigis__HLT")

)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100))
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.MessageLogger.categories.extend(['L1TPFProducer','PatternFile','COEFile','APxPatternFile'])

process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff') # needed to read HCal TPs
process.load('RecoMET.Configuration.GenMETParticles_cff')
process.load('RecoMET.METProducers.genMetTrue_cfi')
process.load('L1Trigger.Phase2L1ParticleFlow.l1ParticleFlow_cff')

# define regions
def goRegional():
    process.l1pfProducerBarrel.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-1.5, -0.75, 0, 0.75, 1.5),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        )
    )
    process.l1pfProducerHGCalNoTK.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-3, -2.5),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        ),
        cms.PSet(
            etaBoundaries = cms.vdouble(2.5, 3),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        )
    )
    process.l1pfProducerHGCal.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-2.5, -1.5),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        ),
        cms.PSet(
            etaBoundaries = cms.vdouble(1.5, 2.5),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        )
    )
    process.l1pfProducerHF.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-5, -4.5, -4, -3.5, -3),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        ),
        cms.PSet(
            etaBoundaries = cms.vdouble(3, 3.5, 4, 4.5, 5),
            etaExtra = cms.double(0.25),
            phiExtra = cms.double(0.25),
            phiSlices = cms.uint32(9)
        )
    )
    process.l1pfProducerTSA.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-3,0,3),
            phiSlices = cms.uint32(9),
            etaExtra = cms.double(0.0),
            phiExtra = cms.double(0.0)
        )
    )
goRegional()

import math,os
process.l1pfProducerTSA.genOrigin = cms.InputTag('genParticles','xyz0')
process.l1pfProducerTSA.debug = cms.untracked.int32(0)
process.l1pfProducerTSA.trackRepresentationMode = cms.untracked.string("integer") #Options: {integer, fixedPoint}
process.l1pfProducerTSA.nTracksMax = cms.untracked.uint32(95)
process.l1pfProducerTSA.nEventsCOEMax = cms.untracked.uint32(74)#64 or all at 124394
process.l1pfProducerTSA.nRowsAPxMax = cms.untracked.uint32(1024)
process.l1pfProducerTSA.nEventsAPxPerFile = cms.untracked.uint32(int(math.floor((2./3.)*process.l1pfProducerTSA.nRowsAPxMax.value()/process.l1pfProducerTSA.nTracksMax.value()))) # 2/3 for the tracks per row in the file
process.l1pfProducerTSA.nEventsAPxMax = cms.untracked.uint32(25)
secondary_file_path = os.getcwd()+'/trackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%itrackmax/' % (len(process.l1pfProducerTSA.regions[0].etaBoundaries)-1,process.l1pfProducerTSA.regions[0].phiSlices.value(),process.l1pfProducerTSA.nTracksMax.value())
process.l1pfProducerTSA.dumpFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200.dump" % (secondary_file_path,len(process.l1pfProducerTSA.regions[0].etaBoundaries)-1,process.l1pfProducerTSA.regions[0].phiSlices.value()))
process.l1pfProducerTSA.COEFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%iEvents.coe" % (secondary_file_path,len(process.l1pfProducerTSA.regions[0].etaBoundaries)-1,process.l1pfProducerTSA.regions[0].phiSlices.value(),process.l1pfProducerTSA.nEventsCOEMax.value()))
process.l1pfProducerTSA.APxFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%iEventsPerFile.txt" % (secondary_file_path,len(process.l1pfProducerTSA.regions[0].etaBoundaries)-1,process.l1pfProducerTSA.regions[0].phiSlices.value(),process.l1pfProducerTSA.nEventsAPxPerFile.value()))

process.p = cms.Path(process.l1ParticleFlow + process.l1ParticleFlow_pf_tsa)
process.out = cms.OutputModule("PoolOutputModule",
     fileName = cms.untracked.string("Phase2L1ParticleFlowOutput.root"),
     outputCommands = cms.untracked.vstring(
       "keep *",
       )
 )
process.e = cms.EndPath(process.out)
