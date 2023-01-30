# Check if there is a change within one day
# This may not be delivered to AWS

$curTime=[DateTimeOffset]::Now.ToUnixTimeSeconds()
$lastCommit=git log -1 --format=%cd --date=unix
$gap=$curTime - $lastCommit
echo "Now is $curTime, last commit is $lastCommit, gap is $gap"

if ($gap -gt 86400) {
  echo "No changes for more than one day, skip build and test"
  exit 1
} else {
  echo "There is a change within one day, start build and test"
  exit 0
}
