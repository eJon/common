##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo mem_db${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: mem_db
# this is the svn URL of current dir
URL: %{_svn_path}
Group: mem_db
License: Commercial
# uncomment below, if depend on other package
Requires: libmemcached >= 1.0.17-3 

AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
mem_db
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS
# create dirs

mkdir -p .%{_prefix}/include/mem_db/
mkdir -p .%{_prefix}/conf/mem_db/
mkdir -p .%{_prefix}/lib64/

# copy files

cp $OLDPWD/../build/debug64/mem_db/mem_db.h .%{_prefix}/include/mem_db/
cp $OLDPWD/../build/debug64/mem_db/mem_db_def.h .%{_prefix}/include/mem_db/
cp $OLDPWD/../misc/mem_db.conf .%{_prefix}/conf/mem_db/
cp $OLDPWD/../build/debug64/lib/libmem_db.so .%{_prefix}/lib64/

%post

# package infomation
%files
/home/w
# set file attribute here
#%defattr(755,root,root)
%defattr(755,root,root)
%config(noreplace)  /home/w/conf/mem_db/mem_db.conf
# need not list every file here, keep it as this
%{_prefix}


# create an empy dir
# need bakup old config file, so indicate here
# indicate the dir for crontab

%changelog
