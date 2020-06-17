#include "L1Trigger/Phase2L1ParticleFlow/interface/COEFile.h"

using namespace l1tpf_impl;

void COEFile::readFile() {
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
	bset_.resize(tracksize);
	bset_string_.resize(tracksize);
	std::string endline;
	endline.resize(2);
	unsigned int lineCounter(headerSize);

	while (!eof()) {
		for (unsigned int isector=0; isector < bset_table_.size(); isector++) {
			file.read(&bset_string_[0],tracksize);
			bset_.reset();
			bset_ = boost::dynamic_bitset<> (bset_string_);

			// Save the bitset for later
			bset_table_[isector].push_back(bset_);
		}
		// Remove the trailing characters
		file.read(&endline[0],2);

		if (endline!=",\n" && endline!=";\n") {
			message.str("");
			message << "Something went wrong reading line " << lineCounter << " of the COE file.\n"
					<< "\tThe line should have ended with \',<newline>\' or \';<newline>\', but instead ended with \'" << endline << "\'\n";
			throw cms::Exception("FileReadError", message.str());
		}

		lineCounter++;
		if (lineCounter%nTracksMax == 0) nEventsProcessed++;
	}

	bset_table_collection_.push_back(bset_table_);
	events_per_file_.push_back(nEventsProcessed);
	resetBitsetTable();
	if (debug_) edm::LogInfo("COEFile") << "@SUB=COEFile::readFile" << "Bitset table " << bset_table_collection_.size()
										<< " (index=" << bset_table_collection_.size()-1 << ") has shape ("
										<< bset_table_collection_.back()[0].size() << "x" << bset_table_collection_.back().size() << ")";
}

void COEFile::writeHeader() {
	file << "; Sample memory initialization file for Dual Port Block Memory,\n"
		 << "; v3.0 or later.\n"
		 << "; Board: VCU118\n"
		 << "; tmux: 1\n"
		 << ";\n"
		 << "; This .COE file specifies the contents for a block memory\n"
		 << "; of depth=" << nTracksMax << ", and width=" << tracksize*nLinks << ". In this case, values are specified\n"
		 << "; in binary format.\n"
		 << "memory_initialization_radix=2;\n"
		 << "memory_initialization_vector=\n";
}

void COEFile::writeObjectsToFile() {
	// Right now the all events are written to a single COE file
	writeHeader();
	bset_table_ = bset_table_collection_[0];

	unsigned int nrows = bset_table_[0].size();
	for (unsigned int irow = 0; irow < nrows; irow++) {
		for (unsigned int icol = 0; icol < bset_table_.size(); icol++) {
			boost::to_string(bset_table_[icol][irow],bset_string_);
			file << bset_string_.c_str();
		}
		if (eof() && irow+1==nrows) file << ";\n";
		else                        file << ",\n";

		if ( (irow!=0) && (irow%nTracksMax==0)) nEventsProcessed++;
	}
}
