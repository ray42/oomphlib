#! /bin/sh


#Set the number of tests to be checked
NUM_TESTS=2

# Setup validation directory
#---------------------------
touch Validation
rm -r -f Validation
mkdir Validation

# Validation for homogenisation problem 
#---------------------------------------------------------
cd Validation

echo "Running two-dimensional homogenisation test (square)"
mkdir RESLT 
cd RESLT
../../two_dim  > ../OUTPUT_two_dim
cd ..
mv RESLT RESLT_two_dim
echo "done"

echo "Running two-dimensional homogenisation test (hexagon)"
mkdir RESLT 
cd RESLT
../../two_dim_hex  > ../OUTPUT_two_dim_hex
cd ..
mv RESLT RESLT_two_dim_hex
echo "done"

echo " " >> validation.log
echo "Two-dimensional homogenisation test " >> validation.log
echo "-------------------------------------------------" >> validation.log
echo " " >> validation.log
echo "Validation directory: " >> validation.log
echo " " >> validation.log
echo "  " `pwd` >> validation.log
echo " " >> validation.log
echo " Square unit cell " >> validation.log
echo " " >> validation.log
cat  RESLT_two_dim/C_eff.dat > ./two_d_hom.dat

if test "$1" = "no_fpdiff"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python or validata" >> validation.log
else
../../../../bin/fpdiff.py ../validata/two_d_hom.dat.gz  \
         ./two_d_hom.dat 0.1 1.0e-6 >> validation.log
fi

echo " " >> validation.log
echo " Hexagonal unit cell " >> validation.log
echo " " >> validation.log
cat  RESLT_two_dim_hex/C_eff.dat > ./two_d_hom_hex.dat

if test "$1" = "no_fpdiff"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python or validata" >> validation.log
else
../../../../bin/fpdiff.py ../validata/two_d_hom_hex.dat.gz  \
         ./two_d_hom_hex.dat 0.1 5.0e-6 >> validation.log
fi


# Append log to main validation log
cat validation.log >> ../../../../validation.log

cd ..


#######################################################################


#Check that we get the correct number of OKs
OK_COUNT=`grep -c 'OK' Validation/validation.log`
if  [ $OK_COUNT -eq $NUM_TESTS ]; then
 echo " "
 echo "======================================================================"
 echo " " 
 echo "All tests in" 
 echo " " 
 echo "    `pwd`    "
 echo " "
 echo "passed successfully."
 echo " "
 echo "======================================================================"
 echo " " 
 exit 0
else
  if [ $OK_COUNT -lt $NUM_TESTS ]; then
   echo " "
   echo "======================================================================"
   echo " " 
   echo "Only $OK_COUNT of $NUM_TESTS test(s) passed; see"
   echo " " 
   echo "    `pwd`/Validation/validation.log"
   echo " " 
   echo "for details" 
   echo " " 
   echo "======================================================================"
   echo " "
   exit 1
  else 
   echo " "
   echo "======================================================================"
   echo " " 
   echo "More OKs than tests! Need to update NUM_TESTS in"
   echo " " 
   echo "    `pwd`/validate.sh"
   echo " "
   echo "======================================================================"
   echo " "
  exit 2
  fi
fi
# Never get here
exit 10