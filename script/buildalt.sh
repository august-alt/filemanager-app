#!/bin/bash -ex

mkdir /home/builder2/hasher 
mkdir /home/builder2/.hasher
mkdir /home/builder2/build
cp /app/hasher/config /home/builder2/.hasher/config 
hsh --initroot-only --no-wait-lock -vv /home/builder2/hasher 

chown -R builder2:builder2 /app/
mkdir -p /app/RPMS/

cd /app/ && gear-hsh
cp /home/builder2/build/repo/x86_64/RPMS.hasher/*.rpm /app/RPMS/
