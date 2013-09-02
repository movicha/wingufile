                         Wingufile command line client
                         ===========================

For the full manual about wingufile CLI client, see [https://github.com/haiwen/wingufile/wiki/Wingufile-CLI-client]

Table of Contents
=================
1 Requirement:
2 Get Started
    2.1 Initialize
    2.2 Start wingufile client
    2.3 Download a library from a server
    2.4 stop wingufile client
3 Uninstall


1 Requirement:
---------------

  - python 2.6/2.7

  - If you use python 2.6, you need to install python "argparse" module

    see [https://pypi.python.org/pypi/argparse]


2 Get Started
--------------


2.1 Initialize
===============

  mkdir ~/wingufile-client
  ./winguf-cli init -d ~/wingufile-client


2.2 Start wingufile client
=========================



  ./winguf-cli start



2.3 Download a library from a server
=====================================

   First retrieve the library id by browsing on the server -> it's in the url after "/repo/"

   Then:

   winguf-cli download -l "the id of the library" -s  "the url + port of server" -d "the folder where the library folder will be downloaded" -u "username on server" [-p "password"]

   winguf-cli status  # check status of ongoing downloads

   # Name  Status  Progress
   # Apps    downloading     9984/10367, 9216.1KB/s


2.4 stop wingufile client
========================

  ./winguf-cli stop

3 Uninstall
------------

  First stop the client:

  winguf-cli stop

  Then remove the data:


  rm -rf ~/.wingufile-client

  rm -rf ~/.ccnet   # note this should not be erased if you run the server on the same host

  rm -rf wingufile-cli-1.5.3

