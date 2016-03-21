##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo monitor-client${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: clickserver
# this is the svn URL of current dir
URL: %{_svn_path}
Group: monitor-client
License: Commercial
# uncomment below, if depend on other package
Requires: bidfeed-sharelib >= 2.0.1
Requires: protobuf >= 1.0.0
Requires: ad_log4cpp >= 4.0.9
Requires: ad_json >= 1.0.0
Requires: libevent >= 2.0
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
clickserver
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
mkdir -p .%{_prefix}/share/monitor-client/example

mkdir -p .%{_prefix}/include/monitor

# copy includes
# copy files
cp $OLDPWD/../build/debug64/lib/libmonitor-client.so         .%{_prefix}/lib64/
cp -rf  $OLDPWD/../build/debug64/sdk/monitor_sdk-0.1.0-debug64/include/monitor/*         .%{_prefix}/include/monitor/
cp $OLDPWD/../misc/release_client/client.conf         .%{_prefix}/share/monitor-client/example/client.conf
cp $OLDPWD/../misc/release_client/log4cpp.conf         .%{_prefix}/share/monitor-client/example/log4cpp.conf
cp $OLDPWD/../tools/monitor_agent_example.cpp         .%{_prefix}/share/monitor-client/example/
cp $OLDPWD/../build/debug64/bin/client_agent_example         .%{_prefix}/share/monitor-client/example/

%post


# package infomation
%files
# set file attribute here
#%defattr(755,root,root)
# need not list every file here, keep it as this
%{_prefix}

%config(noreplace)  %{_prefix}/share/monitor-client/example/log4cpp.conf
%config(noreplace)  %{_prefix}/share/monitor-client/example/client.conf

# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab


%changelog 
