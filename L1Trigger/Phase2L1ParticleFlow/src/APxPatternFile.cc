#include "L1Trigger/Phase2L1ParticleFlow/interface/APxPatternFile.h"

using namespace l1tpf_impl;

bool APxPatternFile::nextFile() {
	if (ifile == nFiles-1) return false;
	else {
		close();
		ifile++;
		fileName = fileNameBase + "_" + std::to_string(ifile) + fileExtension;
		open();
	}
	return true;
}

void APxPatternFile::printDebugInfo(int nrow) {
	message.str("");
	if (nrow == -1) nrow = bset_table_[0].size();
	for (int irow = 0; irow < nrow; irow++) {
		message << "Row " << irow;
		for (unsigned int icol = 0; icol < bset_table_.size(); icol++) {
			boost::to_string(bset_table_[icol][irow],bset_string_);
			message << "\t" << bset_string_;
		}
		message << std::endl;
	}
	message << std::endl << std::endl;
	edm::LogInfo("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::printDebugInfo" << message.str();
}

// Store the tracks for a given events
// In most cases, this should happen once per event, so keep track of:
//   1. The number of events processed
//   2. If we've hit the total number of events to process
//   3. If we've hit the end of a file, we need to put the current
//       table into a collection and reset the working table
void APxPatternFile::storeTracks(const std::vector<Region>& regions) {
	if (debug_) edm::LogInfo("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::storeTracks" << "Storing event " << nEventsProcessed << " ... ";
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
					l1tpf_impl::PatternFile::printDebugInfo(regions[icol],current_track);
				}
			}
			else {
				bset_.reset();
				bset_table_[icol].push_back(bset_);
			}
		}
	}
	if (debug_) printDebugInfo(10);
	if (debug_) edm::LogInfo("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::storeTracks" << "\tDone storing event";

	// If we've hit the last event of the file
	if (eof()) {
		bset_table_collection_.push_back(bset_table_);
		resetBitsetTable();
		if (debug_) edm::LogInfo("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::storeTracks" << "Bitset table " << bset_table_collection_.size()
																 << " (index=" << bset_table_collection_.size()-1 << ") has shape ("
																 << bset_table_collection_.back()[0].size() << "x" << bset_table_collection_.back().size() << ")";
	}
	nEventsProcessed++;
	full = ((nEventsMax != -1) && (nEventsMax == nEventsProcessed));
}

void APxPatternFile::writeHeader() {
	fprintf(file,"#Sideband OFF\n");
	fprintf(file,"#LinkLabel");
	for(unsigned int i=0; i<phiSlices*etaRegions; i++) {
		if (i==0)	fprintf(file,"%16.16s%02u","LINK_",i);
		else		fprintf(file,"%20.20s%02u","LINK_",i);
	}
	fprintf(file,"\n#BeginData\n");
}

void APxPatternFile::writeTracksToFile() {
	unsigned int nrows = std::ceil(bset_table_[0].size()*(float(nstate)/tracks_per_state));
	for (unsigned int irow = 0, itrack_pair = 0; irow < nrows; irow++) {
		int state = irow%3;
		fprintf(file, "0x%4.4x", irow);
		for (unsigned int icol = 0; icol < bset_table_.size(); icol++) {
			if (state == 0)	{
				fprintf(file, "%4.4s0x%16.16lx", "", getTrackLSB(bset_table_[icol][itrack_pair]).to_ulong());
			}
			else if (state == 1) {
				if (itrack_pair+1 <= bset_table_[icol].size()-1) {
					fprintf(file, "%4.4s0x%16.16lx", "", getTrackLSBPlusMSB(bset_table_[icol][itrack_pair],bset_table_[icol][itrack_pair+1]).to_ulong());
				}
				else {
					DynamicBitset tmp(32);
					fprintf(file, "%4.4s0x%16.16lx", "", getTrackLSBPlusMSB(bset_table_[icol][itrack_pair],tmp).to_ulong());
				}
			}
			else if ((state == 2) && (itrack_pair+1 <= bset_table_[icol].size()-1)) {
				fprintf(file, "%4.4s0x%16.16lx", "", getTrackMSB(bset_table_[icol][itrack_pair+1]).to_ulong());
			}

			if (icol == bset_table_.size()-1) fprintf(file, "\n");
		}
		if (state == 2) itrack_pair+=2;
	}
}

void APxPatternFile::writeTracksToFiles() {
	unsigned int start_event(0), end_event(0);
	do {
		start_event = (ifile*nEventsPerFile);
		end_event = std::min(int((ifile+1)*nEventsPerFile-1),nEventsMax-1);
		edm::LogVerbatim("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::writeTracksToFiles" << "Writing file " << ifile+1 << " of " << nFiles
														 << " (events " << start_event << "-" << end_event << " of " << nEventsMax << ") to " << fileName << " ... ";
		assert(is_open());
		bset_table_ = bset_table_collection_[ifile];
		writeHeader();
		writeTracksToFile();
		edm::LogVerbatim("L1TPFProducer|APxPatternFile") << "@SUB=APxPatternFile::writeTracksToFiles" << "\tDone writing file " << ifile+1;
	} while (nextFile());
}