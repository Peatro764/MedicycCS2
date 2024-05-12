dir=/home/hofverberg/dev/build-MedicycCS2-Desktop_Qt_5_12_12_GCC_64bit-Debug/

export LD_LIBRARY_PATH=$dir/CommEquipment/NSingle/src:$dir/CommEquipment/ClientConnection/src:$dir/Utils/src/:$LD_LIBRARY_PATH

./StandardNSingleGui Dipole_M1&
./StandardNSingleGui Dipole_M4&
./StandardNSingleGui Quadrupole_1-V&
./StandardNSingleGui Quadrupole_2-H&
./StandardNSingleGui Quadrupole_3-5-H&
./StandardNSingleGui Quadrupole_4-V&
./StandardNSingleGui Quadrupole_6-8-V&
./StandardNSingleGui Quadrupole_7-9-H&
./StandardNSingleGui Steerer_X&
./StandardNSingleGui Steerer_Y&





