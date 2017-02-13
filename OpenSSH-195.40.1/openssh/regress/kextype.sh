#	$OpenBSD: kextype.sh,v 1.6 2015/03/24 20:19:15 markus Exp $
#	Placed in the Public Domain.

tid="login with different key exchange algorithms"

TIME=/usr/bin/time
cp $OBJ/sshd_proxy $OBJ/sshd_proxy_bak
cp $OBJ/ssh_proxy $OBJ/ssh_proxy_bak

# Make server accept all key exchanges.
#Apple: Skip GSS kex algs as sshd isn't configured for them
ALLKEX=`ssh -Q kex | grep -v gss-`
KEXOPT=`echo $ALLKEX | tr ' ' ,`
echo "KexAlgorithms=$KEXOPT" >> $OBJ/sshd_proxy

tries="1 2 3 4"
for k in `${SSH} -Q kex | grep -v gss-`; do
	verbose "kex $k"
	for i in $tries; do
		${SSH} -F $OBJ/ssh_proxy -o KexAlgorithms=$k x true
		if [ $? -ne 0 ]; then
			fail "ssh kex $k"
		fi
	done
done

