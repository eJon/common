##############################################################
# http://
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo mccli-dev${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: mccli-dev
# this is the svn URL of current dir
URL: %{_svn_path}
Group: mccli-dev 
License: Commercial

AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
mccli-dev
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS

# create dirs
mkdir -p .%{_prefix}/include/mccli/
mkdir -p .%{_prefix}/lib64/

# copy files
cp $OLDPWD/../libmccli.a 		.%{_prefix}/lib64/
cp $OLDPWD/../libmccli.so 		.%{_prefix}/lib64/
cp $OLDPWD/../mccli.h		.%{_prefix}/include/mccli/

%post

# package infomation
%files
# set file attribute here
%defattr(755,weibo,weibo)
# need not list every file here, keep it as this
%{_prefix}
#%config(noreplace)	%{_prefix}/share/mccli-dev/conf/mccli-dev.conf

# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
* Wed Sep 21 2011 zibin<zibin@staff.sina.com.cn> 1.0.0-1
- mccli-dev done 
