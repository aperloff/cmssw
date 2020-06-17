############################################################
# define basic process
############################################################
import FWCore.ParameterSet.Config as cms
import math,os

from Configuration.StandardSequences.Eras import eras
process = cms.Process("IN2", eras.phase2_trigger)

############################################################
# edit options here
############################################################
GEOMETRY = "D49"
#GEOMETRY = "D41"
#GEOMETRY = "D35" # <== to run on old tracker geometry, change flag "geomTkTDR" to *true* in ../interface/Constants.hh

############################################################
# import standard configurations
############################################################
if GEOMETRY == "D35":
    print "using geometry " + GEOMETRY + " (tilted)"
    process = cms.Process('REPR',eras.Phase2C4)
    process.load('Configuration.Geometry.GeometryExtended2026D35Reco_cff')
    process.load('Configuration.Geometry.GeometryExtended2026D35_cff')
elif GEOMETRY == "D41":
    print "using geometry " + GEOMETRY + " (tilted)"
    process = cms.Process('REPR',eras.Phase2C8)
    process.load('Configuration.Geometry.GeometryExtended2026D41Reco_cff')
    process.load('Configuration.Geometry.GeometryExtended2026D41_cff')
elif GEOMETRY == "D49":
    print "using geometry " + GEOMETRY + " (tilted)"
    process = cms.Process('REPR',eras.Phase2C9)
    process.load('Configuration.Geometry.GeometryExtended2026D49Reco_cff')
    process.load('Configuration.Geometry.GeometryExtended2026D49_cff')
else:
    print "this is not a valid geometry!!!"

