#!/bin/bash
set -e

rm -rf ../files ../dist/
mkdir -p ../files ../dist/

docker build ../docker -f ../docker/Dockerfile.builder -t pwn-serenityos

id=$(docker create pwn-serenityos)
docker cp $id:/serenity/disk.qcow2 ../files/disk.qcow2
docker cp $id:/serenity/Prekernel ../files/Prekernel
docker cp $id:/serenity/Kernel ../files/Kernel
docker rm -v $id

cd ../docker/
7z a -r ../dist/dist.7z ./ ../files/