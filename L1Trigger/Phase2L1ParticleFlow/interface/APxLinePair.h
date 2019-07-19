#ifndef L1Trigger_Phase2L1ParticleFlow_APxLinePair_h
#define L1Trigger_Phase2L1ParticleFlow_APxLinePair_h

// STL include files
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

// external package include files
#include <boost/dynamic_bitset.hpp>
#include <boost/multiprecision/cpp_int.hpp>

// user include files
#include "FWCore/Utilities/interface/Exception.h"

namespace l1tpf_impl {

	template <unsigned int objectSize, unsigned int wordSize>
	class APxLinePair {
		public:
			// Enums and Typedefs
			enum LineState {ZeroOne=0,OneTwo=1};
			typedef boost::dynamic_bitset<> DynamicBitset;
			typedef std::vector<DynamicBitset> BitsetCollection;
			typedef std::vector<std::string> StringCollection;
			typedef std::vector<StringCollection> StringTable;

			// Constants
			static constexpr unsigned int bitsToNibbles = 4;

			void addLine(std::string line, bool debug = false) {
				if (lines.size() >= 2 ) {
					throw cms::Exception("LogicError", "Too many lines! Did someone forget to reset the APxLinePair?\n");
				}
				lines.push_back(line);
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::addLine" << "addLine:\nAdding line: " << line << "\n";
			}

			const char* hex_char_to_bin(char c) {
				switch(toupper(c)) {
					case '0': return "0000";
					case '1': return "0001";
					case '2': return "0010";
					case '3': return "0011";
					case '4': return "0100";
					case '5': return "0101";
					case '6': return "0110";
					case '7': return "0111";
					case '8': return "1000";
					case '9': return "1001";
					case 'A': return "1010";
					case 'B': return "1011";
					case 'C': return "1100";
					case 'D': return "1101";
					case 'E': return "1110";
					case 'F': return "1111";
				}
				sstream.str("");
				sstream <<  "Unknown character, \'" << c << "\', to convert from hex to binary.\n";
				throw cms::Exception("LogicError", sstream.str());
				return "";
			}

			std::string hex_str_to_bin_str(const std::string& hex) {
				std::string bin;
				for (char c : hex) {
					bin += hex_char_to_bin(c);
				}
				return bin;
			}

			void mergeLines(LineState lineState, bool debug = false) {
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::mergeLines" << "mergeLines:";
				int halfSizeHex = (wordSize/bitsToNibbles)/2;
				objectStrings.resize(wordStrings.size());
				for (unsigned int iword = 0; iword < links; iword++) {
					if (wordStrings[iword].size() != 2) {
						sstream.str("");
						sstream << "There are " << wordStrings[iword].size() << " strings for the current word when there should be exactly 2.";
						throw cms::Exception("SizeError",sstream.str());
					}
					if (lineState == ZeroOne) {
						objectStrings[iword] = wordStrings[iword][1].substr(halfSizeHex,halfSizeHex)+wordStrings[iword][0];
					}
					else if (lineState == OneTwo) {
						objectStrings[iword] = wordStrings[iword][1]+wordStrings[iword][0].substr(0,halfSizeHex);
					}
					else {
						throw cms::Exception("LogicError","Unknown state for mergeLines.\n");
					}
					if (debug) {
						edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::mergeLines" << "Link " << iword <<  ": " << objectStrings[iword];
					}
				}
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::mergeLines";
			}

			void parseLines(bool debug = false) {
				for (unsigned int iline = 0; iline < lines.size(); iline++) {
					std::string line = lines[iline];
					if (debug) {
						edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::parseLines" << "Working on line: " << line << "\n";
					}
					int iword(0);
					line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
					sstream.clear();
					sstream.str(line);
					while (!sstream.eof()) {
						sstream >> currentWord;
						if (iword == 0) {
							headers.push_back(currentWord);
						}
						else {
							if ( (iline == 0) && (iword > 0) )	links++;
							if (links-1 == wordStrings.size()) wordStrings.resize(links);
							wordStrings[iword-1].push_back(currentWord);
						}
						iword++;
					}
				}
				if (debug) {
					sstream.clear();
					sstream.str("");
					sstream << "parseLines:\n" << "Row headers:";
					for (auto line : headers) {
						sstream << " " << line;
					}
					sstream << "\n";
					for (auto link : wordStrings) {
						sstream << "Link:";
						for (auto word : link) {
							sstream << " " << word;
						}
						sstream << "\n";
					}
					edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::parseLines" << sstream.str();
				}
			}

			void remove0x() {
				for (auto &line : wordStrings) {
					for (auto &word : line) {
						word = word.substr(2,word.size()-2);
					}
				}
			}

			void reset() {
				links = 0;
				sstream.str("");
				currentWord = "";
				lines.clear();
				headers.clear();
				objectStrings.clear();
				wordStrings.clear();
				objects.clear();
			}

			void runAll(LineState lineState, bool debug = false) {
				parseLines(debug);
				remove0x();
				mergeLines(lineState, debug);
				stringsToBitsets(debug);
			}

			void setLines(std::vector<std::string> lines_, bool debug = false) {
				if (lines_.size() != 2 ) {
					throw cms::Exception("LogicError", "The parse was designed to handle two lines at a time!\n");
				}
				lines = lines_;
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::setLines" << "setLines:\nSetting the lines: \n\t" << lines_[0] << "\n\t" << lines_[1] << "\n";
			}

			void stringsToBitsets(bool debug = false) {
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::stringsToBitsets" << "stringsToBitsets:";
				objects.resize(objectStrings.size());
				std::string objectStringBinary;
				for (unsigned int iobject = 0; iobject < links; iobject++) {
					objectStringBinary = hex_str_to_bin_str(objectStrings[iobject]);
					objects[iobject] = boost::dynamic_bitset<>(objectStringBinary);
					if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::stringsToBitsets" << "Object " << iobject << ": " << objects[iobject];
				}
				if (debug) edm::LogVerbatim("APxLinePair") << "@SUB=APxLinePair::mergeLines";
			}

			unsigned int links = 0;
			std::stringstream sstream;
			std::string currentWord;
			StringCollection lines, headers, objectStrings;
			StringTable wordStrings;
			BitsetCollection objects;
	};
} // namespace

#endif