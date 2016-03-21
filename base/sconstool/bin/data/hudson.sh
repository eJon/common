prjName=example

#generate code lines
line=`find $prjName -type f|grep -v ".svn" |grep -E ".cpp|.h$" |xargs wc -l|grep total|awk '{print $0;}'`
echo "##########################################################################"
echo "total code lines : " $line
echo "##########################################################################"

##################################
#plz install compile dependent rpm
##################################
#compile and run unittest, memory check(plz install tcmalloc first)
rm -rf ./build/debug64/
rm -rf ./gtest-output/
scons -D ./ mode=debug coverage=true heapcheck=tcmalloc
if [ $? != 0 ] ; then
    echo "compile or unittest fail"
    exit 1;
fi
echo "compile and unittest pass"

echo "start package "
#package
cd rpm
sh package.sh
if [ $? != 0 ] ; then
    echo "package fail"
    exit 1;
fi
echo "package success pass"
cd ..

rm -rf ./result
mkdir ./result

#generate ut coverage

for subdir in $(ls $prjName);
do

        file_path=$prjName"/"$subdir
        if [ -d $file_path ]
        then
            echo "gather "$file_path
	    gcov "build/debug64/"${file_path}"/*.cpp" -o "build/debug64/"${file_path}"/"
	    lcov -b ./ -d "build/debug64/"${file_path} -c >> ./result/main.info
        fi
done
genhtml -o ./result ./result/main.info

########################################
#plz uninstall dependent rpm
########################################
