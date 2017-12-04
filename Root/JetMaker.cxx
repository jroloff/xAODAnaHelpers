#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODEventInfo/EventInfo.h"


#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetSplitter.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/JetDumper.h"
#include "JetRec/JetToolRunner.h"

#include "AsgTools/Check.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "xAODJet/Jet.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/JetConstituentFiller.h"
#include "JetEDM/FastJetLink.h"
#include "xAODJet/Jet_PseudoJet.icc"
#include "JetRec/JetFromPseudojet.h"
#include "JetEDM/IndexedConstituentUserInfo.h"
#include "JetEDM/LabelIndex.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "AthLinks/ElementLink.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/RectangularGrid.hh"

#include "fastjet/contrib/SoftDrop.hh"
#include "xAODAnaHelpers/HelperFunctions.h"
#include <xAODAnaHelpers/tools/ReturnCheck.h>
#include "xAODAnaHelpers/JetMaker.h"


#include <sstream>
#include <string>

using namespace std;


typedef IJetFromPseudojet::NameList NameList;

ClassImp(JetMaker)


JetMaker::JetMaker():
	Algorithm("JetMaker"),
	m_clust(0)
{
	m_name = "JetMaker";
	m_jetalg = fastjet::cambridge_algorithm;	
	m_jetR = 1.0;	
	m_inContainerName = "";
 	m_outContainerName = m_name;
  //InputClusters = "CaloCalTopoClusters";
  //OutputContainer = "AntiKt10LCTopoJets";

}

