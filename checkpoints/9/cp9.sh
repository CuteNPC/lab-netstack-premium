#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/9/net.txt

echo "Run echo_client and echo_server and redirect STDOUT"
echo "This test will last for 120 seconds. Please wait."
cd ../helper

nohup sudo bash ./execNS ns4 sudo  wireshark > /dev/null  2> /dev/null  &
sleep 10

sudo bash ./execNS ns4 sudo  ../../build/test/lab3/echo_server                    >  ../../checkpoints/9/host4.log    2> /dev/null &
sleep 5
sudo bash ./execNS ns1 sudo  ../../build/test/lab3/echo_client "10.100.3.2"       >  ../../checkpoints/9/host1.log    2> /dev/null &
sudo bash ./execNS ns2 sudo  ../../build/test/lab3/router                         >  ../../checkpoints/9/host2.log    2> /dev/null &
sudo bash ./execNS ns3 sudo  ../../build/test/lab3/router                         >  ../../checkpoints/9/host3.log    2> /dev/null &

sleep 120

echo "Remove the network topology structure."
echo "Please check checkpoints/9/hostX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/9/net.txt
