##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo sconstool${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: sconstool
# this is the svn URL of current dir
URL: %{_svn_path}
Group: sconstool
License: Commercial
# uncomment below, if depend on other package


AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
sconstool
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS
# create dirs

mkdir -p .%{_prefix}/share/sconstool
mkdir -p .%{_prefix}/share/sconstool/data/
mkdir -p .%{_prefix}/share/sconstool/bin/

# copy files
cp -rf $OLDPWD/../bin/data/*         .%{_prefix}/share/sconstool/data/
cp -rf $OLDPWD/../bin/initprj.sh         .%{_prefix}/share/sconstool/bin/

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
