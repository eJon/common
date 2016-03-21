##############################################################
# http://
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo aes ${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary:build mc data for Rs 
# this is the svn URL of current dir
URL: %{_svn_path}
Group: ad
License: Commercial

# uncomment below, if depend on other package
#BuildRequires: zeromq-dev = 1.0.0 

# uncomment below, if depend on other package
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64 depend on OS

# create dirs
mkdir -p ./home/w/conf/
mkdir -p ./home/w/lib64/
mkdir -p ./home/w/include/aes/

# copy files
cp $OLDPWD/../include/aes.h  ./home/w/include/aes/
cp $OLDPWD/../include/_aes.h  ./home/w/include/aes/
cp $OLDPWD/../src/utest/aes.key  ./home/w/conf/
cp $OLDPWD/../lib/libaes.a          ./home/w/lib64/

%post

# package infomation
%files
# set file attribute here
%defattr(755,root,root)
%{_prefix}
# need not list every file here, keep it as this

# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
* Wed Apr 11 2012 zibin<zibin@staff.sina.com.cn> 1.0.0-1
- build done 
