prjName=mem_db

#generate code lines
line=`find $prjName -type f|grep -v ".svn" |grep -E ".cpp|.h$" |xargs wc -l|grep total|awk '{print $0;}'`
echo "##########################################################################"
echo "total code lines : " $line
echo "##########################################################################"

##################################
#plz install compile dependent rpm
rpm -e mem_db;
rpm -e libmemcached-1.0.17;
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/libmemcached-1.0.17-2.x86_64.rpm

wget http://10.210.208.36/rpms-ad/feed-test/memcached-1.4.15.tar.gz;
tar zxvf memcached-1.4.15.tar.gz;
cd memcached-1.4.15;./configure;make;
./memcached -p 20401 -uroot -d
./memcached -p 20402 -uroot -d
cd -;



build_mode=$1
echo $build_mode
sleep 2;

if [ "$build_mode" = "release" ]; then
    echo "release building...................................................."
    sleep 2
    ##################################
    #compile and run unittest, memory check(plz install tcmalloc first)
    rm -rf ./build/release64/

    scons -D ./ mode=release coverage=false heapcheck=tcmalloc
    if [ $? != 0 ] ; then
        echo "compile or unittest fail"
        exit 1;
    fi
    echo "compile and unittest pass"

    echo "start package "
    #package
    cd rpm
    sh package-release.sh
    if [ $? != 0 ] ; then
        echo "package fail"
        exit 1;
    fi
    echo "package success pass"
    cd ..

else
    ##################################
    #compile and run unittest, memory check(plz install tcmalloc first)
    rm -rf ./build/debug64/

    scons -D ./ mode=debug coverage=true heapcheck=tcmalloc
    if [ $? != 0 ] ; then
        echo "compile or unittest fail"
        exit 1;
    fi
    echo "compile and unittest pass"

    echo "start package "
    #package
    cd rpm
    sh package-debug.sh
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
fi
########################################
#plz uninstall dependent rpm
########################################
rpm -e libmemcached-1.0.17-2
ps axu| grep "./memcached -p 20401 -uroot -d"|grep -v grep |head -n 1|awk '{print $2}' |xargs kill
ps axu| grep "./memcached -p 20402 -uroot -d"|grep -v grep |head -n 1|awk '{print $2}' |xargs kill
rm -rf memcached-1.4.15*;
