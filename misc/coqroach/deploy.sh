set -e

rm -rf .deploy
mkdir .deploy
cp -r challenge/* .deploy > /dev/null

cd .deploy
for file in $(cat ../.gitignore); do
  find -name "$file" -exec rm -rf '{}' ';'
done
rm -f deploy.sh proofs/SimplSoln.v soln.py flag
cd ..

TARGET=coqroach
mv .deploy $TARGET
tar -acvf dist/handout.tar.gz $TARGET
rm -rf $TARGET
