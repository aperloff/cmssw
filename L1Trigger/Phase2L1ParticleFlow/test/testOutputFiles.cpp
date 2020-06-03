// STL includes
#include <bitset>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <math.h>
#include <numeric>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

// ROOT includes
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "Math/GenVector/LorentzVector.h"
#include "Math/Vector4D.h"

// CMSSW includes
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"
#include "FWCore/PluginManager/interface/PresenceFactory.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "DataFormats/FWLite/interface/LuminosityBlock.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Phase2L1ParticleFlow/interface/PFTrack.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/DiscretePFInputsIO.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/Region.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/COEFile.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/APxPatternFile.h"

#define NTEST 2
#define REPORT_EVERY_N 1
#define NOBJECTS_PER_SECTOR 95
#define NBITS_PER_OBJECT 96
static std::vector<l1tpf_impl::Region> regions_;

typedef l1tpf_impl::InputRegion Region;
typedef std::pair<int,int> SectorObjectIndex;
typedef std::map<SectorObjectIndex,ROOT::Math::PtEtaPhiEVector> ObjectMap;

struct Event {
	uint32_t run, lumi; uint64_t event;
	float z0, genZ0;
	std::vector<float> puGlobals; // [float] alphaCMed, alphaCRms, alphaFMed, alphaFRms
	std::vector<Region> regions;
	
	Event() : run(0), lumi(0), event(0), z0(0.), regions() {}
	bool readFromFile(FILE *fRegionDump) {
		if (!fread(&run, sizeof(uint32_t), 1, fRegionDump)) return false;
		fread(&lumi, sizeof(uint32_t), 1, fRegionDump);
		fread(&event, sizeof(uint64_t), 1, fRegionDump);
		l1tpf_impl::readManyFromFile(regions, fRegionDump); 
		fread(&z0, sizeof(float), 1, fRegionDump);
		fread(&genZ0, sizeof(float), 1, fRegionDump);
		l1tpf_impl::readManyFromFile(puGlobals, fRegionDump);
		return true;
	}
};

ObjectMap get_tracks_from_root_file(fwlite::Event& ev, int entry = 0, bool print = false) {
	ObjectMap tracks_root;

	// clear the tracks currently stored in the regions
	for (l1tpf_impl::Region &r : regions_) { r.track.clear(); }

	// go to the event under test
	if (!ev.to(entry)) {
		std::cerr << "ERROR::testDumpFile::get_tracks_from_root_file Unable to load the event at entry " << entry << std::endl;
		assert(ev.to(entry));
	}
	if (print) printf("ROOT::Run %u, lumi %u, event %llu \n", ev.getRun().id().run(), ev.getLuminosityBlock().id().luminosityBlock(), ev.eventAuxiliary().id().event());

	edm::InputTag trackLabel ("pfTracksFromL1TracksBarrel");
	edm::Handle< std::vector<l1t::PFTrack>> h_track;
	ev.getByLabel(trackLabel, h_track);
	assert( h_track.isValid() );

	int ntrackstotal(0);
	const auto & tracks = *h_track;
	for (unsigned int itk = 0, ntk = tracks.size(); itk < ntk; ++itk) {
		const auto & tk = tracks[itk];
		if (tk.pt() <= 2.0 || tk.nStubs() < 4 || tk.normalizedChi2() >= 15.0) continue;
		for (l1tpf_impl::Region &r : regions_) {
			bool inside = r.contains(tk.eta(), tk.phi());;
			if (inside) {
				l1tpf_impl::PropagatedTrack prop;
				prop.fillInput(tk.pt(), r.localEta(tk.eta()), r.localPhi(tk.phi()), tk.charge(), tk.vertex().Z(), tk.quality(), &tk);
				prop.fillPropagated(tk.pt(), tk.trkPtError(), tk.caloPtError(), r.localEta(tk.caloEta()), r.localPhi(tk.caloPhi()), tk.quality(), tk.isMuon());
				prop.hwStubs = tk.nStubs();
				prop.hwChi2  = round(tk.chi2()*10);
				r.track.push_back(prop);
			}
		}
		ntrackstotal++;
	}
	for (unsigned int iregion = 0; iregion < regions_.size(); ++iregion) {
		std::vector<l1tpf_impl::PropagatedTrack> tracks_in_region = regions_[iregion].track;
		if (print) printf("\tFound region %u (eta=[%0.4f,%0.4f] phi=[%0.4f,%0.4f]) with %lu tracks\n", iregion, regions_[iregion].etaMin,regions_[iregion].etaMax, regions_[iregion].phiCenter-regions_[iregion].phiHalfWidth, regions_[iregion].phiCenter+regions_[iregion].phiHalfWidth, tracks_in_region.size());
		for (unsigned int it=0; it<tracks_in_region.size(); it++) {
			if (print) printf("\t\t Track %u (pT,eta,phi): (%.4f,%.4f,%.4f)\n", it, tracks_in_region[it].src->p4().pt(), tracks_in_region[it].src->p4().eta(), tracks_in_region[it].src->p4().phi());
			tracks_root[std::make_pair(iregion,it)] = ROOT::Math::PtEtaPhiEVector(tracks_in_region[it].src->pt(),tracks_in_region[it].src->eta(),tracks_in_region[it].src->phi(),tracks_in_region[it].src->pt());
		}
	}
	if (print) {
		printf("\t================================= \n");
		printf("\tTotal tracks %u \n\n", ntrackstotal);
	}

	return tracks_root;
}

