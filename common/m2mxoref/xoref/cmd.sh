for f in accessRightAnnc accessRight accessRights applicationAnnc application applications common containerAnnc container containers contentInstance contentInstances errorInfo extern groupAnnc group groups locationContainerAnnc locationContainer m2mPoc m2mPocs membersContent misc notificationChannel notificationChannels notify responseNotify sclBase scl scls subscription subscriptions
do
	echo	"$f.xor	: $f.ref misc.ref common.ref extern.ref"
	echo	"	\$(COMPXO) $f"
	echo
done
