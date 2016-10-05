import sys

from distutils.core import setup, Extension


if sys.version_info.major >= 3:
    sources = ['dublintraceroute/py3/_dublintraceroute.cc']
else:
    sources = ['dublintraceroute/py2/_dublintraceroute.cc']


dublintraceroute = Extension(
    'dublintraceroute._dublintraceroute',
    language='c++',
    libraries=['dublintraceroute'],
    include_dirs=[
        '../include',
        '/usr/include/jsoncpp',  # specific to debian-like systems
    ],
    sources=sources,
    extra_compile_args=[
        '-std=c++11',
        '-ldublintraceroute',
    ],
    extra_link_args=[],
)


setup(
    name='dublintraceroute',
    packages=['dublintraceroute'],
    version='0.4.0',
    author='Andrea Barberio',
    author_email='insomniac@slackware.it',
    description='NAT-aware multipath traceroute',
    license='BSD',
    url='https://www.dublin-traceroute.net',
    download_url='https://github.com/insomniacslk/python-dublin-traceroute/archive/v0.4.0.tar.gz',
    platforms=['linux', 'darwin'],
    ext_modules=[dublintraceroute],
    keywords=['traceroute', 'networking', 'multipath', 'nat'],
    classifiers=[],
)
