#!/bin/sh

ps -ef | grep "qemu" | awk '{print $2}' | xargs kill 
