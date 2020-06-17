// STL includes
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>

// ROOT includes
#include "TROOT.h"
#include "TSystem.h"

// CMSSW includes
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"
#include "FWCore/PluginManager/interface/PresenceFactory.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/Region.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/COEPatternFile.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/APxPatternFile.h"

static std::vector<l1tpf_impl::Region> regions_;
typedef l1tpf_impl::InputRegion Region;

int testPatternFiles(int argc, char *argv[]) {
	// assign the command-line parameters to variables and setup the regions
	std::string const programName = argv[0];
	int debug = atoi(argv[1]);
	std::string filename_coe = argv[2];
	std::string filename_apx = argv[3];
	unsigned int bitsPerTrack, tracksPerSector, nRegionsPhi;
	float etaExtra, phiExtra;
	std::vector<double> etaBoundaries;
	try {
		bitsPerTrack = atoi(argv[4]);
		tracksPerSector = atoi(argv[5]);
		etaExtra = atof(argv[6]);
		phiExtra = atof(argv[7]);
		nRegionsPhi = atoi(argv[8]);
		std::vector<std::string> etaBoundariesStrings(argv + 9, argv + argc);
		std::size_t pos;
		for (unsigned int i=0; i<etaBoundariesStrings.size(); i++) {
			etaBoundaries.push_back(std::stoi(etaBoundariesStrings[i], &pos));
			if (pos < etaBoundariesStrings[i].size()) {
				std::cerr << "Trailing characters after number: " << etaBoundariesStrings[i] << '\n';
			}
		}
		float phiWidth = 2*M_PI/nRegionsPhi;
		for (unsigned int ieta = 0, neta = etaBoundaries.size()-1; ieta < neta; ++ieta) {
			for (unsigned int iphi = 0; iphi < nRegionsPhi; ++iphi) {
				float phiCenter = (iphi+0.5)*phiWidth-M_PI;
				regions_.push_back(l1tpf_impl::Region(etaBoundaries[ieta], etaBoundaries[ieta+1], phiCenter, phiWidth, phiExtra, etaExtra, false, 0, 0, 0, 0, 0, 0)); 
			}
		}
	} catch (std::invalid_argument const &ex) {
		std::cerr << "Invalid number in one of the eta-phi arguments" << std::endl;
		return -2;
	} catch (std::out_of_range const &ex) {
		std::cerr << "Number out of range in one of the eta-phi arguments" << std::endl;
		return -3;
	}

	// report the program configuraion
	edm::LogVerbatim("testPatternFile") << "@SUB=testPatternFile::testPatternFiles" << "Configuration:\n"
										<< "==============\n"
										<< "COE Filename: " << filename_coe << "\n"
										<< "APx Filename: " << filename_apx << "\n"
										<< "Number of regions (in eta-phi): " << regions_.size();
	for (unsigned int iregion=0; iregion<regions_.size(); iregion++) {
		edm::LogVerbatim("testPatternFile") << "@SUB=testPatternFile::testPatternFiles" << "\t" << iregion << " : eta=[" << std::setprecision(5) << regions_[iregion].etaMin
											<< "," << std::setprecision(5) << regions_[iregion].etaMax << "] phi=[" << std::setprecision(5)
											<< regions_[iregion].phiCenter-regions_[iregion].phiHalfWidth << "," << std::setprecision(5)
											<< regions_[iregion].phiCenter+regions_[iregion].phiHalfWidth << "]";
	}
	edm::LogVerbatim("testPatternFile") << "@SUB=testPatternFile::testPatternFiles" << "Number of tracks per sector: " << tracksPerSector << "\n"
										<< "Number of bits per track: " << bitsPerTrack << "\n"
										<< "Debug: " << debug << "\n"
										<< "==============\n";

	// make the parameter set needed by the PatternFile constructor
	edm::ParameterSet iConfigRegion;
	iConfigRegion.addParameter("etaBoundaries",etaBoundaries);
	iConfigRegion.addParameter("phiSlices",nRegionsPhi);
	iConfigRegion.addParameter("etaExtra",etaExtra);
	iConfigRegion.addParameter("phiExtra",phiExtra);
	edm::ParameterSet iConfig;
	iConfig.addUntrackedParameter("COEFileName",filename_coe);
	iConfig.addUntrackedParameter("APxFileName",filename_apx);
	iConfig.addUntrackedParameter("nTracksMax",tracksPerSector);
	iConfig.addUntrackedParameter("nEventsCOEMax",(unsigned int)-1);
	iConfig.addUntrackedParameter("nEventsAPxMax",(unsigned int)-1);
	iConfig.addUntrackedParameter("nEventsCOEPerFile",(unsigned int)0);
	iConfig.addUntrackedParameter("nEventsAPxPerFile",(unsigned int)0);
	iConfig.addParameter("regions",std::vector<edm::ParameterSet>{iConfigRegion});
	iConfig.addUntrackedParameter("debug",debug);

	edm::LogVerbatim("COEPatternFile") << "@SUB=testPatternFile::testPatternFiles" << "Testing the COEPatternFile code:";
	l1tpf_impl::COEPatternFile coefile(iConfig,std::ios_base::in);
	coefile.readHeader();
	coefile.readFile();
	coefile.loadEvent();
	coefile.printTableSizes("Counts before filtering");
	coefile.filterEmptyObjects();
	coefile.printTableSizes("Counts after filtering");
	coefile.printCommonFormat(coefile.convertBitsetTableToCommonFormat(),regions_);
	std::vector<unsigned int> lines(4);
    std::iota(lines.begin(),lines.end(),0);
	coefile.printEventDebugInfo(lines,true);

	edm::LogVerbatim("APxPatternFile") << "@SUB=testPatternFile::testPatternFiles" << "Testing the APxPatternFile code:";
	l1tpf_impl::APxPatternFile apxfile(iConfig,std::ios_base::in);
	apxfile.readHeader();
	apxfile.readFile();
	apxfile.loadEvent();
	apxfile.printTableSizes("Counts before filtering");
	apxfile.filterEmptyObjects();
	apxfile.printTableSizes("Counts after filtering");
	apxfile.printCommonFormat(apxfile.convertBitsetTableToCommonFormat(),regions_);
	apxfile.printEventDebugInfo(lines,true);

	return 0;
}

