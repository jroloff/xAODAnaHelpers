#ifndef xAODAnaHelpers_JetMaker_H
#define xAODAnaHelpers_JetMaker_H

// making it more like a tool
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "JetRec/JetFromPseudojet.h"

//#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "JetInterface/IJetExecuteTool.h"

#include <EventLoop/Algorithm.h>
#include "JetRec/JetRecTool.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetToolRunner.h"
#include "JetRec/PseudoJetGetter.h"
#include "xAODRootAccess/Init.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetSplitter.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/JetDumper.h"
#include "JetRec/JetToolRunner.h"


// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

#include <map>
#include <memory>
#include <vector>

using namespace std;



class JetMaker : public xAH::Algorithm{
	public:

  std::string m_inContainerName;
	std::string m_outContainerName;

  std::string m_jetAlgo;
	std::string m_outputAlgo;
 	bool m_setAFII;
  std::string m_calibConfigFullSim;
	std::string m_calibConfigAFII;
  std::string m_calibConfigData;
  std::string m_calibConfig;
	std::string m_calibSequence;
	double m_zcut; 
	double m_ZCut; 
	double m_beta; 
	double m_Beta; 
	double m_jetR; 
	fastjet::JetAlgorithm m_jetalg; //!
  xAOD::TEvent *m_event;  //!
  xAOD::TStore *m_store;  //!

  ToolHandleArray<IPseudoJetGetter> hgets; //!
  ToolHandleArray<IJetExecuteTool> hrecs; //!
  //PseudoJetGetter* plcget; //!
  JetToolRunner * jrun; //!

public:
  // Tree *myTree; //!
  // TH1 *myHist; //!
  bool SkipNegativeEnergy; //!
  TString Label; //!
	float JetRadius = 1.0; 
  float PtMin; //!
  TString JetAlgorithm; //!
  TString InputType; //!
  TString InputLabel; //!
  TString InputClusters = "CaloCalTopoCluster"; 
  TString OutputContainer = "MyAntiKt10Jets"; 
  TString m_PseudoJetGetter; //!
  TString ClusterOutputContainer; //!

  private:

		vector<fastjet::PseudoJet> m_clusters;//!
	  const xAOD::CaloClusterContainer *m_clust;//!
	  const xAOD::JetContainer *m_jets;//!
//		JetFromPseudojet m_bld;//!




  /* Properties */
    // input jet container to use as a constituent proxy
    std::string m_inputClusterContainer;
    // output jet container to store reclustered jets
    std::string m_outputJetContainer;

		double m_p;//!
		double m_mu;//!

  int m_numEvent;         //!
  int m_numObject;        //!

  bool m_isMC;            //!
	bool m_isFullSim; //!
	JetRecTool* pjrf;//!
  JetFromPseudojet* pbuild;//!
  JetFinder* pfind;//!
  PseudoJetGetter* plcget;//!
		
	public:
	JetMaker ();
	//JetMaker (std::string className = "JetMaker", fastjet::JetAlgorithm fj_jetalg = fastjet::cambridge_algorithm, float jet_radius = 1.2, double beta = 1., double zcut = 0.1);

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
	virtual EL::StatusCode histFinalize ();


	ClassDef(JetMaker, 1);
};

#endif
