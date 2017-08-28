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

#include "xAODAnaHelpers/CAR.h"



using namespace std;


ClusterAngularResolution::ClusterAngularResolution(const std::string& name) : JetConstituentModifierBase(name)
                                                                      
{

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif


  myrand_global = new TRandom3(0); //for uncerts.
}


StatusCode ClusterAngularResolution::process(xAOD::CaloClusterContainer* cont) const {
  for(xAOD::CaloCluster* cl : *cont) {
		double E = cl->e();
		double eta = cl->eta();
    double phi_smear = myrand_global->Gaus(cl->phi(),0.005);
    double eta_smear = myrand_global->Gaus(cl->eta(),0.005);
    double E_smear = E*cosh(eta)/cosh(eta_smear);

    cl->setE(E_smear);
    cl->setPhi(phi_smear);
    cl->setEta(eta_smear);
    cl->setM(E_smear);
  }
  return StatusCode::SUCCESS;
}

StatusCode ClusterAngularResolution::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}






