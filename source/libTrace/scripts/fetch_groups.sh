egrep -hr '^#define' ../../../../ |sed -n 's/\#define\s*TRACE_GROUP_[^\s]*\s*\"\([^\"]*\)\"/\1/p' > groups.txt
