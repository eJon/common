##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo delivery_test${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: delivery_test
# this is the svn URL of current dir
URL: %{_svn_path}
Group: delivery_test
License: Commercial
# uncomment below, if depend on other package
#BuildRequires: zeromq-dev = 1.0.0 

AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
delivery_test
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS
# create dirs

mkdir -p .%{_prefix}/share/delivery_test/
mkdir -p .%{_prefix}/share/delivery_test/lib/
mkdir -p .%{_prefix}/share/delivery_test/conf/

# copy files
cp $OLDPWD/../testdata/lib/libdelivery_test.so         .%{_prefix}/share/delivery_test/lib/

%post

# package infomation
%files
# set file attribute here
#%defattr(644,root,root)
# need not list every file here, keep it as this
%{_prefix}

# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
