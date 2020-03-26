#!/bin/bash

#- $Id: StartHere.sh MetaPhaze $

#-- If a special path is needed to the batch files set it here
BATCH_PATH="/home/dmr/shard/SH/"
#----------

#-- RETURN_TO_MENU() FUNCTION
RETURN_TO_MENU()
	{
	MENU
	}
	
#-- REALM_GEN() FUNCTION
REALM_GEN()
	{
	"${BATCH_PATH}"RealmGen.sh
	RETURN_TO_MENU
	}

#-- ECOMPILE() FUNCTION
ECOMPILE()
	{
	"${BATCH_PATH}"ECompile.sh
#RETURN_TO_MENU
	}
	
#-- CLEANUP() FUNCTION
CLEANUP()
	{
	"${BATCH_PATH}"CleanUp.sh
	RETURN_TO_MENU
	}
#-- QUIT FUNCTION
QUIT()
	{
	clear
	exit 0
	}

#-- MENU FUNCTION
MENU()
	{
	clear
	echo " StartHere.sh (v 1.0) by MetaPhaze"
	echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
	echo " Command        Purpose"
	echo "---------   -------------------------------------------"
	echo "  [ a ]   - RealmGen menu        (Realm building tools)"
	echo "  [ b ]   - Ecompiler menu       (Ecompile tools)"
	echo "  [ c ]   - Cleanup menu         (File removal tools)"
	echo "  [ x ]   - Quit"

	read -n 1 -p "Command: " CMD

	if [  "${CMD}" == "a" ]
		then REALM_GEN
		fi
	if [  "${CMD}" == "b" ]
		then ECOMPILE
		fi
	if [  "${CMD}" == "c" ]
		then CLEANUP
		fi
	if [  "${CMD}" == "x" ]
		then QUIT
		fi

	echo ""
	echo " Invalid command."
	RETURN_TO_MENU
	}

MENU