ObjectMap get_tracks_from_dump_file(FILE *dfile_ = nullptr, bool print = false) {
	ObjectMap tracks_dump;
	Event event_;

	if (feof(dfile_)) {
		std::cerr << "ERROR::testDumpFile::get_tracks_from_dump_file We have already reached the end of the dump file" << std::endl;
		assert(!feof(dfile_));
	}
	if (!event_.readFromFile(dfile_)) {
		std::cerr << "ERROR::testDumpFile::get_tracks_from_dump_file Something went wrong reading from the dump file" << std::endl; 
		assert(event_.readFromFile(dfile_));
	}
	if (event_.regions.size() != regions_.size()) {
		printf("ERROR::testDumpFile::get_tracks_from_dump_file Mismatching number of input regions: %lu\n", event_.regions.size());
		assert(event_.regions.size() == regions_.size());
	}
	if (print) printf("Dump::Run %u, lumi %u, event %lu, regions %lu \n", event_.run, event_.lumi, event_.event, event_.regions.size());

	unsigned int ntrackstotal(0);
	float maxabseta(0), maxz(0), minz(0);

	int pv_gen   = round(event_.genZ0 * l1tpf_impl::InputTrack::Z0_SCALE);
	int pv_cmssw = round(event_.z0    * l1tpf_impl::InputTrack::Z0_SCALE);

	for (unsigned int is = 0; is < regions_.size(); ++is) {
		const Region & r = event_.regions[is];
		if (print) printf("\tRead region %u [%0.2f,%0.2f] with %lu tracks\n", is, r.phiCenter-r.phiHalfWidth, r.phiCenter+r.phiHalfWidth, r.track.size());
		ntrackstotal+=r.track.size();
		for (unsigned int it=0; it<r.track.size(); it++) {
			tracks_dump[std::make_pair(is,it)] = ROOT::Math::PtEtaPhiEVector(r.track[it].floatVtxPt(),r.track[it].floatVtxEta(),r.track[it].floatVtxPhi(),r.track[it].floatVtxPt());
			if(abs(r.track[it].hwVtxEta) > maxabseta) maxabseta = abs(r.track[it].hwVtxEta);
			if(r.track[it].hwZ0 > maxz) maxz = r.track[it].hwZ0;
			if(r.track[it].hwZ0 < minz) minz = r.track[it].hwZ0;
			if (print) printf("\t\t Track %u (pT,eta,phi): (%.4f,%.4f,%.4f)\n", it, r.track[it].floatVtxPt(), r.track[it].floatVtxEta(), r.track[it].floatVtxPhi());
		}
	}

	if (print) {
		printf("\t================================= \n");
		printf("\tTotal tracks %u \n", ntrackstotal);
		printf("\tMax abs(eta) %.2f [hw units] \n", maxabseta);
		printf("\tMax abs(eta) %.4f \n", maxabseta/l1tpf_impl::InputTrack::VTX_ETA_SCALE);
		printf("\t[Min,max] track z0 [%.2f,%.2f] [hw units] \n", minz, maxz);
		printf("\t[Min,max] track z0 [%.2f,%.2f] [cm] \n", minz/l1tpf_impl::InputTrack::Z0_SCALE, maxz/l1tpf_impl::InputTrack::Z0_SCALE);
		printf("\tPV (GEN) %u \n", pv_gen);
		printf("\tPV (CMSSW) %u \n\n", pv_cmssw);
	}

	return tracks_dump;
}

