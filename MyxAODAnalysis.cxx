#include <AsgTools/MessageCheck.h>
#include <MyAnalysis/MyxAODAnalysis.h>
#include <xAODEventInfo/EventInfo.h>
#include <TSystem.h>
#include <xAODJet/JetContainer.h>
#include <xAODCore/ShallowCopy.h>

using namespace std;

MyxAODAnalysis :: MyxAODAnalysis (const std::string& name,ISvcLocator *pSvcLocator): EL::AnaAlgorithm (name, pSvcLocator)
, m_grl ("GoodRunsListSelectionTool/grl", this)
, m_jetCleaning ("JetCleaningTool/JetCleaning", this)
//, m_JERTool ("JERTool", this)
//, m_JetCalibrationTool_handle ("JetCalibrationTool", this) 
//, m_jvt ("JetMomentTools/JetVertexTaggerTool", this) //Working on this
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0. Note that things like resetting
  // statistics variables should rather go into the initialize() function.
  
  total_grl = 0;
  event_number = 0;
  index = 0;
  numGoodJets = 0;
  numGoodJets_calib = 0;
}



StatusCode MyxAODAnalysis :: initialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  ANA_MSG_INFO ("Initializing GRL, ");
  
  // Histograms Initialize 
  //book (TH1D ("Event_Number", "Event_Number", 100, 0, 500)); 
  book (TH1D ("h_jetPt", "h_jetPt", 100, 0, 500)); // jet pt [GeV]);
  book (TH1F ("h_jeteta", "h_jeteta", 100, 10.0, 10.)); // jet eta
  book (TH2F ("h_jet1vjet2", "h_jet1vjet2", 50, 0., 500., 50, 0., 500.));
  
  
  // GRL Initialize 
  const char* GRLFilePath = "/cluster/home/awhite/ITC_Dijet_Study_7/RootCoreBin/data/MyAnalysis/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml";
  const char* fullGRLFilePath = gSystem->ExpandPathName (GRLFilePath);
  std::vector<std::string> vecStringGRL;
  vecStringGRL.push_back(fullGRLFilePath);
  ANA_CHECK(m_grl.setProperty( "GoodRunsListVec", vecStringGRL));
  ANA_CHECK(m_grl.setProperty("PassThrough", false)); // if true (default) will ignore result of GRL and will just pass all events
  ANA_CHECK(m_grl.initialize());
  
  // initialize and configure the jet cleaning tool
  ANA_CHECK (m_jetCleaning.setProperty( "CutLevel", "LooseBad"));
  ANA_CHECK (m_jetCleaning.setProperty("DoUgly", false));
  ANA_CHECK (m_jetCleaning.initialize());
  
  // instantiate and initialize the JER (using default configurations)
  //ANA_CHECK(m_JERTool.initialize());

  // Ntuple initialize 
  ANA_CHECK (book (TTree ("analysis", "My analysis ntuple")));
  TTree* m_tree = tree ("analysis");
  m_tree->Branch ("RunNumber", &m_runNumber);
  m_tree->Branch ("EventNumber", &m_eventNumber);
  m_jetEta = new std::vector<float>();
  m_tree->Branch ("JetEta", &m_jetEta);
  m_jetPhi = new std::vector<float>();
  m_tree->Branch ("JetPhi", &m_jetPhi);
  m_jetPt = new std::vector<float>();
  m_tree->Branch ("JetPt", &m_jetPt);
  //m_jetE = new std::vector<float>();
  //m_tree->Branch ("JetE", &m_jetE);

  // calibrated jets total
  m_jetEta_calib = new std::vector<float>();
  m_tree->Branch ("JetEta_calib", &m_jetEta_calib);
  m_jetPt_calib = new std::vector<float>();
  m_tree->Branch ("JetPt_calib", &m_jetPt_calib);
  m_jetPhi_calib = new std::vector<float>();
  m_tree->Branch ("JetPhi_calib", &m_jetPhi_calib);
  
  // calibrated jet individual
  m_jetEta_calib_1 = new std::vector<float>();
  m_tree->Branch ("JetEta_calib_1", &m_jetEta_calib_1);
  m_jetPt_calib_1 = new std::vector<float>();
  m_tree->Branch ("JetPt_calib_1", &m_jetPt_calib_1);
  m_jetPhi_calib_1 = new std::vector<float>();
  m_tree->Branch ("JetPhi_calib_1", &m_jetPhi_calib_1);

  m_jetEta_calib_2 = new std::vector<float>();
  m_tree->Branch ("JetEta_calib_2", &m_jetEta_calib_2);
  m_jetPt_calib_2 = new std::vector<float>();
  m_tree->Branch ("JetPt_calib_2", &m_jetPt_calib_2);
  m_jetPhi_calib_2 = new std::vector<float>();
  m_tree->Branch ("JetPhi_calib_2", &m_jetPhi_calib_2);

  m_jetEta_calib_3 = new std::vector<float>();
  m_tree->Branch ("JetEta_calib_3", &m_jetEta_calib_3);
  m_jetPt_calib_3 = new std::vector<float>();
  m_tree->Branch ("JetPt_calib_3", &m_jetPt_calib_3);
  m_jetPhi_calib_3 = new std::vector<float>();
  m_tree->Branch ("JetPhi_calib_3", &m_jetPhi_calib_3);

  // Jet Calibration initialize and configure
  //const std::string name = "MyxAODAnalysis"; //string describing the current thread, for logging //Testing 
  TString jetAlgo = "AntiKt4EMTopo";  // Jet collection, for example AntiKt4EMTopo or AntiKt4LCTopo (see below)
  // Config file WITH TG3 applied
  TString config = "JES_data2017_2016_2015_Recommendation_Feb2018_rel21.config";  // Global config (see below)
  // Config file WITHOUT TG3 applied
  // TString config = "JES_MC15cRecommendation_May2016.config";
  // Calibration sequence WITH TG3 applied
  TString calibSeq = "JetArea_Residual_EtaJES_GSC"; // Calibration sequence to apply (see below) for MC ("JetArea_Residual_EtaJES_GSC_Insitu" if Data)
  // Calibration sequence WITHOUT TG3 applied
  //TString calibSeq = "JetArea_Residual_Origin_EtaJES_GSC_Insitu";
  TString calibArea = "00-04-81"; // Calibration Area tag (see below)
  bool isData = true; // bool describing if the events are data or from simulation


  m_JetCalibrationTool_handle.setTypeAndName("JetCalibrationTool/name"); 
  if( !m_JetCalibrationTool_handle.isUserConfigured() ){
    //ANA_CHECK(ASG_MAKE_ANA_TOOL(m_JetCalibrationTool_handle, JetCalibrationTool )); //<--- Was getting error with this line
    ANA_CHECK( m_JetCalibrationTool_handle.setProperty("JetCollection",jetAlgo.Data()) );
    ANA_CHECK( m_JetCalibrationTool_handle.setProperty("ConfigFile",config.Data()) );
    ANA_CHECK( m_JetCalibrationTool_handle.setProperty("CalibSequence",calibSeq.Data()) );
    ANA_CHECK( m_JetCalibrationTool_handle.setProperty("CalibArea",calibArea.Data()) );
    ANA_CHECK( m_JetCalibrationTool_handle.setProperty("IsData",isData) );
    //ANA_CHECK( m_JetCalibrationTool_handle.initialize());
    ANA_CHECK( m_JetCalibrationTool_handle.retrieve());
  }

  return StatusCode::SUCCESS;
}
// Destructor for Ntuple
MyxAODAnalysis::~MyxAODAnalysis() 
{
   if (m_jetEta) delete m_jetEta;
   //if (m_jetPhi) delete m_jetPhi;
   if (m_jetPt)  delete m_jetPt;
   //if (m_jetE)   delete m_jetE;

   // calibrated jets Total
   if (m_jetEta_calib) delete m_jetEta_calib;
   if (m_jetPt_calib)  delete m_jetPt_calib;
   if (m_jetPhi_calib)  delete m_jetPhi_calib;

   // calibrated jets individual
   if (m_jetEta_calib_1) delete m_jetEta_calib_1;
   if (m_jetPt_calib_1)  delete m_jetPt_calib_1;
   if (m_jetPhi_calib_1)  delete m_jetPhi_calib_1;

   if (m_jetEta_calib_2) delete m_jetEta_calib_2;
   if (m_jetPt_calib_2)  delete m_jetPt_calib_2;
   if (m_jetPhi_calib_2)  delete m_jetPhi_calib_2;

   if (m_jetEta_calib_3) delete m_jetEta_calib_3;
   if (m_jetPt_calib_3)  delete m_jetPt_calib_3;
   if (m_jetPhi_calib_3)  delete m_jetPhi_calib_3;
}


