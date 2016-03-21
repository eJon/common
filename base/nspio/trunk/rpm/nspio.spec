##############################################################
# http://                                                                                                                    
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo nspio${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary: nspio
# this is the svn URL of current dir
URL: %{_svn_path}
Group: nspio
License: Commercial
# uncomment below, if depend on other package

Requires: uuid >= 1.5.1
Requires: uuid-devel >= 1.5.1
Requires: libevent >= 2.0
Requires: ad_json >= 1.0.0
Requires: protobuf >= 1.0.0
Requires: ad_log4cpp >= 4.0.9 
Requires: bidfeed-sharelib >= 2.0.1
Requires: monitor-client >= 1.0
AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
nspio
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
mkdir -p .%{_prefix}/bin/
mkdir -p .%{_prefix}/logs/nspio/cli
mkdir -p .%{_prefix}/logs/nspio/svr
mkdir -p .%{_prefix}/logs/nspio/benchmark
mkdir -p .%{_prefix}/include/nspio
mkdir -p .%{_prefix}/conf/nspio/
mkdir -p .%{_prefix}/share/nspio/example/


# library files
cp $OLDPWD/../build/debug64/lib/libnspio_api.so				.%{_prefix}/lib64/libnspio_api.so.$VERSION.$RELEASE
ln -fs %{_prefix}/lib64/libnspio_api.so.$VERSION.$RELEASE		.%{_prefix}/lib64/libnspio_api.so
cp $OLDPWD/../build/debug64/lib/libnspio_common.so	       		.%{_prefix}/lib64/libnspio_common.so.$VERSION.$RELEASE
ln -fs %{_prefix}/lib64/libnspio_common.so.$VERSION.$RELEASE		.%{_prefix}/lib64/libnspio_common.so
cp $OLDPWD/../build/debug64/lib/libnspio_core.so	       		.%{_prefix}/lib64/libnspio_core.so.$VERSION.$RELEASE
ln -fs %{_prefix}/lib64/libnspio_core.so.$VERSION.$RELEASE		.%{_prefix}/lib64/libnspio_core.so


# header files
cp -rf $OLDPWD/../nspio/include/nspio	.%{_prefix}/include/

## execuate
cp $OLDPWD/../build/debug64/bin/nspiod		 .%{_prefix}/bin/
cp $OLDPWD/../build/debug64/bin/nspio_benchmark  .%{_prefix}/bin/

## startup script
cp $OLDPWD/../misc/script/nspio		       .%{_prefix}/bin/
cp $OLDPWD/../misc/script/nspiocli	       .%{_prefix}/bin/
cp $OLDPWD/../misc/script/nspiosvr	       .%{_prefix}/bin/

## config file
cp -rf $OLDPWD/../misc/conf/cli	       	       .%{_prefix}/conf/nspio/
cp -rf $OLDPWD/../misc/conf/svr	               .%{_prefix}/conf/nspio/
cp -rf $OLDPWD/../misc/conf/benchmark	       .%{_prefix}/conf/nspio/

## example
cp -rf $OLDPWD/../misc/example		       .%{_prefix}/share/nspio/


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
