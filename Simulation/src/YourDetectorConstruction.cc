
#include "YourDetectorConstruction.hh"
#include "MySD.hh"  // the sensitive detector and hit class links

#include "G4Material.hh"
#include "G4MaterialTable.hh" // table needed for LXe and PTFE!
#include "G4NistManager.hh"
#include "G4Tubs.hh" // cylinder
#include "G4Box.hh" // box
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4Colour.hh" // for colour
#include "G4VisAttributes.hh" // for vis attributes
#include "G4OpticalSurface.hh" // for optical surface things
#include "G4OpBoundaryProcess.hh" // for optical boundary processes
#include "G4LogicalBorderSurface.hh" // for making bordersurfaces
#include "G4RunManager.hh" // for updating runmanager
#include "G4SystemOfUnits.hh"// Adding system of units (ns, cm, MeV, etc.)
#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Log.hh"
#include "G4SDManager.hh"

/*for reading in the PMT position data */
#include "input.hh"

YourDetectorConstruction::YourDetectorConstruction() 
: G4VUserDetectorConstruction() {
    
  // set default target thickness
  fTargetThickness = 1.0*CLHEP::cm;
  // initial gun-x position 
  fGunXPosition    = -20;

  // DefineMaterials is used for liquid Xenon and PTFE materials
  DefineMaterials();
}

YourDetectorConstruction::~YourDetectorConstruction() {}


//---------------------------Materials--------------------------------------

void YourDetectorConstruction::DefineMaterials(){
  
  // Nist manager
  G4NistManager* nistManager = G4NistManager::Instance();

  // Define wavelength limits for material energy definition
  lambda_min = 200*nm ; // 1.7 eV
  lambda_max = 700*nm ; // 6.1 eV

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;
    
  // Elements
  G4Element* fC = new G4Element("C", "C", z=6., a=12.01*g/mole);
  G4Element* fF = new G4Element("F", "F", z=9, a=19.00*g/mole);
  G4Element* elN = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
  G4Element* elO = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);
  G4Element* fSi = new G4Element("Silicon", "Si", z=14., a=28.09*g/mole);
  
  // Build Materials
  // Liquid Xenon
  fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  
  // Xenon gas defined using NIST Manager
  fGXe = nistManager->FindOrBuildMaterial("G4_Xe");
  
  //PTFE
  fPTFE = new G4Material("PTFE", density=2.2*g/cm3, 2);
  fPTFE->AddElement(fC, 2);
  fPTFE->AddElement(fF, 4);
  
  // Quartz
  // density = 2.200*g/cm3; // fused quartz 
  density = 2.64*g/cm3;  // crystalline quartz (c.f. PDG) 
  fQuartz = new G4Material("Quartz",density, 2);
  fQuartz->AddElement(fSi, 1) ;
  fQuartz->AddElement(elO , 2) ;
  
  // Material properties tables
  // fLXe 
  G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
  const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

  G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
  assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
  G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
  assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
  G4double lxe_ABSL[]  = { 50.*m, 50.*m, 50.*m};
  assert(sizeof(lxe_ABSL) == sizeof(lxe_Energy));
  fLXe_mt = new G4MaterialPropertiesTable();
  fLXe_mt->AddProperty("FASTCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("SLOWCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("RINDEX",        lxe_Energy, lxe_RIND,  lxenum);
  fLXe_mt->AddProperty("ABSLENGTH",     lxe_Energy, lxe_ABSL,  lxenum);
  fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
  fLXe_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
  fLXe_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
  fLXe_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  fLXe_mt->AddConstProperty("YIELDRATIO",1.0);
  fLXe->SetMaterialPropertiesTable(fLXe_mt);

  // Set the Birks Constant for the LXe scintillator
  fLXe->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  //fPTFE Material Properties
  fPTFE_mt = new G4MaterialPropertiesTable();
  const G4int NUM = 2;
  G4double XX[NUM] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ; 
  G4double PTFE_refl[NUM]      = { 0.99, 0.99 };
  G4double rIndexPTFE[]={ 1.38, 1.38};
  fPTFE_mt->AddProperty("RINDEX", XX,rIndexPTFE,NUM);
  fPTFE_mt->AddProperty("REFLECTIVITY", XX, PTFE_refl, NUM);
  fPTFE->SetMaterialPropertiesTable(fPTFE_mt);

  // fGXe Properties table
  fGXe_mt = new G4MaterialPropertiesTable();

  G4double wls_Energy2[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV};
  const G4int wlsnum2 = sizeof(wls_Energy2)/sizeof(G4double);
 
  G4double rIndexGXe[]={ 1., 1., 1., 1.};
  fGXe_mt->AddProperty("RINDEX", wls_Energy2,rIndexGXe,wlsnum2);
  fGXe->SetMaterialPropertiesTable(fGXe_mt);

  //------------------ Quarts Properties (probably not needed)-------------------------
//           Photomultiplier (PMT) window       
// The refractive index is for lime glass; 
// wavelength dependence is not included and value at 400nm is used.
  
  fQuartz_mt = new G4MaterialPropertiesTable();

  const G4int N_RINDEX_QUARTZ = 2 ;
  G4double X_RINDEX_QUARTZ[N_RINDEX_QUARTZ] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ; 
  G4double RINDEX_QUARTZ[N_RINDEX_QUARTZ] = {1.54, 1.54};

  
  fQuartz_mt->AddProperty("RINDEX", X_RINDEX_QUARTZ, RINDEX_QUARTZ, N_RINDEX_QUARTZ);
  fQuartz->SetMaterialPropertiesTable(fQuartz_mt);

}

