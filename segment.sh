#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SEGMENT_BINARY="${SCRIPT_DIR}/./segment.py"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"
SWIG_MODULE_PATH="${SCRIPT_DIR}/debug/src/"

if [[ $# -lt 1 ]]; then
	echo "Usage: $0 <segmentation field> <file to segment>"
	exit -1;
fi


python3 "${SEGMENT_BINARY}" --student-archive-path="${STUDENT_ARCHIVE_PATH}" \
	--course-archive-path="${COURSE_ARCHIVE_PATH}" \
	--swig-module-path="${SWIG_MODULE_PATH}" "$@"
