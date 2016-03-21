##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo scribeprocessor${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: scribeprocessor
# this is the svn URL of current dir
URL: %{_svn_path}
Group: scribeprocessor
License: Commercial
# uncomment below, if depend on other package
BuildRequires: ad_log4cpp >= 4.0.9
Requires: ad_log4cpp >= 4.0.9
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
scribeprocessor
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
mkdir -p .%{_prefix}/include/scribe/
mkdir -p .%{_prefix}/share/scribe/tools/
mkdir -p .%{_prefix}/share/scribe/scribeserver/conf/

# copy files

cp $OLDPWD/../build/release64/lib/libscribe_processor.so   .%{_prefix}/lib64/libscribe_processor.so
cp $OLDPWD/../build/release64/scribe/tools/scribe_client_tool  .%{_prefix}/share/scribe/tools/
cp $OLDPWD/../misc/tool/scribe_client_tool.sh  .%{_prefix}/share/scribe/tools/scribe_client_tool.sh
cp $OLDPWD/../misc/tool/scribe_client_tool.conf  .%{_prefix}/share/scribe/tools/scribe_client_tool.conf

cp $OLDPWD/../misc/scribeserver.conf    .%{_prefix}/share/scribe/scribeserver/conf/scribeserver.conf
cp -rf $OLDPWD/../build/release64/sdk/scribe_sdk-0.1.0-release64/include/scribe/*         .%{_prefix}/include/scribe/
%post

# package infomation
%files
# set file attribute here
#%defattr(755,root,root)
# need not list every file here, keep it as this
%{_prefix}
%config(noreplace)  %{_prefix}/share/scribe/scribeserver/conf/scribeserver.conf
%config(noreplace)  %{_prefix}/share/scribe/tools/scribe_client_tool.sh
%config(noreplace)  %{_prefix}/share/scribe/tools/scribe_client_tool.conf
# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
