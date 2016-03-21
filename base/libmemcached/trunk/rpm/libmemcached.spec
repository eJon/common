##############################################################
# http://
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo libmemcached${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary:libmemcached 1.0.17
# this is the svn URL of current dir
URL: %{_svn_path}
Group: libmemcached
License: Commercial

# uncomment below, if depend on other package
#BuildRequires: zeromq-dev = 1.0.0 

# uncomment below, if depend on other package
#Requires: libmemcached >= 1.0.10 

AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS

# create dirs
mkdir -p ./home/w/include/libmemcached/
mkdir -p ./home/w/include/libmemcached-1.0/
mkdir -p ./home/w/include/libmemcached-1.0/struct/
mkdir -p ./home/w/include/libmemcached-1.0/types/
mkdir -p ./home/w/include/libmemcachedutil-1.0/
mkdir -p ./home/w/include/libhashkit/
mkdir -p ./home/w/include/libhashkit-1.0/
mkdir -p ./home/w/lib64/
mkdir -p ./home/w/bin/

# copy files
cp $OLDPWD/../libmemcached/memcached.h**  ./home/w/include/libmemcached/
cp $OLDPWD/../libmemcached-1.0/*.h**  ./home/w/include/libmemcached-1.0/
cp -r $OLDPWD/../libmemcached-1.0/struct/*.h**  ./home/w/include/libmemcached-1.0/struct/
cp -r $OLDPWD/../libmemcached-1.0/types/*.h**  ./home/w/include/libmemcached-1.0/types/
cp $OLDPWD/../libmemcachedutil-1.0/*.h**  ./home/w/include/libmemcachedutil-1.0/
cp $OLDPWD/../libhashkit/hashkit.h  ./home/w/include/libhashkit/
cp $OLDPWD/../libhashkit-1.0/*.h**  ./home/w/include/libhashkit-1.0/
cp $OLDPWD/../clients/.libs/mem*  ./home/w/bin/


cp $OLDPWD/../libmemcached/.libs/libmemcached.*  ./home/w/lib64/
cp $OLDPWD/../libmemcached/.libs/libmemcachedutil.*  ./home/w/lib64/

%post

# package infomation
%files
/home/w
# set file attribute here
%defattr(755,root,root)

%changelog
# Oct 23 2013 tieniu<tieniu@staff.sina.com.cn> 1.0.0-1
# build done 
