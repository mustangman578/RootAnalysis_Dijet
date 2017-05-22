#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# pyLCIO analysis for SiD calorimeters
#
# A. Myers, University of Texas at Arlington
# March 2017 
#
# Input: Reconstructed .slcio file
# Output: ROOT Histograms of the total energy deposited in each event, both for the entire calorimeter 
#
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

print "Starting Analysis.  This may take a moment..."

from pyLCIO import IOIMPL, EVENT,UTIL
from pyLCIO.io.LcioReader import LcioReader
from array import array
from ROOT import *

# Input File *change to slcio file that will analyized 
slcioFile = '10GeV_65_pions_reco.slcio'
# Change to desired output name
outFile = "10GeV_65_pions_reco_analysis.root"

# Create root file
rootFile = TFile(outFile,"recreate")

# Change depending on geometry 
HCAL_Barrel_Max_Layers = 40
HCAL_Endcap_Max_Layers = 45


# create Histograms
HitEnergyHistogram = TH1D( 'TotalDepositedEnergy', 'Event Energy Deposit;Hit Energy [GeV];Entries', 100, 0., 10.)
ECALvsHCALHitEnergyHistogram = TH2D("ECALvsHCALEnergy", "ECAL vs HCAL Total Energy;HCAL Energy [GeV];ECAL Energy [GeV]", 300, 0., 150, 160, 0., 80) 
HCalHitsvsLayerHistogram_Barrel = TProfile("HitsvsLayersBarrel","Hits vs. Layers Barrel;Layers;Hits", HCAL_Barrel_Max_Layers, 0., HCAL_Barrel_Max_Layers) 
HCalHitsvsLayerHistogram_Endcap = TProfile("HitsvsLayersEndcap","Hits vs. Layers Endcap;Layers;Hits", HCAL_Endcap_Max_Layers, 0., HCAL_Endcap_Max_Layers)
NumberOfHitsPerEventHistogram = TH1D( 'HitsPerEvent', 'Hits Per Event;Hits;Events', 400, 0., 5000.)
HitEnergyHistogram_Reco = TH1D( 'TotalDepositedEnergyReco', 'Event Energy Deposit DigiReco;Hit Energy [GeV];Entries', 100, 0., 25.)
HCAL_Cut_HitEnergyHistogram_Reco = TH1D( 'CutDepositedEnergyReco', 'Cut Total Energy of all hits AHCAL per reco event;Hit Energy [GeV];Entries', 100, 0., 25.)
HCAL_HitEnergyHistogram_Reco = TH1D( 'TotalDepositedEnergyReco_HCAL', 'Total Energy of all hits AHCAL per reco event;Hit Energy [GeV];Entries', 100, 0., 25.)
ECAL_HitEnergyHistogram_Reco = TH1D( 'TotalDepositedEnergyReco_ECAL', 'Total Energy of all hits ECAL per reco event;Hit Energy [GeV];Entries', 100, 0., 25.)

# create a reader and open the slcio file
reader = IOIMPL.LCFactory.getInstance().createLCReader()
reader.open(slcioFile)
 
