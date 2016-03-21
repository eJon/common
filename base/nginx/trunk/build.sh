rm -rf build
make clean


./configure   --add-module=./mod_adfront  
make;
cd mod_adfront/rpm;
sh package.sh;
