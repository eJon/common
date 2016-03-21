##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo scribeprocessor-dev${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: scribeprocessor-dev
# this is the svn URL of current dir
URL: %{_svn_path}
Group: scribeprocessor-dev
License: Commercial
# uncomment below, if depend on other package
BuildRequires: ad_log4cpp >= 4.0.9
Requires: ad_log4cpp >= 4.0.9
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
scribeprocessor-dev
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
mkdir -p .%{_prefix}/include/scribe/common/
mkdir -p .%{_prefix}/include/scribe/scribeclient/
mkdir -p .%{_prefix}/include/scribe/scribeserver/
mkdir -p .%{_prefix}/share/scribe/tools/

# copy files

cp $OLDPWD/../build/debug64/lib/libscribe_processor.so         .%{_prefix}/lib64/libscribe_processor.so
cp $OLDPWD/../build/debug64/scribe/tools/scribe_client_tool  .%{_prefix}/share/scribe/tools/scribe_client_tool
cp -rf $OLDPWD/../build/debug64/sdk/scribe_sdk-0.1.0-debug64/include/scribe/*         .%{_prefix}/include/scribe/
%post

# package infomation
%files
# set file attribute here
#%defattr(755,root,root)
# need not list every file here, keep it as this
%{_prefix}


# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
