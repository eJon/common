##############################################################
# http://
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name:%(echo nginx_adfront${SUFFIX})
# 
Version:%{_version}
# if you want get version number from outside, use like this
Release:%(echo $RELEASE)%{?dist}
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary:nginx 2.0.1
# this is the svn URL of current dir
URL: %{_svn_path}
Group: nginx
License: Commercial

# uncomment below, if depend on other package
#BuildRequires: zeromq-dev = 1.0.0 

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
mkdir -p ./usr/local/nginx/conf/
mkdir -p ./usr/local/nginx/logs/
mkdir -p ./usr/local/nginx/sbin/


# copy files
cp $OLDPWD/../../objs/nginx ./usr/local/nginx/sbin/
cp $OLDPWD/../conf/nginx.conf ./usr/local/nginx/conf/

%post

# package infomation
%files
# set file attribute here
%defattr(755,root,root)
/usr/local/nginx/

%config(noreplace) /usr/local/nginx/conf/nginx.conf
%changelog
# Oct 12 2012 tieniu<tieniu@staff.sina.com.cn> 1.0.0-1
# build done 
