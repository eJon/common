##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo bidfeed-sharelib${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: bidfeed-sharelib
# this is the svn URL of current dir
URL: %{_svn_path}
Group: bidfeed-sharelib
License: Commercial
# uncomment below, if depend on other package


Requires: ad_log4cpp >= 4.0.9
Requires: libevent >= 2.0
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
bidfeed-sharelib
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
mkdir -p .%{_prefix}/share/sharelib/
mkdir -p .%{_prefix}/include/sharelib/
mkdir -p .%{_prefix}/share/sharelib/conf/

# copy files
cp $OLDPWD/../build/release64/lib/libsharelib.so         .%{_prefix}/lib64/libbidfeed-sharelib.so
cp -rf $OLDPWD/../build/release64/sdk/sharelib_sdk-0.1.0-release64/include/sharelib/*         .%{_prefix}/include/sharelib/
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