ObjectMap get_tracks_from_coe_file(l1tpf_impl::COEFile &cfile_, bool print = false, bool debug = false) {
	cfile_.loadNextEvent();
	cfile_.filterEmptyObjects();
	ObjectMap ret = cfile_.convertBitsetTableToCommonFormat();
	if (print) cfile_.printCommonFormat(ret,regions_);
	return ret;
}

ObjectMap get_tracks_from_apx_file(l1tpf_impl::APxPatternFile &afile_, bool print = false, bool debug = false) {
	afile_.loadNextEvent();
	afile_.filterEmptyObjects();
	ObjectMap ret = afile_.convertBitsetTableToCommonFormat();
	if (print) afile_.printCommonFormat(ret,regions_);
	return ret;
}

std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
	file.seekg(std::ios::beg);
	for(unsigned int i=0; i < num - 1; ++i){
		file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	}
	return file;
}

bool compare_lv_with_tolerance(ROOT::Math::PtEtaPhiEVector a, ROOT::Math::PtEtaPhiEVector b, const std::vector<float>& tolerance = {0,0,0,0}) {
	/*
	Example (Tolerance = 0.0005):
		Track from ROOT file: pt=16.3452797
		InputTrack::INVPT_SCALE = 2E4
		std::numeric_limits<uint16_t>::max() = 65535
		hwInvpt = std::min<double>(round(1/pt  * InputTrack::INVPT_SCALE), std::numeric_limits<uint16_t>::max()) = 1224.0000
		floatVtxPt() = 1/(float(hwInvpt) / InputTrack::INVPT_SCALE) = 16.339869
		So loss of precision comes from rounding
		Difference is DeltaPt=0.00541114807
	*/
	if (abs(a.Pt()-b.Pt())>tolerance[0] || abs(a.Eta()-b.Eta())>tolerance[1] || abs(a.Phi()-b.Phi())>tolerance[2] || abs(a.E()-b.E())>tolerance[3]) {
		std::cerr<<std::setprecision(9);
		std::cerr << std::endl << "\tMismatching " << std::flush;
		if (abs(a.Pt()-b.Pt())>tolerance[0]) std::cerr << "pT! " << a.Pt() << " vs " << b.Pt() << " where DeltaPt=" << abs(a.Pt()-b.Pt()) << " and epsilon=" << tolerance[0] << std::endl;
		else if (abs(a.Eta()-b.Eta())>tolerance[1]) std::cerr << "eta! " << a.Eta() << " vs " << b.Eta() << " where DeltaEta=" << abs(a.Eta()-b.Eta()) << " and epsilon=" << tolerance[1] << std::endl;
		else if (abs(a.Phi()-b.Phi())>tolerance[2]) std::cerr << "phi! " << a.Phi() << " vs " << b.Phi() << " where DeltaPhi=" << abs(a.Phi()-b.Phi()) << " and epsilon=" << tolerance[2] << std::endl;
		else if (abs(a.E()-b.E())>tolerance[3]) std::cerr << "E! " << a.E() << " vs " << b.E() << " where DeltaE=" << abs(a.E()-b.E()) << " and epsilon=" << tolerance[3] << std::endl;
		return false;
	}
	return true;
}

