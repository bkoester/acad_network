#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

QUERY_BINARY="${SCRIPT_DIR}/./query_students.py"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"
SWIG_MODULE_PATH="${SCRIPT_DIR}/debug/src/"


python $QUERY_BINARY --student-archive-path="${STUDENT_ARCHIVE_PATH}" \
	--course-archive-path="${COURSE_ARCHIVE_PATH}" \
	--swig-module-path="${SWIG_MODULE_PATH}"
