#!/bin/sh

ENVIRONMENT_VARS="$(grep -v '^#' .env | xargs)"
if [ ! -z "$ENVIRONMENT_VARS" ]; then
	export $ENVIRONMENT_VARS
fi

if [ -z "$PYTHON_PATH" ]; then
	echo "Remember to set PYTHON_PATH in .env"
	exit 1
fi

if [ "$(uname)" = "Darwin" -a "$(sw_vers -productVersion | cut -d. -f1)" -lt 11 ]; then
	echo "Pip installing older version of opencv"
	$PYTHON_PATH -m pip install -v "opencv-python==4.5.1.48"
else
	echo "Pip installing opencv"
	$PYTHON_PATH -m pip install opencv-python
fi

touch cgi-check