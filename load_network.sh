#! /bin/bash

LOAD_BINARY="build/src/load"
STUDENT_FILE="../data/student.tab"
ENROLLMENT_FILE="../data/enrollment.tab"
STUDENT_NETWORK_ARCHIVE="../../data/student_network.txt"

$LOAD_BINARY --student_file="$STUDENT_FILE" \
	--enrollment_file="$ENROLLMENT_FILE" --network_to_load=student \
	--student_archive="$STUDENT_NETWORK_ARCHIVE"
