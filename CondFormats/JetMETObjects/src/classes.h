#include "CondFormats/JetMETObjects/src/headers.h"

#include <vector>

JetCorrectorParameters corr;
JetCorrectorParameters::Definitions def;
JetCorrectorParameters::Record record;
std::vector<JetCorrectorParameters> corrv;
std::vector<JetCorrectorParameters::Record> recordv;
JetCorrectorParametersCollection coll;
JetCorrectorParametersCollection::pair_type pair_type;
JetCorrectorParametersCollection::collection_type colltype;
std::vector<JetCorrectorParametersCollection> collv;
JetCorrectionUncertainty corr_jcu;
std::vector<JetCorrectionUncertainty> corrv_jcu;
JetCorrectionUncertaintyCollection coll_jcu;
JetCorrectionUncertaintyCollection::pair_type pair_type_jcu;
JetCorrectionUncertaintyCollection::value_type value_type_jcu;
JetCorrectionUncertaintyCollection::label_type label_type_jcu;
JetCorrectionUncertaintyCollection::collection_type collection_type_jcu;
FFTJetCorrectorParameters fftcorr;
QGLikelihoodCategory qgcat;
QGLikelihoodObject qgobj;
QGLikelihoodObject::Entry qgentry;
std::vector< QGLikelihoodObject::Entry > qgentryv;
QGLikelihoodSystematicsObject qgsystobj;
QGLikelihoodSystematicsObject::Entry qgsystentry;
std::vector< QGLikelihoodSystematicsObject::Entry > qgsystentryv;
METCorrectorParameters METcorr;
MEtXYcorrectParameters MEtXYcorr;
JME::JetResolutionObject jerobj;
JME::JetResolutionObject::Definition jerdef;
JME::JetResolutionObject::Record jerrecord;
JME::JetResolutionObject::Range jerrange;
std::vector<JME::JetResolutionObject::Record> jerrecordvec;
std::vector<JME::JetResolutionObject::Range> jerrangevec;
