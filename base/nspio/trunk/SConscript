# -*- mode: python -*-

# Inherit the environment from my parent.
Import('env')
import os
import commands
# Make a copy of the environment, so my changes are limited in this directory and sub-directories.
env = env.Clone()
#############################################################################################################
env.Append(ACSUBST = {'BUILDDIR': env['relative_build_dir']})
env.Append(ACSUBST = {'TOP_BUILDDIR': env['builddir']})
env.Append(ACSUBST = {'TOP_SRCDIR': env['topdir']})
env.Append(ACSUBST = {'abs_top_srcdir': env['topdir']})


loggerconf = env['topdir'] + '/' + env['prjName'] + '/test/log4cpp.conf'

env.Append(ACSUBST = {'DOTEST_LOGGER_CONF': loggerconf})

env.aACSubst(target = [env['prjName'] + '/test/test.h'], 
             source = [env['prjName'] + '/test/test.h.in'])

#env.aDuplicate('testdata/configuration/service.json')

#################################################################################################
env.AppendENVPath('LD_LIBRARY_PATH',
                  Dir(env['LIB_DIR']).abspath + ':' +env['engine_commonlibdir']+ ':'
                  '/lib64/:/usr/local/lib64:/usr/local/lib')

env.Append(CPPPATH=['#'])
env.Append(CPPPATH =
           [
        env['engine_commonheaderdir'],
        '/usr/local/include/',
        env['topdir'] + '/' + env['prjName'] + '/include/',
        env['topdir'] + '/' + env['prjName'] + '/core/',
        env['topdir'] + '/' + env['prjName'] + '/api/',
        ])

env.Append(LIBPATH = [ '/usr/local/lib/', '/usr/local/lib64', env['engine_commonlibdir'] ])

# Test Library


if env['heapchecktype'] == 'tcmalloc':
    env.aCheckLibraryNotAutoAdd('tcmalloc')

#env.aCheckLibrary('dl')
#env.aCheckLibrary('expat')

#env.Append(CPPPATH = env['topdir'] + '/contribute/mxml-2.2.2/')
#env.aDirs('contribute/mxml-2.2.2')
# List my sub-directories.


#env.aDirs('sdk')
env.aDirs('tools')

env.aDirs(env['prjName'])
###########################################################################################
#Add Additional share obj to ha2_package
'''
ret, expat = commands.getstatusoutput("ldd /usr/local/bin/AliWS  |grep expat |awk -F\"=>\" '{print $2}' |awk -F\"(\" '{print $1;}'|awk -F\" \" '{print $1;}'");
print "expat is %s" % expat

ret, enet = commands.getstatusoutput("file /usr/local/lib/libenet.so |awk -F\"\`\" '{print $2;}' |awk -F\"'\" '{print $1;}'");
enet = '/usr/local/lib/%s' % enet
print "enet is %s" %enet

'''
env.aPackage(env['prjName'],
             [
              
              ],
             

             strip = False
             )


#env.aPackage(env['prjName'],
#             ['default_conf/analyzer.json',
#              '/usr/local/libdata/AliWS/conf/AliTokenizer.conf'],
#             subdir="default_conf"
#             )
##################################################################################################
'''
# Make distribution
env['DIST_DIR'] = '#dist/';
env['DIST_INCLUDE_DIR'] = env['DIST_DIR'] + 'include/' + prjName

if env['mode'] == 'debug':
    dir_suffix = 'd'
else:
    dir_suffix = ''

env['DIST_LIB_DIR'] = env['DIST_DIR'] + 'lib' + env['target'] + dir_suffix
env['DIST_BIN_DIR'] = env['DIST_DIR'] + 'bin' + env['target'] + dir_suffix

dist_includes = []

dist_libs = []

dist_bins = []

for i in dist_includes:
    t = env.InstallAs(env['DIST_INCLUDE_DIR'] + i, 'include/apsara/' + i)
    env.Alias('dist', t)
    Env.Default(t)

for i in dist_libs:
    t = env.Install(env['DIST_LIB_DIR'], 'lib/' + i)
    env.Alias('dist', t)
    env.Default(t)

for i in dist_bins:
    t = env.Install(env['DIST_BIN_DIR'], 'bin/' + i)
    env.Alias('dist', t)
    env.Default(t)
'''

