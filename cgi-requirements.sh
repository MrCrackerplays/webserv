#!/bin/sh

PYTHON="/usr/local/bin/python3.9"

if [ "$(uname)" = "Darwin" -a "$(sw_vers -productVersion | cut -d. -f1)" -lt 11 ]; then
	echo "Pip installing older version of opencv"
	$PYTHON -m pip install -v "opencv-python==4.5.1.48"
else
	echo "Pip installing opencv"
	$PYTHON -m pip install opencv-python
fi

touch cgi-check