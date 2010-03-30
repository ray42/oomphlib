#!MC 1120

$!NEWLAYOUT



$!VarSet |is_open| = 0

$!LOOP 2

$!VarSet |field| = "solid"

$!IF |LOOP|==1
   $!VarSet |field| = "fluid"
$!ENDIF

$!READDATASET  '"|field|_mesh_before_snap.dat" '
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3" "V4" "V5" "V6" "V7"'
$!FIELDLAYERS SHOWSHADE = YES
$!FIELDLAYERS SHOWMESH = NO
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{EDGETYPE = BORDERSANDCREASES}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{COLOR = RED}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.1}
$!VIEW FIT

$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 9.652725664665523
    Y = 14.20454545454545
    }
  TEXT = 'Original simplex mesh'

$!REDRAWALL 



$!EXPORTSETUP EXPORTFORMAT = AVI
$!EXPORTSETUP IMAGEWIDTH = 806

$!IF |is_open|==0
     $!EXPORTSETUP EXPORTFNAME = 'show_snap.avi'
     $!EXPORTSTART 
       EXPORTREGION = CURRENTFRAME
     $!VARSET |is_open|=1
$!ELSE
     $!EXPORTNEXTFRAME
$!ENDIF

$!READDATASET  '"|field|_mesh_after_snap.dat" '
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3" "V4" "V5" "V6" "V7"'
$!FIELDLAYERS SHOWSHADE = YES
$!FIELDLAYERS SHOWMESH = NO
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{EDGETYPE = BORDERSANDCREASES}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{COLOR = RED}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.1}
$!VIEW FIT


$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 9.652725664665523
    Y = 14.20454545454545
    }
  TEXT = 'Boundary snapped onto quadratic surface'

$!REDRAWALL 

$!EXPORTNEXTFRAME


$!READDATASET  '"|field|_mesh_after_poisson_smooth.dat" '
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D
  VARNAMELIST = '"V1" "V2" "V3" "V4" "V5" "V6" "V7"'
$!FIELDLAYERS SHOWSHADE = YES
$!FIELDLAYERS SHOWMESH = NO
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{EDGETYPE = BORDERSANDCREASES}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{COLOR = RED}
$!FIELDMAP [1-|NUMZONES|]  EDGELAYER{LINETHICKNESS = 0.1}
$!VIEW FIT


$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 9.652725664665523
    Y = 14.20454545454545
    }
  TEXT = 'Interior nodes moved by Poisson solves'

$!REDRAWALL 

$!EXPORTNEXTFRAME

$!ENDLOOP

$!EXPORTFINISH