bool compare_maps(ObjectMap ref, ObjectMap test) {
	ROOT::Math::PtEtaPhiEVector tlv;
	for (auto it=ref.begin(); it!=ref.end(); it++) {
		if (test.find(it->first)==test.end()) {
			std::cerr << std::endl << "\tERROR::compare_maps Can't find the test track with (sector,index)=(" << it->first.first << "," << it->first.second << ")" << std::endl;
			return false;
		}
		tlv = test.find(it->first)->second;
		// The pT tolerance should be 1.0/l1tpf_impl::CaloCluster::PT_SCALE, but because of the rounding this is not true and the actual resolution isn't always as good
		// Instead, we will use max(1% of the pT of the reference LorentzVector,0.25)
		// We use the max statement because at low pT, the 1% definition doesn't hold anymore. This wouldn't be a problem if 1/pT were encoded rather than pT.
		if (!compare_lv_with_tolerance(it->second,tlv,{float(std::max(it->second.Pt()*1E-2,1.0/l1tpf_impl::CaloCluster::PT_SCALE)),1.0/l1tpf_impl::InputTrack::VTX_ETA_SCALE,1.0/l1tpf_impl::InputTrack::VTX_PHI_SCALE,float(std::max(it->second.Pt()*1E-2,1.0/l1tpf_impl::CaloCluster::PT_SCALE))})) {
			std::cerr << std::endl << "\tERROR::compare_maps Can't find the test track with LorentzVector (" << it->second.Pt() << "," << it->second.Eta() << "," << it->second.Phi() << "," << it->second.E() << ")" << std::endl
					  << "\t\tInstead found (" << tlv.Pt() << "," << tlv.Eta() << "," << tlv.Phi() << "," << tlv.E() << ") at the position (sector,index)=(" << it->first.first << "," << it->first.second << ")" << std::endl;
			return false;
		}
	}
	return true;
}

