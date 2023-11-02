#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/4/net.txt

echo "Run four 'looptest' and redirect STDOUT"
echo "This test will last for one minute. Please wait."
cd ../helper

sudo bash ./execNS ns1 sudo  ../../build/test/lab2/looptest flag1    >  ../../checkpoints/4/host1.log    2> /dev/null &
sudo bash ./execNS ns2 sudo  ../../build/test/lab2/looptest flag2    >  ../../checkpoints/4/host2.log    2> /dev/null &
sudo bash ./execNS ns3 sudo  ../../build/test/lab2/looptest flag3    >  ../../checkpoints/4/host3.log    2> /dev/null &
sudo bash ./execNS ns4 sudo  ../../build/test/lab2/looptest flag4    >  ../../checkpoints/4/host4.log    2> /dev/null &

sleep 20

pid=$(pgrep -f "looptest flag2")
kill $pid

sleep 20

sudo bash ./execNS ns2 sudo  ../../build/test/lab2/looptest flag2    >  ../../checkpoints/4/host2_back.log    2> /dev/null &

sleep 20


echo "Remove the network topology structure."
echo "Please check checkpoints/4/hostXXX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/4/net.txt