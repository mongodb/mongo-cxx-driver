#!/bin/bash

mongo --quiet -p 27017 --eval 'db.version();'
mongo --quiet -p 27017 --eval 'db.adminCommand({setFeatureCompatibilityVersion: "3.4"});'
