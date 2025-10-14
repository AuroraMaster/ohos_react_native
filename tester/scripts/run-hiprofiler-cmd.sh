# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

DEFAULT_TIME="10" # Check if the argument is provided, if not, use the default value
if [ -z "$1" ]; then
    TIME=$DEFAULT_TIME
else
    TIME=$1
fi
hdc file send ./scripts/hiprofiler-config.txt /data/local/tmp/
hdc shell "\
hiprofiler_cmd \
-c hiprofiler-config.txt \
-o /data/local/tmp/hiprofiler_data.htrace \
-t $TIME \
-s \
-k"
hdc file recv /data/local/tmp/hiprofiler_data.htrace ./
echo "#################################################"
echo "Hiprofiler data saved to ./hiprofiler_data.htrace"
echo "#################################################"
