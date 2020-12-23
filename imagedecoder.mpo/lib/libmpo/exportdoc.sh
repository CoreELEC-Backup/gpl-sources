#!/bin/sh
if [ "$CC" = "clang" ] && [ "$TRAVIS_REPO_SLUG" = "Lectem/libmpo" ] && [ "$TRAVIS_PULL_REQUEST" = "false" ] && [ "$TRAVIS_BRANCH" = "master" ]; then
sudo apt-get install -qq doxygen
git clone --branch=gh-pages --single-branch --depth 1 https://${GH_TOKEN}@github.com/Lectem/libmpo html
cd html
git rm -rf ./*
cd ..
doxygen Doxyfile
cd html
git add --all
git commit -m"Doc generated from Travis build #$TRAVIS_BUILD_NUMBER"
git push -f origin gh-pages

fi