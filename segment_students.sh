#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SEGMENT_BINARY="${SCRIPT_DIR}/build/src/segment"
STUDENT_FILE="${SCRIPT_DIR}/../data/student.tab"
ENROLLMENT_FILE="${SCRIPT_DIR}/../data/enrollment.tab"

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <segmentation field>"
	exit -1;
fi


$SEGMENT_BINARY --student_file="$STUDENT_FILE" \
	--enrollment_file="$ENROLLMENT_FILE" -s $1