for event in reader:
    print "Collecting Hits in HCAL Barrel, HCAL End Cap, ECAL Barrel, ECAL End Cap and summing energies for event %s" % ( event.getEventNumber() )

    # Initialize energy variables and number of hits variable
    hitEnergyTotal = 0
    hitEnergyTotal_Reco = 0
    numberofHits = 0
    HCAL_hitEnergyTotal = 0
    ECAL_hitEnergyTotal = 0
    
    # Cycle through Reco collections and sum their energies
    for Pre_Reco_CollectionName in ['HCalBarrelHits', 'HCalEndcapHits', 'ECalBarrelHits', 'ECalEndcapHits']:

        if Pre_Reco_CollectionName == 'HCalBarrelHits':
    
            collection = event.getCollection( Pre_Reco_CollectionName )

            # get the cell ID encoding string from the collection parameters for HCAL
            cellIdEncoding = collection.getParameters().getStringVal( EVENT.LCIO.CellIDEncoding )
            # define a cell ID decoder for the collection
            idDecoder = UTIL.BitField64( cellIdEncoding )

            #creat an array for the layer information
            LayerHitArray = [0 for i in range (HCAL_Barrel_Max_Layers)]

            for hit in collection:
            
                # combine the two 32 bit cell IDs of the hit into one 64 bit integer
                cellID = long( hit.getCellID0() & 0xffffffff ) | ( long( hit.getCellID1() ) << 32 )

                # set up the ID decoder for this cell ID
                idDecoder.setValue( cellID )

                #Get energies 
                hitEnergyTotal+=hit.getEnergy()

                numberofHits += 1
                
                # access the field information using a valid field from the cell ID encoding string
                LayerHitArray[idDecoder['layer'].value()] += 1

            for i in range(HCAL_Barrel_Max_Layers):
                HCalHitsvsLayerHistogram_Barrel.Fill(i,LayerHitArray[i])

        elif Pre_Reco_CollectionName == 'HCalEndcapHits':
            collection = event.getCollection( Pre_Reco_CollectionName )

            # get the cell ID encoding string from the collection parameters for HCAL
            cellIdEncoding = collection.getParameters().getStringVal( EVENT.LCIO.CellIDEncoding )
            # define a cell ID decoder for the collection
            idDecoder = UTIL.BitField64( cellIdEncoding )

            #creat an array for the layer information
            LayerHitArray = [0 for i in range (HCAL_Endcap_Max_Layers)]

            for hit in collection:
            
                # combine the two 32 bit cell IDs of the hit into one 64 bit integer
                cellID = long( hit.getCellID0() & 0xffffffff ) | ( long( hit.getCellID1() ) << 32 )

                # set up the ID decoder for this cell ID
                idDecoder.setValue( cellID )
                
                # Get energies
                hitEnergyTotal+=hit.getEnergy()

                numberofHits += 1
                
                # access the field information using a valid field from the cell ID encoding string
                LayerHitArray[idDecoder['layer'].value()] += 1

            for i in range(HCAL_Barrel_Max_Layers):
                HCalHitsvsLayerHistogram_Endcap.Fill(i,LayerHitArray[i])
        else:
                collection = event.getCollection( Pre_Reco_CollectionName )

                for hit in collection:

                    hitEnergyTotal+=hit.getEnergy()

                    numberofHits += 1
            
    # Cycle through Reco collections and sum their energies
    for Reco_CollectionName in ['ECalBarrelReco', 'ECalEndcapReco', 'HCalEndcapReco', 'HCalBarrelReco']:
            try:
                if Reco_CollectionName == 'HCalEndcapReco' or Reco_CollectionName == 'HCalBarrelReco':
                    collection = event.getCollection( Reco_CollectionName )
                    for hit in collection:
                        hitEnergyTotal_Reco+=hit.getEnergy()
                        HCAL_hitEnergyTotal+=hit.getEnergy()
                else:
                    collection = event.getCollection( Reco_CollectionName )
                    for hit in collection:
                        hitEnergyTotal_Reco+=hit.getEnergy()
                        ECAL_hitEnergyTotal+=hit.getEnergy()
            except:
                continue
            
    # Cuts for HCAL 95% of total reco energy deposited
    try:
        cut_percent = ECAL_hitEnergyTotal/hitEnergyTotal_Reco
        if cut_percent <= 0.05:
            HCAL_Cut_HitEnergyHistogram_Reco.Fill(hitEnergyTotal_Reco)
    except:
        continue 
        
       
    # Fill the histograms
    HCAL_HitEnergyHistogram_Reco.Fill(HCAL_hitEnergyTotal)
    HitEnergyHistogram.Fill(hitEnergyTotal)
    NumberOfHitsPerEventHistogram.Fill(numberofHits)
    HitEnergyHistogram_Reco.Fill(hitEnergyTotal_Reco)
    ECALvsHCALHitEnergyHistogram.Fill(HCAL_hitEnergyTotal,ECAL_hitEnergyTotal) 

reader.close()
 
    
# Fit histograms with Gaussian       ~ working on errors~
#HitEnergyHistogram.Fit("gaus")
#NumberOfHitsPerEventHistogram.Fit("gaus")
#HitEnergyHistogram_Reco.Fit("gaus")

# write and close the root file
rootFile.Write()
rootFile.Close()

print "Root file created: "+str(outFile)



    


        

