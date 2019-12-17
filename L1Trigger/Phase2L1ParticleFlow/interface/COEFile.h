#ifndef L1Trigger_Phase2L1ParticleFlow_CoeFile_h
#define L1Trigger_Phase2L1ParticleFlow_CoeFile_h

// user include files
#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

namespace l1tpf_impl {

	class COEFile: public PatternFile {

		protected:
			// Constants
			static constexpr unsigned int header_size = 10;

		public:
			COEFile(const edm::ParameterSet& iConfig, std::ios_base::openmode openMode) : PatternFile(iConfig,openMode,l1tpf_impl::PatternFile::PatternFileType::COE) {}
			~COEFile() {}

			//bool			eof() {}
			unsigned int	getHeaderLines() { return header_size; }
			bool			readFile();
			void			writeHeader();
			void			writeTracksToFile(const std::vector<Region>& regions, bool print = false);
	};
} // namespace

#endif