int testOutputFiles(int argc, char *argv[], std::string usage) {
	// assign the command-line parameters to variables and setup the regions
	std::string filename_root = argv[1];
	std::string filename_dump = argv[2];
	std::string filename_coe = argv[3];
	std::string filename_apx = argv[4];
	float etaExtra, phiExtra;
	unsigned int nRegionsPhi;
	std::vector<double> etaBoundaries;
	try {
		etaExtra = atof(argv[5]);
		phiExtra = atof(argv[6]);
		nRegionsPhi = atoi(argv[7]);
		std::vector<std::string> etaBoundariesStrings(argv + 8, argv + argc);
		std::size_t pos;
		for (unsigned int i=0; i<etaBoundariesStrings.size(); i++) {
			etaBoundaries.push_back(std::stoi(etaBoundariesStrings[i], &pos));
			if (pos < etaBoundariesStrings[i].size()) {
				edm::LogWarning("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Trailing characters after number: " << etaBoundariesStrings[i];
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
		throw cms::Exception("InvalidArgument","Invalid number in one of the eta-phi arguments\n");
	} catch (std::out_of_range const &ex) {
		throw cms::Exception("OutOfRange","Number out of range in one of the eta-phi arguments\n");
	}

	// check the filenames
	if (filename_root.find(".root")==std::string::npos)	{
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Filename 1 must be a ROOT (.root) file\n" << usage;
		return -5;
	}
	else if (filename_dump.find(".dump")==std::string::npos) {
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Filename 2 must be a binary (.dump) file\n" << usage;
		return -6;
	}
	else if (filename_coe.find(".coe")==std::string::npos) {
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Filename 3 must be a COE (.coe) file\n" << usage;
		return -7;
	}
	else if (filename_apx.find(".txt")==std::string::npos) {
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Filename 4 must be an APx (.txt) file\n" << usage;
		return -8;
	}

	// report the program configuraion
	edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Configuration:\n"
										<< "==============\n"
										<< "ROOT Filename: " << filename_root << "\n"
										<< "Dump Filename: " << filename_dump << "\n"
										<< "COE Filename: " << filename_coe << "\n"
										<< "APx Filename: " << filename_apx << "\n"
										<< "Number of tests (events): " << NTEST << "\n"
										<< "Report every N tests: " << REPORT_EVERY_N << "\n"
										<< "Number of regions (in eta-phi): " << regions_.size();
	for (unsigned int iregion=0; iregion<regions_.size(); iregion++) {
		edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "\t" << iregion << " : eta=[" << std::setprecision(5) << regions_[iregion].etaMin
											<< "," << std::setprecision(5) << regions_[iregion].etaMax << "] phi=[" << std::setprecision(5)
											<< regions_[iregion].phiCenter-regions_[iregion].phiHalfWidth << "," << std::setprecision(5)
											<< regions_[iregion].phiCenter+regions_[iregion].phiHalfWidth << "]";
	}
	edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Number of tracks per sector: " << NOBJECTS_PER_SECTOR << "\n"
										<< "Number of bits per track: " << NBITS_PER_OBJECT << "\n"
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
	iConfig.addUntrackedParameter("nTracksMax",(unsigned int)NOBJECTS_PER_SECTOR);
	iConfig.addUntrackedParameter("nEventsCOEMax",(unsigned int)-1);
	iConfig.addUntrackedParameter("nEventsAPxMax",(unsigned int)-1);
	iConfig.addUntrackedParameter("nEventsCOEPerFile",(unsigned int)0);
	iConfig.addUntrackedParameter("nEventsAPxPerFile",(unsigned int)0);
	iConfig.addParameter("regions",std::vector<edm::ParameterSet>{iConfigRegion});
	iConfig.addUntrackedParameter("debug",0);

	// open the files for testing
	TFile* rfile_ = TFile::Open(filename_root.c_str(),"READ");
	if (!rfile_) {
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Cannot open '" << filename_root << "'";
		return -9;
	}
	fwlite::Event rfileentry_(rfile_);
	FILE *dfile_(fopen(filename_dump.c_str(),"rb"));
	if (!dfile_) {
		edm::LogError("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Cannot open '" << filename_dump << "'";
		return -10;
	}
	l1tpf_impl::COEFile cfile_(iConfig,std::ios_base::in);
	cfile_.readHeader();
	cfile_.readFile();
	l1tpf_impl::APxPatternFile afile_(iConfig,std::ios_base::in);
	afile_.readHeader();
	afile_.readFile();

	ObjectMap tracks_root, tracks_dump, tracks_coe, tracks_apx;

	// run the tests for multiple events
	for (int test = 1; test <= NTEST; ++test) {
		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Doing test " << test << " ... ";

		tracks_root = get_tracks_from_root_file(rfileentry_,test-1,test==1);
		tracks_dump = get_tracks_from_dump_file(dfile_,test==1);
		tracks_coe  = get_tracks_from_coe_file(cfile_,test==1);
		tracks_apx  = get_tracks_from_apx_file(afile_,test==1);

		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Comparing the ROOT tracks to the dump tracks in event " << test << " ... ";
		if (!compare_maps(tracks_root,tracks_dump)) return -12;
		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "\tDONE";

		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Comparing the ROOT tracks to the COE tracks in event " << test << " ... ";
		if (!compare_maps(tracks_root,tracks_coe))  return -13;
		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "\tDONE";

		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "Comparing the ROOT tracks to the APx tracks in event " << test << " ... ";
		if (!compare_maps(tracks_root,tracks_apx))  return -14;
		if (test%REPORT_EVERY_N == 1) edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "\tDONE\n";
	}

	edm::LogVerbatim("testOutputFiles") << "@SUB=testOutputFiles::testOutputFiles" << "\nThe dump, COE, and APx outputs match the ROOT outputs for all events!";
	return 0;
}

int main(int argc, char *argv[]) {

	// store some programatic information
	std::stringstream usage;
	usage << "usage: " << argv[0] << " <filename>.root <filename>.dump <filename>.coe <filename>.txt <etaExtra> <phiExtra> <nRegionsPhi> <etaBoundaries>\n\n"
		  << "Standalong compilation:\n"
		  << "\tg++ -I${CMSSW_BASE}/src/L1Trigger/Phase2L1ParticleFlow/interface/ -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 testOutputFiles.cpp\n"
		  << "\tg++ -o testOutputFiles testOutputFiles.o\n"
		  << "\t./testOutputFiles <arguments>\n\n"
		  << "Run single test:\n"
		  << "\t${CMSSW_BASE}/test/${SCRAM_ARCH}/testPatternFile <arguments>\n\n"
		  << "Run all tests:\n"
		  << "\tscram b runtests" << std::endl;

	// load framework libraries
	gSystem->Load("libFWCoreFWLite");
	FWLiteEnabler::enable();

	// argc should be at least 9 for correct execution
	// We print argv[0] assuming it is the program name
	if ( argc < 10 ) {
		std::cerr << "ERROR::testOutputFiles " << argc << " arguments provided" << std::endl;
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
		rc = testOutputFiles(argc,argv,usage.str());
	}
	
	//  Deal with any exceptions that may have been thrown.
	catch (cms::Exception& e) {
		std::cout << "cms::Exception caught in " << kProgramName << "\n" << e.explainSelf();
		rc = -2;
	} catch (std::exception& e) {
		std::cout << "Standard library exception caught in " << kProgramName << "\n" << e.what();
		rc = -3;
	} catch (...) {
		std::cout << "Unknown exception caught in " << kProgramName;
		rc = -4;
	}

	return rc;
}