##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo monitor-server${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: monitor-server
# this is the svn URL of current dir
URL: %{_svn_path}
Group: bidfeed
License: Commercial
# uncomment below, if depend on other package
Requires: bidfeed-sharelib >= 2.0.2
Requires: protobuf >= 1.0.0
Requires: ad_log4cpp >= 4.0.9
Requires: ad_json >= 1.0.0
Requires: libevent >= 2.0
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
monitor-server
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS
# create dirs

mkdir -p .%{_prefix}/lib64/
mkdir -p .%{_prefix}/share/monitor-server/conf
mkdir -p .%{_prefix}/share/monitor-server/appconf
mkdir -p .%{_prefix}/share/monitor-server/data
mkdir -p .%{_prefix}/share/monitor-server/bin
mkdir -p .%{_prefix}/share/monitor-server/log

# copy files
cp $OLDPWD/../misc/monitor-server-stop.sh         .%{_prefix}/share/monitor-server/bin/
cp $OLDPWD/../misc/monitor-server-start.sh         .%{_prefix}/share/monitor-server/bin/
cp $OLDPWD/../misc/monitor-server-restart.sh         .%{_prefix}/share/monitor-server/bin/

cp $OLDPWD/../build/debug64/lib/libmonitor-server.so         .%{_prefix}/lib64/

cp $OLDPWD/../build/debug64/bin/monitorserver         .%{_prefix}/share/monitor-server/bin/

cp $OLDPWD/../misc/release_server/app-example         .%{_prefix}/share/monitor-server/appconf/app-example
cp $OLDPWD/../misc/release_server/monitor_server.conf         .%{_prefix}/share/monitor-server/conf/monitor_server.conf
cp $OLDPWD/../misc/release_server/log4cpp.conf         .%{_prefix}/share/monitor-server/conf/log4cpp.conf
%post


# package infomation
%files
# set file attribute here
#%defattr(755,root,root)
# need not list every file here, keep it as this
%{_prefix}

%config(noreplace)  %{_prefix}/share/monitor-server/conf/log4cpp.conf
%config(noreplace) %{_prefix}/share/monitor-server/conf/monitor_server.conf


# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab


%changelog 