int main(int argc, char *argv[]) {

	// store some programatic information
	std::stringstream usage;
	usage << "usage: " << argv[0] << " <debug> <filename>.coe <filename>.txt <bitsPerTrack> <tracksPerSector> <etaExtra> <phiExtra> <nRegionsPhi> <etaBoundaries>";

	// argc should be at least  for correct execution
	// We print argv[0] assuming it is the program name
	if ( argc < 11 ) {
		std::cerr << "ERROR::testPatternFile " << argc << " arguments provided" << std::endl;
		for (int i=0; i<argc; i++) {
			std::cerr << "\tArgument " << i << ": " << argv[i] << std::endl;
		}
		std::cerr << usage.str() << std::endl;
		return -1;
	}

	// load framework libraries
	gSystem->Load("libFWCoreFWLite");
	FWLiteEnabler::enable();

	// needed to handle the MessageLogger used in the PatternFile classes
	int rc(0);
	std::string const kProgramName = argv[0];
	try {
		// A.  Instantiate a plug-in manager first.
		edm::AssertHandler ah;
	
		// B.  Load the message service plug-in.  Forget this and bad things happen!
		//     In particular, the job hangs as soon as the output buffer fills up.
		//     That's because, without the message service, there is no mechanism for
		//     emptying the buffers.
		std::shared_ptr<edm::Presence> theMessageServicePresence;
		theMessageServicePresence =
			std::shared_ptr<edm::Presence>(edm::PresenceFactory::get()->makePresence("MessageServicePresence").release());
	
		// C.  Manufacture a configuration and establish it.
#include <L1Trigger/Phase2L1ParticleFlow/test/messageLoggerConfiguration.h>
	
		// D.  Create the services.
		std::unique_ptr<edm::ParameterSet> params;
		edm::makeParameterSets(config, params);
		edm::ServiceToken tempToken(edm::ServiceRegistry::createServicesFromConfig(std::move(params)));
	
		// E.  Make the services available.
		edm::ServiceRegistry::Operate operate(tempToken);
	
		//  Generate a bunch of messages.
		rc = testPatternFiles(argc,argv);
	}
	
	//  Deal with any exceptions that may have been thrown.
	catch (cms::Exception& e) {
		std::cout << "cms::Exception caught in " << kProgramName << "\n" << e.explainSelf();
		rc = 1;
	} catch (std::exception& e) {
		std::cout << "Standard library exception caught in " << kProgramName << "\n" << e.what();
		rc = 1;
	} catch (...) {
		std::cout << "Unknown exception caught in " << kProgramName;
		rc = 2;
	}

	return rc;
}