// --------------------------- GEOMETRY -----------------------------

G4VPhysicalVolume* YourDetectorConstruction::Construct() {
  G4cout << " === YourDetectorConstruction::Construct() method === " << G4endl;
  // define dimensions
  G4double targetXSize  = 1*cm;
  G4double targetYZSize = 1*targetXSize;
  G4double worldXSize   = 500*targetXSize;
  G4double worldYZSize  = 500*targetYZSize;
  // compute gun-x position 
  fGunXPosition  = -0.5*(targetXSize+worldXSize);
  // create a material for the world
  G4double zet      = 1.0;
  G4double amass    = 1.01*CLHEP::g/CLHEP::mole;
  G4double density  = CLHEP::universe_mean_density;
  G4double pressure = 3.0E-18*CLHEP::pascal;
  G4double tempture = 2.73*CLHEP::kelvin;
  G4Material* materialWorld = new G4Material("Galactic", zet, amass, density, 
                                             kStateGas, tempture, pressure); 
 
  // world 
  G4Box* worldSolid = new G4Box( "solid-World",   // name
                                 0.5*worldXSize,  // box half x-size
                                 0.5*worldYZSize, // box half y-size
                                 0.5*worldYZSize  // box half z-size
                                );                                
  G4LogicalVolume* worldLogical = new G4LogicalVolume( worldSolid,    // solid 
                                                       materialWorld, // material                             
                                                       "logic-World"  // name
                                                     );       
  G4VPhysicalVolume* worldPhysical = new G4PVPlacement( nullptr,                    // (no) rotation 
                                                        G4ThreeVector(0., 0., 0.),  // translation
                                                        worldLogical,               // logical volume
                                                        "World",                    // name
                                                        nullptr,                    // mother volume (!)
                                                        false,                      // don't care
                                                        0                           // cpy number
                                                      );
  
  //---------------------Time Projection Chamber----------------------------

  // GXe cylinder
  G4Tubs* gasSolid = new G4Tubs ("solid-gas", 0, 96/2*targetXSize, 97/2*targetXSize,  0, 2*pi );
  G4LogicalVolume* gasLogical = new G4LogicalVolume(gasSolid, fGXe, "logic-gas");
  G4VPhysicalVolume* gasPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 0.),
                                                        gasLogical, 
                                                        "Cylinder",
                                                        worldLogical,
                                                        false,
                                                        0);
  // Set visual attributes
  G4VisAttributes* GXeVisAtt = new G4VisAttributes(G4Colour(0.3,0.85,0.3));
  GXeVisAtt->SetVisibility(true);
  gasLogical->SetVisAttributes(GXeVisAtt);   

  // LXe cylinder
  G4Tubs* cylSolid = new G4Tubs ( "solid-cylinder", 0, 96/2*targetXSize, 91.5/2*targetXSize,  0, 2*pi );
  G4LogicalVolume* cylLogical = new G4LogicalVolume(cylSolid, fLXe, "logic-cylinder");                                 
  G4VPhysicalVolume* cylPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., -2.75*targetXSize),
                                                        cylLogical, 
                                                        "Cylinder",
                                                        gasLogical,
                                                        false,
                                                        0);
  
  // Set visual attributes
  G4VisAttributes* LXeVisAtt = new G4VisAttributes(G4Colour(0.1,0.9,0.1));
  LXeVisAtt->SetVisibility(true);
  cylLogical->SetVisAttributes(LXeVisAtt);
  
  // Set optical surface properties
  G4OpticalSurface* OpLGSurface = new G4OpticalSurface("Liquid-Gas Xenon Surface");
  OpLGSurface-> SetModel(unified);
  OpLGSurface -> SetType(dielectric_dielectric);
  OpLGSurface -> SetFinish(groundbackpainted);                                        
  
  // PTFE cylinder
  G4Tubs* cylPTFE = new G4Tubs ( "PTFE-cylinder", 96/2*targetXSize, (96/2 + 0.3)*targetXSize, 97/2*targetXSize,  0, 2*pi );
  G4LogicalVolume* logicalPTFE = new G4LogicalVolume(cylPTFE, 
                                                       fPTFE, 
                                                       "logic-TPC PTFE");                                 
  G4VPhysicalVolume* physicalPTFE = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 0.),
                                                        logicalPTFE, 
                                                        "Cylinder PTFE",
                                                        worldLogical,
                                                        false,
                                                        0);

  // Set visual attributes
  G4VisAttributes* SurfaceVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  SurfaceVisAtt->SetVisibility(true);
  logicalPTFE->SetVisAttributes(SurfaceVisAtt);

