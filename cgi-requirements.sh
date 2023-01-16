#!/bin/sh

ENVIRONMENT_VARS="$(grep -v '^#' .env | xargs)"
if [ ! -z "$ENVIRONMENT_VARS" ]; then
	export $ENVIRONMENT_VARS
fi

if [ -z "$PYTHON_VERSION" ]; then
	echo "Remember to set PYTHON_VERSION in .env"
	exit
fi

if [ "$(uname)" = "Darwin" -a "$(sw_vers -productVersion | cut -d. -f1)" -lt 11 ]; then
	echo "Pip installing older version of opencv"
	$PYTHON_VERSION -m pip install -v "opencv-python==4.5.1.48"
else
	echo "Pip installing opencv"
	$PYTHON_VERSION -m pip install opencv-python
fi

