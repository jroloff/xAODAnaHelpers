

'''
    Example python script to be run like

        xAH_run.py --files ... --config gamma_b.py ... <driver>

    Note that unlike the JSON configuration example, you also have access to
the command line arguments passed in and parsed by xAH_run.py under the global
variable `args`. For example, there is an `isMC` flag which can be used to
configure your job options dynamically

        if not args.is_MC:
            systName = "Nominal"
            systVal = 0
        else:
            systName = "All"
            systVal = 1

    If you want to know what arguments are passed in, you can simply write

        print(args)

    to see the namespace associated with it, or look at the source xAH_run.py
here: https://github.com/UCATLAS/xAODAnaHelpers/blob/master/scripts/xAH_run.py
'''

from xAH_config import xAH_config
c = xAH_config()



c.setalg("BasicEventSelection", {"m_truthLevelOnly": False,
																 "m_name": "EventSelection",
                                 "m_applyGRLCut": False,
                                 "m_doPUreweighting": True,
                                 "m_vertexContainerName": "PrimaryVertices",
                                 "m_PVNTrack": 2,
                                 "m_useMetaData": False,
                                })



# Create the various softdrop jet collections 

#Apply origin correction to the clusters
c.setalg("OriginCorrectionTool", {"m_name": "OriginCorrections",
                          "InputContainer": "CaloCalTopoClusters",
                          "OutputContainer": "CaloCalOrigTopoClusters"
                        })


c.setalg("ClusterUncertaintyTool", {"m_name": "ClusterUncertainty"})


# Create R = 0.8 jets
c.setalg("JetMaker", {"m_name": "JetMaker",
                          "JetRadius": 1.0,
                          "InputClusters": "CaloCalOrigTopoClusters",
                          "OutputContainer": "MyAntiKt10LCTopoJets"
                        })

#c.setalg("JetMaker", {"m_name": "JetMakerCAR",
#                          "JetRadius": 1.0,
#                          "InputClusters": "CAR_CaloCalTopoClusters",
#                          "OutputContainer": "MyAntiKt10LCTopoCARJets"
#                        })
#
#c.setalg("JetMaker", {"m_name": "JetMakerCE",
#                          "JetRadius": 1.0,
#                          "InputClusters": "CE_CaloCalTopoClusters",
#                          "OutputContainer": "MyAntiKt10LCTopoCEJets"
#                        })
#
#c.setalg("JetMaker", {"m_name": "JetMakerCES",
#                          "JetRadius": 1.0,
#                          "InputClusters": "CES_CaloCalTopoClusters",
#                          "OutputContainer": "MyAntiKt10LCTopoCESJets"
#                        })
#
#c.setalg("JetMaker", {"m_name": "JetMakerCESUp",
#                          "JetRadius": 1.0,
#                          "InputClusters": "CESUp_CaloCalTopoClusters",
#                          "OutputContainer": "MyAntiKt10LCTopoCESUpJets"
#                        })
##
#c.setalg("JetMaker", {"m_name": "JetMakerCESDown",
#                          "JetRadius": 1.0,
#                          "InputClusters": "CESDown_CaloCalTopoClusters",
#                          "OutputContainer": "MyAntiKt10LCTopoCESDownJets"
#                        })


c.setalg("TreeAlgo", {"m_name": "OutputTree",
                      #"m_fatJetContainerName": "MyAntiKt10LCTopoJets", 
                      #"m_fatJetContainerName": "MyAntiKt10LCTopoJets MyAntiKt10LCTopoCESJets MyAntiKt10LCTopoCARJets MyAntiKt10LCTopoCEJets MyAntiKt10LCTopoCESUpJets MyAntiKt10LCTopoCESDownJets", 
                      #"m_fatJetDetailStr": "kinematic energy scales",
											"m_trigDetailStr": "basic menuKeys passTriggers",
                      #"m_evtDetailStr": "pileup shapeLC truth"
                    })