process.load('Configuration.StandardSequences.Services_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '103X_upgrade2023_realistic_v2', '')

############################################################
# input and output
############################################################
if GEOMETRY == "D35": # Tilted barrel T6 tracker
    inputMC = ['/store/relval/CMSSW_10_4_0_mtd5/RelValTTbar_Tauola_14TeV/GEN-SIM-DIGI-RAW/PU25ns_103X_upgrade2023_realistic_v2_2023D35PU200-v1/20000/FE88E939-4FA1-D74F-AF49-64FA18C48E95.root']
elif GEOMETRY == "D41":
    inputMC = ['/store/mc/PhaseIITDRSpring19DR/TTbar_14TeV_TuneCP5_Pythia8/GEN-SIM-DIGI-RAW/PU200_106X_upgrade2023_realistic_v3_ext1-v3/60000/FFB5D0CA-208F-6040-A9BF-3F5354D0AA59.root']
elif GEOMETRY == "D49":
    #inputMC = ['/store/relval/CMSSW_11_1_0_pre1/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/110X_mcRun4_realistic_v2_2026D49noPU-v1/20000/DFF1257E-6DB1-434F-BCC2-64EC6DEFCDAF.root']
    from inputs111X_TTbar_PU200_cff import readFiles
    inputMC = readFiles
else:
    print "this is not a valid geometry!!!"

process.source = cms.Source("PoolSource",
    #fileNames = readFiles,
    fileNames = cms.untracked.vstring(*inputMC),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    inputCommands = cms.untracked.vstring("keep *", 
        "drop l1tEMTFHit2016Extras_simEmtfDigis_CSC_HLT",
        "drop l1tEMTFHit2016Extras_simEmtfDigis_RPC_HLT",
        "drop l1tEMTFHit2016s_simEmtfDigis__HLT",
        "drop l1tEMTFTrack2016Extras_simEmtfDigis__HLT",
        "drop l1tEMTFTrack2016s_simEmtfDigis__HLT"
    )

)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger.categories.extend(['L1TPFProducer','PatternFile','COEPatternFile','APxPatternFile','APxLinePair'])
#process.add_(cms.Service("Tracer"))

############################################################
# load modules and define regions
############################################################
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff') # needed to read HCal TPs
process.load('RecoMET.Configuration.GenMETParticles_cff')
process.load('RecoMET.METProducers.genMetTrue_cfi')
process.load('L1Trigger.Phase2L1ParticleFlow.l1ParticleFlow_cff')

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
    process.l1pfProducerGTT.regions = cms.VPSet(
        cms.PSet(
            etaBoundaries = cms.vdouble(-3,0,3),
            phiSlices = cms.uint32(9),
            etaExtra = cms.double(0.0),
            phiExtra = cms.double(0.0)
        )
    )
goRegional()

# General GTT options
process.l1pfProducerGTT.genOrigin = cms.InputTag('genParticles','xyz0')
process.l1pfProducerGTT.debug = cms.untracked.int32(0)
process.l1pfProducerGTT.trackRepresentationMode = cms.untracked.string("integer") #Options: {integer, fixedPoint}
process.l1pfProducerGTT.nTracksMax = cms.untracked.uint32(95) #107 in the future
secondary_file_path = os.getcwd()+'/trackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%itrackmax/' % (len(process.l1pfProducerGTT.regions[0].etaBoundaries)-1,process.l1pfProducerGTT.regions[0].phiSlices.value(),process.l1pfProducerGTT.nTracksMax.value())
if not os.path.exists(secondary_file_path): os.makedirs(secondary_file_path)

# DumpFile options
process.l1pfProducerGTT.dumpFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%iEventsTotal.dump" % (secondary_file_path,len(process.l1pfProducerGTT.regions[0].etaBoundaries)-1,process.l1pfProducerGTT.regions[0].phiSlices.value(),process.maxEvents.input.value()))

# COEPatternFile options
process.l1pfProducerGTT.nEventsCOEPerFile = cms.untracked.uint32(64) #64 is the maximum we can store on the VCU118
process.l1pfProducerGTT.nEventsCOEMax = cms.untracked.uint32(128)
process.l1pfProducerGTT.COEFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%iEventsPerFile.coe" % (secondary_file_path,len(process.l1pfProducerGTT.regions[0].etaBoundaries)-1,process.l1pfProducerGTT.regions[0].phiSlices.value(),process.l1pfProducerGTT.nEventsCOEPerFile.value()))

# APxPatternFile options
process.l1pfProducerGTT.nRowsAPxMax = cms.untracked.uint32(1024) #1024 is the maximum as set by the APx Firmware Shell
process.l1pfProducerGTT.nEventsAPxPerFile = cms.untracked.uint32(int(math.floor((2./3.)*process.l1pfProducerGTT.nRowsAPxMax.value()/process.l1pfProducerGTT.nTracksMax.value()))) # 2/3 for the tracks per row in the file
process.l1pfProducerGTT.nEventsAPxMax = cms.untracked.uint32(105)
process.l1pfProducerGTT.APxFileName = cms.untracked.string("%strackerRegion_alltracks_sectors_%ix%i_TTbar_PU200_%iEventsPerFile.txt" % (secondary_file_path,len(process.l1pfProducerGTT.regions[0].etaBoundaries)-1,process.l1pfProducerGTT.regions[0].phiSlices.value(),process.l1pfProducerGTT.nEventsAPxPerFile.value()))

# PatternFile sanity checks
if any(module.value() > process.maxEvents.input.value() for module in [process.l1pfProducerGTT.nEventsAPxMax,process.l1pfProducerGTT.nEventsCOEMax]):
    print "The number of events to be processed is less than the number of events requested for at least one of the pattern files."
    if process.l1pfProducerGTT.nEventsCOEMax.value() > process.maxEvents.input.value():
        print "\tResetting the nEventsCOEMax paramter to be %i." % process.maxEvents.input.value()
        process.l1pfProducerGTT.nEventsCOEMax = process.maxEvents.input
    if process.l1pfProducerGTT.nEventsAPxMax.value() > process.maxEvents.input.value():
        print "\tResetting the nEventsAPxMax paramter to be %i." % process.maxEvents.input.value()
        process.l1pfProducerGTT.nEventsAPxMax = process.maxEvents.input

############################################################
# define the path and output
############################################################
process.p = cms.Path(process.l1ParticleFlow * process.l1ParticleFlow_pf_gtt)
process.out = cms.OutputModule("PoolOutputModule",
     fileName = cms.untracked.string("Phase2L1ParticleFlowOutput.root"),
     outputCommands = cms.untracked.vstring(
       "keep *",
       )
 )
process.e = cms.EndPath(process.out)
