#!/bin/sh
#/***************************************************************************
# * 
# * Copyright (c) 2010 Baidu.com, Inc. All Rights Reserved
# * 
# **************************************************************************/



#/**
# * @file local_build.sh
# * @author luozuzhi@baidu.com
# * @date 2010/11/29/ 15:28:37
# * @version $Revision: 1.0 $ 
# * @brief  本地提交代码前进行测试的脚本, 包含1.编译，2.单测，3.功能测试
# *  
# **/



LOG_FATAL=1
LOG_WARNING=2
LOG_NOTICE=4
LOG_TRACE=8
LOG_DEBUG=16
LOG_LEVEL_TEXT=(
	[1]="FATAL"
	[2]="WARNING"
	[4]="NOTICE"
	[8]="TRACE"
	[16]="DEBUG"
)

TTY_FATAL=1
TTY_PASS=2
TTY_TRACE=4
TTY_INFO=8
TTY_MODE_TEXT=(
	[1]="[FAIL ]"
	[2]="[PASS ]"
	[4]="[TRACE]"
	[8]=""
)

#0  OFF  
#1  高亮显示  
#4  underline  
#5  闪烁  
#7  反白显示  
#8  不可见 

#30  40  黑色
#31  41  红色  
#32  42  绿色  
#33  43  黄色  
#34  44  蓝色  
#35  45  紫红色  
#36  46  青蓝色  
#37  47  白色 
TTY_MODE_COLOR=(
	[1]="1;31"	
	[2]="1;32"
	[4]="0;36"	
	[8]="1;33"
)

CONF_LOG_FILE="_local_build.log"
CCOVER_GCC_PATH=/opt/BullseyeCoverage/bin

##! @BRIEF: print info to tty & log file
##! @IN[int]: $1 => tty mode
##! @IN[string]: $2 => message
##! @RETURN: 0 => sucess; 1 => failure
function Print()
{
	local tty_mode=$1
	local message="$2"

	local time=`date "+%m-%d %H:%M:%S"`
	echo "${LOG_LEVEL_TEXT[$log_level]}: $time: ${MODULE_NAME} * $$ $message" >> ${CONF_LOG_FILE}
	echo -e "\e[${TTY_MODE_COLOR[$tty_mode]}m${TTY_MODE_TEXT[$tty_mode]} ${message}\e[m"
	return $?
}


#编译环境的搭建，使用comake2
comake2_env()
{
	Print $LOG_NOTICE "download the dependency lib and generate the makefile"

    TIME_BEFORE_ENV=`date +%s` 

    if [[ ${HUDSON_BUILD} -ne 1 ]] ; then
        comake2 -U && comake2 -B && comake2 && cd test && comake2 && cd ..;
    else
        comake2 -U && comake2 -B && comake2 -P && cd test && comake2 -P && cd ..;
    fi

	if [ $? -ne 0 ] ; then
		Print $LOG_FATAL "build the make environment failed !"; 
		return -1;
	else
		Print $LOG_NOTICE "build the make environment sucess !"; 
        TIME_AFTER_ENV=`date +%s`
        ENV_TIME=$(($TIME_AFTER_ENV-$TIME_BEFORE_ENV))
        echo "===============env_time:"$ENV_TIME
		return 0;
	fi
}

#rb编译环境的搭建，使用comake2
comake2_rb_env()
{
	Print $LOG_TRACE "download the dependency lib for rb and generate the makefile"
	comake2 -U -I config.scm  && comake2 -B -I config.scm  && comake2 -I config.scm && cd test && comake2 -I ../config.scm && cd ..;

	if [ $? -ne 0 ] ; then
		Print $LOG_FATAL "build the make rb environment failed !"; 
		return -1;
	else
		Print $LOG_TRACE "build the make rb environment sucess !"; 
		return 0;
	fi
}

