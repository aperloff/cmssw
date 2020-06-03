#include "L1Trigger/Phase2L1ParticleFlow/interface/APxPatternFile.h"

using namespace l1tpf_impl;

bool APxPatternFile::nextFile() {
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

void APxPatternFile::readFile() {
	// Check that you're not at the end of the open file
	if (eof()) {
		message.str("");
		message << "Unable to read event " << nEventsProcessed << ".\nYou've already reached the end of the file.\n";
		throw cms::Exception("FileReadError", message.str());
	}
	if (header.empty()) {
		throw cms::Exception("FileReadError", "The header hasn't been read yet. As the file pointer will be out of position, we cannot parse the file.\n");
	}

	resetBitsetTable();
	std::string line;
	StringCollection lines;
	unsigned int trackCounter(0);
	unsigned int eventCounter(0);
	bool oddEven = false;
	l1tpf_impl::APxLinePair<tracksize,wordsize> linePair;

	// Gather all of the remaining lines in the file
	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	// Parse all of the lines
	// If all goes right, the order of the line headers should be
	//   0x0000 0x0001
	//   0x0001 0x0002
	//   0x0003 0x0004
	//   0x0004 0x0005
	//   0x0006 0x0007
	//   0x0007 0x0008
	//   0x0009 0x000a
	//   0x000a 0x000b
	//   ...
	//   0x0088 0x0089
	//   0x008a 0x008b
	//   0x008b 0x008c
	//   0x008d 0x008e
	//   0x008e 0x008f
	//   0x0090 0x0091
	//   0x0091 0x0092
	//   0x0093 0x0094
	//   ...
	//   0x03d9 0x03da
	//   0x03db 0x03dc
	//   0x03dc 0x03dd
	//   0x03de 0x03df
	//   0x03df 0x03e0
	//   0x03e1 0x03e2
	//   0x03e2 0x03e3
	//   0x03e4 0x03e5
	// assuming 95 tracks per event, 96 bits (1.5 lines) per track, and 7 events per file
	for (unsigned int iline = 0; iline < lines.size(); iline++) {
		linePair.reset();
		if ( (trackCounter > 0) && (trackCounter%2 == oddEven) ) iline++;
		linePair.setLines({lines[iline],lines[iline+1]},debug_);
		l1tpf_impl::APxLinePair<tracksize,wordsize>::LineState ls = (trackCounter%2==oddEven) ? l1tpf_impl::APxLinePair<tracksize,wordsize>::ZeroOne
																							  : l1tpf_impl::APxLinePair<tracksize,wordsize>::OneTwo;
		linePair.runAll(ls,debug_);
		for (unsigned int isector=0; isector<bset_table_.size(); isector++) {
			bset_table_[isector].push_back(linePair.objects[isector]);
		}
		trackCounter++;
		if (trackCounter == nTracksMax) {
			nEventsProcessed++;
			eventCounter++;
			trackCounter = 0;
			oddEven = !oddEven;
		}
		if (iline+2 == lines.size()) break;
	}

	bset_table_collection_.push_back(bset_table_);
	events_per_file_.push_back(eventCounter);
	resetBitsetTable();
	if (debug_) edm::LogInfo("APxPatternFile") << "@SUB=APxPatternFile::readFile()" << "Bitset table " << bset_table_collection_.size()
											   << " (index=" << bset_table_collection_.size()-1 << ") has shape ("
											   << bset_table_collection_.back()[0].size() << "x" << bset_table_collection_.back().size() << ")";
}

void APxPatternFile::writeHeader() {
	file << "#Sideband OFF\n#LinkLabel";
	for(unsigned int i=0; i<nLinks; i++) {

		if (i==0)	file << std::setw(wordchars) << "LINK_" << std::setw(2) << std::setfill('0') << std::dec << i << std::setfill(' ');
		else		file << std::setw(20) << "LINK_" << std::setw(2) << std::setfill('0') << std::dec << i << std::setfill(' ');
	}
	file << "\n#BeginData\n";
}

void APxPatternFile::writeObjectsToFile() {
	unsigned int nrows = std::ceil(bset_table_[0].size()*(float(nState)/tracksPerState));
	for (unsigned int irow = 0, itrack_pair = 0; irow < nrows; irow++) {
		state = irow%3;
		file << "0x" << std::setw(4) << std::setfill('0') << std::hex << irow << std::setfill(' ');
		for (unsigned int icol = 0; icol < bset_table_.size(); icol++) {
			if (state == 0)	{
				file << std::setw(6) << "0x" << std::setw(wordchars) << std::setfill('0') << std::hex << getTrackLSB(bset_table_[icol][itrack_pair]).to_ulong() << std::setfill(' ');
			}
			else if (state == 1) {
				if (itrack_pair+1 <= bset_table_[icol].size()-1) {
					file << std::setw(6) << "0x" << std::setw(wordchars) << std::setfill('0') << std::hex
						 << getTrackLSBPlusMSB(bset_table_[icol][itrack_pair],bset_table_[icol][itrack_pair+1]).to_ulong() << std::setfill(' ');
				}
				else {
					DynamicBitset tmp(32);
					file << std::setw(6) << "0x" << std::setw(wordchars) << std::setfill('0') << std::hex << getTrackLSBPlusMSB(bset_table_[icol][itrack_pair],tmp).to_ulong() << std::setfill(' ');
				}
			}
			else if ((state == 2) && (itrack_pair+1 <= bset_table_[icol].size()-1)) {
				file << std::setw(6) << "0x" << std::setw(wordchars) << std::setfill('0') << std::hex << getTrackMSB(bset_table_[icol][itrack_pair+1]).to_ulong() << std::setfill(' ');
			}

			if (icol == bset_table_.size()-1) file << "\n";
		}
		if (state == 2) itrack_pair+=2;
	}
}

void APxPatternFile::writeObjectsToFiles() {
	unsigned int start_event(0), end_event(0);
	do {
		start_event = (ifile*nEventsPerFile);
		end_event = std::min(int((ifile+1)*nEventsPerFile-1),nEventsProcessed);
		edm::LogVerbatim("APxPatternFile") << "@SUB=APxPatternFile::writeObjectsToFiles" << "Writing file " << ifile+1 << " of " << nFiles
										   << " (events " << start_event << "-" << end_event << " of " << nEventsProcessed << ") to " << fileName << " ... ";
		assert(is_open());
		bset_table_ = bset_table_collection_[ifile];
		writeHeader();
		writeObjectsToFile();
		edm::LogVerbatim("APxPatternFile") << "@SUB=APxPatternFile::writeObjectsToFiles" << "\tDone writing file " << ifile+1;
	} while (nextFile());
}