//---------------------------Top Cap Cylinder--------------------------------------

 G4Tubs* TopSolid = new G4Tubs ( "Top-cylinder", 0, 96/2*targetXSize, targetXSize,  0, 2*pi );


  G4LogicalVolume* TopLogical = new G4LogicalVolume(TopSolid, 
                                                       fPTFE,     //material is teflon
                                                       "logic-top");
  G4VPhysicalVolume* TopPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 98/2*targetXSize),
                                                        TopLogical, 
                                                        "Top",
                                                        worldLogical, //mother volume is the world
                                                        false,
                                                        0);
 //---------------------------Bottom Cap Cylinder --------------------------------

 G4Tubs* BottomSolid = new G4Tubs ( "Bottom-cylinder", 0, 96/2*targetXSize, targetXSize,  0, 2*pi );


  G4LogicalVolume* BottomLogical = new G4LogicalVolume(BottomSolid, 
                                                       fPTFE,       //material is teflon
                                                       "logic-bottom");                                 
  G4VPhysicalVolume* BottomPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., -98/2*targetXSize),
                                                        BottomLogical, 
                                                        "Bottom",
                                                        worldLogical, //mother volume is the world
                                                        false,
                                                        0);   

//-------------------------------PMT--------------------------------------- 

// INPUT the coordinates of the PMT ------------------

  G4cout << "PMT nr. " << pmt_num[0] << G4endl;
  G4cout << "x[0] = " << x_pos[0] << G4endl;
  G4cout << "y[0] = " << y_pos[0] << G4endl;

// Define the geometry of the PMTs ----------------------
G4Tubs* pmtSolid = new G4Tubs ( "pmt-cylinder", 0, 7.62/2*targetXSize, targetXSize,  0, 2*pi );           
                                  


G4LogicalVolume* pmtLogical = new G4LogicalVolume(pmtSolid, 
                                                       fQuartz, 
                                                       "logic-pmt");

// Making the sensitive detector and adding it to "logic-pmt"
auto PMTSD = new MySD("MySD", "MyHitsCollection");
SetSensitiveDetector("logic-pmt", PMTSD);

// Top cap PMTs                                                       
  for (int i=0; i<127; i++) {
      G4VPhysicalVolume* pmtPhysical = new G4PVPlacement(nullptr,
                                                            G4ThreeVector(x_pos[i]*targetXSize, y_pos[i]*targetXSize, 0.),
                                                            pmtLogical, 
                                                            std::to_string(i),
                                                            TopLogical,
                                                            false,
                                                            0);

  }
// Bottom Cap PMTs
  for (int j=127; j<248; j++) {
      G4VPhysicalVolume* pmtPhysical = new G4PVPlacement(nullptr,
                                                            G4ThreeVector(x_pos[j]*targetXSize, y_pos[j]*targetXSize, 0.),
                                                            pmtLogical, 
                                                            std::to_string(j),
                                                            BottomLogical,
                                                            false,
                                                            0);
  }                                                          

//	--------------------- Surfaces ----------------------

// Create optical surface between liquid xenon and gas xenon
  G4OpticalSurface* opLXeSurface = new G4OpticalSurface("LXeSurface");
  opLXeSurface->SetType(dielectric_LUTDAVIS);
  opLXeSurface->SetFinish(Rough_LUT);
  opLXeSurface->SetModel(DAVIS);

  G4LogicalBorderSurface* LXeSurface =
          new G4LogicalBorderSurface("LXeSurface",
                                 cylPhysical,gasPhysical,opLXeSurface);

  G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*>
        (LXeSurface->GetSurface(cylPhysical,gasPhysical)->
                                                       GetSurfaceProperty());
  if (opticalSurface) opticalSurface->DumpInfo();
  
  // Create optical surface for the PTFE
  fPTFEOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
  fPTFEOpticalSurface->SetModel(unified);
  fPTFEOpticalSurface->SetType(dielectric_dielectric);
  fPTFEOpticalSurface->SetFinish(polishedfrontpainted);

  // set bordersurface skinsurface and update runmanager
  new G4LogicalSkinSurface("ReflectorSurface",logicalPTFE,fPTFEOpticalSurface);

  G4RunManager* runManager = G4RunManager::GetRunManager();
  runManager->GeometryHasBeenModified();

   // return the root (i.e. world worldPhysical volume ptr)
   return worldPhysical;                                                         
}

void YourDetectorConstruction::SetReflectionType(G4String rtype)
{
  G4RunManager* runManager = G4RunManager::GetRunManager();

  fReflectionType = rtype;

}