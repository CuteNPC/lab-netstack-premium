#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/2/net.txt


echo "Run 'sender' and 'receiver' and redirect STDOUT"
cd ../helper
sudo bash ./execNS ns2 sudo  ../../build/test/lab1/receiver < ../../checkpoints/2/receive_input.txt   >  ../../checkpoints/2/receive_output.log 2> /dev/null &
sleep 0.5
sudo bash ./execNS ns1 sudo  ../../build/test/lab1/sender   < ../../checkpoints/2/send_input.txt      >  ../../checkpoints/2/send_output.log    2> /dev/null &
sleep 5

echo "Remove the network topology structure."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/2/net.txt

echo "Please check checkpoints/2/send_output.log and checkpoints/2/receive_output.log for the result."