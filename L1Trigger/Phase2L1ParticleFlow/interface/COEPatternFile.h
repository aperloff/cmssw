#ifndef L1Trigger_Phase2L1ParticleFlow_CoePatternFile_h
#define L1Trigger_Phase2L1ParticleFlow_CoePatternFile_h

// user include files
#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

namespace l1tpf_impl {

	class COEPatternFile: public PatternFile {

		protected:
			// Constants
			static constexpr l1tpf_impl::PatternFile::PatternFileType classType = l1tpf_impl::PatternFile::PatternFileType::COE;
			static constexpr unsigned int headerSize = 10;

		public:
			COEPatternFile(const edm::ParameterSet& iConfig, std::ios_base::openmode openMode) : PatternFile(iConfig,openMode,classType) {}
			~COEPatternFile() {}

			PatternFileType getClassType() { return classType; }
			unsigned int	getHeaderLines() { return headerSize; }
			void			readFile();
			void			writeHeader();
			void			writeObjectsToFile();
			void			writeObjectsToFiles();
	};
} // namespace

#endif
