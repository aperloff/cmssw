#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

using namespace l1tpf_impl;

PatternFile::PatternFile(const edm::ParameterSet& iConfig, std::ios_base::openmode fileOpenMode, const PatternFileType patternFileType) :
	openMode(fileOpenMode),
	fileName(iConfig.getUntrackedParameter<std::string>(getFileTypeString(patternFileType)+"FileName", "")),
	bset_string_(""),
	nTracksMax(iConfig.getUntrackedParameter<unsigned int>("nTracksMax")),
	phiSlices(iConfig.getParameter<std::vector<edm::ParameterSet>>("regions")[0].getParameter<uint32_t>("phiSlices")),
	etaRegions(iConfig.getParameter<std::vector<edm::ParameterSet>>("regions")[0].getParameter<vector<double>>("etaBoundaries").size()-1),
	nEventsMax(iConfig.getUntrackedParameter<unsigned int>(std::string("nEvents")+getFileTypeString(patternFileType)+"Max",-1)),
	nEventsPerFile(iConfig.getUntrackedParameter<unsigned int>(std::string("nEvents")+getFileTypeString(patternFileType)+"PerFile",0)),
	nEventsProcessed(0),
	debug_(iConfig.getUntrackedParameter<int>("debug",0))
{
	// Information about the number of events to be processed, the state of the file, and the number of files to process
	if (nEventsPerFile == 0) nEventsPerFile = nEventsMax;
	if (nEventsMax == 0) full = true;
	else 				 full = false;
	nFiles = std::ceil(float(nEventsMax)/nEventsPerFile);

	// Information about the representation (integer or fixed) of the tracks within the files/bitsets
	std::string trackRepresentationMode = "integer";
	if (iConfig.existsAs<std::string>("trackRepresentationMode")) trackRepresentationMode = iConfig.getParameter<std::string>("trackRepresentationMode");
	if (trackRepresentationMode == "integer")          trackRepresentationMode_ = integer;
	else if (trackRepresentationMode == "fixedPoint")  trackRepresentationMode_ = fixedPoint;
	else throw cms::Exception("Configuration", "Unsupported value for trackRepresentationMode: " + trackRepresentationMode+" (allowed are 'integer', 'fixedPoint')");
	edm::LogVerbatim("PatternFile") << "@SUB=PatternFile" << getFileTypeString(patternFileType) << " file track representation: " << trackRepresentationMode;

	// Bitset operations
	bset_.resize(tracksize);

	// File and filename operations
	fileNameBase = fileName.substr(0,fileName.rfind('.'));
	fileExtension = fileName.substr(fileName.rfind('.'));
	if (nEventsMax > nEventsPerFile) {
		// Prepare for the case where there will be multiple files and a suffix must be appended to each filename
		fileName = fileNameBase + "_" + std::to_string(0) + fileExtension;
	}
	bool success = open();
	if (!success) {
		throw cms::Exception("FileOpenError", "Unable to open the file "+fileName+"\n");
	}
}

// Convert the values in a PropogatedTrack to a 96-bit track word
void PatternFile::assignTrackToBitset(const PropagatedTrack& track) {
	for (unsigned int iblock=0; iblock<track_word_block_sizes.size(); iblock++) {
		for (unsigned int ibit=0; ibit<track_word_block_sizes[iblock]; ibit++) {
			int offset = std::accumulate(track_word_block_sizes.begin(), track_word_block_sizes.begin()+iblock, 0);
			switch(iblock) {
				case 0 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwPt,ibit)); break;
				case 1 : bset_.set(ibit+offset,track.hwCharge); break;
				case 2 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwVtxPhi,ibit)); break;
				case 3 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwVtxEta,ibit)); break;
				case 4 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwZ0,ibit)); break;
				case 5 : bset_.set(ibit+offset,0); break;
				case 6 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwChi2,ibit)); break;
				case 7 : bset_.set(ibit+offset,0); break;
				case 8 : bset_.set(ibit+offset,getBit<uint16_t>(track.hwStubs,ibit)); break;
				case 9 : bset_.set(ibit+offset,0); break;
			}
		}
	}
}

boost::dynamic_bitset<> PatternFile::getBits(const DynamicBitset& track, unsigned int start_bit, unsigned int end_bit) {
	DynamicBitset ret(end_bit-start_bit+1);
	for (unsigned int i = start_bit; i <= end_bit; i++) {
		ret.set(i-start_bit,track[i]);
	}
	return ret;
}

boost::dynamic_bitset<> PatternFile::getTrackLSBPlusMSB(const DynamicBitset& track1, const DynamicBitset& track2) {
	DynamicBitset track1_MSB = getBits(track1,64,95);
	DynamicBitset track2_LSB = getBits(track2,0,31);
	DynamicBitset ret(track2_LSB.size()+track1_MSB.size());
	for (unsigned int i = 0; i < track1_MSB.size(); i++) {
		ret.set(i,track1_MSB[i]);
		ret.set(i+track1_MSB.size(),track2_LSB[i]);
	}
	return ret;
}

// Print some region and track information
// The track information will be from the l1t::PFTrack, the l1tpf_impl::PropagatedTrack, and the bitset
// This allows for comparison of the tracks as they are accessed
void PatternFile::printDebugInfo(const Region& region, const PropagatedTrack& track) {
	message.str("");
	message << "region: eta=[" << region.etaMin << "," << region.etaMax << "] phi=" << region.phiCenter << "+/-" << region.phiHalfWidth << std::endl
			<< "l1t::PFTrack (pT,eta,phi) [float] = (" << track.src->p4().Pt() << "," << track.src->p4().Eta() << "," << track.src->p4().Phi() << ")" << std::endl
			<< "l1t::PFTrack (pT,eta,phi) [int] = (" << track.src->hwPt() << "," << track.src->hwEta() << "," << track.src->hwPhi() << ")" << std::endl
			<< "l1tpf_impl::PropagatedTrack (1/pT,eta,phi) [int,10] = (" << track.hwPt << "," << track.hwVtxEta << "," << track.hwVtxPhi << ")" << std::endl
			<< "l1tpf_impl::PropagatedTrack (1/pT,eta,phi) [int,2] = (" << std::bitset<16>(track.hwPt).to_string() << "," << std::bitset<32>(track.hwVtxEta).to_string() << "," << std::bitset<32>(track.hwVtxPhi).to_string() << ")" << std::endl
			<< "bitset = " << bset_string_ << std::endl;
	edm::LogInfo("PatternFile") << "@SUB=PatternFile::printDebugInfo" << message.str();
}

std::string PatternFile::readHeader() {
	if (header != "") {
		edm::LogError("PatternFile") << "@SUB=PatternFile::readHeader" << "The header is already filled and will not be reset!";
	}
	else {
		if (file.tellg() != 0) {
			throw cms::Exception("LogicError", "The position indicator of the file is not at the beginning.\n The header cannot be parsed correctly.\n");
		}
		std::string line;
		for (unsigned int iline = 0; iline < this->getHeaderLines() && std::getline(file,line); ++iline) {
			header.append(line);
		}
	}
	return header;
}