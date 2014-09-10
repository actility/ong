TARGET=./m2m/applications/NW_*/containers/DESCRIPTOR/contentInstances/latest/create.xml
./xoparse.sh -B $TARGET | grep "ref href" | grep DEV_ | wc -l
