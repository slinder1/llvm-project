#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

help() {
    printf 'usage: %s\n' "$0"
    printf '\t%s\n' '-h  print help'
    printf '\t%s\n' '-s  skip summarize step'
    exit $1
}

SKIP_SUMMARIZE=0
while getopts "hs" opt; do
    case "$opt" in
        h) help 0 ;;
        s) SKIP_SUMMARIZE=1 ;;
        \?) help 1 ;;
    esac
done
shift $((OPTIND - 1))

summarize() {
local CFG=$1; shift
local RUN_FILE=$1; shift
awk -F: -v fields=3 -f <(cat <<'EOF'
{
    for (i = 1; i <= fields; i++)
        sum[i] += $i
}
END {
    printf "%d", NR
    for (i = 1; i <= fields; i++)
        printf ":%d", sum[i]
    printf "\n"
}
EOF
) $RUN_FILE >>$ROOT/$CFG.summary
}

stats() {
local CFG="$1"; shift
if ! ((SKIP_SUMMARIZE)); then
    rm -f $ROOT/$CFG.summary
    for f in $ROOT/$CFG/*; do
        summarize $CFG $f
    done
fi
printf 'Stats for "%s"\n' $CFG
awk -F: -v fields=4 -f <(cat <<'EOF'
!record_idx++ {
    for (i = 1; i <= fields; i++) {
        min[i] = $i
        max[i] = $i
    }
}
{
    for (i = 1; i <= fields; i++) {
        if ($i < min[i])
            min[i] = $i
        if ($i > max[i])
            max[i] = $i
        sum[i] += $i
    }
}
END {
    for (i = 1; i <= fields; i++) {
        avg[i] = (NR == 0 ? 0.0 : sum[i]/NR)
    }
    stats_for("#AllExprs", 1)
    stats_for("#UsedExprs", 2)
    printf "%UsedExprs:\tavg:%.2f\n", (avg[2]/avg[1])
    stats_for("#OpsPerExpr", 3)
    stats_for("#UIntsPerExpr", 4)
    printf "%DIOpBloat:\tavg:%.2f\n", ((avg[3]*2)/avg[4])
}
function stats_for(name, i) {
    printf "%s:\tavg:%.2f\tmin:%d\tmax:%d\n", name, avg[i], min[i], max[i]
}
EOF
) $ROOT/$CFG.summary | tee $ROOT/$CFG.stats
}

stats RelWithDebInfo
#stats Debug
