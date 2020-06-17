#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

using namespace l1tpf_impl;

PatternFile::PatternFile(const edm::ParameterSet& iConfig, std::ios_base::openmode fileOpenMode, const PatternFileType patternFileType) :
	openMode(fileOpenMode),
	fileName(iConfig.getUntrackedParameter<std::string>(getFileTypeString(patternFileType)+"FileName", "")),
	bset_string_(""),
	nTracksMax(iConfig.getUntrackedParameter<unsigned int>("nTracksMax")),
	phiSlices(iConfig.getParameter<std::vector<edm::ParameterSet>>("regions")[0].getParameter<uint32_t>("phiSlices")),
	etaRegions(iConfig.getParameter<std::vector<edm::ParameterSet>>("regions")[0].getParameter<vector<double>>("etaBoundaries").size()-1),
	nLinks(phiSlices*etaRegions),
	ifile(0),
	ievent(0),
	nEventsMax(iConfig.getUntrackedParameter<unsigned int>(std::string("nEvents")+getFileTypeString(patternFileType)+"Max",-1)),
	nEventsPerFile(iConfig.getUntrackedParameter<unsigned int>(std::string("nEvents")+getFileTypeString(patternFileType)+"PerFile",0)),
	nEventsProcessed(0),
	debug_(iConfig.getUntrackedParameter<int>("debug",0))
{
	// Information about the number of events to be processed, the state of the file, and the number of files to process
	if (nEventsPerFile <= 0) nEventsPerFile = nEventsMax;
	if (nEventsMax == 0) full = true;
	else 				 full = false;
	nFiles = std::ceil(float(nEventsMax)/nEventsPerFile);

	// Information about the representation (integer or fixed) of the objects within the files/bitsets
	std::string objectRepresentationMode = "integer";
	if (iConfig.existsAs<std::string>("trackRepresentationMode")) objectRepresentationMode = iConfig.getParameter<std::string>("trackRepresentationMode");
	if (objectRepresentationMode == "integer")          objectRepresentationMode_ = integer;
	else if (objectRepresentationMode == "fixedPoint")  objectRepresentationMode_ = fixedPoint;
	else throw cms::Exception("Configuration", "Unsupported value for objectRepresentationMode: " + objectRepresentationMode+" (allowed are 'integer', 'fixedPoint')");
	edm::LogVerbatim("PatternFile") << "@SUB=PatternFile" << getFileTypeString(patternFileType) << " file object representation: " << objectRepresentationMode;

	// Bitset operations
	bset_.resize(tracksize);
	bset_table_.resize(nLinks);

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

ROOT::Math::PtEtaPhiEVector PatternFile::bitsetToLorentzVector(const DynamicBitset& b) {

	DynamicBitset pt = getBits(b,0,13);
	DynamicBitset vtxPhi = getBits(b,15,26);
	DynamicBitset vtxEta = getBits(b,27,42);
	float vtxPt_f = (float(pt.to_ulong()) / l1tpf_impl::CaloCluster::PT_SCALE);
	float vtxEta_f = float(bitsetToInt64<16>(vtxEta)) / l1tpf_impl::InputTrack::VTX_ETA_SCALE;
	float vtxPhi_f = float(bitsetToInt64<12>(vtxPhi)) / l1tpf_impl::InputTrack::VTX_PHI_SCALE;

	if (debug_) {
		message.str("");
		message << "\tbset_string_ = " << b << "\n";
		message << "\tpt (0b) = "; for (int i=14-1; i>=0; i--) {message << b[i];} message << "\n";
		message << "\tvtxPhi (0b) = "; for (int i=12-1; i>=0; i--) {message << b[i+15];} message << "\n";
		message << "\tvtxEta (0b) = "; for (int i=16-1; i>=0; i--) {message << b[i+27];} message << "\n";
		message << "\tpt (int) = " << pt.to_ulong() << "\n"
				<< "\tvtxPhi (int) = " << bitsetToInt64<12>(vtxPhi) << "\n"
				<< "\tvtxEta (int) = " << bitsetToInt64<16>(vtxEta) << "\n"
				<< "\tvtxPt_f (float) = " << vtxPt_f << "\n"
				<< "\tvtxPhi_f (float) = " << vtxPhi_f << "\n"
				<< "\tvtxEta_f (float) = " << vtxEta_f << "\n";
		edm::LogInfo("PatternFile") << "@SUB=PatternFile::bitsetToLorentzVector()" << message.str();
	}

	return makeLorentzVector(vtxPt_f,vtxEta_f,vtxPhi_f,vtxPt_f);
}

l1tpf_impl::PatternFile::CommonFormat PatternFile::convertBitsetTableToCommonFormat() {
	CommonFormat ret;
	unsigned int nobjects(0);
	for (unsigned int ilink = 0; ilink < bset_table_.size(); ilink++) {
		for (unsigned int iobject = 0; iobject < bset_table_[ilink].size(); iobject++) {
			if (bset_table_[ilink][iobject].any()) {
				nobjects++;
				ret[std::make_pair(ilink,iobject)] = bitsetToLorentzVector(bset_table_[ilink][iobject]);
			}
		}
	}
	if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::convertBitsetTabletToCommonFormat" << "The total number of objects converted is " << nobjects;
	return ret;
}

bool PatternFile::eof() {
	 bool reachedMaxEvents = (nEventsMax > -1) && (nEventsProcessed+1 == nEventsMax);
	 bool reachedMaxEventsPerFile = (nEventsPerFile > 0) && ((nEventsProcessed+1)%nEventsPerFile==0);
	 bool endOfFile = file.eof();
	 bool ret = reachedMaxEvents || reachedMaxEventsPerFile || endOfFile;
	 if (debug_ && ret) {
	 	message.str("");
		message << "End of file reached:\n"
				<< "\tNumber of events processed: " << nEventsProcessed << "\n"
				<< "\tTotal number of events: " << nEventsMax << "\n"
				<< "\tNumber of events per file: " << nEventsPerFile << "\n"
				<< "\tifstream eof status: " << file.eof() << "\n"
				<< "\treachedMaxEvents (bool): " << reachedMaxEvents << "\n"
				<< "\treachedMaxEventsPerFile (bool): " << reachedMaxEventsPerFile << "\n"
				<< "\tEOF formula returning \'true\': reachedMaxEvents || reachedMaxEventsPerFile || ifstream eof status\n";
		edm::LogInfo("PatternFile") << "@SUB=PatternFile::eof()" << message.str();
	}
	return ret;
}

bool PatternFile::emptyTable() {
	// Count the number of links that have a stored bitset
	unsigned int ret = count_if(bset_table_.begin(), bset_table_.end(), [](BitsetCollection bc){return bc.size() > 0;});
	// Return true if the table is empty, otherwise return false if any of the links are storing a bitset
	return ret == 0;
}

void PatternFile::filterEmptyObjects() {
	for (unsigned int ilink = 0; ilink < bset_table_.size(); ilink++) {
		bset_table_[ilink].erase(std::remove_if(bset_table_[ilink].begin(),bset_table_[ilink].end(),[](DynamicBitset db){return !db.any();}),bset_table_[ilink].end());
	}
	if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::filterEmptyObjects" << "The current length of column 0 for the BitsetTable " << bset_table_[0].size();
}

// Generic implementation to search if a given value exists in a map or not.
// Adds all the keys with given value in the vector
template<typename K, typename V, typename T>
bool PatternFile::findAllFromLink(std::vector<K> & vec, std::map<K, V> mapOfElemen, T value) {
	bool bResult = false;
	auto it = mapOfElemen.begin();
	// Iterate through the map
	while(it != mapOfElemen.end())
	{
		// Check if value of this entry matches with given value
		if(it->first.first == value)
		{
			// Yes found
			bResult = true;
			// Push the key in given map
			vec.push_back(it->first);
		}
		// Go to next entry in map
		it++;
	}
	return bResult;
}

l1tpf_impl::PatternFile::DynamicBitset PatternFile::getBits(const DynamicBitset& bset, unsigned int start_bit, unsigned int end_bit) {
	DynamicBitset ret(end_bit-start_bit+1);
	for (unsigned int i = start_bit; i <= end_bit; i++) {
		ret.set(i-start_bit,bset[i]);
	}
	return ret;
}

unsigned int PatternFile::getNEventsInFile(unsigned int ifile_) {
	// If there are no BitsetTables saved in the BitsetTableCollection, then an exception will be thrown	
	if (events_per_file_.size() == 0) {
		throw cms::Exception("LogicError", "The BitsetTableCollection is empty!\nPerhaps no events were stored and/or no events were retrieved from a PatternFile.\n");
	}
	// If the number of BitsetTables is 1 and a file other than 0 is selected, a warning will be issued and the first event count will be returned
	if ( (events_per_file_.size() == 1) && (ifile_ != 0) ) {
		edm::LogError("PatternFile") << "@SUB=PatternFile::getNEventsInFile" << "There is only one BitsetTable available. The ifile_ pararmeter will be ignored.";
		ifile_ = 0;
	}
	// If the number of BitsetTables is greater than 1, but the number requested is outside the bounds of the list, an exception will be thrown
	if ( (events_per_file_.size() > 1) && (ifile_ > bset_table_collection_.size()-1) ) {
		message.str("");
		message << "The requested BitsetTable (ifile_ = " << ifile_ << ") is outside the bounds of currently stored BitsetTableCollection (size = "
				<< events_per_file_.size() << ").\n";
		throw cms::Exception("LogicError", message.str());
	}
	return events_per_file_[ifile_];
}

l1tpf_impl::PatternFile::DynamicBitset PatternFile::getTrackLSBPlusMSB(const DynamicBitset& bset1, const DynamicBitset& bset2) {
	DynamicBitset bset1_MSB = getBits(bset1,64,95);
	DynamicBitset bset2_LSB = getBits(bset2,0,31);
	DynamicBitset ret(bset2_LSB.size()+bset1_MSB.size());
	for (unsigned int i = 0; i < bset1_MSB.size(); i++) {
		ret.set(i,bset1_MSB[i]);
		ret.set(i+bset1_MSB.size(),bset2_LSB[i]);
	}
	return ret;
}

// Selects objects from a BitsetTable after selecting the correct table for that file
bool PatternFile::loadEvent(unsigned int ifile_, unsigned int ievent_) {
	// If there are no BitsetTables saved in the BitsetTableCollection, then an exception will be thrown	
	if (bset_table_collection_.size() == 0) {
		throw cms::Exception("LogicError", "The BitsetTableCollection is empty!\nPerhaps no events were stored and/or no events were retrieved from a PatternFile.\n");
	}
	// If the number of BitsetTables is 1 and a file other than 0 is selected, a warning will be issued and false will be returned
	if ( (bset_table_collection_.size() == 1) && (ifile_ != 0) ) {
		edm::LogError("PatternFile") << "@SUB=PatternFile::loadEvent" << "There is only one BitsetTable available. The ifile pararmeter will be ignored.";
		return false;
	}
	// If the number of BitsetTables is greater than 1, but the number requested is outside the bounds of the list, andexception will be thrown
	if ( (bset_table_collection_.size() > 1) && (ifile_ > bset_table_collection_.size()-1) ) {
		message.str("");
		message << "The requested BitsetTable (ifile_ = " << ifile_ << ") is outside the bounds of currently stored BitsetTableCollection (size = "
				<< bset_table_collection_.size() << ").\n";
		throw cms::Exception("LogicError", message.str());
	}

	// Set the internal ifile and ievent values
	ifile = ifile_;
	ievent = ievent_;

	int start_track = (ievent*nTracksMax);
	int end_track_p1 = (ievent+1)*nTracksMax;
	for (unsigned int ilink = 0; ilink < bset_table_collection_[ifile].size(); ilink++) {
		bset_table_[ilink].assign(bset_table_collection_[ifile][ilink].begin()+start_track, bset_table_collection_[ifile][ilink].begin()+end_track_p1);
		if (bset_table_[ilink].size() != nTracksMax) {
			message.str("");
			message << "Uh oh! Something went wrong when retrieving the objects for event " << ievent << " from file " << ifile << ".\n"
					<< bset_table_[ilink].size() << " objects were retrieved for link " << ilink << " rather than " << nTracksMax << ".\n";
			throw cms::Exception("LogicError", message.str());
		}
	}
	if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::loadEvent" << "The current BitsetTable shape is ("
										<< bset_table_[0].size() << "x" << bset_table_.size() << ")";

	return true;
}

bool PatternFile::loadNextEvent() {
	// If ifile==0 and ievent==0, but bset_table_ is empty, load the first event
	if ( (ifile == 0) && (ievent == 0) && (emptyTable()) ) {
		return loadEvent();
	}

	// If you've run out of events in the current file, go to the next file if one exists
	// Otherwise, return false
	else if (ievent+1 == events_per_file_[ifile]) {
		if (ifile+1 < bset_table_collection_.size()) {
			return loadEvent(++ifile,0);
		}
		else {
			edm::LogError("PatternFile") << "@SUB=PatternFile::loadNextEvent" << "There are no more files/events to load.";
			return false;
		}
	}

	// Otherwise, if there are more events in the file, load the next event (ievent++)
	else {
		return loadEvent(ifile,++ievent);
	}
}

bool PatternFile::nextFile() {
	if (ifile == nFiles-1) return false;
	else {
		close();
		ifile++;
		fileName = fileNameBase + "_" + std::to_string(ifile) + fileExtension;
		bool success = open();
		if (!success) {
			throw cms::Exception("FileOpenError", fileName+"\n\tfstream goodbit set to false!\n");
		}
		else if (!file.is_open()) {
			throw cms::Exception("FileOpenError", "Unable to open the file "+fileName+"\n");
		}
	}
	return true;
}

void PatternFile::printCommonFormat(CommonFormat objects, const std::vector<Region>& regions) {
	if ( (regions.size() != 0) && (regions.size() != nLinks) ) {
		message.str("");
		message << "The number of regions (" << regions.size() << ") is not equal to the number of links (" << nLinks << ")";
		edm::LogWarning("PatternFile") << "@SUB=PatternFile::printCommonFormat" << message.str();
	}

	message.str("");
	message << getFileTypeString(this->getClassType()) << "::Run \"unknown\", lumi \"unknown\", event \"unknown\", regions " << regions.size() << "\n";
	for (unsigned int ilink = 0; ilink < nLinks; ilink++) {
		LinkObjectIndexCollection objectsFromLink;
		findAllFromLink<LinkObjectIndex,ROOT::Math::PtEtaPhiEVector,int>(objectsFromLink,objects,ilink);
		message << "\tRegion " << ilink;
		if (regions.size() != 0) {
			message << " (eta=[" << std::setprecision(5) << regions[ilink].etaMin << "," << std::setprecision(5) << regions[ilink].etaMax
					<< "] phi=[" << std::setprecision(5) << regions[ilink].phiCenter-regions[ilink].phiHalfWidth << "," << std::setprecision(5)
					<< regions[ilink].phiCenter+regions[ilink].phiHalfWidth << "])";
		}
		message << " has " << objectsFromLink.size() << " objects\n";
		for (unsigned int iobject = 0; iobject < objectsFromLink.size(); iobject++) {
			ROOT::Math::PtEtaPhiEVector tmp = objects[objectsFromLink[iobject]];
			message << "\t\tObject " << iobject << " (pT,eta,phi): (" << std::setprecision(4) << tmp.Pt() << "," << std::setprecision(4) << tmp.Eta() << "," << std::setprecision(4) << tmp.Phi() << ")\n";
		}
	}
	message << "\t=================================\n\tTotal objects: " << objects.size() << "\n";

	edm::LogVerbatim("PatternFile") << "@SUB=PatternFile::printCommonFormat" << message.str();
}

void PatternFile::printEventDebugInfo(unsigned int row, bool verbatim) {
	message.str("");
	if (row > bset_table_[0].size()-1) {
		message << "The selected row (" << row << ") is not within the bounds of the BitsetTable (0," << bset_table_[0].size()-1 << ").\n";
		throw cms::Exception("OutOfRange",message.str());
	}

	message << "Row " << row << ": ";
	for (unsigned int icol = 0; icol < bset_table_.size(); icol++) {
		boost::to_string(bset_table_[icol][row],bset_string_);
		message << " " << bset_string_;
	}
	message << std::endl;
	if (verbatim) edm::LogVerbatim("PatternFile") << "@SUB=PatternFile::printEventDebugInfo" << message.str();
	else edm::LogInfo("PatternFile") << "@SUB=PatternFile::printEventDebugInfo" << message.str();
}

/*
Example Table:
****************************
* Header                   *
****************************
*      Link * 00 * 01 * 02 *
****************************
*      Size *  n *  n *  n *
****************************
* Non-empty *  m *  m *  m *
****************************
*/
void PatternFile::printTableSizes(std::string tableHeader) {
	std::vector<int> sizes;
	std::vector<int> nonZeroCount;
	std::vector<int> zeroCount;
	sizes.resize(bset_table_.size());
	nonZeroCount.resize(bset_table_.size());
	zeroCount.resize(bset_table_.size());
	for (unsigned int ilink = 0; ilink < bset_table_.size(); ilink++) {
		sizes[ilink] = bset_table_[ilink].size();
		nonZeroCount[ilink] = count_if(bset_table_[ilink].begin(), bset_table_[ilink].end(), [](DynamicBitset db){return db.any();});
		zeroCount[ilink] = count_if(bset_table_[ilink].begin(), bset_table_[ilink].end(), [](DynamicBitset db){return !db.any();});
	}

	//Count the width of each column
	std::vector<std::string> rowHeaders = {"Link","Size","Non-empty","Empty"};
	int rowHeaderWidth = std::max_element(rowHeaders.begin(),rowHeaders.end(), [](std::string s1, std::string s2){return s1.size() < s2.size();})->size();
	int linkWidth      = std::ceil(std::log10(bset_table_.size()));
	std::vector<int> columnWidths;
	columnWidths.resize(bset_table_.size());
	for (unsigned int ilink = 0; ilink < bset_table_.size(); ilink++) {
		std::vector<int> values = {linkWidth, (int)std::ceil(std::log10(sizes[ilink])), (int)std::ceil(std::log10(nonZeroCount[ilink])), (int)std::ceil(std::log10(zeroCount[ilink]))};
		columnWidths[ilink] = *std::max_element(values.begin(),values.end());
	}

	// Find the total width of the table
	int tableWidth = 2+rowHeaderWidth+2+(3*bset_table_.size())+std::accumulate(columnWidths.begin(),columnWidths.end(),0);

	// Now print the table
	message.str("");
	if (!tableHeader.empty()) {
		message << std::string(tableWidth, '*') << "\n";
		message << "* " << std::left << tableHeader << std::right << std::setw(tableWidth-tableHeader.size()-1) << "*\n";
	}
	message << std::string(tableWidth, '*') << "\n";
	for (unsigned int irow = 0; irow < rowHeaders.size(); irow++) {
		message << "* " << std::setw(rowHeaderWidth) << rowHeaders[irow] << " *";
		for (unsigned int ilink = 0; ilink < bset_table_.size(); ilink++) {
			if (irow == 0) {
				message << std::setw(columnWidths[ilink]-linkWidth+1) << " " << std::setw(linkWidth) << std::setfill('0') << ilink << std::setfill(' ') << " *";
			}
			else if (irow == 1) {
				message << std::setw(columnWidths[ilink]+1) << sizes[ilink] << " *";
			}
			else if (irow == 2) {
				message << std::setw(columnWidths[ilink]+1) << nonZeroCount[ilink] << " *";
			}
			else if (irow == 3) {
				message << std::setw(columnWidths[ilink]+1) << zeroCount[ilink] << " *";
			}
		}
		message << "\n" << std::string(tableWidth, '*') << "\n";
	}
	edm::LogVerbatim("PatternFile") << "@SUB=PatternFile::printTableSizes" << message.str();
}

// Print some region and track information
// The track information will be from the l1t::PFTrack, the l1tpf_impl::PropagatedTrack, and the bitset
// This allows for comparison of the tracks as they are accessed
void PatternFile::printTrackDebugInfo(const Region& region, const PropagatedTrack& track) {
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

// Store the tracks for a given events
// In most cases, this should happen once per event, so keep track of:
//   1. The number of events processed
//   2. If we've hit the total number of events to process
//   3. If we've hit the end of a file, we need to put the current
//       table into a collection and reset the working table
void PatternFile::storeTracks(const std::vector<Region>& regions) {
	if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::storeTracks" << "Storing event " << nEventsProcessed << " ... ";
	PropagatedTrack current_track;
	bool has_track = false;
	for (unsigned int irow = 0; irow < nTracksMax; irow++) {
		for (unsigned int icol = 0; icol < regions.size(); icol++) {
			if (regions[icol].track.size()<=irow) has_track = false;
			else has_track = true;

			if (has_track) {
				// Select the track that will be converted to a bit string
				current_track = regions[icol].track[irow];

				// Convert the values in a PropogatedTrack to a 96-bit track word
				assignTrackToBitset(current_track);

				// Save the bitset for later
				bset_table_[icol].push_back(bset_);

				// Print some debugging information
				if (debug_ && irow==0 && icol==0) {
					printTrackDebugInfo(regions[icol],current_track);
				}
			}
			else {
				bset_.reset();
				bset_table_[icol].push_back(bset_);
			}
		}
	}
	if (debug_) printEventDebugInfo(10);
	if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::storeTracks" << "\tDone storing event";

	// If we've hit the last event of the file
	if (eof()) {
		bset_table_collection_.push_back(bset_table_);
		resetBitsetTable();
		if (debug_) edm::LogInfo("PatternFile") << "@SUB=PatternFile::storeTracks" << "Bitset table " << bset_table_collection_.size()
												<< " (index=" << bset_table_collection_.size()-1 << ") has shape ("
												<< bset_table_collection_.back()[0].size() << "x" << bset_table_collection_.back().size() << ")";
	}
	nEventsProcessed++;
	full = ((nEventsMax != -1) && (nEventsMax == nEventsProcessed));
}

void PatternFile::writeObjectsToFiles() {
	unsigned int start_event(0), end_event(0);
	do {
		start_event = (ifile*nEventsPerFile);
		end_event = std::min(int((ifile+1)*nEventsPerFile-1),nEventsProcessed);
		edm::LogVerbatim("PatternFile") << "@SUB=PatternFile::writeObjectsToFiles" << "Writing " << getFileTypeString(this->getClassType()) << "PatternFile " << ifile+1 << " of "
										<< nFiles << " (events " << start_event << "-" << end_event << " of " << nEventsProcessed << ") to " << fileName << " ... ";
		assert(is_open());
		bset_table_ = bset_table_collection_[ifile];
		writeHeader();
		writeObjectsToFile();
		edm::LogVerbatim("PatternFile") << "@SUB=PatternFile::writeObjectsToFiles" << "\tDone writing file " << ifile+1;
	} while (nextFile());
}