#!/bin/sh

@ECHO off
git config user.name "AutomaticCommit"
git config user.email "Automatic@commit.com"

git config --global user.name "AutomaticCommit"
git config --global user.email "Automatic@commit.com"

git add .

git commit -am "Automatic Commit"

git push origin


git config user.name unset
git config user.email unset
git config --global user.name unset
git config --global user.email unset
