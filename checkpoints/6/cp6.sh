#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/6/net.txt

echo "Run sendercp6 and receivercp6 and redirect STDOUT"
echo "This test will last for 10 seconds. Please wait."
cd ../helper

sudo bash ./execNS ns1 sudo  ../../build/test/lab2/sendercp6 flag1      >  ../../checkpoints/6/host1.log    2> /dev/null &
sudo bash ./execNS ns2 sudo  ../../build/test/lab2/receivercp6 flag2    >  ../../checkpoints/6/host2.log    2> /dev/null &
sudo bash ./execNS ns3 sudo  ../../build/test/lab2/receivercp6 flag3    >  ../../checkpoints/6/host3.log    2> /dev/null &

sleep 10

echo "Remove the network topology structure."
echo "Please check checkpoints/6/hostXXX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/6/net.txt