cd legacy_130_src && make -j LINUX=1 NONX86=1 X=1 SDL=1 clean
make -j LINUX=1 NONX86=1 X=1 SDL=1
make -j LINUX=1 NONX86=1 X=1 SDL=1
cd ../bin
./lsdldoom -iwad Srb2.srb
cd ..
