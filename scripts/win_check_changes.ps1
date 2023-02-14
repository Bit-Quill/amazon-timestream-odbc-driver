# Check if there is a change within one day
# This may not be delivered to AWS

$curTime=[DateTimeOffset]::Now.ToUnixTimeSeconds()
$lastCommit=git log -1 --format=%cd --date=unix
$gap=$curTime - $lastCommit
Write-Output "Now is $curTime, last commit is $lastCommit, gap is $gap"

if ($gap -gt 86400) {
  Write-Output "No changes for more than one day, skip build and test"
} else {
  # continue case must contain key word 'continue' which is checked in github workflow
  Write-Output "There is a change within one day, continue build and test"
}