EL::StatusCode JetMaker :: setupJob (EL::Job& job){
  Info("setupJob()", "Calling setupJob");

  job.useXAOD ();
  xAOD::Init("JetMaker").ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetMaker :: histInitialize (){
  RETURN_CHECK("xAH::Algorithm::algInitialize()", xAH::Algorithm::algInitialize(), "");
  return EL::StatusCode::SUCCESS;
}




EL::StatusCode JetMaker :: fileExecute (){
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetMaker :: changeInput (bool /*firstFile*/){
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode JetMaker::initialize(){
  m_event = wk()->xaodEvent();
	m_store = wk()->xaodStore();
 const xAOD::EventInfo* eventInfo(nullptr);
  RETURN_CHECK("JetCalibrator::execute()", HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, m_verbose) ,"");
	m_isMC = ( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );


  SkipNegativeEnergy = true;
  Label = "LCTopo";
  //JetRadius = m_jetR;
  JetAlgorithm = "AntiKt";
  PtMin =  0.1;
  InputLabel = "LCTopo";
  m_PseudoJetGetter = "lcget";
	TString m_outcont = OutputContainer;
	m_outcont += "PJ";

	//std::cout << JetRadius << std::endl;

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  //PseudoJetGetter* plcget = new PseudoJetGetter("m_pjgetter");
  plcget = new PseudoJetGetter("m_pjgetter");
  EL_RETURN_CHECK("initialize()", plcget->setProperty("InputContainer", (const char *) InputClusters) );
  EL_RETURN_CHECK("initialize()", plcget->setProperty("OutputContainer", (const char *) m_outcont ) );
  //EL_RETURN_CHECK("initialize()", plcget->setProperty("Label", "LCTopo" ));
  EL_RETURN_CHECK("initialize()", plcget->setProperty("Label", (const char *) InputLabel) );
  EL_RETURN_CHECK("initialize()", plcget->setProperty("SkipNegativeEnergy", true) );
  EL_RETURN_CHECK("initialize()", plcget->setProperty("GhostScale", 0.0) );
  EL_RETURN_CHECK("initialize()", plcget->initialize() );
  ToolHandle<IPseudoJetGetter> hlcget(plcget);
  hgets.push_back(hlcget);

  //check jetrec.cxx for jvt and many more attributes to be added
  //cout << "Creating jet builder." << endl;
  //JetFromPseudojet* pbuild = new JetFromPseudojet("jetbuild");
  pbuild = new JetFromPseudojet("jetbuild");
  ToolHandle<IJetFromPseudojet> hbuild(pbuild);
  vector<string> jetbuildatts;
  jetbuildatts.push_back("ActiveArea");
  jetbuildatts.push_back("ActiveAreaFourVector");
  EL_RETURN_CHECK("initialize()", pbuild->setProperty("Attributes", jetbuildatts) );
  EL_RETURN_CHECK("initialize()", pbuild->initialize() );

  //cout << "Creating jet finder." << endl;
  //JetFinder* pfind = new JetFinder("jetfind");
  pfind = new JetFinder("jetfind");
  EL_RETURN_CHECK("initialize()", pfind->setProperty("JetAlgorithm", (const char *) JetAlgorithm) );
  EL_RETURN_CHECK("initialize()", pfind->setProperty("JetRadius", JetRadius));
  EL_RETURN_CHECK("initialize()", pfind->setProperty("PtMin", PtMin));
  EL_RETURN_CHECK("initialize()", pfind->setProperty("GhostArea", 0.01));
  EL_RETURN_CHECK("initialize()", pfind->setProperty("RandomOption", 1));
  EL_RETURN_CHECK("initialize()", pfind->setProperty("JetBuilder", hbuild));
  ToolHandle<IJetFinder> hfind(pfind);

  EL_RETURN_CHECK("initialize()",pfind->initialize() );
  //cout << "Creating jetrec tool." << endl;
  //JetRecTool* pjrf = new JetRecTool("jrfind");
  pjrf = new JetRecTool("jrfind");
  EL_RETURN_CHECK("initialize()", pjrf->setProperty("OutputContainer", (const char *) OutputContainer));
  EL_RETURN_CHECK("initialize()", pjrf->setProperty("PseudoJetGetters", hgets));
  EL_RETURN_CHECK("initialize()", pjrf->setProperty("JetFinder", hfind));
  EL_RETURN_CHECK("initialize()", pjrf->initialize());
  //ToolHandle<IJetExecuteTool> hjrf(pjrf);
  hrecs.push_back(pjrf);
  //vector<string> fvmoms;

  //fvmoms.push_back("jetP4()");
  //fvmoms.push_back("JetConstitScaleMomentum");
  //vector<string> fmoms;
  //fmoms.push_back("ActiveArea");

  //ToolHandleArray<IJetExecuteTool> hjettools;
  //hjettools.push_back(pjrf);

  jrun = new JetToolRunner("jetrunner");
  EL_RETURN_CHECK("initialize()", jrun->setProperty("Tools", hrecs) );
  //cout << "Initializing tools." << endl;
  EL_RETURN_CHECK("initialize()", jrun->initialize() );
  jrun->print();
/*
    const std::string stringMeta = wk()->metaData()->castString("SimulationFlavour");
    if ( m_setAFII ) {
      Info("initialize()", "Setting simulation flavour to AFII");
      m_isFullSim = false;
    }else if ( stringMeta.empty() ) {
      Warning("initialize()", "Could not access simulation flavour from EL::Worker. Treating MC as FullSim by default!" );
    } else {
      m_isFullSim = (stringMeta == "AFII") ? false : true;
    }
    if ( !m_isFullSim ) {
      m_calibConfig = m_calibConfigAFII;
		}
*/


	//m_bld = JetFromPseudojet("m_jetBuilder");


  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetMaker::execute() {
  if ( m_debug ) { Info("execute()", "Applying SoftDrop... "); }
	//std::cout << "Input clusters: " << InputClusters << "\t" << "Output container: " << OutputContainer << std::endl;
	//plcget->execute();
	//std::cout << __LINE__ << std::endl;
  //pbuild->execute();
	//std::cout << __LINE__ << std::endl;
	m_numEvent++;
  jrun->execute();

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetMaker :: postExecute (){
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetMaker :: finalize (){
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetMaker :: histFinalize (){

  Info("histFinalize()", "Calling histFinalize");
  RETURN_CHECK("xAH::Algorithm::algFinalize()", xAH::Algorithm::algFinalize(), "");
  return EL::StatusCode::SUCCESS;
}