#编译
project_make()
{
	Print $LOG_NOTICE "project make !"

    TIME_BEFORE_MAKE=`date +%s`

	if [ "$make_parallel" == "" ] ; then
        rm -f ./test/*.o && make clean && make -j4
    else
        rm -f ./test/*.o && make clean && make -j$make_parallel
    fi

	if [ $? -ne 0 ] ; then
		Print $LOG_FATAL "project make failed !"; 
		return -1;
	else
		Print $LOG_NOTICE "project make sucess !";
        TIME_AFTER_MAKE=`date +%s`
        MAKE_TIME=$(($TIME_AFTER_MAKE-$TIME_BEFORE_MAKE))
        echo "===============make_time:"$MAKE_TIME
		return 0;
	fi
}

#检测btest是否安装
btest_install_test()
{
	autorun.sh -h >/dev/null 2>&1

	if [ $? -ne 0 ] ; then
		Print $LOG_FATAL "btest not install !";
		Print $LOG_NOTICE "see more : http://com.baidu.com/twiki/bin/view/Main/Btest_install";
		return -1;
	else
		return 0;
	fi
}


##! @BRIEF: 单元测试, 包括bs 和 afs-bdlib, afs-fclib, (后两者待将单测代码放到bs/test目录中)
##! @IN[string]: $1 => xml开关,默认为空，为xml则程序输出xml文件: ./report.xml
unit_test()
{
	
	Print $LOG_NOTICE "unit test !"

	TIME_BEFORE_UNIT=`date +%s`
	
	if [ "$1" == "xml" ] ;then
		#cd test && autorun.sh -u
		cd test && autorun.sh -r -u
	else
		#cd test && autorun.sh -r
		cd test && autorun.sh -r
	fi


	if [ $? -ne 0 ] ; then
		convert_to_junit "."; cd ..;
		Print $LOG_FATAL "unit test failed!";
		return -1;
	else
		convert_to_junit "."; cd ..;
		Print $LOG_NOTICE "unit test sucess!";
        TIME_AFTER_UNIT=`date +%s`
        UNIT_TIME=$(($TIME_AFTER_UNIT-$TIME_BEFORE_UNIT))
        echo $UNIT_TIME > .ut_time
        echo "===============unit_time:"$UNIT_TIME
		return 0;
	fi
}

#检测ccover是否安装
ccover_install_test()
{
	#ccover 编译使用ccover的gcc, 通过PATH环境变量的方式
	TEMP_PATH=$PATH
	export PATH=$CCOVER_GCC_PATH:$TEMP_PATH
	cov01 -h  #>/dev/null 2>&1
    ret=$?
	#ccover的返回值总是1 ...

	#恢复PATH环境变量, 重新使用系统默认的gcc
	export PATH=$TEMP_PATH

	if [ $ret -ne 1 ] ; then
		Print $LOG_FATAL "ccover not install !";
		Print $LOG_NOTICE "mail to: sqa@baidu.com";
		return -1;
	else
		return 0;
	fi
}


##! @BRIEF: 检查ccover覆盖率阈值是否符合[0,100]的范围, 参数可以为空
##! @IN[int]: $1 => function coverage
##! @IN[int]: $2 => condition coverage  
##! @RETURN: 0 => sucess; -1 => failure
check_ccover_param()
{
	ret=0
	if [ "$1" == "" ] || [ "$2" == "" ]; then
		return $ret
	fi

	expr $1 + 0 1>/dev/null 2>&1 && expr $2 + 0 1>/dev/null 2>&1 

	if [ $? -ne 0 ]; then 
		Print $LOG_FATAL "ccover paraments [$1,$2] are not all num!"
		return -1;
	fi

	if [[ $1 -lt 0 ]] || [[ $1 -gt 100 ]]; then
		Print $LOG_FATAL "function coverage parament $1 out range [0,100]"
		ret=-1
	fi
	if [[ $2 -lt 0 ]] || [[ $2 -gt 100 ]]; then
		Print $LOG_FATAL "condition coverage parament $2 out range [0,100]"
		ret=-1
	fi
	return $ret
}

##! @BRIEF: 产出单元测试的代码覆盖率, 并进行覆盖率阈值判断, fclib目前还没有统计
##          注意，执行ccover前对output/bin/bs进行了备份, 执行完ccover后对其恢复,用于smoke test
##! @IN[int]: $1 => function coverage
##! @IN[int]: $2 => condition coverage  
##! @RETURN: 0 => sucess; -1 => failure
unit_ccover()
{
	if [ "$1" != "" ] && [ "$2" != "" ]; then
		func_cov=$1
		condi_cov=$2
	else
		func_cov=0
		condi_cov=0
		Print $LOG_WARNING "ccover parament set to func_cov:0, condi_cov:0 !"
	fi

	Print $LOG_NOTICE "run unit ccover !"

        bin_name=smalltable

	#备份output/bin/$bin_name产出

	if [ -e output/bin/$bin_name ]; then
		Print $LOG_NOTICE "bakup output/bin/$bin_name to .output.bin.$bin_name.bak !"
		mv output/bin/$bin_name .output.bin.$bin_name.bak 
	fi

	#ccover 编译使用ccover的gcc, 通过PATH环境变量的方式
	TEMP_PATH=$PATH
	export PATH=$CCOVER_GCC_PATH:$TEMP_PATH

	#-c " -j4" make参数， -M 1 只执行project的make , -P 用!进行文件夹和文件的覆盖率统计过滤，文件夹需要以/结尾
    cd test && autorun.sh -C1 .. -c " -j4" -M 1 -P '. !test/' \
	&& get_ccover_html \
	&& COV_ARR=(`tail -n 1 ccover/project_covsrc.txt | awk '{print $6,$11}'`) \
	&& COV_ARR[0]=`echo ${COV_ARR[0]} | sed 's/%//g'` \
	&& COV_ARR[1]=`echo ${COV_ARR[1]} | sed 's/%//g'` \
	&& cd ..

	if [ $? -ne 0 ]; then 
		Print $LOG_FATAL "unit code coverage failed!";
		#恢复$bin_name的备份
		if [ -e .output.bin.$bin_name.bak ]; then
			Print $LOG_NOTICE "recover  .output.bin.$bin_name.bak to output/bin/$bin_name !"
			mv .output.bin.$bin_name.bak output/bin/$bin_name
		fi
		return -1;
	fi

	#恢复$bin_name的备份
	if [ -e .output.bin.$bin_name.bak ]; then
		Print $LOG_NOTICE "recover  .output.bin.$bin_name.bak to output/bin/$bin_name !"
		mv .output.bin.$bin_name.bak output/bin/$bin_name
	fi

	#恢复PATH环境变量, 重新使用系统默认的gcc
	export PATH=$TEMP_PATH

	func_cov_fail=0
	condi_cov_fail=0

	if [[ ${COV_ARR[0]} -lt "$func_cov" ]] ; then
		func_cov_fail=1
	fi
	if [[ ${COV_ARR[1]} -lt "$condi_cov" ]] ; then
		condi_cov_fail=1
	fi

	if [[ $func_cov_fail -eq 1 ]] ;then
		Print $LOG_FATAL "function coverage too low: ${COV_ARR[0]}% < $func_cov%";
	fi
	if [[ $condi_cov_fail -eq 1 ]] ;then
		Print $LOG_FATAL "condition coverage too low: ${COV_ARR[1]}% < $condi_cov%";
	fi

	if [[ $func_cov_fail -eq 1 ]] || [[ $condi_cov_fail -eq 1 ]]; then
		Print $LOG_FATAL "unit code coverage failed!";
		return -1;
	else
		Print $LOG_NOTICE "unit code coverage sucess!";
		return 0;
	fi
}

#将报告转为junit.xml
#需要传入一个参数，指向原始报告所在目录
convert_to_junit()
{
	if [ $# -ne 1 ] ; then
		return 0;
	fi

	if [ -a ~/CI/bin/btest2junit ] ; then
		echo "start to convert to junit xml!";
		sh ~/CI/bin/btest2junit $1;
		echo "end to convert to junit xml!"
	fi

	return 0;
}


#其他测试，如大数据对比等
extern_test()
{
	#TODO
	return 0;
}

#for rb，产生版本号给comake2
produce_version()
{
	export COMAKE2_BUILD_VERSION="$1.$BUILD_NUMBER"
	return 0;
}

ccover_compile()
{
    current_hour=$(date +%H)
    is_debug=0
    if [ $# -ne 0 ];then
        is_debug=1
    fi
    if [ $current_hour -ge 3 -a $current_hour -lt 4 ] || [ $is_debug -eq 1 ];then 
        Print $LOG_NOTICE "in mid night, start to run ccover, or is_debug flag setted"
	    #ccover 编译使用ccover的gcc, 通过PATH环境变量的方式
	    TEMP_PATH=$PATH
	    export PATH=$CCOVER_GCC_PATH:$TEMP_PATH
        comake2_env && project_make && autorun.sh -C . -c " -j4" 
        ret=$?
	    #恢复PATH环境变量, 重新使用系统默认的gcc
	    export PATH=$TEMP_PATH
        return $ret
    else
        Print $LOG_NOTICE "ignore ccover"
        return 0
    fi
}

get_ccover_html()
{
	source ~/.bash_profile;
	export export COVFILE="../test.cov";
	sh /home/work/CI/tools/bullshtml/bullshtml  ./;
}

ccover_analyze()
{
    if [ -e "test.cov" ]  ; then
        mv test.cov test.cov.ori
    fi
    if [ -e 'test.cov.fc' ] && [ -e 'test.cov.bd' ];then
        Print $LOG_NOTICE "merge test.cov and analyze ccoverage"
	    #ccover 编译使用ccover的gcc, 通过PATH环境变量的方式
	    TEMP_PATH=$PATH
	    export PATH=$CCOVER_GCC_PATH:$TEMP_PATH
        covmerge -c -f test.cov test.cov.fc test.cov.bd
        if [ $? -ne 0 ];then
            Print $LOG_FATAL "covmerge FAILED"
            return -1
        fi
	    autorun.sh -C0 . -c " -j4" -M 1 -P '. !bdlib/ !fclib/ !test/ !idl/ !testfixture/ !TestFixture/ !framework/bs_debug.cpp !framework/bs_str_util.cpp !framework/main.cpp !src/SiteStopList.cpp !src/bs_plsi.cpp'
        if [ $? -ne 0 ];then
            Print $LOG_FATAL "ccoverage test.cov analyze failed"
            return -1
        fi
        export COVFILE="./test.cov"
        sh /home/work/CI/tools/bullshtml/bullshtml  ./
        mv test.cov ccover/test.cov.$(date +%Y%m%d)
        mv test.cov.fc test.cov.bd ccover/
        if [ -e "test.cov.ori" ]  ; then
            mv test.cov.ori test.cov.ori.$(date +%Y%m%d)
        fi
	    #恢复PATH环境变量, 重新使用系统默认的gcc
	    export PATH=$TEMP_PATH
    else
        Print $LOG_NOTICE "test.cov.fc test.cov.bd not exist,possibly no ccover action takes"
    fi
    return 0
}

#打印帮助信息
print_help()
{
	echo "samples:"
	echo "-----------------------------------------------------------------------------------"
	echo "local_build.sh all          :     env + make + unit + smoke [用于准入测试]"
	echo "local_build.sh quick        :     env + make + unit"
	echo "local_build.sh night        :     env + make + unit + ccover [ 每天晚上定时运行 ]"
	echo "-----------------------------------------------------------------------------------"
	echo "local_build.sh env          :     download the dependency libs and generate the makefile"
	echo "local_build.sh make         :     make clean & make for project"
	echo "local_build.sh unit         :     unit test"
	echo "local_build.sh ccover       :     unit test code coverage"
	echo "local_build.sh smoke        :     smoke test"
	echo "local_build.sh extern       :     extern test" 
	echo "local_build.sh env_rb       :     download the dependency libs for rb and generate the makefile"
	echo "-----------------------------------------------------------------------------------"
	echo "local_build.sh ci_all       :     env + make + unit + ccover + smoke + extern [用于CI环境]"
	echo "local_build.sh for_rb       :     env_rb + make + unit [用于rb环境]"
	echo "-----------------------------------------------------------------------------------"
	echo "local_build.sh profile      :    make bs4prof which is used for profiling"
	echo "local_build.sh rb_profile   :    make bs4prof which is used for profiling[用于rb环境]"
	echo "-----------------------------------------------------------------------------------"
}

exec_quick()
{
    TIME_BEGIN=`date +%s` &&
	produce_version "trunk" &&
    #comake2_env &&
    project_make &&
    btest_install_test
    if [ $? -ne 0 ]; then
        Print $LOG_FATAL "Fail to install btest"
        exit -1
    fi

    unit_test "xml" >& ut.log &
    ut_pid=$!

    wait $ut_pid
    ut_result=$?
    cat ut.log
    if [ $ut_result -ne 0 ]; then
        Print $LOG_FATAL "UT Smoke Failed"
    fi

    if [ $ut_result -ne 0 ]; then
    # this line is used to turn on red light
        exit -1
    fi
    
    read UNIT_TIME < .ut_time
    TIME_END=`date +%s` &&
    echo "===============SUM===============" &&
    echo "===============env_time:"$ENV_TIME &&
    echo "===============make_time:"$MAKE_TIME &&
    echo "===============unit_time:"$UNIT_TIME &&
    echo "===============make_bs_for_profile_time:"$MAKE_BS_FOR_PROFILE_TIME &&
    echo "===============total_time:"$(($TIME_END-$TIME_BEGIN))
}

#参数解析，功能调度
Main() 
{
	#写死的函数覆盖率和条件覆盖率
	func_cov=55
	condi_cov=30
	if [ "$1" == "-h" ]; then
		print_help
	elif [ "$1" == "all" ]; then
		( \
			comake2_env && project_make \
			&& btest_install_test && unit_test "xml"
		)
	elif [ "$1" == "pre-commit" ]; then
		comake2_env && project_make && btest_install_test && unit_test

	elif [ "$1" == "night" ]; then
		comake2_env && project_make \
		&& btest_install_test && unit_test "xml"\
		&& ccover_install_test && unit_ccover $func_cov $condi_cov 
	elif [ "$1" == "quick" ]; then
        exec_quick 
			#&& ccover_install_test && unit_ccover $func_cov $condi_cov 
	elif [ "$1" == "ci_all" ]; then
		( \
			comake2_env && project_make \
			&& btest_install_test && unit_test "xml"\
			&& ccover_install_test && unit_ccover $func_cov $condi_cov \
			&& smoke_test \
			&& extern_test \
		)
    elif [ "$1" == "for_rb" ]; then
        produce_version $2 && comake2_rb_env && project_make && btest_install_test && unit_test
#下面是原子的操作
	elif [ "$1" == "env" ]; then
		comake2_env
	elif [ "$1" == "env_rb" ]; then
		comake2_rb_env
	elif [ "$1" == "make" ]; then
		project_make
	elif [ "$1" == "unit" ]; then
		btest_install_test && unit_test
	elif [ "$1" == "ccover" ]; then
		btest_install_test && ccover_install_test && unit_ccover $func_cov $condi_cov
	else 
		print_help
	fi
	ret=$?
	if [ 0 -ne $ret ]; then
		sh /home/work/CI/tools/sms_cli/alarm_sms.sh >> /dev/null 2>&1
	fi

	return $ret
}
		
Main "$@" 

