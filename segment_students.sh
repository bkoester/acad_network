#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SEGMENT_BINARY="${SCRIPT_DIR}/build/src/segment"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"

if [[ $# -ne 1 ]]; then
	echo "Usage: $0 <segmentation field>"
	exit -1;
fi


$SEGMENT_BINARY --student_archive_path="$STUDENT_ARCHIVE_PATH" \
	--course_archive_path="$COURSE_ARCHIVE_PATH" -s $1
