#include "L1Trigger/Phase2L1ParticleFlow/interface/COEFile.h"

using namespace l1tpf_impl;

bool COEFile::readFile() {
	return true;
}

void COEFile::writeHeader() {
	file << "; Sample memory initialization file for Dual Port Block Memory,\n"
		 << "; v3.0 or later.\n"
		 << "; Board: VCU118\n"
		 << "; tmux: 1\n"
		 << ";\n"
		 << "; This .COE file specifies the contents for a block memory\n"
		 << "; of depth=" << nTracksMax << ", and width=" << tracksize*phiSlices*etaRegions << ". In this case, values are specified\n"
		 << "; in binary format.\n"
		 << "memory_initialization_radix=2;\n"
		 << "memory_initialization_vector=\n";
}

void COEFile::writeTracksToFile(const std::vector<Region>& regions, bool print) {
	PropagatedTrack current_track;
	bool has_track = false;
	for (unsigned int irow = 0; irow < nTracksMax; irow++) {
		for (unsigned int icol = 0; icol < regions.size(); icol++) {
			if (regions[icol].track.size()<=irow) has_track = false;
			else has_track = true;

			if (has_track) {
				// select the track that will be converted to a bit string
				current_track = regions[icol].track[irow];

				// convert the values in a PropogatedTrack to a 96-bit track word
				assignTrackToBitset(current_track);

				// print the track word to the COE file
				boost::to_string(bset_,bset_string_);
				file << bset_string_.c_str();

				// print some debugging information
				if (debug_ && print && irow==0 && icol==0) {
					l1tpf_impl::PatternFile::printDebugInfo(regions[icol],current_track);
				}
			}
			else {
				bset_.reset();
				boost::to_string(bset_,bset_string_);
				file << bset_string_.c_str();
			}
		}
		if ( (irow==nTracksMax-1) && (nEventsProcessed+1==nEventsMax) )	file << ";\n";
		else															file << ",\n";
	}
	nEventsProcessed++;
	full = ((nEventsMax!=-1) && (nEventsMax==nEventsProcessed));
}

