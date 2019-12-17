#ifndef L1Trigger_Phase2L1ParticleFlow_APxPatternFile_h
#define L1Trigger_Phase2L1ParticleFlow_APxPatternFile_h

// user include files
#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

namespace l1tpf_impl {

	class APxPatternFile: public PatternFile {

		protected:
			// Constants
			static constexpr unsigned int nstate = 3;
			static constexpr unsigned int tracks_per_state = 2;
			static constexpr unsigned int header_size = 3;

		public:
			APxPatternFile(const edm::ParameterSet& iConfig, std::ios_base::openmode openMode) : PatternFile(iConfig,openMode,l1tpf_impl::PatternFile::PatternFileType::APx), ifile(0) {
				bset_table_.resize(phiSlices*etaRegions);
			}
			~APxPatternFile() {}

			bool			eof() { return (nEventsProcessed+1 == nEventsMax) || ((nEventsProcessed+1)%nEventsPerFile==0); }
			unsigned int	getHeaderLines() { return header_size; }
			bool			nextFile();
			void			printDebugInfo(int nrow = -1);
			bool			readFile() { return true; }
			void			resetBitsetTable() { bset_table_.clear(); bset_table_.resize(phiSlices*etaRegions); }
			void			storeTracks(const std::vector<Region>& regions);
			void			writeHeader();
			void			writeTracksToFile();
			void			writeTracksToFiles();

		private:
			unsigned int ifile;
	};
} // namespace

#endif