print "Starting Analysis.  This may take a moment..."


from pyLCIO import IOIMPL, EVENT,UTIL
from pyLCIO.io.LcioReader import LcioReader
from array import array
from ROOT import *

# Input File *change to slcio file that will analyized 
slcioFile = 'sitracks_10GeV_pions_65.slcio'

# output root file *change sample.root to desired name
rootFile = TFile("sample.root","recreate")

# create Histograms
HitEnergyHistogram = TH1D( 'TotalDepositedEnergy', 'Event Energy Deposit;HCAL Barrel Hit Energy [GeV];Entries', 100, 0., 1.)
HCalBarrelHitsvsLayer = TH1D("HitsvsLayer","Hits vs. Layer", 40, 0., 39.)
NumberOfHitsPerEventHistogram = TH1D( 'HitsPerEvent', 'Hits Per Event for HCalBarrelHits;HCAL Barrel Hits;Events', 500, 0., 1300.)


# create a reader and open the slcio file)
reader = IOIMPL.LCFactory.getInstance().createLCReader()
reader.open(slcioFile)

# loop over all events in the file
for event in reader:
    
    print "Collecting HCalBarrelHits and summing energies for event %s" % ( event.getEventNumber() )
    
    hitTotal = 0
    numberofHits = 0
    hitCollection = event.getCollection( 'HCalBarrelHits' )
    
    # get the cell ID encoding string from the collection parameters
    cellIdEncoding = hitCollection.getParameters().getStringVal( EVENT.LCIO.CellIDEncoding )
    # define a cell ID decoder for the collection
    idDecoder = UTIL.BitField64( cellIdEncoding )
    
    # Cycle through each hit and record the amount of hits and sum their energies of the event 
    for hit in hitCollection:
        # combine the two 32 bit cell IDs of the hit into one 64 bit integer
        cellID = long( hit.getCellID0() & 0xffffffff ) | ( long( hit.getCellID1() ) << 32 )
        # set up the ID decoder for this cell ID
        idDecoder.setValue( cellID )
        # access the field information using a valid field from the cell ID encoding string
        HCalBarrelHitsvsLayer.Fill(idDecoder['layer'].value())
        
        numberofHits += 1
        hitTotal+=hit.getEnergy()

    # Fill the histograms 
    HitEnergyHistogram.Fill(hitTotal)
    #HCalBarrelEnergyPerHit.Fill(hitTotal)
    NumberOfHitsPerEventHistogram.Fill(numberofHits)
reader.close()
    

# write and close the root file
rootFile.Write()
rootFile.Close()

print "Root file created: "+str(rootFile)




