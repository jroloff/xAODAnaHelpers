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

#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/Algorithm.h"
#include <xAODAnaHelpers/tools/ReturnCheck.h>
#include "xAODAnaHelpers/ClusterUncertaintyTool.h"
#include "xAODAnaHelpers/CE.h"
#include "xAODAnaHelpers/CES.h"
#include "xAODAnaHelpers/CAR.h"



#include <sstream>
#include <string>
#include "TString.h"

using namespace std;


typedef IJetFromPseudojet::NameList NameList;

ClassImp(ClusterUncertaintyTool)


ClusterUncertaintyTool::ClusterUncertaintyTool():
	xAH::Algorithm("ClusterUncertaintyTool")//,
	//m_clust(0)
{
	m_name = "ClusterUncertaintyTool";
	m_inContainerName = "";
 	m_outContainerName = m_name;

}

EL::StatusCode ClusterUncertaintyTool :: setupJob (EL::Job& job){
  Info("setupJob()", "Calling setupJob");

  job.useXAOD ();
  xAOD::Init("ClusterUncertaintyTool").ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode ClusterUncertaintyTool :: histInitialize (){
  RETURN_CHECK("xAH::Algorithm::algInitialize()", xAH::Algorithm::algInitialize(), "");
  return EL::StatusCode::SUCCESS;
}




EL::StatusCode ClusterUncertaintyTool :: fileExecute (){
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClusterUncertaintyTool :: changeInput (bool /*firstFile*/){
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ClusterUncertaintyTool::initialize(){
  m_event = wk()->xaodEvent();
	m_store = wk()->xaodStore();
	//TString clusterColl = "CaloCalTopoClusters";
	std::string clusterColl = "CaloCalTopoClusters";

  ClusterEnergyScale* cesUp = new ClusterEnergyScale("cesUp");
	cesUp->setProperty("Method", "Up");
  ToolHandle<IJetConstituentModifier> iCESUp(cesUp);
  hclusts_cesUp.push_back(iCESUp);

  ClusterEnergyScale* cesDown = new ClusterEnergyScale("cesDown");
	cesUp->setProperty("Method", "Down");
  ToolHandle<IJetConstituentModifier> iCESDown(cesDown);
  hclusts_cesDown.push_back(iCESDown);

  ClusterEnergyScale* ces = new ClusterEnergyScale("ces");
	cesUp->setProperty("Method", "Other");
  ToolHandle<IJetConstituentModifier> iCES(ces);
  hclusts_ces.push_back(iCES);

  ClusterEfficiency* ce = new ClusterEfficiency("ce");
  ToolHandle<IJetConstituentModifier> iCE(ce);
  hclusts_ce.push_back(iCE);

  ClusterAngularResolution* car = new ClusterAngularResolution("car");
  ToolHandle<IJetConstituentModifier> iCAR(car);
  hclusts_car.push_back(iCAR);

  cesUpModSeq = new JetConstituentModSequence( "CesUpModifSequence");
  cesUpModSeq->setProperty("InputContainer", clusterColl);
  cesUpModSeq->setProperty("OutputContainer", ("CESUp_"+clusterColl));
  cesUpModSeq->setProperty("InputType", "CaloCluster");
  cesUpModSeq->setProperty("Modifiers", hclusts_cesUp );
  cesUpModSeq->initialize();
  ToolHandle<IJetExecuteTool> iclustModTool1(cesUpModSeq);

  cesDownModSeq = new JetConstituentModSequence( "CesDwonModifSequence");
  cesDownModSeq->setProperty("InputContainer", clusterColl);
  cesDownModSeq->setProperty("OutputContainer", ("CESDown_"+clusterColl));
  cesDownModSeq->setProperty("Method", "Down");
  cesDownModSeq->setProperty("Modifiers", hclusts_cesDown );
  cesDownModSeq->initialize();
  ToolHandle<IJetExecuteTool> iclustModTool2(cesDownModSeq);

  carModSeq = new JetConstituentModSequence( "CarModifSequence");
  carModSeq->setProperty("InputContainer", clusterColl);
  carModSeq->setProperty("OutputContainer", ("CAR_"+clusterColl));
  carModSeq->setProperty("InputType", "CaloCluster");
  carModSeq->setProperty("Modifiers", hclusts_car );
  carModSeq->initialize();
  ToolHandle<IJetExecuteTool> iclustModTool3(carModSeq);

  ceModSeq = new JetConstituentModSequence( "CeModifSequence");
  ceModSeq->setProperty("InputContainer", clusterColl);
  ceModSeq->setProperty("OutputContainer", ("CE_"+clusterColl));
  ceModSeq->setProperty("InputType", "CaloCluster");
  ceModSeq->setProperty("Modifiers", hclusts_ce );
  ceModSeq->initialize();
  ToolHandle<IJetExecuteTool> iclustModTool4(ceModSeq);

/*
	cesJets = JetMakerTool("myJetMaker2");
                          "JetRadius": 1.0,
                          "InputClusters": "CESUp_CaloCalTopoClusters",
                          "OutputContainer": "MyAntiKt10LCTopoCESJets"

*/
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClusterUncertaintyTool::execute() {
 cesUpModSeq->execute();
 cesDownModSeq->execute();
 carModSeq->execute();
 ceModSeq->execute();

  return EL::StatusCode::SUCCESS;
}




EL::StatusCode ClusterUncertaintyTool :: postExecute (){
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode ClusterUncertaintyTool :: finalize (){
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode ClusterUncertaintyTool :: histFinalize (){
  return EL::StatusCode::SUCCESS;
}






