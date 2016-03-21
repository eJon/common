##############################################################
# http://
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo alertsocket${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:1
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary:alertsocket 1.0.0
# this is the svn URL of current dir
URL: %{_svn_path}
Group: alertsocket
License: Commercial

# uncomment below, if depend on other package
# uncomment below, if depend on other package

AutoReq: no

%description
# if you want publish current svn URL or Revision use these macros
nginx
%{_svn_path}
%{_svn_revision}

# prepare your files
%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/w"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS

# create dirs
mkdir -p ./home/w/lib64/
mkdir -p ./home/w/include/alertsocket/

# copy files
cp $OLDPWD/../alert/lib/libalertsocket.so ./home/w/lib64/
cp $OLDPWD/../alert/alert_socket.h ./home/w/include/alertsocket/

%post

# package infomation
%files
# set file attribute here
%defattr(755,root,root)
/home/w/lib64/
/home/w/include/alertsocket/

%changelog
# Mar 12 2013 hongguan<hongguan@staff.sina.com.cn> 1.0.0-1
# build done 
