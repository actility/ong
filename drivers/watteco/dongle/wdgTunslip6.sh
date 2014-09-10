#! /bin/bash

## ============================================================
##
## Role       : "Chien de garde" logiciel 
##
## Auteur     : PME
##
## Historique :
##   05/04/12 : Creation
## 
## ============================================================

# Repertoires
# -----------

base=`pwd`
#"/home/ipsensor/Bureau/tunslip6"

# Fichiers
# --------

journal="$base/journal.log"
config="$base/dongle.conf"

# Dates
# -----

laDate="date +%Y/%m/%d"
lHeure="date +%H:%M:%S"

# Initialisation des variables par defaut
# ---------------------------------------

WDGTUNSLIP="wdgTunslip6.sh"

# Description de l'utilisation
# ----------------------------

function usage() {
	echo "Utilisation  : $1"
	echo "               Watchdog logiciel"
	echo ""
	echo "  -h         : Affiche le mode d'emploi"
}

# ecrire
# ------

function ecrire() {
	echo $1 >> $2
	fic=`cat $2 | tail -n100`
	echo "$fic" > $2
}

# Decodage des parametres
# -----------------------

while (( $# > 0 ))
do
	case $1 in
		(-h) 	usage $WDGTUNSLIP; exit 0 ;;
		(*)	ecrire "`$laDate` `$lHeure` [ER] Parametre '$1' inconnu" $journal
			echo ""
			echo "$WDGTUNSLIP : Parametre '$1' inconnu"
			echo ""
			usage $WDGTUNSLIP
			echo ""
			exit -1 ;;
  	esac
	
	shift 1
done

#===================
# Execution du role
#===================

cd $base
while (( 1 ))
do
	sleep 5

	if [[ (( -e $config )) ]]
	then
		localAddrs=`cat $config | grep tun | awk '{print $1;}'`
		for lAddr in $localAddrs
		do
			tunCfg=`cat $config | grep $lAddr | awk '{print $3;}'`
			isTunExist=`ifconfig | grep -c $tunCfg`
			if [[ (( $isTunExist == 0 )) ]]
			then
				ecrire "`$laDate` `$lHeure` [KO] Le tunnel $tunCfg du dongle '$lAddr' n'existe pas" $journal	
				devs=`ls /dev/ttyUSB*`
				for leDev in $devs
				do
					nbProc=`ps -aef | grep "tunslip6" | grep -c "$leDev"`				
					if [[ (( $nbProc == 0 )) ]]
					then
						ret=`./dongle $leDev`
						if [[ (( $ret == $lAddr )) ]]
						then
							ipAddr=`cat $config | grep $lAddr | awk '{print $2;}'`
							panID=`cat $config | grep $lAddr | awk '{print $4;}'`
							./tunslip6 $ipAddr -s $leDev -t $tunCfg -P $panID &
							ecrire "`$laDate` `$lHeure` [OK] Le tunnel $tunCfg du dongle '$lAddr' a ete relance" $journal
						fi
					fi
				done
#			else
#				ecrire "`$laDate` `$lHeure` [OK] Le tunnel $tunCfg du dongle '$lAddr' existe" $journal
			fi
		done
	else
		ecrire "`$laDate` `$lHeure` [ER] Le fichier de configuration 'dongle.conf' n'existe pas" $journal
	fi		

	ecrire "" $journal
done

exit 0

