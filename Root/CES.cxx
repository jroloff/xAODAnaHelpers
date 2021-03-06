#include <vector>

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

#include "TRandom3.h"

#include "xAODAnaHelpers/CES.h"



using namespace std;


ClusterEnergyScale::ClusterEnergyScale(const std::string& name) : JetConstituentModifierBase(name)                                                                      
{

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("Method", m_method = "");
  myrand_global = new TRandom3(0); //for uncerts.
}


StatusCode ClusterEnergyScale::process(xAOD::CaloClusterContainer* cont) const {
	// Need to double check if this is done per-jet, per-event, etc.
  double CES_option2_factors[7]={myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1)};

	vector<double> etabins =  {0,    1.1,  1.4,  1.5,  1.8,  1.9,  5.0 };
	vector<double> alpha =    {0.05, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
	vector<double> beta =     {500,  500,  500,  0,    500,  0,    500 };

  for(xAOD::CaloCluster* cl : *cont) {
    double m_alpha = 0.05;
    double m_beta = 500;
		double eta = cl->eta();
		double E = cl->e();

		int etabin = -1;
		for(int i=0; i<etabins.size(); i++){
			if(fabs(eta) < etabins[i]){
				etabin = i;
			}
		}

    double CESfactor = alpha[etabin]*(1.+beta[etabin]/E);

    double e_ces =  E*(1.+CES_option2_factors[etabin]*CESfactor);
    double e_cesu = E*(1.+CESfactor);
    double e_cesd = E*(1.-CESfactor);

	  if(strcmp("Up", m_method.c_str()) == 1) cl->setE( E * (1. + CESfactor) );
		else if(strcmp("Down", m_method.c_str()) == 1) cl->setE( E * (1. - CESfactor) );
		else cl->setE( E * (1. + CESfactor * CES_option2_factors[etabin]) );
  }
  return StatusCode::SUCCESS;
}

StatusCode ClusterEnergyScale::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}






