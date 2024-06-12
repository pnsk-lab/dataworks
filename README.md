# DataWorks

[![Build DataWorks](https://github.com/pnsk-lab/dataworks/actions/workflows/build.yml/badge.svg)](https://github.com/pnsk-lab/dataworks/actions/workflows/build.yml) [![Jenkins](https://jenkins.nishi.boats/job/dataworks/badge/icon?subject=Jenkins&color=%23800)](https://jenkins.nishi.boats/job/dataworks)

![Logo](Binary/dataworks-white.png)

DataWorks is a simple DBMS written in C.

## Features
1. File database
2. Can be run as a server
3. Works even on your 80286 machine
4. Server can be run over modem if you are on DOS
5. Cross-platform database

## Platform support
1. DOS (16-bit)
 
	Works on 80286 with 1MB RAM.
2. DOS with DOS/4GW or its compatibles
 
	Works on 80386 with 2MB RAM.
3. Windows 32-bit/64-bit
 
	Works on Windows 7 or later.
4. NetBSD
 
	Works on amd64 10.0, and macppc 9.3.
5. Linux

## Server support
1. DOS with modem
2. Windows or NetBSD/Linux with TCP/IP socket support

## Mailing list
1. [dataworks-users](https://mail-index.nishi.boats/mailman/listinfo/dataworks-users)
2. [dataworks-commits](https://mail-index.nishi.boats/mailman/listinfo/dataworks-commits)

## Mirrors
Original: http://sw.nishi.boats/svn/nishi-dataworks
1. [Nishi](https://git-mirror.nishi.boats/?p=dataworks.git)
2. [GitHub](https://github.com/pnsk-lab/dataworks)

## Directory Structure
 - `Binary`

 	Contains images
 - `Client`
 
 	Contains client source code
 - `Document`
 
 	Contains files related to generate a document
 - `Grammar`
 
 	Contains Yacc/Flex files
 - `Library`
 
 	Contains base library source code
 - `Makefiles`
 
	Contains Makefiles used to build DataWorks
 
	- `Makefiles/Platforms`
	 
		Contains Makefiles to solve platform-dependent stuff
 - `Package`
 
	Contains files related to package manager
 - `Server`
 
	Contains server source code
 - `Tool`
 
	Contains tools used for source codes formatting, and etc
 - `Vim`
 
	Contains syntax file for Vim
