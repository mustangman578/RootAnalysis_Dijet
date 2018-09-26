void ATestRun (const std::string& submitDir)
{
  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use
  SH::SampleHandler sh;

  // scan for datasets in the given directory
  // this works if you are on lxplus, otherwise you'd want to copy over files
  // to your local machine and use a local path.  if you do so, make sure
  // that you copy all subdirectories and point this to the directory
  // containing all the files, not the subdirectories.

  // use SampleHandler to scan all of the subdirectories of a directory for particular MC single file:
  //const char* inputFilePath = gSystem->ExpandPathName ("/cluster/home/awhite/ITC_Jet_Ptbalance/"); // Dr. White test 
  const char* inputFilePath = gSystem->ExpandPathName ("/cluster/home/andrew.myers/release21_rucio_download");
  //SH::ScanDir().filePattern(".pool.root.1").scan(sh,inputFilePath);
  //SH::ScanDir().filePattern("AOD.11182705._000001.pool.root.1").scan(sh,inputFilePath); // Dr. White test
  //SH::scanRucio (sh, "data16_13TeV.*.physics_Main.deriv.DAOD_JETM1.*_p3601");
  SH::ScanDir().scan (sh, inputFilePath);


  // set the name of the tree in our files
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"
  sh.setMetaString ("nc_tree", "CollectionTree");

  // further sample handler configuration may go here

  // print out the samples we found
  sh.print ();

  // this is the basic description of our job
  EL::Job job;
  
  job.sampleHandler (sh); // use SampleHandler in this job
  job.options()->setDouble (EL::Job::optMaxEvents, 100000000); // for testing purposes, limit to run over the first 500 events only!
  EL::OutputStream output  ("ANALYSIS");
  job.outputAdd(output); 
  // add our algorithm to the job
  EL::AnaAlgorithmConfig config;
  config.setType ("MyxAODAnalysis");

  // set the name of the algorithm (this is the name use with
  // messages)
  config.setName ("AnalysisAlg");

  // later on we'll add some configuration options for our algorithm that go here

  job.algsAdd (config);
  
  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit (job, submitDir);
}
