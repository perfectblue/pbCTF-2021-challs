set -e

rm -rf .deploy
mkdir .deploy
cp np lockfile .deploy/

TARGET=np_handout
mv .deploy $TARGET
tar -cvf handout.tar $TARGET
bzip2 handout.tar
mv handout.tar.bz2 ../dist/
rm -rf $TARGET



