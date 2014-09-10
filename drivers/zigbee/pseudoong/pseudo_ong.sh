#
# -mxxx usleep(xxx*1000) before each request
# -P no piggy backing
$ROOTACT/zigbee/pseudoong/simong.x -R `pwd` $*