StatusCode MyxAODAnalysis :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  //ANA_MSG_INFO ("in execute: Getting event info");
  event_number++;
  
  //----------------------------
  // Event information
  //--------------------------- 
  const xAOD::EventInfo* eventInfo = nullptr;
  ANA_CHECK(evtStore()->retrieve( eventInfo, "EventInfo"));

  m_runNumber = eventInfo->runNumber ();
  m_eventNumber = eventInfo->eventNumber ();
  //Fill Event histogram with event with MC or Real data?
  //hist("Event_Number")->Fill (m_eventNumber); // GeV
  
  // check if the event is data or MC
  // (many tools are applied either to data or MC)
  bool isMC = false;
  // check if the event is MC
  if (eventInfo->eventType (xAOD::EventInfo::IS_SIMULATION)) {
    isMC = true; // can do something with this later
  }

  // if data check if event passes GRL
  if (!isMC) { // it's data!
    if (!m_grl->passRunLB(*eventInfo)) {
      //ANA_MSG_INFO ("drop event: GRL");
      return StatusCode::SUCCESS; // go to next event
    }
  } // end if not MC

  //ANA_MSG_INFO ("keep event: GRL");
  total_grl++; //Ask Dr. White what is a good run? Real data or MC?

  //---------------------------
  // Jets
  //---------------------------
  
  // get jet container of interest
  const xAOD::JetContainer* jets = nullptr;
  ANA_CHECK(evtStore()->retrieve( jets, "AntiKt4EMTopoJets" ));
  //ANA_MSG_INFO ("number of jets in event = " << jets->size());

  // For Ntuple
   m_jetEta->clear();
   m_jetPhi->clear();
   m_jetPt->clear();
   //m_jetE->clear();

   // calibrated jets total
   m_jetEta_calib->clear();
   m_jetPt_calib->clear();
   m_jetPhi_calib->clear();

   // calibrated jets individual 
   m_jetEta_calib_1->clear();
   m_jetPt_calib_1->clear();
   m_jetPhi_calib_1->clear();

   m_jetEta_calib_2->clear();
   m_jetPt_calib_2->clear();
   m_jetPhi_calib_2->clear();

   m_jetEta_calib_3->clear();
   m_jetPt_calib_3->clear();
   m_jetPhi_calib_3->clear();

  // loop over the jets in the container
  for (auto jet : *jets) {
    if (!m_jetCleaning->keep (*jet)) //only keep good clean jets
    { 
     //ANA_MSG_INFO ("Bad Jet");
     continue;
    }
    ++ numGoodJets;
    //ANA_MSG_INFO ("jet pt = " << (jet->pt() * 0.001) << " GeV"); // just to print out something
	  //ANA_MSG_INFO ("jet eta = " << (jet->eta()));
    //ANA_MSG_INFO ("jet phi = " << (jet->phi());
	
	// event-by-event (execute) and in a loop over jets:
    // JER and uncert
     //if(isMC){ // assuming isMC flag has been set based on eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) 
        // Get the MC resolution
        //double mcRes = m_JERTool->getRelResolutionMC(jet);
        // Get the resolution uncertainty
        //double uncert = m_JERTool->getUncertainty(jet); // you can provide a second argument specify which nuisance parameter, the default is all 
        //ANA_MSG_INFO ("execute(): jet mcRes = " << mcRes << " , uncert = " << uncert);
     //} // end if MC

     // Fill Histograms
     hist("h_jetPt")->Fill (jet->pt() * 0.001); // GeV
     hist("h_jeteta")->Fill (jet->eta()); // GeV

     // For Ntuple
      m_jetEta->push_back (jet->eta ()); 
      m_jetPhi->push_back (jet->phi ());
      m_jetPt->push_back (jet->pt ());
      //m_jetE-> push_back (jet->e ());
    
  } // end for loop over jets
  
  // Create a transient object store. Needed for the tools.
  //xAOD::TStore store;
  
  //--------------
  // shallow copy 
  //--------------

  auto shallowCopy = xAOD::shallowCopyContainer (*jets);
  std::unique_ptr<xAOD::JetContainer> shallowJets (shallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> shallowAux (shallowCopy.second);

/*if (shallowJets->size() <= 3)
{
  ANA_MSG_INFO ("Shallow copy created with jets = " << jets->size());
}*/

jet_count = 0;
for (auto jetSC : *shallowJets) 
{
  if (shallowJets->size() <= 3)
  {
    index++;
    jet_count++;
    if (!m_jetCleaning->keep (*jetSC)) //only keep good clean jets
    {
      //ANA_MSG_INFO ("Jet " << (jet_count) << " is bad");
      continue;
    }
    ++ numGoodJets_calib;
    m_JetCalibrationTool_handle->applyCalibration(*jetSC);
    if (jet_count == 1){
      //ANA_MSG_INFO ("calibrated jet1 pt = " << (jetSC->pt() * 0.001) << " GeV");
      //ANA_MSG_INFO ("calibrated jet1 eta = " << (jetSC->eta()));
      //ANA_MSG_INFO ("calibrated jet1 phi = " << (jetSC->phi()));
      m_jetEta_calib_1->push_back ((jetSC)->eta()); 
      m_jetPt_calib_1->push_back ((jetSC)->pt() * 0.001); // GeV
      m_jetPhi_calib_1->push_back ((jetSC)->phi()); 
    }
    if (jet_count == 2){
      //ANA_MSG_INFO ("calibrated jet2 pt = " << (jetSC->pt() * 0.001) << " GeV");
      //ANA_MSG_INFO ("calibrated jet2 eta = " << (jetSC->eta()));
      //ANA_MSG_INFO ("calibrated jet2 phi = " << (jetSC->phi()));
      m_jetEta_calib_2->push_back ((jetSC)->eta()); 
      m_jetPt_calib_2->push_back ((jetSC)->pt() * 0.001 ); // GeV
      m_jetPhi_calib_2->push_back ((jetSC)->phi()); 
    }
    if (jet_count == 3){
      //ANA_MSG_INFO ("calibrated jet3 pt = " << (jetSC->pt() * 0.001) << " GeV");
      //ANA_MSG_INFO ("calibrated jet3 eta = " << (jetSC->eta()));
      //ANA_MSG_INFO ("calibrated jet3 phi = " << (jetSC->phi()));
      m_jetEta_calib_3->push_back ((jetSC)->eta()); 
      m_jetPt_calib_3->push_back ((jetSC)->pt() * 0.001 ); // GeV
      m_jetPhi_calib_3->push_back ((jetSC)->phi()); 
    }
  }
} // end for loop over shallow copy jets
// Fill Ntuple
tree ("analysis")->Fill ();
return StatusCode::SUCCESS;
}

StatusCode MyxAODAnalysis :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.
  
  
  ANA_MSG_INFO("Total number of good runs = " << total_grl);
  ANA_MSG_INFO("Total number of events processed before calibration = " << event_number);
  ANA_MSG_INFO("Total number of shallow copy events processed after calibration = " << index);
  ANA_MSG_INFO("Total number of good jets = " << numGoodJets);
  ANA_MSG_INFO("Total number of shallow copy events good jets = " << numGoodJets_calib);
  return StatusCode::SUCCESS;
}
